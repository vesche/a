/*
 * gguf.c -- Minimal GGUF model loader + CPU inference engine for "a" language
 *
 * Supports: LLaMA/Mistral/Gemma architecture, Q4_0/Q8_0/F16/F32 quantization.
 * CPU-only, single-threaded. Designed for small models (1-7B parameters).
 *
 * Not competing with llama.cpp -- providing a minimal inference path
 * that runs without any external process or dependency.
 */

#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef WASM_BUILD

/* ===== GGUF Format Constants ===== */

#define GGUF_MAGIC 0x46554747 /* "GGUF" */
#define GGUF_VERSION_3 3

enum gguf_type {
    GGUF_TYPE_UINT8 = 0, GGUF_TYPE_INT8, GGUF_TYPE_UINT16, GGUF_TYPE_INT16,
    GGUF_TYPE_UINT32, GGUF_TYPE_INT32, GGUF_TYPE_FLOAT32, GGUF_TYPE_BOOL,
    GGUF_TYPE_STRING, GGUF_TYPE_ARRAY, GGUF_TYPE_UINT64, GGUF_TYPE_INT64,
    GGUF_TYPE_FLOAT64
};

enum ggml_type {
    GGML_TYPE_F32 = 0, GGML_TYPE_F16 = 1,
    GGML_TYPE_Q4_0 = 2, GGML_TYPE_Q4_1 = 3,
    GGML_TYPE_Q5_0 = 6, GGML_TYPE_Q5_1 = 7,
    GGML_TYPE_Q8_0 = 8, GGML_TYPE_Q8_1 = 9,
    GGML_TYPE_Q2_K = 10, GGML_TYPE_Q3_K = 11,
    GGML_TYPE_Q4_K = 12, GGML_TYPE_Q5_K = 13,
    GGML_TYPE_Q6_K = 14, GGML_TYPE_Q8_K = 15,
    GGML_TYPE_IQ2_XXS = 16, GGML_TYPE_IQ2_XS = 17,
    GGML_TYPE_IQ3_XXS = 18, GGML_TYPE_IQ1_S = 19, GGML_TYPE_IQ4_NL = 20,
    GGML_TYPE_BF16 = 30
};

/* Q4_0 block: delta (f16) + 16 bytes (32 nibbles) = 18 bytes per 32 values */
#define QK4_0 32
typedef struct { uint16_t d; uint8_t qs[QK4_0 / 2]; } block_q4_0;

/* Q8_0 block: delta (f16) + 32 bytes = 34 bytes per 32 values */
#define QK8_0 32
typedef struct { uint16_t d; int8_t qs[QK8_0]; } block_q8_0;

/* ===== F16 Conversion ===== */

static float f16_to_f32(uint16_t h) {
    uint32_t sign = (uint32_t)(h & 0x8000) << 16;
    uint32_t expo = (h >> 10) & 0x1F;
    uint32_t mant = h & 0x3FF;
    uint32_t f;
    if (expo == 0) {
        if (mant == 0) { f = sign; }
        else {
            expo = 1;
            while (!(mant & 0x400)) { mant <<= 1; expo--; }
            mant &= 0x3FF;
            f = sign | ((uint32_t)(expo + 127 - 15) << 23) | ((uint32_t)mant << 13);
        }
    } else if (expo == 31) {
        f = sign | 0x7F800000 | ((uint32_t)mant << 13);
    } else {
        f = sign | ((uint32_t)(expo + 127 - 15) << 23) | ((uint32_t)mant << 13);
    }
    float result;
    memcpy(&result, &f, 4);
    return result;
}

/* ===== Dequantization ===== */

static void dequantize_q4_0(const void* src, float* dst, int n) {
    const block_q4_0* blocks = (const block_q4_0*)src;
    int nb = n / QK4_0;
    for (int i = 0; i < nb; i++) {
        float d = f16_to_f32(blocks[i].d);
        for (int j = 0; j < QK4_0 / 2; j++) {
            int v0 = (blocks[i].qs[j] & 0x0F) - 8;
            int v1 = ((blocks[i].qs[j] >> 4) & 0x0F) - 8;
            dst[i * QK4_0 + j]              = v0 * d;
            dst[i * QK4_0 + j + QK4_0 / 2]  = v1 * d;
        }
    }
}

static void dequantize_q8_0(const void* src, float* dst, int n) {
    const block_q8_0* blocks = (const block_q8_0*)src;
    int nb = n / QK8_0;
    for (int i = 0; i < nb; i++) {
        float d = f16_to_f32(blocks[i].d);
        for (int j = 0; j < QK8_0; j++) {
            dst[i * QK8_0 + j] = blocks[i].qs[j] * d;
        }
    }
}

static void dequantize_f16(const void* src, float* dst, int n) {
    const uint16_t* s = (const uint16_t*)src;
    for (int i = 0; i < n; i++) dst[i] = f16_to_f32(s[i]);
}

/* ===== Tensor ===== */

typedef struct {
    char name[256];
    int n_dims;
    int64_t dims[4];
    int type;
    int64_t offset;
    int64_t n_elements;
    int64_t byte_size;
} GGUFTensor;

/* ===== Model Context ===== */

#define MAX_TENSORS 512
#define MAX_VOCAB 128000
#define MAX_SEQ 4096

typedef struct {
    /* GGUF metadata */
    int version;
    int64_t n_tensors;
    char arch[64];
    char model_name[256];

    /* Architecture params */
    int n_vocab;
    int n_embd;
    int n_layer;
    int n_head;
    int n_head_kv;
    int n_ff;
    int n_ctx;
    float rope_freq_base;
    float rms_norm_eps;

    /* Vocabulary */
    char** vocab;
    float* vocab_scores;
    int* vocab_types;

    /* Tensors */
    GGUFTensor tensors[MAX_TENSORS];
    int tensor_count;

    /* Raw data (mmap or malloc) */
    uint8_t* data;
    int64_t data_size;
    uint8_t* tensor_data_start;
    FILE* file;

    /* KV cache */
    float* key_cache;
    float* value_cache;
    int kv_len;

    /* Scratch buffers */
    float* scratch;
    int scratch_size;
} LLMModel;

/* ===== GGUF Parser ===== */

static int gguf_read_string(FILE* f, char* out, int max) {
    uint64_t len;
    if (fread(&len, 8, 1, f) != 1) return -1;
    if ((int64_t)len >= max) len = max - 1;
    if (fread(out, 1, (size_t)len, f) != (size_t)len) return -1;
    out[len] = '\0';
    return (int)len;
}

static int gguf_skip_value(FILE* f, int type) {
    switch (type) {
        case GGUF_TYPE_UINT8: case GGUF_TYPE_INT8: case GGUF_TYPE_BOOL:
            fseek(f, 1, SEEK_CUR); return 0;
        case GGUF_TYPE_UINT16: case GGUF_TYPE_INT16:
            fseek(f, 2, SEEK_CUR); return 0;
        case GGUF_TYPE_UINT32: case GGUF_TYPE_INT32: case GGUF_TYPE_FLOAT32:
            fseek(f, 4, SEEK_CUR); return 0;
        case GGUF_TYPE_UINT64: case GGUF_TYPE_INT64: case GGUF_TYPE_FLOAT64:
            fseek(f, 8, SEEK_CUR); return 0;
        case GGUF_TYPE_STRING: {
            uint64_t len;
            if (fread(&len, 8, 1, f) != 1) return -1;
            fseek(f, (long)len, SEEK_CUR);
            return 0;
        }
        case GGUF_TYPE_ARRAY: {
            uint32_t elem_type;
            uint64_t count;
            if (fread(&elem_type, 4, 1, f) != 1) return -1;
            if (fread(&count, 8, 1, f) != 1) return -1;
            for (uint64_t i = 0; i < count; i++) {
                if (gguf_skip_value(f, elem_type) != 0) return -1;
            }
            return 0;
        }
        default: return -1;
    }
}

static int64_t gguf_type_block_size(int type) {
    switch (type) {
        case GGML_TYPE_F32: return 4;
        case GGML_TYPE_F16: return 2;
        case GGML_TYPE_Q4_0: return sizeof(block_q4_0);
        case GGML_TYPE_Q8_0: return sizeof(block_q8_0);
        default: return 0;
    }
}

static int64_t gguf_type_block_count(int type) {
    switch (type) {
        case GGML_TYPE_F32: case GGML_TYPE_F16: return 1;
        case GGML_TYPE_Q4_0: return QK4_0;
        case GGML_TYPE_Q8_0: return QK8_0;
        default: return 1;
    }
}

static void dequantize_tensor(const void* src, float* dst, int n, int type) {
    switch (type) {
        case GGML_TYPE_F32:  memcpy(dst, src, n * sizeof(float)); break;
        case GGML_TYPE_F16:  dequantize_f16(src, dst, n); break;
        case GGML_TYPE_Q4_0: dequantize_q4_0(src, dst, n); break;
        case GGML_TYPE_Q8_0: dequantize_q8_0(src, dst, n); break;
        default:
            memset(dst, 0, n * sizeof(float));
            break;
    }
}

static float* get_tensor_data(LLMModel* m, const char* name, int expected) {
    for (int i = 0; i < m->tensor_count; i++) {
        if (strcmp(m->tensors[i].name, name) == 0) {
            GGUFTensor* t = &m->tensors[i];
            int n = (int)t->n_elements;
            if (expected > 0 && n != expected) return NULL;
            float* out = (float*)malloc(n * sizeof(float));
            if (!out) return NULL;
            dequantize_tensor(m->tensor_data_start + t->offset, out, n, t->type);
            return out;
        }
    }
    return NULL;
}

static LLMModel* gguf_load(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    uint32_t magic;
    if (fread(&magic, 4, 1, f) != 1 || magic != GGUF_MAGIC) { fclose(f); return NULL; }

    uint32_t version;
    if (fread(&version, 4, 1, f) != 1 || version < GGUF_VERSION_3) { fclose(f); return NULL; }

    LLMModel* m = (LLMModel*)calloc(1, sizeof(LLMModel));
    if (!m) { fclose(f); return NULL; }
    m->file = f;
    m->version = version;

    uint64_t n_tensors, n_kv;
    fread(&n_tensors, 8, 1, f);
    fread(&n_kv, 8, 1, f);
    m->n_tensors = (int64_t)n_tensors;

    /* Default params */
    m->rms_norm_eps = 1e-5f;
    m->rope_freq_base = 10000.0f;
    m->n_ctx = 2048;
    strcpy(m->arch, "llama");

    /* Parse metadata KV pairs */
    for (uint64_t i = 0; i < n_kv; i++) {
        char key[256];
        if (gguf_read_string(f, key, 256) < 0) break;
        uint32_t vtype;
        if (fread(&vtype, 4, 1, f) != 1) break;

        if (strcmp(key, "general.architecture") == 0 && vtype == GGUF_TYPE_STRING) {
            gguf_read_string(f, m->arch, 64);
        } else if (strcmp(key, "general.name") == 0 && vtype == GGUF_TYPE_STRING) {
            gguf_read_string(f, m->model_name, 256);
        } else if ((strstr(key, ".embedding_length") || strstr(key, ".hidden_size")) && vtype == GGUF_TYPE_UINT32) {
            fread(&m->n_embd, 4, 1, f);
        } else if (strstr(key, ".block_count") && vtype == GGUF_TYPE_UINT32) {
            fread(&m->n_layer, 4, 1, f);
        } else if (strstr(key, ".attention.head_count") && !strstr(key, "_kv") && vtype == GGUF_TYPE_UINT32) {
            fread(&m->n_head, 4, 1, f);
        } else if (strstr(key, ".attention.head_count_kv") && vtype == GGUF_TYPE_UINT32) {
            fread(&m->n_head_kv, 4, 1, f);
        } else if (strstr(key, ".feed_forward_length") && vtype == GGUF_TYPE_UINT32) {
            fread(&m->n_ff, 4, 1, f);
        } else if (strstr(key, ".context_length") && vtype == GGUF_TYPE_UINT32) {
            uint32_t ctx;
            fread(&ctx, 4, 1, f);
            m->n_ctx = (int)ctx;
        } else if (strstr(key, "rope.freq_base") && vtype == GGUF_TYPE_FLOAT32) {
            fread(&m->rope_freq_base, 4, 1, f);
        } else if (strstr(key, "layer_norm_rms_epsilon") && vtype == GGUF_TYPE_FLOAT32) {
            fread(&m->rms_norm_eps, 4, 1, f);
        } else if (strcmp(key, "tokenizer.ggml.tokens") == 0 && vtype == GGUF_TYPE_ARRAY) {
            uint32_t arr_type;
            uint64_t arr_count;
            fread(&arr_type, 4, 1, f);
            fread(&arr_count, 8, 1, f);
            m->n_vocab = (int)(arr_count < MAX_VOCAB ? arr_count : MAX_VOCAB);
            m->vocab = (char**)calloc(m->n_vocab, sizeof(char*));
            for (int j = 0; j < m->n_vocab; j++) {
                uint64_t slen;
                fread(&slen, 8, 1, f);
                m->vocab[j] = (char*)malloc(slen + 1);
                fread(m->vocab[j], 1, (size_t)slen, f);
                m->vocab[j][slen] = '\0';
            }
            for (uint64_t j = m->n_vocab; j < arr_count; j++) {
                uint64_t slen;
                fread(&slen, 8, 1, f);
                fseek(f, (long)slen, SEEK_CUR);
            }
        } else if (strcmp(key, "tokenizer.ggml.scores") == 0 && vtype == GGUF_TYPE_ARRAY) {
            uint32_t arr_type;
            uint64_t arr_count;
            fread(&arr_type, 4, 1, f);
            fread(&arr_count, 8, 1, f);
            int nv = (int)(arr_count < MAX_VOCAB ? arr_count : MAX_VOCAB);
            m->vocab_scores = (float*)malloc(nv * sizeof(float));
            fread(m->vocab_scores, 4, nv, f);
            if ((int)arr_count > nv) fseek(f, (long)((arr_count - nv) * 4), SEEK_CUR);
        } else if (strcmp(key, "tokenizer.ggml.token_type") == 0 && vtype == GGUF_TYPE_ARRAY) {
            uint32_t arr_type;
            uint64_t arr_count;
            fread(&arr_type, 4, 1, f);
            fread(&arr_count, 8, 1, f);
            int nv = (int)(arr_count < MAX_VOCAB ? arr_count : MAX_VOCAB);
            m->vocab_types = (int*)calloc(nv, sizeof(int));
            if (arr_type == GGUF_TYPE_INT32 || arr_type == GGUF_TYPE_UINT32) {
                fread(m->vocab_types, 4, nv, f);
                if ((int)arr_count > nv) fseek(f, (long)((arr_count - nv) * 4), SEEK_CUR);
            } else {
                for (uint64_t j = 0; j < arr_count; j++) gguf_skip_value(f, arr_type);
            }
        } else {
            gguf_skip_value(f, vtype);
        }
    }

    if (m->n_head_kv == 0) m->n_head_kv = m->n_head;

    /* Parse tensor info */
    m->tensor_count = (int)(n_tensors < MAX_TENSORS ? n_tensors : MAX_TENSORS);
    for (int i = 0; i < m->tensor_count; i++) {
        GGUFTensor* t = &m->tensors[i];
        gguf_read_string(f, t->name, 256);
        uint32_t ndims;
        fread(&ndims, 4, 1, f);
        t->n_dims = (int)ndims;
        t->n_elements = 1;
        for (int d = 0; d < t->n_dims; d++) {
            uint64_t dim;
            fread(&dim, 8, 1, f);
            t->dims[d] = (int64_t)dim;
            t->n_elements *= (int64_t)dim;
        }
        uint32_t dtype;
        fread(&dtype, 4, 1, f);
        t->type = (int)dtype;
        uint64_t off;
        fread(&off, 8, 1, f);
        t->offset = (int64_t)off;
        int64_t bs = gguf_type_block_size(t->type);
        int64_t bc = gguf_type_block_count(t->type);
        if (bs > 0 && bc > 0) {
            t->byte_size = (t->n_elements / bc) * bs;
        } else {
            t->byte_size = t->n_elements * 4;
        }
    }
    /* Skip remaining tensors if > MAX_TENSORS */
    for (int64_t i = m->tensor_count; i < (int64_t)n_tensors; i++) {
        char skip_name[256];
        gguf_read_string(f, skip_name, 256);
        uint32_t ndims;
        fread(&ndims, 4, 1, f);
        fseek(f, (long)(ndims * 8 + 4 + 8), SEEK_CUR);
    }

    /* Align to 32 bytes and load tensor data */
    long pos = ftell(f);
    long aligned = (pos + 31) & ~31L;
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    m->data_size = file_size - aligned;

    if (m->data_size > 0) {
        m->data = (uint8_t*)malloc(m->data_size);
        if (m->data) {
            fseek(f, aligned, SEEK_SET);
            fread(m->data, 1, m->data_size, f);
        }
    }
    m->tensor_data_start = m->data;

    /* Allocate KV cache */
    int head_dim = m->n_embd / (m->n_head > 0 ? m->n_head : 1);
    int kv_dim = head_dim * m->n_head_kv;
    int cache_size = m->n_layer * m->n_ctx * kv_dim;
    if (cache_size > 0 && cache_size < 500000000) {
        m->key_cache = (float*)calloc(cache_size, sizeof(float));
        m->value_cache = (float*)calloc(cache_size, sizeof(float));
    }
    m->kv_len = 0;

    /* Scratch buffer */
    int scratch = m->n_embd * 4 + (m->n_ff > 0 ? m->n_ff * 2 : m->n_embd * 8);
    m->scratch_size = scratch;
    m->scratch = (float*)malloc(scratch * sizeof(float));

    fclose(f);
    m->file = NULL;
    return m;
}

static void gguf_free(LLMModel* m) {
    if (!m) return;
    if (m->vocab) {
        for (int i = 0; i < m->n_vocab; i++) free(m->vocab[i]);
        free(m->vocab);
    }
    free(m->vocab_scores);
    free(m->vocab_types);
    free(m->data);
    free(m->key_cache);
    free(m->value_cache);
    free(m->scratch);
    if (m->file) fclose(m->file);
    free(m);
}

/* ===== Tokenizer (BPE) ===== */

typedef struct { int id; float score; int len; } TokenMatch;

static int tokenize_bpe(LLMModel* m, const char* text, int* tokens, int max_tokens) {
    int text_len = (int)strlen(text);
    if (text_len == 0 || !m->vocab || m->n_vocab == 0) return 0;

    int n = 0;
    int pos = 0;

    while (pos < text_len && n < max_tokens) {
        TokenMatch best = {-1, -1e9f, 0};

        for (int v = 0; v < m->n_vocab; v++) {
            const char* tok = m->vocab[v];
            int tlen = (int)strlen(tok);
            if (tlen == 0 || tlen > text_len - pos) continue;
            if (memcmp(text + pos, tok, tlen) != 0) continue;
            float sc = m->vocab_scores ? m->vocab_scores[v] : (float)tlen;
            if (tlen > best.len || (tlen == best.len && sc > best.score)) {
                best.id = v;
                best.score = sc;
                best.len = tlen;
            }
        }

        if (best.id >= 0) {
            tokens[n++] = best.id;
            pos += best.len;
        } else {
            /* Fallback: encode as byte tokens */
            unsigned char byte = (unsigned char)text[pos];
            char byte_tok[16];
            snprintf(byte_tok, sizeof(byte_tok), "<0x%02X>", byte);
            int found = -1;
            for (int v = 0; v < m->n_vocab; v++) {
                if (strcmp(m->vocab[v], byte_tok) == 0) { found = v; break; }
            }
            tokens[n++] = found >= 0 ? found : 0;
            pos++;
        }
    }
    return n;
}

static const char* detokenize_one(LLMModel* m, int token) {
    if (token >= 0 && token < m->n_vocab && m->vocab) return m->vocab[token];
    return "";
}

/* ===== Math Utilities ===== */

static void rmsnorm(float* out, const float* x, const float* w, int n, float eps) {
    float ss = 0.0f;
    for (int i = 0; i < n; i++) ss += x[i] * x[i];
    ss = 1.0f / sqrtf(ss / n + eps);
    for (int i = 0; i < n; i++) out[i] = x[i] * ss * w[i];
}

static void softmax(float* x, int n) {
    float max_val = x[0];
    for (int i = 1; i < n; i++) if (x[i] > max_val) max_val = x[i];
    float sum = 0.0f;
    for (int i = 0; i < n; i++) { x[i] = expf(x[i] - max_val); sum += x[i]; }
    for (int i = 0; i < n; i++) x[i] /= sum;
}

static void matmul(float* out, const float* x, const float* w, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        float val = 0.0f;
        const float* row = w + r * cols;
        for (int c = 0; c < cols; c++) val += row[c] * x[c];
        out[r] = val;
    }
}

static float silu(float x) {
    return x / (1.0f + expf(-x));
}

static void rope(float* q, float* k, int head_dim, int pos, float freq_base) {
    for (int i = 0; i < head_dim; i += 2) {
        float freq = 1.0f / powf(freq_base, (float)i / (float)head_dim);
        float angle = (float)pos * freq;
        float cos_a = cosf(angle), sin_a = sinf(angle);
        float q0 = q[i], q1 = q[i + 1];
        q[i]   = q0 * cos_a - q1 * sin_a;
        q[i+1] = q0 * sin_a + q1 * cos_a;
        float k0 = k[i], k1 = k[i + 1];
        k[i]   = k0 * cos_a - k1 * sin_a;
        k[i+1] = k0 * sin_a + k1 * cos_a;
    }
}

/* ===== Transformer Forward Pass ===== */

static float* llm_forward(LLMModel* m, int token, int pos) {
    int dim = m->n_embd;
    int n_heads = m->n_head;
    int n_kv_heads = m->n_head_kv;
    int head_dim = dim / n_heads;
    int kv_dim = head_dim * n_kv_heads;
    int n_layers = m->n_layer;
    float eps = m->rms_norm_eps;

    if (dim == 0 || n_heads == 0 || n_layers == 0) return NULL;

    /* Embedding lookup */
    float* x = (float*)malloc(dim * sizeof(float));
    float* emb = get_tensor_data(m, "token_embd.weight", 0);
    if (!emb) { free(x); return NULL; }
    memcpy(x, emb + (int64_t)token * dim, dim * sizeof(float));
    free(emb);

    float* xb = (float*)malloc(dim * sizeof(float));
    float* q  = (float*)malloc(dim * sizeof(float));
    float* att = (float*)malloc(n_heads * m->n_ctx * sizeof(float));

    for (int l = 0; l < n_layers; l++) {
        char tname[256];

        /* Attention norm */
        snprintf(tname, 256, "blk.%d.attn_norm.weight", l);
        float* attn_norm_w = get_tensor_data(m, tname, dim);
        if (!attn_norm_w) { attn_norm_w = (float*)calloc(dim, sizeof(float)); for(int i=0;i<dim;i++) attn_norm_w[i]=1.0f; }
        rmsnorm(xb, x, attn_norm_w, dim, eps);
        free(attn_norm_w);

        /* QKV projections */
        snprintf(tname, 256, "blk.%d.attn_q.weight", l);
        float* wq = get_tensor_data(m, tname, 0);
        snprintf(tname, 256, "blk.%d.attn_k.weight", l);
        float* wk = get_tensor_data(m, tname, 0);
        snprintf(tname, 256, "blk.%d.attn_v.weight", l);
        float* wv = get_tensor_data(m, tname, 0);

        float* k_out = (float*)calloc(kv_dim, sizeof(float));
        float* v_out = (float*)calloc(kv_dim, sizeof(float));

        if (wq) matmul(q, xb, wq, dim, dim);
        if (wk) matmul(k_out, xb, wk, kv_dim, dim);
        if (wv) matmul(v_out, xb, wv, kv_dim, dim);

        /* RoPE */
        int kv_heads_per_group = n_kv_heads;
        for (int h = 0; h < n_heads; h++) {
            int kv_h = h * kv_heads_per_group / n_heads;
            rope(q + h * head_dim, k_out + kv_h * head_dim, head_dim, pos, m->rope_freq_base);
        }

        /* Store KV in cache */
        if (m->key_cache && m->value_cache) {
            int kv_offset = l * m->n_ctx * kv_dim + pos * kv_dim;
            memcpy(m->key_cache + kv_offset, k_out, kv_dim * sizeof(float));
            memcpy(m->value_cache + kv_offset, v_out, kv_dim * sizeof(float));
        }

        /* Multi-head attention */
        float scale = 1.0f / sqrtf((float)head_dim);
        memset(xb, 0, dim * sizeof(float));

        for (int h = 0; h < n_heads; h++) {
            int kv_h = h * kv_heads_per_group / n_heads;
            float* q_h = q + h * head_dim;
            float* att_h = att + h * m->n_ctx;

            for (int t = 0; t <= pos; t++) {
                float* k_t = m->key_cache + l * m->n_ctx * kv_dim + t * kv_dim + kv_h * head_dim;
                float score = 0.0f;
                for (int d = 0; d < head_dim; d++) score += q_h[d] * k_t[d];
                att_h[t] = score * scale;
            }
            softmax(att_h, pos + 1);

            float* xb_h = xb + h * head_dim;
            for (int t = 0; t <= pos; t++) {
                float* v_t = m->value_cache + l * m->n_ctx * kv_dim + t * kv_dim + kv_h * head_dim;
                float a = att_h[t];
                for (int d = 0; d < head_dim; d++) xb_h[d] += a * v_t[d];
            }
        }

        /* Output projection */
        snprintf(tname, 256, "blk.%d.attn_output.weight", l);
        float* wo = get_tensor_data(m, tname, 0);
        float* attn_out = (float*)malloc(dim * sizeof(float));
        if (wo) matmul(attn_out, xb, wo, dim, dim);
        else memcpy(attn_out, xb, dim * sizeof(float));

        /* Residual */
        for (int i = 0; i < dim; i++) x[i] += attn_out[i];

        /* FFN norm */
        snprintf(tname, 256, "blk.%d.ffn_norm.weight", l);
        float* ffn_norm_w = get_tensor_data(m, tname, dim);
        if (!ffn_norm_w) { ffn_norm_w = (float*)calloc(dim, sizeof(float)); for(int i=0;i<dim;i++) ffn_norm_w[i]=1.0f; }
        rmsnorm(xb, x, ffn_norm_w, dim, eps);
        free(ffn_norm_w);

        /* FFN: SwiGLU -- gate = silu(W_gate * xb), up = W_up * xb, out = W_down * (gate * up) */
        int ff_dim = m->n_ff > 0 ? m->n_ff : dim * 4;
        snprintf(tname, 256, "blk.%d.ffn_gate.weight", l);
        float* w_gate = get_tensor_data(m, tname, 0);
        snprintf(tname, 256, "blk.%d.ffn_up.weight", l);
        float* w_up = get_tensor_data(m, tname, 0);
        snprintf(tname, 256, "blk.%d.ffn_down.weight", l);
        float* w_down = get_tensor_data(m, tname, 0);

        float* ff_gate = (float*)malloc(ff_dim * sizeof(float));
        float* ff_up = (float*)malloc(ff_dim * sizeof(float));
        float* ff_out = (float*)malloc(dim * sizeof(float));

        if (w_gate) matmul(ff_gate, xb, w_gate, ff_dim, dim);
        if (w_up) matmul(ff_up, xb, w_up, ff_dim, dim);

        for (int i = 0; i < ff_dim; i++) ff_gate[i] = silu(ff_gate[i]) * ff_up[i];

        if (w_down) matmul(ff_out, ff_gate, w_down, dim, ff_dim);
        else memset(ff_out, 0, dim * sizeof(float));

        /* Residual */
        for (int i = 0; i < dim; i++) x[i] += ff_out[i];

        free(wq); free(wk); free(wv); free(wo);
        free(k_out); free(v_out); free(attn_out);
        free(w_gate); free(w_up); free(w_down);
        free(ff_gate); free(ff_up); free(ff_out);
    }

    /* Final norm */
    float* final_norm = get_tensor_data(m, "output_norm.weight", dim);
    if (!final_norm) final_norm = get_tensor_data(m, "model.norm.weight", dim);
    if (final_norm) {
        rmsnorm(xb, x, final_norm, dim, eps);
        memcpy(x, xb, dim * sizeof(float));
        free(final_norm);
    }

    /* Logits */
    float* logits = (float*)malloc(m->n_vocab * sizeof(float));
    float* lm_head = get_tensor_data(m, "output.weight", 0);
    if (!lm_head) lm_head = get_tensor_data(m, "lm_head.weight", 0);
    if (lm_head) {
        matmul(logits, x, lm_head, m->n_vocab, dim);
        free(lm_head);
    } else {
        memset(logits, 0, m->n_vocab * sizeof(float));
    }

    free(x); free(xb); free(q); free(att);
    return logits;
}

/* ===== Sampling ===== */

static int sample_token(float* logits, int n_vocab, float temperature, float top_p) {
    if (temperature <= 0.0f) {
        int best = 0;
        for (int i = 1; i < n_vocab; i++) {
            if (logits[i] > logits[best]) best = i;
        }
        return best;
    }

    for (int i = 0; i < n_vocab; i++) logits[i] /= temperature;
    softmax(logits, n_vocab);

    /* Top-p (nucleus) sampling */
    if (top_p < 1.0f) {
        typedef struct { float p; int id; } ProbIdx;
        ProbIdx* sorted = (ProbIdx*)malloc(n_vocab * sizeof(ProbIdx));
        for (int i = 0; i < n_vocab; i++) { sorted[i].p = logits[i]; sorted[i].id = i; }
        for (int i = 0; i < n_vocab - 1; i++) {
            for (int j = i + 1; j < n_vocab; j++) {
                if (sorted[j].p > sorted[i].p) {
                    ProbIdx tmp = sorted[i]; sorted[i] = sorted[j]; sorted[j] = tmp;
                }
            }
        }
        float cumsum = 0.0f;
        int cutoff = n_vocab;
        for (int i = 0; i < n_vocab; i++) {
            cumsum += sorted[i].p;
            if (cumsum >= top_p) { cutoff = i + 1; break; }
        }
        float sum = 0.0f;
        for (int i = 0; i < cutoff; i++) sum += sorted[i].p;
        float r = ((float)rand() / (float)RAND_MAX) * sum;
        float acc = 0.0f;
        for (int i = 0; i < cutoff; i++) {
            acc += sorted[i].p;
            if (acc >= r) { int id = sorted[i].id; free(sorted); return id; }
        }
        int id = sorted[0].id;
        free(sorted);
        return id;
    }

    /* Multinomial */
    float r = (float)rand() / (float)RAND_MAX;
    float cdf = 0.0f;
    for (int i = 0; i < n_vocab; i++) {
        cdf += logits[i];
        if (cdf >= r) return i;
    }
    return n_vocab - 1;
}

/* ===== Generate ===== */

static char* llm_generate(LLMModel* m, const char* prompt, int max_tokens, float temperature, float top_p) {
    if (!m || !prompt) return NULL;

    int* tokens = (int*)malloc(MAX_SEQ * sizeof(int));
    int n_prompt = tokenize_bpe(m, prompt, tokens, MAX_SEQ / 2);
    if (n_prompt == 0) { free(tokens); return NULL; }

    m->kv_len = 0;

    int cap = 4096, len = 0;
    char* output = (char*)malloc(cap);
    output[0] = '\0';

    int total = n_prompt + max_tokens;
    if (total > MAX_SEQ) total = MAX_SEQ;
    if (total > m->n_ctx) total = m->n_ctx;

    int next_token = tokens[0];
    for (int pos = 0; pos < total; pos++) {
        float* logits = llm_forward(m, next_token, pos);
        if (!logits) break;

        if (pos < n_prompt - 1) {
            next_token = tokens[pos + 1];
        } else {
            next_token = sample_token(logits, m->n_vocab, temperature, top_p);
            const char* piece = detokenize_one(m, next_token);
            int plen = (int)strlen(piece);
            if (len + plen + 1 > cap) { cap *= 2; output = (char*)realloc(output, cap); }
            memcpy(output + len, piece, plen);
            len += plen;
            output[len] = '\0';

            /* Stop on EOS */
            if (next_token <= 2) break;
        }
        free(logits);
    }

    free(tokens);
    return output;
}

/* ===== Embedding ===== */

static float* llm_embed(LLMModel* m, const char* text, int* out_dim) {
    if (!m || !text) return NULL;

    int* tokens = (int*)malloc(MAX_SEQ * sizeof(int));
    int n = tokenize_bpe(m, text, tokens, MAX_SEQ);
    if (n == 0) { free(tokens); return NULL; }

    int dim = m->n_embd;
    float* sum = (float*)calloc(dim, sizeof(float));

    m->kv_len = 0;
    for (int i = 0; i < n; i++) {
        float* logits = llm_forward(m, tokens[i], i);
        free(logits);
    }

    /* Mean pooling over token embeddings */
    float* emb_w = get_tensor_data(m, "token_embd.weight", 0);
    if (emb_w) {
        for (int i = 0; i < n; i++) {
            float* tok_emb = emb_w + (int64_t)tokens[i] * dim;
            for (int d = 0; d < dim; d++) sum[d] += tok_emb[d];
        }
        free(emb_w);
    }

    float norm = 0.0f;
    for (int d = 0; d < dim; d++) { sum[d] /= n; norm += sum[d] * sum[d]; }
    norm = sqrtf(norm);
    if (norm > 0.0f) for (int d = 0; d < dim; d++) sum[d] /= norm;

    free(tokens);
    *out_dim = dim;
    return sum;
}

/* ===== AValue API ===== */

#define MAX_LLM_HANDLES 16
static LLMModel* llm_handles[MAX_LLM_HANDLES];

AValue a_llm_load(AValue path_val) {
    if (path_val.tag != TAG_STRING)
        return a_err(a_string("llm.load: expected string path"));

    int slot = -1;
    for (int i = 0; i < MAX_LLM_HANDLES; i++) {
        if (!llm_handles[i]) { slot = i; break; }
    }
    if (slot < 0) return a_err(a_string("llm.load: too many models loaded (max 16)"));

    LLMModel* m = gguf_load(path_val.sval->data);
    if (!m) return a_err(a_string("llm.load: failed to load GGUF model"));

    llm_handles[slot] = m;
    return a_ok(a_int(slot));
}

AValue a_llm_generate(AValue handle, AValue prompt, AValue opts) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.generate: expected int handle"));
    if (prompt.tag != TAG_STRING) return a_err(a_string("llm.generate: expected string prompt"));

    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.generate: invalid model handle"));

    LLMModel* m = llm_handles[slot];

    int max_tokens = 256;
    float temperature = 0.7f;
    float top_p = 0.9f;

    if (opts.tag == TAG_MAP) {
        AValue mt = a_map_get(opts, a_string("max_tokens"));
        if (mt.tag == TAG_INT) max_tokens = (int)mt.ival;
        AValue temp = a_map_get(opts, a_string("temperature"));
        if (temp.tag == TAG_FLOAT) temperature = (float)temp.fval;
        if (temp.tag == TAG_INT) temperature = (float)temp.ival;
        AValue tp = a_map_get(opts, a_string("top_p"));
        if (tp.tag == TAG_FLOAT) top_p = (float)tp.fval;
    }

    srand((unsigned)time(NULL));
    char* result = llm_generate(m, prompt.sval->data, max_tokens, temperature, top_p);
    if (!result) return a_err(a_string("llm.generate: inference failed"));

    AValue out = a_string(result);
    free(result);
    return a_ok(out);
}

AValue a_llm_embed(AValue handle, AValue text) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.embed: expected int handle"));
    if (text.tag != TAG_STRING) return a_err(a_string("llm.embed: expected string text"));

    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.embed: invalid model handle"));

    LLMModel* m = llm_handles[slot];
    int dim = 0;
    float* emb = llm_embed(m, text.sval->data, &dim);
    if (!emb) return a_err(a_string("llm.embed: embedding failed"));

    AValue arr = a_array_new(0);
    for (int i = 0; i < dim; i++) {
        arr = a_array_push(arr, a_float((double)emb[i]));
    }
    free(emb);
    return a_ok(arr);
}

AValue a_llm_unload(AValue handle) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.unload: expected int handle"));
    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.unload: invalid handle"));
    gguf_free(llm_handles[slot]);
    llm_handles[slot] = NULL;
    return a_void();
}

AValue a_llm_info(AValue handle) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.info: expected int handle"));
    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.info: invalid handle"));

    LLMModel* m = llm_handles[slot];
    AValue info = a_map_new(0);
    info = a_map_set(info, a_string("architecture"), a_string(m->arch));
    info = a_map_set(info, a_string("name"), a_string(m->model_name));
    info = a_map_set(info, a_string("vocab_size"), a_int(m->n_vocab));
    info = a_map_set(info, a_string("embedding_dim"), a_int(m->n_embd));
    info = a_map_set(info, a_string("layers"), a_int(m->n_layer));
    info = a_map_set(info, a_string("heads"), a_int(m->n_head));
    info = a_map_set(info, a_string("kv_heads"), a_int(m->n_head_kv));
    info = a_map_set(info, a_string("ff_dim"), a_int(m->n_ff));
    info = a_map_set(info, a_string("context_length"), a_int(m->n_ctx));
    info = a_map_set(info, a_string("tensor_count"), a_int(m->tensor_count));
    return a_ok(info);
}

AValue a_llm_tokenize(AValue handle, AValue text) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.tokenize: expected int handle"));
    if (text.tag != TAG_STRING) return a_err(a_string("llm.tokenize: expected string text"));

    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.tokenize: invalid handle"));

    LLMModel* m = llm_handles[slot];
    int* tokens = (int*)malloc(MAX_SEQ * sizeof(int));
    int n = tokenize_bpe(m, text.sval->data, tokens, MAX_SEQ);

    AValue arr = a_array_new(0);
    for (int i = 0; i < n; i++) {
        arr = a_array_push(arr, a_int(tokens[i]));
    }
    free(tokens);
    return a_ok(arr);
}

AValue a_llm_detokenize(AValue handle, AValue token_arr) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.detokenize: expected int handle"));
    if (token_arr.tag != TAG_ARRAY) return a_err(a_string("llm.detokenize: expected array of token ids"));

    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.detokenize: invalid handle"));

    LLMModel* m = llm_handles[slot];
    int cap = 1024, len = 0;
    char* buf = (char*)malloc(cap);
    buf[0] = '\0';

    for (int i = 0; i < token_arr.aval->len; i++) {
        AValue tid = token_arr.aval->items[i];
        if (tid.tag != TAG_INT) continue;
        const char* piece = detokenize_one(m, (int)tid.ival);
        int plen = (int)strlen(piece);
        if (len + plen + 1 > cap) { cap *= 2; buf = (char*)realloc(buf, cap); }
        memcpy(buf + len, piece, plen);
        len += plen;
        buf[len] = '\0';
    }

    AValue result = a_string_len(buf, len);
    free(buf);
    return a_ok(result);
}

AValue a_llm_vocab_size(AValue handle) {
    if (handle.tag != TAG_INT) return a_err(a_string("llm.vocab_size: expected int handle"));
    int slot = (int)handle.ival;
    if (slot < 0 || slot >= MAX_LLM_HANDLES || !llm_handles[slot])
        return a_err(a_string("llm.vocab_size: invalid handle"));
    return a_int(llm_handles[slot]->n_vocab);
}

#else /* WASM_BUILD */

AValue a_llm_load(AValue p) { return a_err(a_string("llm.load: not available in WASM")); }
AValue a_llm_generate(AValue h, AValue p, AValue o) { return a_err(a_string("llm.generate: not available in WASM")); }
AValue a_llm_embed(AValue h, AValue t) { return a_err(a_string("llm.embed: not available in WASM")); }
AValue a_llm_unload(AValue h) { return a_err(a_string("llm.unload: not available in WASM")); }
AValue a_llm_info(AValue h) { return a_err(a_string("llm.info: not available in WASM")); }
AValue a_llm_tokenize(AValue h, AValue t) { return a_err(a_string("llm.tokenize: not available in WASM")); }
AValue a_llm_detokenize(AValue h, AValue t) { return a_err(a_string("llm.detokenize: not available in WASM")); }
AValue a_llm_vocab_size(AValue h) { return a_err(a_string("llm.vocab_size: not available in WASM")); }

#endif /* !WASM_BUILD */
