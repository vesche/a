#include "runtime.h"
#include "miniz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int g_argc = 0;
char** g_argv = NULL;

jmp_buf a_try_stack[A_TRY_STACK_MAX];
AValue a_try_err;
int a_try_depth = 0;

/* --- Constructors --- */

AValue a_int(int64_t v) { return (AValue){.tag = TAG_INT, .ival = v}; }
AValue a_float(double v) { return (AValue){.tag = TAG_FLOAT, .fval = v}; }
AValue a_bool(int v) { return (AValue){.tag = TAG_BOOL, .bval = v != 0}; }
AValue a_void(void) { return (AValue){.tag = TAG_VOID}; }

AValue a_ptr(void* p) { return (AValue){.tag = TAG_PTR, .pval = p}; }
AValue a_ptr_null(void) { return (AValue){.tag = TAG_PTR, .pval = NULL}; }
AValue a_is_null(AValue v) { return a_bool(v.tag == TAG_PTR && v.pval == NULL); }

AValue a_string(const char* s) {
    int len = s ? (int)strlen(s) : 0;
    return a_string_len(s, len);
}

AValue a_string_len(const char* s, int len) {
    AString* as = malloc(sizeof(AString) + len + 1);
    as->rc = 1;
    as->len = len;
    if (s && len > 0) memcpy(as->data, s, len);
    as->data[len] = '\0';
    return (AValue){.tag = TAG_STRING, .sval = as};
}

/* --- Refcounting --- */

AValue a_retain(AValue v) {
    if (v.tag == TAG_STRING && v.sval) v.sval->rc++;
    else if (v.tag == TAG_ARRAY && v.aval) v.aval->rc++;
    else if (v.tag == TAG_MAP && v.mval) v.mval->rc++;
    else if (v.tag == TAG_CLOSURE && v.cval) v.cval->rc++;
    return v;
}

void a_release(AValue v) {
    if (v.tag == TAG_STRING && v.sval) {
        if (v.sval->rc <= 0) return;
        if (--v.sval->rc == 0) free(v.sval);
    } else if (v.tag == TAG_ARRAY && v.aval) {
        if (v.aval->rc <= 0) return;
        if (--v.aval->rc == 0) {
            for (int i = 0; i < v.aval->len; i++) a_release(v.aval->items[i]);
            free(v.aval->items);
            free(v.aval);
        }
    } else if (v.tag == TAG_CLOSURE && v.cval) {
        if (v.cval->rc <= 0) return;
        if (--v.cval->rc == 0) {
            a_release(v.cval->env);
            free(v.cval);
        }
    } else if (v.tag == TAG_MAP && v.mval) {
        if (v.mval->rc <= 0) return;
        if (--v.mval->rc == 0) {
            for (int i = 0; i < v.mval->len; i++) {
                free(v.mval->keys[i]);
                a_release(v.mval->vals[i]);
            }
            free(v.mval->keys);
            free(v.mval->vals);
            free(v.mval);
        }
    } else if (v.tag == TAG_RESULT && v.rval.inner) {
        a_release(*v.rval.inner);
        free(v.rval.inner);
    }
}

/* --- Truthiness --- */

int a_truthy(AValue v) {
    switch (v.tag) {
        case TAG_BOOL: return v.bval;
        case TAG_INT: return v.ival != 0;
        case TAG_VOID: return 0;
        case TAG_STRING: return v.sval && v.sval->len > 0;
        case TAG_ARRAY: return v.aval && v.aval->len > 0;
        default: return 1;
    }
}

int a_ilen(AValue v) {
    if (v.tag == TAG_ARRAY && v.aval) return v.aval->len;
    if (v.tag == TAG_STRING && v.sval) return v.sval->len;
    if (v.tag == TAG_MAP && v.mval) return v.mval->len;
    return 0;
}

AValue a_iterable(AValue v) {
    if (v.tag == TAG_MAP) return a_map_entries(v);
    return a_retain(v);
}

/* --- Arithmetic --- */

AValue a_add(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_int(a.ival + b.ival);
    if (a.tag == TAG_FLOAT && b.tag == TAG_FLOAT) return a_float(a.fval + b.fval);
    if (a.tag == TAG_INT && b.tag == TAG_FLOAT) return a_float((double)a.ival + b.fval);
    if (a.tag == TAG_FLOAT && b.tag == TAG_INT) return a_float(a.fval + (double)b.ival);
    if (a.tag == TAG_STRING && b.tag == TAG_STRING) return a_str_concat(a, b);
    return a_void();
}

AValue a_sub(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_int(a.ival - b.ival);
    if (a.tag == TAG_FLOAT && b.tag == TAG_FLOAT) return a_float(a.fval - b.fval);
    if (a.tag == TAG_INT && b.tag == TAG_FLOAT) return a_float((double)a.ival - b.fval);
    if (a.tag == TAG_FLOAT && b.tag == TAG_INT) return a_float(a.fval - (double)b.ival);
    return a_void();
}

AValue a_mul(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_int(a.ival * b.ival);
    if (a.tag == TAG_FLOAT && b.tag == TAG_FLOAT) return a_float(a.fval * b.fval);
    if (a.tag == TAG_INT && b.tag == TAG_FLOAT) return a_float((double)a.ival * b.fval);
    if (a.tag == TAG_FLOAT && b.tag == TAG_INT) return a_float(a.fval * (double)b.ival);
    return a_void();
}

AValue a_div(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) {
        if (b.ival == 0) { fprintf(stderr, "division by zero\n"); exit(1); }
        return a_int(a.ival / b.ival);
    }
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return a_float(fa / fb);
}

AValue a_mod(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_int(a.ival % b.ival);
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return a_float(fmod(fa, fb));
}

AValue a_neg(AValue a) {
    if (a.tag == TAG_INT) return a_int(-a.ival);
    if (a.tag == TAG_FLOAT) return a_float(-a.fval);
    return a_void();
}

/* --- Comparison --- */

static int val_eq(AValue a, AValue b) {
    if (a.tag != b.tag) {
        if ((a.tag == TAG_INT || a.tag == TAG_FLOAT) && (b.tag == TAG_INT || b.tag == TAG_FLOAT)) {
            double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
            double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
            return fa == fb;
        }
        return 0;
    }
    switch (a.tag) {
        case TAG_INT: return a.ival == b.ival;
        case TAG_FLOAT: return a.fval == b.fval;
        case TAG_BOOL: return a.bval == b.bval;
        case TAG_VOID: return 1;
        case TAG_STRING: return a.sval->len == b.sval->len && memcmp(a.sval->data, b.sval->data, a.sval->len) == 0;
        default: return 0;
    }
}

AValue a_eq(AValue a, AValue b) { return a_bool(val_eq(a, b)); }
AValue a_neq(AValue a, AValue b) { return a_bool(!val_eq(a, b)); }

AValue a_lt(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_bool(a.ival < b.ival);
    if (a.tag == TAG_STRING && b.tag == TAG_STRING)
        return a_bool(strcmp(a.sval->data, b.sval->data) < 0);
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return a_bool(fa < fb);
}
AValue a_gt(AValue a, AValue b) { return a_lt(b, a); }
AValue a_lteq(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_bool(a.ival <= b.ival);
    if (a.tag == TAG_STRING && b.tag == TAG_STRING)
        return a_bool(strcmp(a.sval->data, b.sval->data) <= 0);
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return a_bool(fa <= fb);
}
AValue a_gteq(AValue a, AValue b) { return a_lteq(b, a); }
AValue a_not(AValue a) { return a_bool(!a_truthy(a)); }
AValue a_and(AValue a, AValue b) { return a_truthy(a) ? b : a; }
AValue a_or(AValue a, AValue b) { return a_truthy(a) ? a : b; }

/* --- Strings --- */

static void val_to_buf(AValue v, char* buf, int cap) {
    switch (v.tag) {
        case TAG_INT: snprintf(buf, cap, "%lld", (long long)v.ival); break;
        case TAG_FLOAT: snprintf(buf, cap, "%g", v.fval); break;
        case TAG_BOOL: snprintf(buf, cap, "%s", v.bval ? "true" : "false"); break;
        case TAG_VOID: snprintf(buf, cap, "void"); break;
        case TAG_STRING: snprintf(buf, cap, "%.*s", v.sval->len, v.sval->data); break;
        case TAG_ARRAY: {
            int pos = 0;
            pos += snprintf(buf + pos, cap - pos, "[");
            for (int i = 0; i < v.aval->len && pos < cap - 5; i++) {
                if (i > 0) pos += snprintf(buf + pos, cap - pos, ", ");
                char tmp[256];
                val_to_buf(v.aval->items[i], tmp, 256);
                pos += snprintf(buf + pos, cap - pos, "%s", tmp);
            }
            snprintf(buf + pos, cap - pos, "]");
            break;
        }
        case TAG_MAP: {
            int pos = 0;
            pos += snprintf(buf + pos, cap - pos, "#{");
            for (int i = 0; i < v.mval->len && pos < cap - 10; i++) {
                if (i > 0) pos += snprintf(buf + pos, cap - pos, ", ");
                char tmp[256];
                val_to_buf(v.mval->vals[i], tmp, 256);
                pos += snprintf(buf + pos, cap - pos, "\"%s\": %s", v.mval->keys[i], tmp);
            }
            snprintf(buf + pos, cap - pos, "}");
            break;
        }
        case TAG_RESULT: {
            char tmp[256];
            val_to_buf(*v.rval.inner, tmp, 256);
            if (v.rval.is_ok) snprintf(buf, cap, "Ok(%s)", tmp);
            else snprintf(buf, cap, "Err(%s)", tmp);
            break;
        }
        case TAG_CLOSURE: snprintf(buf, cap, "<closure>"); break;
        case TAG_PTR: snprintf(buf, cap, "<ptr:%p>", v.pval); break;
        default: snprintf(buf, cap, "<value>"); break;
    }
}

AValue a_to_str(AValue v) {
    if (v.tag == TAG_STRING) return a_retain(v);
    char buf[4096];
    val_to_buf(v, buf, 4096);
    return a_string(buf);
}

AValue a_str_concat(AValue a, AValue b) {
    AValue sa = a_to_str(a);
    AValue sb = a_to_str(b);
    int newlen = sa.sval->len + sb.sval->len;
    AString* ns = malloc(sizeof(AString) + newlen + 1);
    ns->rc = 1;
    ns->len = newlen;
    memcpy(ns->data, sa.sval->data, sa.sval->len);
    memcpy(ns->data + sa.sval->len, sb.sval->data, sb.sval->len);
    ns->data[newlen] = '\0';
    a_release(sa);
    a_release(sb);
    return (AValue){.tag = TAG_STRING, .sval = ns};
}

AValue a_concat_n(int n, ...) {
    va_list ap;
    va_start(ap, n);
    AValue parts[64];
    int total_len = 0;
    int count = (n < 64) ? n : 64;
    for (int i = 0; i < count; i++) {
        parts[i] = a_to_str(va_arg(ap, AValue));
        total_len += parts[i].sval->len;
    }
    va_end(ap);
    AString* ns = malloc(sizeof(AString) + total_len + 1);
    ns->rc = 1;
    ns->len = total_len;
    int pos = 0;
    for (int i = 0; i < count; i++) {
        memcpy(ns->data + pos, parts[i].sval->data, parts[i].sval->len);
        pos += parts[i].sval->len;
    }
    ns->data[total_len] = '\0';
    for (int i = 0; i < count; i++) a_release(parts[i]);
    return (AValue){.tag = TAG_STRING, .sval = ns};
}

AValue a_len(AValue v) {
    if (v.tag == TAG_STRING) return a_int(v.sval->len);
    if (v.tag == TAG_ARRAY) return a_int(v.aval->len);
    if (v.tag == TAG_MAP) return a_int(v.mval->len);
    return a_int(0);
}

AValue a_str_split(AValue s, AValue delim) {
    if (s.tag != TAG_STRING || delim.tag != TAG_STRING) return a_array_new(0);
    const char* src = s.sval->data;
    int slen = s.sval->len;
    const char* d = delim.sval->data;
    int dlen = delim.sval->len;
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = 0; arr->cap = 8;
    arr->items = malloc(sizeof(AValue) * arr->cap);
    int start = 0;
    for (int i = 0; i <= slen - dlen; i++) {
        if (dlen > 0 && memcmp(src + i, d, dlen) == 0) {
            if (arr->len >= arr->cap) { arr->cap *= 2; arr->items = realloc(arr->items, sizeof(AValue) * arr->cap); }
            arr->items[arr->len++] = a_string_len(src + start, i - start);
            i += dlen - 1;
            start = i + 1;
        }
    }
    if (arr->len >= arr->cap) { arr->cap *= 2; arr->items = realloc(arr->items, sizeof(AValue) * arr->cap); }
    arr->items[arr->len++] = a_string_len(src + start, slen - start);
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_str_contains(AValue s, AValue sub) {
    if (s.tag != TAG_STRING || sub.tag != TAG_STRING) return a_bool(0);
    return a_bool(strstr(s.sval->data, sub.sval->data) != NULL);
}

AValue a_str_replace(AValue s, AValue from, AValue to) {
    if (s.tag != TAG_STRING || from.tag != TAG_STRING || to.tag != TAG_STRING) return s;
    const char* src = s.sval->data;
    const char* f = from.sval->data;
    const char* t = to.sval->data;
    int flen = from.sval->len, tlen = to.sval->len;
    if (flen == 0) return a_retain(s);
    int cap = s.sval->len * 2 + 64;
    char* buf = malloc(cap);
    int pos = 0;
    for (int i = 0; i < s.sval->len; ) {
        if (i <= s.sval->len - flen && memcmp(src + i, f, flen) == 0) {
            if (pos + tlen >= cap) { cap = cap * 2 + tlen; buf = realloc(buf, cap); }
            memcpy(buf + pos, t, tlen); pos += tlen; i += flen;
        } else {
            if (pos + 1 >= cap) { cap *= 2; buf = realloc(buf, cap); }
            buf[pos++] = src[i++];
        }
    }
    AValue result = a_string_len(buf, pos);
    free(buf);
    return result;
}

AValue a_str_trim(AValue s) {
    if (s.tag != TAG_STRING) return s;
    const char* d = s.sval->data;
    int start = 0, end = s.sval->len;
    while (start < end && isspace((unsigned char)d[start])) start++;
    while (end > start && isspace((unsigned char)d[end - 1])) end--;
    return a_string_len(d + start, end - start);
}

AValue a_str_upper(AValue s) {
    if (s.tag != TAG_STRING) return s;
    AString* ns = malloc(sizeof(AString) + s.sval->len + 1);
    ns->rc = 1; ns->len = s.sval->len;
    for (int i = 0; i < ns->len; i++) ns->data[i] = toupper((unsigned char)s.sval->data[i]);
    ns->data[ns->len] = '\0';
    return (AValue){.tag = TAG_STRING, .sval = ns};
}

AValue a_str_lower(AValue s) {
    if (s.tag != TAG_STRING) return s;
    AString* ns = malloc(sizeof(AString) + s.sval->len + 1);
    ns->rc = 1; ns->len = s.sval->len;
    for (int i = 0; i < ns->len; i++) ns->data[i] = tolower((unsigned char)s.sval->data[i]);
    ns->data[ns->len] = '\0';
    return (AValue){.tag = TAG_STRING, .sval = ns};
}

AValue a_str_join(AValue arr, AValue sep) {
    if (arr.tag != TAG_ARRAY || sep.tag != TAG_STRING) return a_string("");
    int count = arr.aval->len;
    AValue* parts = malloc(sizeof(AValue) * (count > 0 ? count : 1));
    int total = 0;
    for (int i = 0; i < count; i++) {
        parts[i] = a_to_str(arr.aval->items[i]);
        total += parts[i].sval->len;
        if (i > 0) total += sep.sval->len;
    }
    AString* ns = malloc(sizeof(AString) + total + 1);
    ns->rc = 1; ns->len = total;
    int pos = 0;
    for (int i = 0; i < count; i++) {
        if (i > 0) { memcpy(ns->data + pos, sep.sval->data, sep.sval->len); pos += sep.sval->len; }
        memcpy(ns->data + pos, parts[i].sval->data, parts[i].sval->len); pos += parts[i].sval->len;
    }
    ns->data[total] = '\0';
    for (int i = 0; i < count; i++) a_release(parts[i]);
    free(parts);
    return (AValue){.tag = TAG_STRING, .sval = ns};
}

AValue a_str_chars(AValue s) {
    if (s.tag != TAG_STRING) return a_array_new(0);
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = s.sval->len; arr->cap = s.sval->len;
    arr->items = malloc(sizeof(AValue) * (arr->cap > 0 ? arr->cap : 1));
    for (int i = 0; i < s.sval->len; i++)
        arr->items[i] = a_string_len(s.sval->data + i, 1);
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_str_slice(AValue s, AValue start, AValue end) {
    if (s.tag != TAG_STRING) return a_string("");
    int st = (start.tag == TAG_INT) ? (int)start.ival : 0;
    int en = (end.tag == TAG_INT) ? (int)end.ival : s.sval->len;
    if (st < 0) st = 0;
    if (en > s.sval->len) en = s.sval->len;
    if (st >= en) return a_string("");
    return a_string_len(s.sval->data + st, en - st);
}

AValue a_str_starts_with(AValue s, AValue pre) {
    if (s.tag != TAG_STRING || pre.tag != TAG_STRING) return a_bool(0);
    if (pre.sval->len > s.sval->len) return a_bool(0);
    return a_bool(memcmp(s.sval->data, pre.sval->data, pre.sval->len) == 0);
}

AValue a_str_ends_with(AValue s, AValue suf) {
    if (s.tag != TAG_STRING || suf.tag != TAG_STRING) return a_bool(0);
    if (suf.sval->len > s.sval->len) return a_bool(0);
    return a_bool(memcmp(s.sval->data + s.sval->len - suf.sval->len, suf.sval->data, suf.sval->len) == 0);
}

AValue a_str_find(AValue s, AValue needle) {
    if (s.tag != TAG_STRING || needle.tag != TAG_STRING) return a_int(-1);
    char* p = strstr(s.sval->data, needle.sval->data);
    if (!p) return a_int(-1);
    return a_int((int)(p - s.sval->data));
}

AValue a_str_count(AValue s, AValue needle) {
    if (s.tag != TAG_STRING || needle.tag != TAG_STRING) return a_int(0);
    int count = 0;
    const char* p = s.sval->data;
    int nlen = needle.sval->len;
    if (nlen == 0) return a_int(0);
    while ((p = strstr(p, needle.sval->data)) != NULL) { count++; p += nlen; }
    return a_int(count);
}

/* --- Arrays --- */

AValue a_array_new(int n, ...) {
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = n; arr->cap = n > 0 ? n : 1;
    arr->items = malloc(sizeof(AValue) * arr->cap);
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) arr->items[i] = a_retain(va_arg(ap, AValue));
    va_end(ap);
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_array_get(AValue arr, AValue idx) {
    if (arr.tag == TAG_MAP && idx.tag == TAG_STRING) {
        return a_map_get(arr, idx);
    }
    if (idx.tag != TAG_INT) return a_void();
    int i = (int)idx.ival;
    if (arr.tag == TAG_STRING) {
        if (i < 0 || i >= arr.sval->len) return a_void();
        return a_string_len(arr.sval->data + i, 1);
    }
    if (arr.tag != TAG_ARRAY) return a_void();
    if (i < 0 || i >= arr.aval->len) return a_void();
    return a_retain(arr.aval->items[i]);
}

AValue a_index_set(AValue coll, AValue idx, AValue val) {
    if (coll.tag == TAG_MAP) return a_map_set(coll, idx, val);
    if (coll.tag != TAG_ARRAY || idx.tag != TAG_INT) return coll;
    int i = (int)idx.ival;
    int n = coll.aval->len;
    if (i < 0 || i >= n) return coll;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * n);
    for (int j = 0; j < n; j++) {
        na->items[j] = (j == i) ? a_retain(val) : a_retain(coll.aval->items[j]);
    }
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

AValue a_array_push(AValue arr, AValue val) {
    int olen = (arr.tag == TAG_ARRAY) ? arr.aval->len : 0;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = olen + 1; na->cap = olen + 1;
    na->items = malloc(sizeof(AValue) * na->cap);
    if (arr.tag == TAG_ARRAY)
        for (int i = 0; i < olen; i++) na->items[i] = a_retain(arr.aval->items[i]);
    na->items[olen] = a_retain(val);
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

AValue a_array_slice(AValue arr, AValue start, AValue end) {
    if (arr.tag != TAG_ARRAY) return a_array_new(0);
    int st = (start.tag == TAG_INT) ? (int)start.ival : 0;
    int en = (end.tag == TAG_INT) ? (int)end.ival : arr.aval->len;
    if (st < 0) st = 0; if (en > arr.aval->len) en = arr.aval->len;
    if (st >= en) return a_array_new(0);
    int n = en - st;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * n);
    for (int i = 0; i < n; i++) na->items[i] = a_retain(arr.aval->items[st + i]);
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

static int sort_cmp(const void* a, const void* b) {
    const AValue* va = a; const AValue* vb = b;
    if (va->tag == TAG_INT && vb->tag == TAG_INT) return (va->ival > vb->ival) - (va->ival < vb->ival);
    if (va->tag == TAG_STRING && vb->tag == TAG_STRING) return strcmp(va->sval->data, vb->sval->data);
    return 0;
}

AValue a_sort(AValue arr) {
    if (arr.tag != TAG_ARRAY) return arr;
    int n = arr.aval->len;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * (n > 0 ? n : 1));
    for (int i = 0; i < n; i++) na->items[i] = a_retain(arr.aval->items[i]);
    qsort(na->items, n, sizeof(AValue), sort_cmp);
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

AValue a_contains(AValue arr, AValue val) {
    if (arr.tag != TAG_ARRAY) return a_bool(0);
    for (int i = 0; i < arr.aval->len; i++)
        if (val_eq(arr.aval->items[i], val)) return a_bool(1);
    return a_bool(0);
}

AValue a_reverse_arr(AValue arr) {
    if (arr.tag != TAG_ARRAY) return arr;
    int n = arr.aval->len;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * (n > 0 ? n : 1));
    for (int i = 0; i < n; i++) na->items[i] = a_retain(arr.aval->items[n - 1 - i]);
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

AValue a_concat_arr(AValue a, AValue b) {
    if (a.tag != TAG_ARRAY || b.tag != TAG_ARRAY) return a_array_new(0);
    int n = a.aval->len + b.aval->len;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * (n > 0 ? n : 1));
    for (int i = 0; i < a.aval->len; i++) na->items[i] = a_retain(a.aval->items[i]);
    for (int i = 0; i < b.aval->len; i++) na->items[a.aval->len + i] = a_retain(b.aval->items[i]);
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

/* --- Maps --- */

AValue a_map_new(int n, ...) {
    AMap* m = malloc(sizeof(AMap));
    m->rc = 1; m->len = n; m->cap = n > 0 ? n : 1;
    m->keys = malloc(sizeof(char*) * m->cap);
    m->vals = malloc(sizeof(AValue) * m->cap);
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) {
        const char* k = va_arg(ap, const char*);
        m->keys[i] = strdup(k);
        m->vals[i] = a_retain(va_arg(ap, AValue));
    }
    va_end(ap);
    return (AValue){.tag = TAG_MAP, .mval = m};
}

static int map_find(AMap* m, const char* key) {
    for (int i = 0; i < m->len; i++)
        if (strcmp(m->keys[i], key) == 0) return i;
    return -1;
}

static const char* val_as_key(AValue key) {
    static char buf[256];
    if (key.tag == TAG_STRING) return key.sval->data;
    val_to_buf(key, buf, 256);
    return buf;
}

AValue a_map_get(AValue m, AValue key) {
    if (m.tag != TAG_MAP) return a_void();
    int idx = map_find(m.mval, val_as_key(key));
    return idx >= 0 ? a_retain(m.mval->vals[idx]) : a_void();
}

AValue a_map_set(AValue m, AValue key, AValue val) {
    AMap* old = (m.tag == TAG_MAP) ? m.mval : NULL;
    int olen = old ? old->len : 0;
    const char* k = val_as_key(key);
    int existing = old ? map_find(old, k) : -1;
    int newlen = (existing >= 0) ? olen : olen + 1;
    AMap* nm = malloc(sizeof(AMap));
    nm->rc = 1; nm->len = newlen; nm->cap = newlen;
    nm->keys = malloc(sizeof(char*) * nm->cap);
    nm->vals = malloc(sizeof(AValue) * nm->cap);
    for (int i = 0; i < olen; i++) {
        nm->keys[i] = strdup(old->keys[i]);
        nm->vals[i] = (i == existing) ? a_retain(val) : a_retain(old->vals[i]);
    }
    if (existing < 0) {
        nm->keys[olen] = strdup(k);
        nm->vals[olen] = a_retain(val);
    }
    return (AValue){.tag = TAG_MAP, .mval = nm};
}

AValue a_map_has(AValue m, AValue key) {
    if (m.tag != TAG_MAP) return a_bool(0);
    return a_bool(map_find(m.mval, val_as_key(key)) >= 0);
}

AValue a_map_keys(AValue m) {
    if (m.tag != TAG_MAP) return a_array_new(0);
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = m.mval->len; arr->cap = m.mval->len;
    arr->items = malloc(sizeof(AValue) * (arr->cap > 0 ? arr->cap : 1));
    for (int i = 0; i < m.mval->len; i++) arr->items[i] = a_string(m.mval->keys[i]);
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_map_values(AValue m) {
    if (m.tag != TAG_MAP) return a_array_new(0);
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = m.mval->len; arr->cap = m.mval->len;
    arr->items = malloc(sizeof(AValue) * (arr->cap > 0 ? arr->cap : 1));
    for (int i = 0; i < m.mval->len; i++) arr->items[i] = a_retain(m.mval->vals[i]);
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_map_merge(AValue a, AValue b) {
    if (a.tag != TAG_MAP || b.tag != TAG_MAP) return a;
    AValue result = a;
    for (int i = 0; i < b.mval->len; i++) {
        AValue key = a_string(b.mval->keys[i]);
        result = a_map_set(result, key, a_retain(b.mval->vals[i]));
        a_release(key);
    }
    return result;
}

AValue a_map_delete(AValue m, AValue key) {
    if (m.tag != TAG_MAP) return m;
    const char* k = val_as_key(key);
    int idx = map_find(m.mval, k);
    if (idx < 0) return m;
    AMap* nm = malloc(sizeof(AMap));
    nm->rc = 1; nm->len = m.mval->len - 1;
    nm->cap = nm->len > 0 ? nm->len : 1;
    nm->keys = malloc(sizeof(char*) * nm->cap);
    nm->vals = malloc(sizeof(AValue) * nm->cap);
    int j = 0;
    for (int i = 0; i < m.mval->len; i++) {
        if (i == idx) continue;
        nm->keys[j] = strdup(m.mval->keys[i]);
        nm->vals[j] = a_retain(m.mval->vals[i]);
        j++;
    }
    return (AValue){.tag = TAG_MAP, .mval = nm};
}

AValue a_map_entries(AValue m) {
    if (m.tag != TAG_MAP) return a_array_new(0);
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = m.mval->len; arr->cap = arr->len > 0 ? arr->len : 1;
    arr->items = malloc(sizeof(AValue) * arr->cap);
    for (int i = 0; i < m.mval->len; i++)
        arr->items[i] = a_array_new(2, a_string(m.mval->keys[i]), a_retain(m.mval->vals[i]));
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_map_from_entries(AValue arr) {
    if (arr.tag != TAG_ARRAY) return a_map_new(0);
    AValue m = a_map_new(0);
    for (int i = 0; i < arr.aval->len; i++) {
        AValue entry = arr.aval->items[i];
        if (entry.tag == TAG_ARRAY && entry.aval->len >= 2)
            m = a_map_set(m, entry.aval->items[0], a_retain(entry.aval->items[1]));
    }
    return m;
}

/* --- I/O --- */

static void print_val(AValue v) {
    if (v.tag == TAG_STRING) { fwrite(v.sval->data, 1, v.sval->len, stdout); return; }
    char buf[4096];
    val_to_buf(v, buf, 4096);
    fputs(buf, stdout);
}

AValue a_println(AValue v) { print_val(v); putchar('\n'); return (AValue){.tag = TAG_VOID}; }
AValue a_print(AValue v) { print_val(v); return (AValue){.tag = TAG_VOID}; }
AValue a_eprintln(AValue v) {
    char buf[4096];
    val_to_buf(v, buf, 4096);
    fprintf(stderr, "%s\n", buf);
    return (AValue){.tag = TAG_VOID};
}

AValue a_io_read_file(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("read_file: expected string path"));
    FILE* f = fopen(path.sval->data, "r");
    if (!f) return a_err(a_string("read_file: cannot open file"));
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(sz + 1);
    size_t n = fread(buf, 1, sz, f);
    buf[n] = '\0';
    fclose(f);
    AValue result = a_string_len(buf, (int)sz);
    free(buf);
    return result;
}

AValue a_io_write_file(AValue path, AValue contents) {
    if (path.tag != TAG_STRING || contents.tag != TAG_STRING) return a_err(a_string("write_file: expected strings"));
    FILE* f = fopen(path.sval->data, "w");
    if (!f) return a_err(a_string("write_file: cannot open file"));
    fwrite(contents.sval->data, 1, contents.sval->len, f);
    fclose(f);
    return a_ok(a_void());
}

AValue a_fs_ls(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("fs.ls: expected string path"));
    DIR* d = opendir(path.sval->data);
    if (!d) return a_err(a_string("fs.ls: cannot open directory"));
    AValue arr = a_array_new(0);
    struct dirent* entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' ||
            (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) continue;
        AValue name = a_string(entry->d_name);
        int is_dir = (entry->d_type == DT_DIR);
        AValue item = a_map_new(0);
        item = a_map_set(item, a_string("name"), name);
        item = a_map_set(item, a_string("is_dir"), a_bool(is_dir));
        arr = a_array_push(arr, item);
    }
    closedir(d);
    return arr;
}

AValue a_fs_mkdir(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("fs.mkdir: expected string path"));
    if (mkdir(path.sval->data, 0755) == 0) return a_void();
    return a_err(a_string("fs.mkdir: failed"));
}

AValue a_fs_cwd(void) {
    char buf[4096];
    if (getcwd(buf, sizeof(buf))) return a_string(buf);
    return a_err(a_string("fs.cwd: failed"));
}

AValue a_fs_exists(AValue path) {
    if (path.tag != TAG_STRING) return a_bool(0);
    return a_bool(access(path.sval->data, F_OK) == 0);
}

AValue a_fs_is_dir(AValue path) {
    if (path.tag != TAG_STRING) return a_bool(0);
    struct stat st;
    if (stat(path.sval->data, &st) != 0) return a_bool(0);
    return a_bool(S_ISDIR(st.st_mode));
}

AValue a_exec(AValue cmd) {
    if (cmd.tag != TAG_STRING) return a_err(a_string("exec: expected string"));
    FILE* p = popen(cmd.sval->data, "r");
    if (!p) {
        AValue r = a_map_new(0);
        r = a_map_set(r, a_string("stdout"), a_string(""));
        r = a_map_set(r, a_string("stderr"), a_string("exec: popen failed"));
        r = a_map_set(r, a_string("code"), a_int(-1));
        return r;
    }
    size_t cap = 4096, len = 0;
    char* buf = malloc(cap);
    size_t n;
    while ((n = fread(buf + len, 1, cap - len, p)) > 0) {
        len += n;
        if (len >= cap) { cap *= 2; buf = realloc(buf, cap); }
    }
    buf[len] = '\0';
    int status = pclose(p);
    int code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    AValue stdout_val = a_string_len(buf, (int)len);
    free(buf);
    AValue r = a_map_new(0);
    r = a_map_set(r, a_string("stdout"), stdout_val);
    r = a_map_set(r, a_string("stderr"), a_string(""));
    r = a_map_set(r, a_string("code"), a_int(code));
    return r;
}

AValue a_env_get(AValue key) {
    if (key.tag != TAG_STRING) return a_void();
    const char* val = getenv(key.sval->data);
    if (val) return a_string(val);
    return a_void();
}

/* --- JSON parser --- */

static AValue json_parse_value(const char** p);

static void json_skip_ws(const char** p) {
    while (**p == ' ' || **p == '\t' || **p == '\n' || **p == '\r') (*p)++;
}

static AValue json_parse_string(const char** p) {
    (*p)++; /* skip opening quote */
    size_t cap = 256, len = 0;
    char* buf = malloc(cap);
    while (**p && **p != '"') {
        if (**p == '\\') {
            (*p)++;
            switch (**p) {
                case '"': buf[len++] = '"'; break;
                case '\\': buf[len++] = '\\'; break;
                case '/': buf[len++] = '/'; break;
                case 'n': buf[len++] = '\n'; break;
                case 't': buf[len++] = '\t'; break;
                case 'r': buf[len++] = '\r'; break;
                case 'b': buf[len++] = '\b'; break;
                case 'f': buf[len++] = '\f'; break;
                default: buf[len++] = **p; break;
            }
        } else {
            buf[len++] = **p;
        }
        (*p)++;
        if (len >= cap - 1) { cap *= 2; buf = realloc(buf, cap); }
    }
    if (**p == '"') (*p)++;
    buf[len] = '\0';
    AValue result = a_string_len(buf, (int)len);
    free(buf);
    return result;
}

static AValue json_parse_number(const char** p) {
    const char* start = *p;
    int is_float = 0;
    if (**p == '-') (*p)++;
    while (isdigit(**p)) (*p)++;
    if (**p == '.') { is_float = 1; (*p)++; while (isdigit(**p)) (*p)++; }
    if (**p == 'e' || **p == 'E') { is_float = 1; (*p)++; if (**p == '+' || **p == '-') (*p)++; while (isdigit(**p)) (*p)++; }
    if (is_float) return a_float(strtod(start, NULL));
    return a_int(strtoll(start, NULL, 10));
}

static AValue json_parse_array(const char** p) {
    (*p)++; /* skip [ */
    json_skip_ws(p);
    AValue arr = a_array_new(0);
    if (**p == ']') { (*p)++; return arr; }
    while (1) {
        json_skip_ws(p);
        AValue v = json_parse_value(p);
        arr = a_array_push(arr, v);
        json_skip_ws(p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == ']') { (*p)++; break; }
        break;
    }
    return arr;
}

static AValue json_parse_object(const char** p) {
    (*p)++; /* skip { */
    json_skip_ws(p);
    AValue m = a_map_new(0);
    if (**p == '}') { (*p)++; return m; }
    while (1) {
        json_skip_ws(p);
        if (**p != '"') break;
        AValue key = json_parse_string(p);
        json_skip_ws(p);
        if (**p == ':') (*p)++;
        json_skip_ws(p);
        AValue val = json_parse_value(p);
        m = a_map_set(m, key, val);
        json_skip_ws(p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == '}') { (*p)++; break; }
        break;
    }
    return m;
}

static AValue json_parse_value(const char** p) {
    json_skip_ws(p);
    if (**p == '"') return json_parse_string(p);
    if (**p == '[') return json_parse_array(p);
    if (**p == '{') return json_parse_object(p);
    if (**p == 't' && strncmp(*p, "true", 4) == 0) { *p += 4; return a_bool(1); }
    if (**p == 'f' && strncmp(*p, "false", 5) == 0) { *p += 5; return a_bool(0); }
    if (**p == 'n' && strncmp(*p, "null", 4) == 0) { *p += 4; return a_void(); }
    if (**p == '-' || isdigit(**p)) return json_parse_number(p);
    return a_void();
}

AValue a_json_parse(AValue input) {
    if (input.tag != TAG_STRING) return a_err(a_string("json.parse: expected string"));
    const char* p = input.sval->data;
    return json_parse_value(&p);
}

/* --- Result --- */

AValue a_ok(AValue v) {
    AValue r; r.tag = TAG_RESULT;
    r.rval.is_ok = 1;
    r.rval.inner = malloc(sizeof(AValue));
    *r.rval.inner = v;
    return r;
}

AValue a_err(AValue v) {
    AValue r; r.tag = TAG_RESULT;
    r.rval.is_ok = 0;
    r.rval.inner = malloc(sizeof(AValue));
    *r.rval.inner = v;
    return r;
}

AValue a_is_ok(AValue v) { return a_bool(v.tag == TAG_RESULT && v.rval.is_ok); }
AValue a_is_err(AValue v) { return a_bool(v.tag == TAG_RESULT && !v.rval.is_ok); }

AValue a_unwrap(AValue v) {
    if (v.tag == TAG_RESULT) {
        if (v.rval.is_ok) return a_retain(*v.rval.inner);
        fprintf(stderr, "unwrap on Err: ");
        a_eprintln(*v.rval.inner);
        exit(1);
    }
    return v;
}

AValue a_try_unwrap(AValue v) {
    if (v.tag == TAG_RESULT) {
        if (v.rval.is_ok) return *v.rval.inner;
        if (a_try_depth > 0) {
            a_try_err = *v.rval.inner;
            longjmp(a_try_stack[a_try_depth - 1], 1);
        }
        fprintf(stderr, "uncaught error: ");
        a_eprintln(*v.rval.inner);
        exit(1);
    }
    return v;
}

/* --- Utility --- */

AValue a_type_of(AValue v) {
    switch (v.tag) {
        case TAG_INT: return a_string("int");
        case TAG_FLOAT: return a_string("float");
        case TAG_BOOL: return a_string("bool");
        case TAG_VOID: return a_string("void");
        case TAG_STRING: return a_string("str");
        case TAG_ARRAY: return a_string("array");
        case TAG_MAP: return a_string("map");
        case TAG_RESULT: return a_string("result");
        case TAG_CLOSURE: return a_string("closure");
        case TAG_PTR: return a_string("ptr");
        default: return a_string("unknown");
    }
}

AValue a_args(void) {
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = 0; arr->cap = g_argc > 0 ? g_argc : 1;
    arr->items = malloc(sizeof(AValue) * arr->cap);
    for (int i = 1; i < g_argc; i++) {
        arr->items[arr->len++] = a_string(g_argv[i]);
    }
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_argv0(void) {
    if (g_argc > 0 && g_argv[0]) return a_string(g_argv[0]);
    return a_string("a");
}

void a_fail(AValue v) {
    fprintf(stderr, "runtime error: ");
    a_eprintln(v);
    exit(1);
}

AValue a_to_int(AValue v) {
    if (v.tag == TAG_INT) return v;
    if (v.tag == TAG_FLOAT) return a_int((int64_t)v.fval);
    if (v.tag == TAG_STRING) return a_int(atoll(v.sval->data));
    if (v.tag == TAG_BOOL) return a_int(v.bval ? 1 : 0);
    return a_int(0);
}

AValue a_to_float(AValue v) {
    if (v.tag == TAG_FLOAT) return v;
    if (v.tag == TAG_INT) return a_float((double)v.ival);
    if (v.tag == TAG_STRING) return a_float(atof(v.sval->data));
    return a_float(0.0);
}

AValue a_char_code(AValue v) {
    if (v.tag == TAG_STRING && v.sval->len > 0)
        return a_int((unsigned char)v.sval->data[0]);
    return a_int(0);
}

AValue a_from_code(AValue v) {
    if (v.tag == TAG_INT) {
        char c = (char)v.ival;
        return a_string_len(&c, 1);
    }
    return a_string("");
}

AValue a_is_alpha(AValue v) {
    if (v.tag == TAG_STRING && v.sval->len > 0)
        return a_bool(isalpha((unsigned char)v.sval->data[0]));
    return a_bool(0);
}

AValue a_is_digit(AValue v) {
    if (v.tag == TAG_STRING && v.sval->len > 0)
        return a_bool(isdigit((unsigned char)v.sval->data[0]));
    return a_bool(0);
}

AValue a_is_alnum(AValue v) {
    if (v.tag == TAG_STRING && v.sval->len > 0)
        return a_bool(isalnum((unsigned char)v.sval->data[0]));
    return a_bool(0);
}

/* --- Result extras --- */

AValue a_unwrap_or(AValue v, AValue def) {
    if (v.tag == TAG_RESULT && v.rval.is_ok) return a_retain(*v.rval.inner);
    return a_retain(def);
}

AValue a_expect(AValue v, AValue msg) {
    if (v.tag == TAG_RESULT) {
        if (v.rval.is_ok) return a_retain(*v.rval.inner);
        fprintf(stderr, "expect failed: ");
        a_eprintln(msg);
        fprintf(stderr, "  error was: ");
        a_eprintln(*v.rval.inner);
        exit(1);
    }
    return v;
}

/* --- Math --- */

AValue a_math_sqrt(AValue v) {
    double x = (v.tag == TAG_INT) ? (double)v.ival : v.fval;
    return a_float(sqrt(x));
}

AValue a_math_abs(AValue v) {
    if (v.tag == TAG_INT) return a_int(v.ival < 0 ? -v.ival : v.ival);
    if (v.tag == TAG_FLOAT) return a_float(fabs(v.fval));
    return v;
}

AValue a_math_floor(AValue v) {
    double x = (v.tag == TAG_INT) ? (double)v.ival : v.fval;
    return a_int((int64_t)floor(x));
}

AValue a_math_ceil(AValue v) {
    double x = (v.tag == TAG_INT) ? (double)v.ival : v.fval;
    return a_int((int64_t)ceil(x));
}

AValue a_math_round(AValue v) {
    double x = (v.tag == TAG_INT) ? (double)v.ival : v.fval;
    return a_int((int64_t)round(x));
}

AValue a_math_pow(AValue base, AValue exp) {
    double b = (base.tag == TAG_INT) ? (double)base.ival : base.fval;
    double e = (exp.tag == TAG_INT) ? (double)exp.ival : exp.fval;
    double r = pow(b, e);
    if (base.tag == TAG_INT && exp.tag == TAG_INT && exp.ival >= 0)
        return a_int((int64_t)r);
    return a_float(r);
}

AValue a_math_min(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a.ival < b.ival ? a : b;
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return fa < fb ? a : b;
}

AValue a_math_max(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a.ival > b.ival ? a : b;
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return fa > fb ? a : b;
}

/* --- String extras --- */

AValue a_str_lines(AValue s) {
    if (s.tag != TAG_STRING) return a_array_new(0);
    return a_str_split(s, a_string("\n"));
}

/* --- I/O extras --- */

AValue a_io_read_stdin(void) {
    size_t cap = 4096, len = 0;
    char* buf = malloc(cap);
    size_t n;
    while ((n = fread(buf + len, 1, cap - len, stdin)) > 0) {
        len += n;
        if (len >= cap) { cap *= 2; buf = realloc(buf, cap); }
    }
    buf[len] = '\0';
    AValue result = a_string_len(buf, (int)len);
    free(buf);
    return result;
}

AValue a_io_read_line(void) {
    size_t cap = 256, len = 0;
    char* buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len >= cap - 1) { cap *= 2; buf = realloc(buf, cap); }
        buf[len++] = (char)c;
    }
    buf[len] = '\0';
    AValue result = a_string_len(buf, (int)len);
    free(buf);
    return result;
}

AValue a_io_read_bytes(AValue n) {
    int count = (int)n.ival;
    if (count <= 0) return a_string("");
    char* buf = malloc(count + 1);
    size_t total = 0;
    while ((int)total < count) {
        size_t got = fread(buf + total, 1, count - total, stdin);
        if (got == 0) break;
        total += got;
    }
    buf[total] = '\0';
    AValue result = a_string_len(buf, (int)total);
    free(buf);
    return result;
}

AValue a_io_flush(void) {
    fflush(stdout);
    return a_void();
}

/* --- Environment extras --- */

extern char** environ;

AValue a_env_set(AValue key, AValue val) {
    if (key.tag != TAG_STRING || val.tag != TAG_STRING) return a_void();
    setenv(key.sval->data, val.sval->data, 1);
    return a_void();
}

AValue a_env_all(void) {
    AValue m = a_map_new(0);
    for (char** e = environ; *e; e++) {
        char* eq = strchr(*e, '=');
        if (eq) {
            AValue k = a_string_len(*e, (int)(eq - *e));
            AValue v = a_string(eq + 1);
            m = a_map_set(m, k, v);
        }
    }
    return m;
}

/* --- Filesystem extras --- */

AValue a_fs_rm(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("fs.rm: expected string path"));
    if (remove(path.sval->data) == 0) return a_ok(a_void());
    return a_err(a_string("fs.rm: failed"));
}

AValue a_fs_mv(AValue src, AValue dst) {
    if (src.tag != TAG_STRING || dst.tag != TAG_STRING) return a_err(a_string("fs.mv: expected string paths"));
    if (rename(src.sval->data, dst.sval->data) == 0) return a_ok(a_void());
    return a_err(a_string("fs.mv: failed"));
}

AValue a_fs_cp(AValue src, AValue dst) {
    if (src.tag != TAG_STRING || dst.tag != TAG_STRING) return a_err(a_string("fs.cp: expected string paths"));
    FILE* in = fopen(src.sval->data, "rb");
    if (!in) return a_err(a_string("fs.cp: cannot open source"));
    FILE* out = fopen(dst.sval->data, "wb");
    if (!out) { fclose(in); return a_err(a_string("fs.cp: cannot open dest")); }
    char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) fwrite(buf, 1, n, out);
    fclose(in);
    fclose(out);
    return a_ok(a_void());
}

AValue a_fs_abs(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("fs.abs: expected string path"));
    char resolved[4096];
    if (realpath(path.sval->data, resolved)) return a_string(resolved);
    return a_err(a_string("fs.abs: failed"));
}

AValue a_fs_is_file(AValue path) {
    if (path.tag != TAG_STRING) return a_bool(0);
    struct stat st;
    if (stat(path.sval->data, &st) != 0) return a_bool(0);
    return a_bool(S_ISREG(st.st_mode));
}

/* --- Time --- */

#include <sys/time.h>

AValue a_time_now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return a_int((int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

AValue a_time_sleep(AValue ms) {
    if (ms.tag != TAG_INT) return a_void();
    struct timespec ts;
    ts.tv_sec = ms.ival / 1000;
    ts.tv_nsec = (ms.ival % 1000) * 1000000;
    nanosleep(&ts, NULL);
    return a_void();
}

/* --- Hashing --- */

static void sha256_transform(uint32_t state[8], const uint8_t data[64]) {
    static const uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    #define RR(x,n) (((x)>>(n))|((x)<<(32-(n))))
    #define CH(x,y,z) (((x)&(y))^(~(x)&(z)))
    #define MAJ(x,y,z) (((x)&(y))^((x)&(z))^((y)&(z)))
    #define EP0(x) (RR(x,2)^RR(x,13)^RR(x,22))
    #define EP1(x) (RR(x,6)^RR(x,11)^RR(x,25))
    #define SIG0(x) (RR(x,7)^RR(x,18)^((x)>>3))
    #define SIG1(x) (RR(x,17)^RR(x,19)^((x)>>10))
    uint32_t w[64], a,b,c,d,e,f,g,h,t1,t2;
    for (int i = 0; i < 16; i++)
        w[i] = ((uint32_t)data[i*4]<<24)|((uint32_t)data[i*4+1]<<16)|((uint32_t)data[i*4+2]<<8)|data[i*4+3];
    for (int i = 16; i < 64; i++)
        w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
    a=state[0]; b=state[1]; c=state[2]; d=state[3];
    e=state[4]; f=state[5]; g=state[6]; h=state[7];
    for (int i = 0; i < 64; i++) {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + w[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    state[0]+=a; state[1]+=b; state[2]+=c; state[3]+=d;
    state[4]+=e; state[5]+=f; state[6]+=g; state[7]+=h;
    #undef RR
    #undef CH
    #undef MAJ
    #undef EP0
    #undef EP1
    #undef SIG0
    #undef SIG1
}

AValue a_hash_sha256(AValue data) {
    if (data.tag != TAG_STRING) return a_err(a_string("hash.sha256: expected string"));
    const uint8_t* msg = (const uint8_t*)data.sval->data;
    uint64_t bitlen = (uint64_t)data.sval->len * 8;
    uint32_t state[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    int len = data.sval->len;
    int i;
    for (i = 0; i + 64 <= len; i += 64)
        sha256_transform(state, msg + i);
    uint8_t block[64];
    int rem = len - i;
    memcpy(block, msg + i, rem);
    block[rem] = 0x80;
    if (rem >= 56) {
        memset(block + rem + 1, 0, 63 - rem);
        sha256_transform(state, block);
        memset(block, 0, 56);
    } else {
        memset(block + rem + 1, 0, 55 - rem);
    }
    for (int j = 0; j < 8; j++) block[56 + j] = (uint8_t)(bitlen >> (56 - j * 8));
    sha256_transform(state, block);
    char hex[65];
    for (int j = 0; j < 8; j++)
        snprintf(hex + j * 8, 9, "%08x", state[j]);
    return a_string(hex);
}

static void md5_transform(uint32_t state[4], const uint8_t block[64]) {
    static const uint32_t S[64] = {
        7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
        5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
        4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
        6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21
    };
    static const uint32_t K[64] = {
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
        0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
        0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
        0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
        0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
        0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
        0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
    };
    uint32_t M[16];
    for (int i = 0; i < 16; i++)
        M[i] = (uint32_t)block[i*4] | ((uint32_t)block[i*4+1]<<8) | ((uint32_t)block[i*4+2]<<16) | ((uint32_t)block[i*4+3]<<24);
    uint32_t a=state[0], b=state[1], c=state[2], d=state[3];
    for (int i = 0; i < 64; i++) {
        uint32_t F, g;
        if (i < 16)      { F = (b & c) | (~b & d); g = i; }
        else if (i < 32) { F = (d & b) | (~d & c); g = (5*i + 1) % 16; }
        else if (i < 48) { F = b ^ c ^ d;           g = (3*i + 5) % 16; }
        else              { F = c ^ (b | ~d);        g = (7*i) % 16; }
        uint32_t tmp = d; d = c; c = b;
        uint32_t x = a + F + K[i] + M[g];
        b = b + ((x << S[i]) | (x >> (32 - S[i])));
        a = tmp;
    }
    state[0]+=a; state[1]+=b; state[2]+=c; state[3]+=d;
}

AValue a_hash_md5(AValue data) {
    if (data.tag != TAG_STRING) return a_err(a_string("hash.md5: expected string"));
    const uint8_t* msg = (const uint8_t*)data.sval->data;
    uint64_t bitlen = (uint64_t)data.sval->len * 8;
    uint32_t state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    int len = data.sval->len;
    int i;
    for (i = 0; i + 64 <= len; i += 64)
        md5_transform(state, msg + i);
    uint8_t block[64];
    int rem = len - i;
    memcpy(block, msg + i, rem);
    block[rem] = 0x80;
    if (rem >= 56) {
        memset(block + rem + 1, 0, 63 - rem);
        md5_transform(state, block);
        memset(block, 0, 56);
    } else {
        memset(block + rem + 1, 0, 55 - rem);
    }
    for (int j = 0; j < 8; j++) block[56 + j] = (uint8_t)(bitlen >> (j * 8));
    md5_transform(state, block);
    char hex[33];
    for (int j = 0; j < 4; j++) {
        uint32_t v = state[j];
        snprintf(hex + j * 8, 9, "%02x%02x%02x%02x", v&0xff, (v>>8)&0xff, (v>>16)&0xff, (v>>24)&0xff);
    }
    return a_string(hex);
}

/* --- JSON extras --- */

static void json_stringify_val(AValue v, char** buf, int* pos, int* cap, int indent, int pretty) {
    #define JS_GROW(n) do { while (*pos + (n) >= *cap) { *cap *= 2; *buf = realloc(*buf, *cap); } } while(0)
    #define JS_PUT(s) do { int _l = (int)strlen(s); JS_GROW(_l); memcpy(*buf + *pos, s, _l); *pos += _l; } while(0)
    #define JS_INDENT(d) do { if (pretty) { JS_PUT("\n"); for(int _i=0;_i<(d);_i++) JS_PUT("  "); } } while(0)
    switch (v.tag) {
        case TAG_INT: { char tmp[32]; snprintf(tmp, 32, "%lld", (long long)v.ival); JS_PUT(tmp); break; }
        case TAG_FLOAT: { char tmp[64]; snprintf(tmp, 64, "%g", v.fval); JS_PUT(tmp); break; }
        case TAG_BOOL: JS_PUT(v.bval ? "true" : "false"); break;
        case TAG_VOID: JS_PUT("null"); break;
        case TAG_STRING: {
            JS_GROW(v.sval->len * 2 + 2);
            (*buf)[(*pos)++] = '"';
            for (int i = 0; i < v.sval->len; i++) {
                char c = v.sval->data[i];
                if (c == '"') { JS_PUT("\\\""); }
                else if (c == '\\') { JS_PUT("\\\\"); }
                else if (c == '\n') { JS_PUT("\\n"); }
                else if (c == '\r') { JS_PUT("\\r"); }
                else if (c == '\t') { JS_PUT("\\t"); }
                else { JS_GROW(1); (*buf)[(*pos)++] = c; }
            }
            (*buf)[(*pos)++] = '"';
            break;
        }
        case TAG_ARRAY: {
            JS_PUT("[");
            for (int i = 0; i < v.aval->len; i++) {
                if (i > 0) JS_PUT(",");
                JS_INDENT(indent + 1);
                json_stringify_val(v.aval->items[i], buf, pos, cap, indent + 1, pretty);
            }
            if (v.aval->len > 0) JS_INDENT(indent);
            JS_PUT("]");
            break;
        }
        case TAG_MAP: {
            JS_PUT("{");
            for (int i = 0; i < v.mval->len; i++) {
                if (i > 0) JS_PUT(",");
                JS_INDENT(indent + 1);
                JS_GROW((int)strlen(v.mval->keys[i]) * 2 + 4);
                (*buf)[(*pos)++] = '"';
                const char* k = v.mval->keys[i];
                while (*k) { if (*k == '"') JS_PUT("\\\""); else { (*buf)[(*pos)++] = *k; } k++; }
                (*buf)[(*pos)++] = '"';
                JS_PUT(pretty ? ": " : ":");
                json_stringify_val(v.mval->vals[i], buf, pos, cap, indent + 1, pretty);
            }
            if (v.mval->len > 0) JS_INDENT(indent);
            JS_PUT("}");
            break;
        }
        case TAG_RESULT: {
            if (v.rval.inner) json_stringify_val(*v.rval.inner, buf, pos, cap, indent, pretty);
            else JS_PUT("null");
            break;
        }
        default: JS_PUT("null"); break;
    }
    #undef JS_GROW
    #undef JS_PUT
    #undef JS_INDENT
}

AValue a_json_stringify(AValue v) {
    int cap = 256, pos = 0;
    char* buf = malloc(cap);
    json_stringify_val(v, &buf, &pos, &cap, 0, 0);
    buf[pos] = '\0';
    AValue result = a_string_len(buf, pos);
    free(buf);
    return result;
}

AValue a_json_pretty(AValue v) {
    int cap = 256, pos = 0;
    char* buf = malloc(cap);
    json_stringify_val(v, &buf, &pos, &cap, 0, 1);
    buf[pos] = '\0';
    AValue result = a_string_len(buf, pos);
    free(buf);
    return result;
}

/* --- HTTP client (in-process, POSIX sockets + platform TLS) --- */

#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __APPLE__
#include <Security/Security.h>
#include <Security/SecureTransport.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <dlfcn.h>
#endif

/* --- URL parser --- */

typedef struct {
    char scheme[8];
    char host[256];
    int port;
    char path[4096];
} HttpUrl;

static int http_parse_url(const char* url, HttpUrl* out) {
    memset(out, 0, sizeof(*out));
    const char* p = url;
    if (strncmp(p, "https://", 8) == 0) {
        strcpy(out->scheme, "https");
        out->port = 443;
        p += 8;
    } else if (strncmp(p, "http://", 7) == 0) {
        strcpy(out->scheme, "http");
        out->port = 80;
        p += 7;
    } else {
        return -1;
    }
    const char* slash = strchr(p, '/');
    const char* colon = strchr(p, ':');
    if (colon && (!slash || colon < slash)) {
        int hlen = (int)(colon - p);
        if (hlen >= (int)sizeof(out->host)) hlen = (int)sizeof(out->host) - 1;
        memcpy(out->host, p, hlen);
        out->host[hlen] = '\0';
        out->port = atoi(colon + 1);
        if (slash) {
            strncpy(out->path, slash, sizeof(out->path) - 1);
        } else {
            strcpy(out->path, "/");
        }
    } else if (slash) {
        int hlen = (int)(slash - p);
        if (hlen >= (int)sizeof(out->host)) hlen = (int)sizeof(out->host) - 1;
        memcpy(out->host, p, hlen);
        out->host[hlen] = '\0';
        strncpy(out->path, slash, sizeof(out->path) - 1);
    } else {
        strncpy(out->host, p, sizeof(out->host) - 1);
        strcpy(out->path, "/");
    }
    return 0;
}

/* --- TCP connection --- */

static int http_tcp_connect(const char* host, int port, int timeout_ms) {
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    struct addrinfo hints, *res, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port_str, &hints, &res) != 0) return -1;

    int fd = -1;
    for (rp = res; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        int rc = connect(fd, rp->ai_addr, rp->ai_addrlen);
        if (rc == 0) {
            fcntl(fd, F_SETFL, flags);
            break;
        }
        if (errno == EINPROGRESS) {
            struct pollfd pfd = { .fd = fd, .events = POLLOUT };
            int pr = poll(&pfd, 1, timeout_ms > 0 ? timeout_ms : 10000);
            if (pr > 0 && (pfd.revents & POLLOUT)) {
                int err = 0;
                socklen_t elen = sizeof(err);
                getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &elen);
                if (err == 0) { fcntl(fd, F_SETFL, flags); break; }
            }
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    return fd;
}

/* --- I/O abstraction --- */

typedef struct {
    int fd;
    void* tls_ctx;
    int use_tls;
} HttpConn;

static int http_io_write(HttpConn* c, const void* buf, int len);
static int http_io_read(HttpConn* c, void* buf, int len);
static void http_conn_close(HttpConn* c);

/* --- Platform TLS --- */

#ifdef __APPLE__

static OSStatus _tls_read_cb(SSLConnectionRef conn, void* data, size_t* len) {
    int fd = *(int*)conn;
    ssize_t n = recv(fd, data, *len, 0);
    if (n > 0) { *len = (size_t)n; return noErr; }
    if (n == 0) { *len = 0; return errSSLClosedGraceful; }
    *len = 0;
    return (errno == EAGAIN || errno == EWOULDBLOCK) ? errSSLWouldBlock : errSecIO;
}

static OSStatus _tls_write_cb(SSLConnectionRef conn, const void* data, size_t* len) {
    int fd = *(int*)conn;
    ssize_t n = send(fd, data, *len, 0);
    if (n >= 0) { *len = (size_t)n; return noErr; }
    *len = 0;
    return (errno == EAGAIN || errno == EWOULDBLOCK) ? errSSLWouldBlock : errSecIO;
}

typedef struct {
    SSLContextRef ctx;
    int fd;
} AppleTLS;

static int http_tls_connect(HttpConn* c, const char* host) {
    AppleTLS* t = calloc(1, sizeof(AppleTLS));
    t->fd = c->fd;
    t->ctx = SSLCreateContext(NULL, kSSLClientSide, kSSLStreamType);
    if (!t->ctx) { free(t); return -1; }
    SSLSetIOFuncs(t->ctx, _tls_read_cb, _tls_write_cb);
    SSLSetConnection(t->ctx, &t->fd);
    SSLSetPeerDomainName(t->ctx, host, strlen(host));

    OSStatus st;
    do { st = SSLHandshake(t->ctx); } while (st == errSSLWouldBlock);
    if (st != noErr) {
        CFRelease(t->ctx);
        free(t);
        return -1;
    }
    c->tls_ctx = t;
    c->use_tls = 1;
    return 0;
}

static int http_tls_write(HttpConn* c, const void* buf, int len) {
    AppleTLS* t = (AppleTLS*)c->tls_ctx;
    size_t written = 0;
    OSStatus st = SSLWrite(t->ctx, buf, (size_t)len, &written);
    return (st == noErr || written > 0) ? (int)written : -1;
}

static int http_tls_read(HttpConn* c, void* buf, int len) {
    AppleTLS* t = (AppleTLS*)c->tls_ctx;
    size_t read_n = 0;
    OSStatus st = SSLRead(t->ctx, buf, (size_t)len, &read_n);
    if (read_n > 0) return (int)read_n;
    if (st == errSSLClosedGraceful || st == errSSLClosedAbort) return 0;
    return -1;
}

static void http_tls_close(HttpConn* c) {
    if (!c->tls_ctx) return;
    AppleTLS* t = (AppleTLS*)c->tls_ctx;
    SSLClose(t->ctx);
    CFRelease(t->ctx);
    free(t);
    c->tls_ctx = NULL;
}

#pragma clang diagnostic pop

#else /* Linux: OpenSSL via dlopen */

typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_method_st SSL_METHOD;

static void* _ssl_lib = NULL;
static void* _crypto_lib = NULL;
static int _ssl_init_done = 0;
static int _ssl_available = 0;

static SSL_CTX* (*_SSL_CTX_new)(const SSL_METHOD*) = NULL;
static const SSL_METHOD* (*_TLS_client_method)(void) = NULL;
static SSL* (*_SSL_new)(SSL_CTX*) = NULL;
static int (*_SSL_set_fd)(SSL*, int) = NULL;
static int (*_SSL_connect)(SSL*) = NULL;
static int (*_SSL_read)(SSL*, void*, int) = NULL;
static int (*_SSL_write)(SSL*, const void*, int) = NULL;
static int (*_SSL_shutdown)(SSL*) = NULL;
static void (*_SSL_free)(SSL*) = NULL;
static void (*_SSL_CTX_free)(SSL_CTX*) = NULL;
static long (*_SSL_ctrl)(SSL*, int, long, void*) = NULL;

#define SSL_CTRL_SET_TLSEXT_HOSTNAME 55
#define TLSEXT_NAMETYPE_host_name 0

static void _ssl_try_init(void) {
    if (_ssl_init_done) return;
    _ssl_init_done = 1;
    const char* libs[] = { "libssl.so.3", "libssl.so.1.1", "libssl.so", NULL };
    for (int i = 0; libs[i]; i++) {
        _ssl_lib = dlopen(libs[i], RTLD_NOW);
        if (_ssl_lib) break;
    }
    if (!_ssl_lib) return;
    const char* crypto_libs[] = { "libcrypto.so.3", "libcrypto.so.1.1", "libcrypto.so", NULL };
    for (int i = 0; crypto_libs[i]; i++) {
        _crypto_lib = dlopen(crypto_libs[i], RTLD_NOW);
        if (_crypto_lib) break;
    }
    _SSL_CTX_new = dlsym(_ssl_lib, "SSL_CTX_new");
    _TLS_client_method = dlsym(_ssl_lib, "TLS_client_method");
    _SSL_new = dlsym(_ssl_lib, "SSL_new");
    _SSL_set_fd = dlsym(_ssl_lib, "SSL_set_fd");
    _SSL_connect = dlsym(_ssl_lib, "SSL_connect");
    _SSL_read = dlsym(_ssl_lib, "SSL_read");
    _SSL_write = dlsym(_ssl_lib, "SSL_write");
    _SSL_shutdown = dlsym(_ssl_lib, "SSL_shutdown");
    _SSL_free = dlsym(_ssl_lib, "SSL_free");
    _SSL_CTX_free = dlsym(_ssl_lib, "SSL_CTX_free");
    _SSL_ctrl = dlsym(_ssl_lib, "SSL_ctrl");
    if (_SSL_CTX_new && _TLS_client_method && _SSL_new && _SSL_set_fd &&
        _SSL_connect && _SSL_read && _SSL_write && _SSL_shutdown &&
        _SSL_free && _SSL_CTX_free) {
        _ssl_available = 1;
    }
}

typedef struct {
    SSL* ssl;
    SSL_CTX* ctx;
} LinuxTLS;

static int http_tls_connect(HttpConn* c, const char* host) {
    _ssl_try_init();
    if (!_ssl_available) return -1;
    LinuxTLS* t = calloc(1, sizeof(LinuxTLS));
    t->ctx = _SSL_CTX_new(_TLS_client_method());
    if (!t->ctx) { free(t); return -1; }
    t->ssl = _SSL_new(t->ctx);
    if (!t->ssl) { _SSL_CTX_free(t->ctx); free(t); return -1; }
    _SSL_set_fd(t->ssl, c->fd);
    if (_SSL_ctrl)
        _SSL_ctrl(t->ssl, SSL_CTRL_SET_TLSEXT_HOSTNAME, TLSEXT_NAMETYPE_host_name, (void*)host);
    if (_SSL_connect(t->ssl) != 1) {
        _SSL_free(t->ssl);
        _SSL_CTX_free(t->ctx);
        free(t);
        return -1;
    }
    c->tls_ctx = t;
    c->use_tls = 1;
    return 0;
}

static int http_tls_write(HttpConn* c, const void* buf, int len) {
    LinuxTLS* t = (LinuxTLS*)c->tls_ctx;
    return _SSL_write(t->ssl, buf, len);
}

static int http_tls_read(HttpConn* c, void* buf, int len) {
    LinuxTLS* t = (LinuxTLS*)c->tls_ctx;
    return _SSL_read(t->ssl, buf, len);
}

static void http_tls_close(HttpConn* c) {
    if (!c->tls_ctx) return;
    LinuxTLS* t = (LinuxTLS*)c->tls_ctx;
    _SSL_shutdown(t->ssl);
    _SSL_free(t->ssl);
    _SSL_CTX_free(t->ctx);
    free(t);
    c->tls_ctx = NULL;
}

#endif /* __APPLE__ / Linux TLS */

/* --- I/O dispatch --- */

static int http_io_write(HttpConn* c, const void* buf, int len) {
    if (c->use_tls) return http_tls_write(c, buf, len);
    return (int)send(c->fd, buf, (size_t)len, 0);
}

static int http_io_read(HttpConn* c, void* buf, int len) {
    if (c->use_tls) return http_tls_read(c, buf, len);
    return (int)recv(c->fd, buf, (size_t)len, 0);
}

static void http_conn_close(HttpConn* c) {
    if (c->use_tls) http_tls_close(c);
    if (c->fd >= 0) close(c->fd);
    c->fd = -1;
}

/* --- Curl fallback for HTTPS when TLS unavailable --- */

static AValue http_request_curl(const char* method, const char* url, const char* body, AValue headers) {
    int out_pipe[2], err_pipe[2];
    if (pipe(out_pipe) < 0 || pipe(err_pipe) < 0)
        return a_err(a_string("http: pipe failed"));
    pid_t pid = fork();
    if (pid < 0) {
        close(out_pipe[0]); close(out_pipe[1]);
        close(err_pipe[0]); close(err_pipe[1]);
        return a_err(a_string("http: fork failed"));
    }
    if (pid == 0) {
        close(out_pipe[0]); close(err_pipe[0]);
        dup2(out_pipe[1], STDOUT_FILENO);
        dup2(err_pipe[1], STDERR_FILENO);
        close(out_pipe[1]); close(err_pipe[1]);
        int ac = 0; char* av[64];
        av[ac++] = "curl"; av[ac++] = "-s";
        av[ac++] = "-D"; av[ac++] = "/dev/stderr";
        av[ac++] = "-w"; av[ac++] = "\n__HTTP_STATUS__%{http_code}";
        av[ac++] = "-X"; av[ac++] = (char*)method;
        if (headers.tag == TAG_MAP) {
            for (int i = 0; i < headers.mval->len && ac < 56; i++) {
                if (headers.mval->vals[i].tag != TAG_STRING) continue;
                char* h = malloc(strlen(headers.mval->keys[i]) + headers.mval->vals[i].sval->len + 4);
                sprintf(h, "%s: %s", headers.mval->keys[i], headers.mval->vals[i].sval->data);
                av[ac++] = "-H"; av[ac++] = h;
            }
        }
        if (body && strlen(body) > 0) { av[ac++] = "-d"; av[ac++] = (char*)body; }
        av[ac++] = (char*)url; av[ac] = NULL;
        execvp("curl", av); _exit(127);
    }
    close(out_pipe[1]); close(err_pipe[1]);
    size_t bcap = 4096, blen = 0;
    char* bbuf = malloc(bcap);
    ssize_t n;
    while ((n = read(out_pipe[0], bbuf + blen, bcap - blen)) > 0) {
        blen += n;
        if (blen >= bcap - 1) { bcap *= 2; bbuf = realloc(bbuf, bcap); }
    }
    bbuf[blen] = '\0'; close(out_pipe[0]);
    size_t hcap = 4096, hlen = 0;
    char* hbuf = malloc(hcap);
    while ((n = read(err_pipe[0], hbuf + hlen, hcap - hlen)) > 0) {
        hlen += n;
        if (hlen >= hcap - 1) { hcap *= 2; hbuf = realloc(hbuf, hcap); }
    }
    hbuf[hlen] = '\0'; close(err_pipe[0]);
    int st; waitpid(pid, &st, 0);
    if (!WIFEXITED(st) || WEXITSTATUS(st) == 127) {
        free(bbuf); free(hbuf);
        return a_err(a_string("http: curl not found (no TLS library available; install openssl or curl)"));
    }
    int http_status = 0;
    char* marker = strstr(bbuf, "\n__HTTP_STATUS__");
    if (marker) { http_status = atoi(marker + 16); *marker = '\0'; blen = marker - bbuf; }
    AValue resp_headers = a_map_new(0);
    char* line = hbuf;
    while (line && *line) {
        char* eol = strstr(line, "\r\n");
        if (!eol) eol = line + strlen(line);
        char* colon = strchr(line, ':');
        if (colon && colon < eol && line[0] != 'H') {
            *colon = '\0';
            char* val = colon + 1;
            while (*val == ' ') val++;
            int vlen = (int)(eol - val);
            for (char* pp = line; *pp; pp++) *pp = (char)tolower((unsigned char)*pp);
            resp_headers = a_map_set(resp_headers, a_string(line), a_string_len(val, vlen));
        }
        if (*eol) line = eol + 2; else break;
    }
    free(hbuf);
    AValue result = a_map_new(0);
    result = a_map_set(result, a_string("status"), a_int(http_status));
    result = a_map_set(result, a_string("body"), a_string_len(bbuf, (int)blen));
    result = a_map_set(result, a_string("headers"), resp_headers);
    free(bbuf);
    return result;
}

/* --- HTTP/1.1 request/response --- */

static int http_io_write_all(HttpConn* c, const char* buf, int len) {
    int sent = 0;
    while (sent < len) {
        int n = http_io_write(c, buf + sent, len - sent);
        if (n <= 0) return -1;
        sent += n;
    }
    return sent;
}

static AValue http_request_inprocess(const char* method, const char* url_str,
                                      const char* body, int body_len, AValue req_headers) {
    HttpUrl url;
    if (http_parse_url(url_str, &url) != 0)
        return a_err(a_string("http: invalid URL"));

    int need_tls = (strcmp(url.scheme, "https") == 0);

    int fd = http_tcp_connect(url.host, url.port, 10000);
    if (fd < 0) return a_err(a_string("http: connection failed"));

    HttpConn conn = { .fd = fd, .tls_ctx = NULL, .use_tls = 0 };

    if (need_tls) {
        if (http_tls_connect(&conn, url.host) != 0) {
            close(fd);
            return http_request_curl(method, url_str, body, req_headers);
        }
    }

    /* Format request */
    size_t req_cap = 4096 + body_len;
    char* req = malloc(req_cap);
    int rlen = snprintf(req, req_cap, "%s %s HTTP/1.1\r\nHost: %s\r\n", method, url.path, url.host);

    int has_ct = 0;
    int has_accept_enc = 0;
    if (req_headers.tag == TAG_MAP) {
        for (int i = 0; i < req_headers.mval->len; i++) {
            if (req_headers.mval->vals[i].tag != TAG_STRING) continue;
            rlen += snprintf(req + rlen, req_cap - rlen, "%s: %s\r\n",
                             req_headers.mval->keys[i], req_headers.mval->vals[i].sval->data);
            if (strcasecmp(req_headers.mval->keys[i], "content-type") == 0) has_ct = 1;
            if (strcasecmp(req_headers.mval->keys[i], "accept-encoding") == 0) has_accept_enc = 1;
        }
    }
    if (body && body_len > 0 && !has_ct) {
        rlen += snprintf(req + rlen, req_cap - rlen, "Content-Type: application/json\r\n");
    }
    if (body && body_len > 0) {
        rlen += snprintf(req + rlen, req_cap - rlen, "Content-Length: %d\r\n", body_len);
    }
    if (!has_accept_enc) {
        rlen += snprintf(req + rlen, req_cap - rlen, "Accept-Encoding: gzip\r\n");
    }
    rlen += snprintf(req + rlen, req_cap - rlen, "Connection: close\r\n\r\n");
    if (body && body_len > 0) {
        memcpy(req + rlen, body, body_len);
        rlen += body_len;
    }

    if (http_io_write_all(&conn, req, rlen) < 0) {
        free(req); http_conn_close(&conn);
        return a_err(a_string("http: send failed"));
    }
    free(req);

    /* Read response */
    size_t resp_cap = 8192, resp_len = 0;
    char* resp_buf = malloc(resp_cap);
    char* header_end = NULL;

    while (!header_end) {
        if (resp_len >= resp_cap - 1) { resp_cap *= 2; resp_buf = realloc(resp_buf, resp_cap); }
        int n = http_io_read(&conn, resp_buf + resp_len, (int)(resp_cap - resp_len - 1));
        if (n <= 0) break;
        resp_len += n;
        resp_buf[resp_len] = '\0';
        header_end = strstr(resp_buf, "\r\n\r\n");
    }

    if (!header_end) {
        if (resp_len == 0) { free(resp_buf); http_conn_close(&conn); return a_err(a_string("http: no response")); }
        header_end = resp_buf + resp_len;
    }

    /* Parse status line */
    int http_status = 0;
    char* first_space = strchr(resp_buf, ' ');
    if (first_space) http_status = atoi(first_space + 1);

    /* Parse headers */
    AValue resp_headers = a_map_new(0);
    int content_length = -1;
    int chunked = 0;
    int gzipped = 0;

    char* hdr_start = strstr(resp_buf, "\r\n");
    if (hdr_start) {
        hdr_start += 2;
        while (hdr_start < header_end) {
            char* eol = strstr(hdr_start, "\r\n");
            if (!eol || eol == hdr_start) break;
            char* colon = memchr(hdr_start, ':', eol - hdr_start);
            if (colon) {
                int klen = (int)(colon - hdr_start);
                char key[256];
                if (klen >= (int)sizeof(key)) klen = (int)sizeof(key) - 1;
                for (int j = 0; j < klen; j++) key[j] = (char)tolower((unsigned char)hdr_start[j]);
                key[klen] = '\0';
                char* val = colon + 1;
                while (val < eol && *val == ' ') val++;
                int vlen = (int)(eol - val);
                resp_headers = a_map_set(resp_headers, a_string(key), a_string_len(val, vlen));
                if (strcmp(key, "content-length") == 0) content_length = atoi(val);
                if (strcmp(key, "transfer-encoding") == 0 && strstr(val, "chunked")) chunked = 1;
                if (strcmp(key, "content-encoding") == 0 && strstr(val, "gzip")) gzipped = 1;
            }
            hdr_start = eol + 2;
        }
    }

    /* Read body */
    char* body_start = header_end + 4;
    int already = (int)(resp_len - (body_start - resp_buf));
    if (already < 0) already = 0;

    char* body_buf = NULL;
    int body_total = 0;

    if (chunked) {
        size_t bcap = 8192;
        body_buf = malloc(bcap);
        body_total = 0;
        char* chunk_data = malloc(resp_cap);
        int chunk_len = already;
        if (already > 0) memcpy(chunk_data, body_start, already);

        for (;;) {
            char* crlf = NULL;
            while (!(crlf = strstr(chunk_data, "\r\n")) || crlf == chunk_data) {
                if (chunk_len >= (int)resp_cap - 1) { resp_cap *= 2; chunk_data = realloc(chunk_data, resp_cap); }
                int n = http_io_read(&conn, chunk_data + chunk_len, (int)(resp_cap - chunk_len - 1));
                if (n <= 0) goto chunk_done;
                chunk_len += n;
                chunk_data[chunk_len] = '\0';
                crlf = strstr(chunk_data, "\r\n");
            }
            long csize = strtol(chunk_data, NULL, 16);
            if (csize <= 0) break;
            int off = (int)(crlf - chunk_data) + 2;
            while (chunk_len - off < (int)csize + 2) {
                if (chunk_len >= (int)resp_cap - 1) { resp_cap *= 2; chunk_data = realloc(chunk_data, resp_cap); }
                int n = http_io_read(&conn, chunk_data + chunk_len, (int)(resp_cap - chunk_len - 1));
                if (n <= 0) break;
                chunk_len += n;
                chunk_data[chunk_len] = '\0';
            }
            while (body_total + (int)csize > (int)bcap) { bcap *= 2; body_buf = realloc(body_buf, bcap); }
            memcpy(body_buf + body_total, chunk_data + off, csize);
            body_total += (int)csize;
            int consumed = off + (int)csize + 2;
            memmove(chunk_data, chunk_data + consumed, chunk_len - consumed);
            chunk_len -= consumed;
            chunk_data[chunk_len] = '\0';
        }
        chunk_done:
        free(chunk_data);
    } else if (content_length >= 0) {
        body_buf = malloc(content_length + 1);
        if (already > 0) { memcpy(body_buf, body_start, already > content_length ? content_length : already); }
        body_total = already > content_length ? content_length : already;
        while (body_total < content_length) {
            int n = http_io_read(&conn, body_buf + body_total, content_length - body_total);
            if (n <= 0) break;
            body_total += n;
        }
    } else {
        size_t bcap = 8192;
        body_buf = malloc(bcap);
        if (already > 0) { memcpy(body_buf, body_start, already); }
        body_total = already;
        for (;;) {
            if (body_total >= (int)bcap - 1) { bcap *= 2; body_buf = realloc(body_buf, bcap); }
            int n = http_io_read(&conn, body_buf + body_total, (int)(bcap - body_total - 1));
            if (n <= 0) break;
            body_total += n;
        }
    }
    free(resp_buf);
    http_conn_close(&conn);

    /* gzip decompress if needed */
    if (gzipped && body_buf && body_total > 0) {
        AValue gz_in = a_string_len(body_buf, body_total);
        AValue decompressed = a_compress_gunzip(gz_in);
        free(body_buf);
        if (decompressed.tag == TAG_STRING) {
            body_buf = malloc(decompressed.sval->len + 1);
            memcpy(body_buf, decompressed.sval->data, decompressed.sval->len);
            body_total = decompressed.sval->len;
        } else {
            body_buf = malloc(1);
            body_total = 0;
        }
    }

    AValue result = a_map_new(0);
    result = a_map_set(result, a_string("status"), a_int(http_status));
    result = a_map_set(result, a_string("body"), a_string_len(body_buf ? body_buf : "", body_total));
    result = a_map_set(result, a_string("headers"), resp_headers);
    if (body_buf) free(body_buf);
    return result;
}

/* --- Public HTTP API --- */

AValue a_http_get(AValue url, AValue headers) {
    if (url.tag != TAG_STRING) return a_err(a_string("http.get: expected string url"));
    return http_request_inprocess("GET", url.sval->data, NULL, 0, headers);
}

AValue a_http_post(AValue url, AValue body, AValue headers) {
    if (url.tag != TAG_STRING) return a_err(a_string("http.post: expected string url"));
    const char* b = (body.tag == TAG_STRING) ? body.sval->data : "";
    int blen = (body.tag == TAG_STRING) ? body.sval->len : 0;
    return http_request_inprocess("POST", url.sval->data, b, blen, headers);
}

AValue a_http_put(AValue url, AValue body, AValue headers) {
    if (url.tag != TAG_STRING) return a_err(a_string("http.put: expected string url"));
    const char* b = (body.tag == TAG_STRING) ? body.sval->data : "";
    int blen = (body.tag == TAG_STRING) ? body.sval->len : 0;
    return http_request_inprocess("PUT", url.sval->data, b, blen, headers);
}

AValue a_http_patch(AValue url, AValue body, AValue headers) {
    if (url.tag != TAG_STRING) return a_err(a_string("http.patch: expected string url"));
    const char* b = (body.tag == TAG_STRING) ? body.sval->data : "";
    int blen = (body.tag == TAG_STRING) ? body.sval->len : 0;
    return http_request_inprocess("PATCH", url.sval->data, b, blen, headers);
}

AValue a_http_delete(AValue url, AValue headers) {
    if (url.tag != TAG_STRING) return a_err(a_string("http.delete: expected string url"));
    return http_request_inprocess("DELETE", url.sval->data, NULL, 0, headers);
}

/* --- HTTP server (POSIX sockets) --- */

#include <arpa/inet.h>
#include <signal.h>

static int http_recv_all(int fd, char* buf, int cap) {
    int total = 0;
    while (total < cap - 1) {
        int n = (int)recv(fd, buf + total, cap - 1 - total, 0);
        if (n <= 0) break;
        total += n;
        buf[total] = '\0';
        if (strstr(buf, "\r\n\r\n")) break;
    }
    return total;
}

static AValue http_parse_request(const char* raw, int raw_len, int fd) {
    AValue req = a_map_new(0);
    const char* end_of_line = strstr(raw, "\r\n");
    if (!end_of_line) {
        req = a_map_set(req, a_string("method"), a_string(""));
        req = a_map_set(req, a_string("path"), a_string(""));
        req = a_map_set(req, a_string("headers"), a_map_new(0));
        req = a_map_set(req, a_string("body"), a_string(""));
        return req;
    }

    char first_line[2048];
    int fl_len = (int)(end_of_line - raw);
    if (fl_len >= (int)sizeof(first_line)) fl_len = (int)sizeof(first_line) - 1;
    memcpy(first_line, raw, fl_len);
    first_line[fl_len] = '\0';

    char method[16] = "", path[2048] = "";
    sscanf(first_line, "%15s %2047s", method, path);
    req = a_map_set(req, a_string("method"), a_string(method));
    req = a_map_set(req, a_string("path"), a_string(path));

    AValue hdrs = a_map_new(0);
    int content_length = 0;
    const char* line = end_of_line + 2;
    while (line < raw + raw_len) {
        const char* next = strstr(line, "\r\n");
        if (!next || next == line) break;
        int llen = (int)(next - line);
        char hline[4096];
        if (llen >= (int)sizeof(hline)) llen = (int)sizeof(hline) - 1;
        memcpy(hline, line, llen);
        hline[llen] = '\0';

        char* colon = strchr(hline, ':');
        if (colon) {
            *colon = '\0';
            char* val = colon + 1;
            while (*val == ' ') val++;
            for (char* p = hline; *p; p++) *p = (char)tolower((unsigned char)*p);
            hdrs = a_map_set(hdrs, a_string(hline), a_string(val));
            if (strcmp(hline, "content-length") == 0)
                content_length = atoi(val);
        }
        line = next + 2;
    }
    req = a_map_set(req, a_string("headers"), hdrs);

    const char* body_start = strstr(raw, "\r\n\r\n");
    AValue body = a_string("");
    if (body_start && content_length > 0) {
        body_start += 4;
        int already = raw_len - (int)(body_start - raw);
        if (already >= content_length) {
            body = a_string_len(body_start, content_length);
        } else {
            char* bbuf = malloc(content_length + 1);
            memcpy(bbuf, body_start, already);
            int got = already;
            while (got < content_length) {
                int n = (int)recv(fd, bbuf + got, content_length - got, 0);
                if (n <= 0) break;
                got += n;
            }
            bbuf[got] = '\0';
            body = a_string_len(bbuf, got);
            free(bbuf);
        }
    }
    req = a_map_set(req, a_string("body"), body);
    return req;
}

static void http_send_response(int fd, AValue resp) {
    int status = 200;
    const char* resp_body = "";
    int body_len = 0;

    if (resp.tag == TAG_MAP) {
        AValue s = a_map_get(resp, a_string("status"));
        if (s.tag == TAG_INT) status = (int)s.ival;
        AValue b = a_map_get(resp, a_string("body"));
        if (b.tag == TAG_STRING) {
            resp_body = b.sval->data;
            body_len = b.sval->len;
        }
    }

    const char* reason = "OK";
    if (status == 201) reason = "Created";
    else if (status == 204) reason = "No Content";
    else if (status == 301) reason = "Moved Permanently";
    else if (status == 302) reason = "Found";
    else if (status == 304) reason = "Not Modified";
    else if (status == 400) reason = "Bad Request";
    else if (status == 401) reason = "Unauthorized";
    else if (status == 403) reason = "Forbidden";
    else if (status == 404) reason = "Not Found";
    else if (status == 405) reason = "Method Not Allowed";
    else if (status == 500) reason = "Internal Server Error";

    char header[4096];
    int hlen = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\nContent-Length: %d\r\nConnection: close\r\n",
        status, reason, body_len);

    if (resp.tag == TAG_MAP) {
        AValue rh = a_map_get(resp, a_string("headers"));
        if (rh.tag == TAG_MAP) {
            for (int i = 0; i < rh.mval->len && hlen < (int)sizeof(header) - 256; i++) {
                hlen += snprintf(header + hlen, sizeof(header) - hlen,
                    "%s: %s\r\n", rh.mval->keys[i], rh.mval->vals[i].sval->data);
            }
        }
    }
    hlen += snprintf(header + hlen, sizeof(header) - hlen, "\r\n");

    send(fd, header, hlen, 0);
    if (body_len > 0) send(fd, resp_body, body_len, 0);
}

AValue a_http_serve(AValue port, AValue handler) {
    if (port.tag != TAG_INT) return a_err(a_string("http.serve: port must be int"));
    if (handler.tag != TAG_CLOSURE) return a_err(a_string("http.serve: handler must be a function"));

    signal(SIGPIPE, SIG_IGN);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) return a_err(a_string("http.serve: socket failed"));

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((uint16_t)port.ival);

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(srv);
        return a_err(a_string("http.serve: bind failed"));
    }
    if (listen(srv, 64) < 0) {
        close(srv);
        return a_err(a_string("http.serve: listen failed"));
    }

    fprintf(stderr, "listening on http://0.0.0.0:%d\n", (int)port.ival);

    for (;;) {
        struct sockaddr_in client;
        socklen_t clen = sizeof(client);
        int fd = accept(srv, (struct sockaddr*)&client, &clen);
        if (fd < 0) continue;

        char buf[65536];
        int n = http_recv_all(fd, buf, sizeof(buf));
        if (n <= 0) { close(fd); continue; }

        AValue req = http_parse_request(buf, n, fd);
        AValue resp = a_closure_call(handler, 1, req);

        if (resp.tag != TAG_MAP) {
            AValue err_resp = a_map_new(0);
            err_resp = a_map_set(err_resp, a_string("status"), a_int(500));
            err_resp = a_map_set(err_resp, a_string("body"), a_string("Internal Server Error"));
            http_send_response(fd, err_resp);
        } else {
            http_send_response(fd, resp);
        }

        close(fd);
        a_release(req);
        a_release(resp);
    }

    close(srv);
    return a_void();
}

/* --- HTTP static file server --- */

static const char* http_content_type(const char* path) {
    const char* dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0) return "text/html";
    if (strcmp(dot, ".css") == 0) return "text/css";
    if (strcmp(dot, ".js") == 0) return "application/javascript";
    if (strcmp(dot, ".json") == 0) return "application/json";
    if (strcmp(dot, ".txt") == 0) return "text/plain";
    if (strcmp(dot, ".xml") == 0) return "application/xml";
    if (strcmp(dot, ".png") == 0) return "image/png";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(dot, ".gif") == 0) return "image/gif";
    if (strcmp(dot, ".svg") == 0) return "image/svg+xml";
    if (strcmp(dot, ".ico") == 0) return "image/x-icon";
    if (strcmp(dot, ".wasm") == 0) return "application/wasm";
    return "application/octet-stream";
}

AValue a_http_serve_static(AValue port, AValue dir) {
    if (port.tag != TAG_INT) return a_err(a_string("http.serve_static: port must be int"));
    if (dir.tag != TAG_STRING) return a_err(a_string("http.serve_static: dir must be string"));

    signal(SIGPIPE, SIG_IGN);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) return a_err(a_string("http.serve_static: socket failed"));

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((uint16_t)port.ival);

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(srv);
        return a_err(a_string("http.serve_static: bind failed"));
    }
    if (listen(srv, 64) < 0) {
        close(srv);
        return a_err(a_string("http.serve_static: listen failed"));
    }

    fprintf(stderr, "serving %s on http://0.0.0.0:%d\n", dir.sval->data, (int)port.ival);

    for (;;) {
        struct sockaddr_in client;
        socklen_t clen = sizeof(client);
        int fd = accept(srv, (struct sockaddr*)&client, &clen);
        if (fd < 0) continue;

        char buf[65536];
        int n = http_recv_all(fd, buf, sizeof(buf));
        if (n <= 0) { close(fd); continue; }

        char method[16] = "", req_path[2048] = "";
        sscanf(buf, "%15s %2047s", method, req_path);

        if (strstr(req_path, "..")) {
            const char* r = "HTTP/1.1 403 Forbidden\r\nContent-Length: 9\r\nConnection: close\r\n\r\nForbidden";
            send(fd, r, strlen(r), 0);
            close(fd);
            continue;
        }

        char filepath[4096];
        const char* p = req_path;
        if (*p == '/') p++;
        if (*p == '\0') p = "index.html";
        snprintf(filepath, sizeof(filepath), "%s/%s", dir.sval->data, p);

        int plen = (int)strlen(filepath);
        if (plen > 0 && filepath[plen - 1] == '/') {
            snprintf(filepath + plen, sizeof(filepath) - plen, "index.html");
        }

        FILE* f = fopen(filepath, "rb");
        if (!f) {
            const char* r = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\nConnection: close\r\n\r\nNot Found";
            send(fd, r, strlen(r), 0);
            close(fd);
            continue;
        }

        fseek(f, 0, SEEK_END);
        long fsz = ftell(f);
        fseek(f, 0, SEEK_SET);
        char* fbuf = malloc(fsz);
        size_t fread_n = fread(fbuf, 1, fsz, f);
        fclose(f);

        const char* ctype = http_content_type(filepath);
        char hdr[512];
        int hlen = snprintf(hdr, sizeof(hdr),
            "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n",
            ctype, (long)fread_n);
        send(fd, hdr, hlen, 0);
        send(fd, fbuf, fread_n, 0);
        free(fbuf);
        close(fd);
    }

    close(srv);
    return a_void();
}

/* --- Database (SQLite) --- */

#include "sqlite3.h"

AValue a_db_open(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("db.open: expected string path"));
    sqlite3* db = NULL;
    int rc = sqlite3_open(path.sval->data, &db);
    if (rc != SQLITE_OK) {
        const char* msg = sqlite3_errmsg(db);
        AValue err = a_err(a_string(msg ? msg : "db.open failed"));
        sqlite3_close(db);
        return err;
    }
    return a_ptr(db);
}

AValue a_db_close(AValue db) {
    if (db.tag != TAG_PTR || !db.pval) return a_err(a_string("db.close: expected db handle"));
    sqlite3_close((sqlite3*)db.pval);
    return a_void();
}

AValue a_db_exec(AValue db, AValue sql) {
    if (db.tag != TAG_PTR || !db.pval) return a_err(a_string("db.exec: expected db handle"));
    if (sql.tag != TAG_STRING) return a_err(a_string("db.exec: expected string sql"));
    char* errmsg = NULL;
    int rc = sqlite3_exec((sqlite3*)db.pval, sql.sval->data, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        AValue err = a_err(a_string(errmsg ? errmsg : "db.exec failed"));
        sqlite3_free(errmsg);
        return err;
    }
    return a_ok(a_void());
}

AValue a_db_query(AValue db, AValue sql, AValue params) {
    if (db.tag != TAG_PTR || !db.pval) return a_err(a_string("db.query: expected db handle"));
    if (sql.tag != TAG_STRING) return a_err(a_string("db.query: expected string sql"));

    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2((sqlite3*)db.pval, sql.sval->data, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        const char* msg = sqlite3_errmsg((sqlite3*)db.pval);
        return a_err(a_string(msg ? msg : "db.query: prepare failed"));
    }

    if (params.tag == TAG_ARRAY && params.aval) {
        for (int i = 0; i < params.aval->len; i++) {
            AValue p = params.aval->items[i];
            int idx = i + 1;
            if (p.tag == TAG_INT) sqlite3_bind_int64(stmt, idx, p.ival);
            else if (p.tag == TAG_FLOAT) sqlite3_bind_double(stmt, idx, p.fval);
            else if (p.tag == TAG_STRING) sqlite3_bind_text(stmt, idx, p.sval->data, p.sval->len, SQLITE_TRANSIENT);
            else if (p.tag == TAG_BOOL) sqlite3_bind_int(stmt, idx, p.bval ? 1 : 0);
            else sqlite3_bind_null(stmt, idx);
        }
    }

    AValue rows = a_array_new(0);
    int ncols = sqlite3_column_count(stmt);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        AValue row = a_map_new(0);
        for (int c = 0; c < ncols; c++) {
            const char* name = sqlite3_column_name(stmt, c);
            int ctype = sqlite3_column_type(stmt, c);
            AValue val;
            if (ctype == SQLITE_INTEGER) val = a_int(sqlite3_column_int64(stmt, c));
            else if (ctype == SQLITE_FLOAT) val = a_float(sqlite3_column_double(stmt, c));
            else if (ctype == SQLITE_TEXT) val = a_string((const char*)sqlite3_column_text(stmt, c));
            else if (ctype == SQLITE_NULL) val = a_void();
            else val = a_string((const char*)sqlite3_column_text(stmt, c));
            row = a_map_set(row, a_string(name), val);
        }
        rows = a_array_push(rows, row);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        const char* msg = sqlite3_errmsg((sqlite3*)db.pval);
        return a_err(a_string(msg ? msg : "db.query: step failed"));
    }

    return rows;
}

/* --- Closures --- */

AValue a_closure(AClosureFn fn, AValue env) {
    AClosure* c = malloc(sizeof(AClosure));
    c->rc = 1;
    c->fn = fn;
    c->env = env;
    if (env.tag == TAG_ARRAY || env.tag == TAG_STRING || env.tag == TAG_MAP)
        a_retain(env);
    AValue v;
    v.tag = TAG_CLOSURE;
    v.cval = c;
    return v;
}

AValue a_closure_call_arr(AValue closure, int argc, AValue* argv) {
    if (closure.tag != TAG_CLOSURE || !closure.cval) return a_void();
    return closure.cval->fn(closure.cval->env, argc, argv);
}

AValue a_closure_call(AValue closure, int argc, ...) {
    AValue args[argc > 0 ? argc : 1];
    va_list ap;
    va_start(ap, argc);
    for (int i = 0; i < argc; i++) args[i] = va_arg(ap, AValue);
    va_end(ap);
    return a_closure_call_arr(closure, argc, args);
}

/* --- Higher-order functions --- */

AValue a_hof_map(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    AValue result = a_array_new(0);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        AValue val = a_closure_call(fn, 1, item);
        result = a_array_push(result, val);
    }
    return result;
}

AValue a_hof_filter(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    AValue result = a_array_new(0);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        AValue pred = a_closure_call(fn, 1, item);
        if (a_truthy(pred)) result = a_array_push(result, item);
    }
    return result;
}

AValue a_hof_reduce(AValue arr, AValue init, AValue fn) {
    int n = a_ilen(arr);
    AValue acc = init;
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        acc = a_closure_call(fn, 2, acc, item);
    }
    return acc;
}

AValue a_hof_each(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        a_closure_call(fn, 1, item);
    }
    return a_void();
}

static int closure_cmp_fn_global_set = 0;
static AValue closure_cmp_fn_global;

static int closure_sort_cmp(const void* a, const void* b) {
    AValue va = *(const AValue*)a;
    AValue vb = *(const AValue*)b;
    AValue result = a_closure_call(closure_cmp_fn_global, 2, va, vb);
    if (result.tag == TAG_INT) return (int)result.ival;
    if (result.tag == TAG_FLOAT) return result.fval < 0 ? -1 : (result.fval > 0 ? 1 : 0);
    return 0;
}

AValue a_hof_sort_by(AValue arr, AValue fn) {
    if (arr.tag != TAG_ARRAY) return arr;
    int n = arr.aval->len;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * (n > 0 ? n : 1));
    memcpy(na->items, arr.aval->items, sizeof(AValue) * n);
    closure_cmp_fn_global = fn;
    closure_cmp_fn_global_set = 1;
    qsort(na->items, n, sizeof(AValue), closure_sort_cmp);
    closure_cmp_fn_global_set = 0;
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

AValue a_hof_find(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        if (a_truthy(a_closure_call(fn, 1, item))) return a_ok(item);
    }
    return a_err(a_string("not found"));
}

AValue a_hof_any(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        if (a_truthy(a_closure_call(fn, 1, item))) return a_bool(1);
    }
    return a_bool(0);
}

AValue a_hof_all(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        if (!a_truthy(a_closure_call(fn, 1, item))) return a_bool(0);
    }
    return a_bool(1);
}

AValue a_hof_flat_map(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    AValue result = a_array_new(0);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        AValue sub = a_closure_call(fn, 1, item);
        if (sub.tag == TAG_ARRAY) {
            for (int j = 0; j < sub.aval->len; j++)
                result = a_array_push(result, sub.aval->items[j]);
        } else {
            result = a_array_push(result, sub);
        }
    }
    return result;
}

AValue a_hof_min_by(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    if (n == 0) return a_err(a_string("empty"));
    AValue best = a_array_get(arr, a_int(0));
    AValue best_key = a_closure_call(fn, 1, best);
    for (int i = 1; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        AValue key = a_closure_call(fn, 1, item);
        if (a_truthy(a_lt(key, best_key))) { best = item; best_key = key; }
    }
    return a_ok(best);
}

AValue a_hof_max_by(AValue arr, AValue fn) {
    int n = a_ilen(arr);
    if (n == 0) return a_err(a_string("empty"));
    AValue best = a_array_get(arr, a_int(0));
    AValue best_key = a_closure_call(fn, 1, best);
    for (int i = 1; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        AValue key = a_closure_call(fn, 1, item);
        if (a_truthy(a_gt(key, best_key))) { best = item; best_key = key; }
    }
    return a_ok(best);
}

/* --- Array utilities --- */

AValue a_enumerate(AValue arr) {
    int n = a_ilen(arr);
    AValue result = a_array_new(0);
    for (int i = 0; i < n; i++) {
        AValue pair = a_array_new(2, a_int(i), a_array_get(arr, a_int(i)));
        result = a_array_push(result, pair);
    }
    return result;
}

AValue a_zip(AValue a, AValue b) {
    int na = a_ilen(a), nb = a_ilen(b);
    int n = na < nb ? na : nb;
    AValue result = a_array_new(0);
    for (int i = 0; i < n; i++) {
        AValue pair = a_array_new(2, a_array_get(a, a_int(i)), a_array_get(b, a_int(i)));
        result = a_array_push(result, pair);
    }
    return result;
}

AValue a_take(AValue arr, AValue n) {
    if (arr.tag != TAG_ARRAY || n.tag != TAG_INT) return a_array_new(0);
    int count = (int)n.ival;
    if (count > arr.aval->len) count = arr.aval->len;
    if (count <= 0) return a_array_new(0);
    return a_array_slice(arr, a_int(0), a_int(count));
}

AValue a_drop(AValue arr, AValue n) {
    if (arr.tag != TAG_ARRAY || n.tag != TAG_INT) return a_array_new(0);
    int start = (int)n.ival;
    if (start >= arr.aval->len) return a_array_new(0);
    if (start < 0) start = 0;
    return a_array_slice(arr, a_int(start), a_int(arr.aval->len));
}

AValue a_unique(AValue arr) {
    int n = a_ilen(arr);
    AValue result = a_array_new(0);
    for (int i = 0; i < n; i++) {
        AValue item = a_array_get(arr, a_int(i));
        if (!a_truthy(a_contains(result, item)))
            result = a_array_push(result, item);
    }
    return result;
}

AValue a_chunk(AValue arr, AValue n) {
    if (arr.tag != TAG_ARRAY || n.tag != TAG_INT || n.ival <= 0) return a_array_new(0);
    int sz = (int)n.ival;
    int total = arr.aval->len;
    AValue result = a_array_new(0);
    for (int i = 0; i < total; i += sz) {
        int end = i + sz;
        if (end > total) end = total;
        result = a_array_push(result, a_array_slice(arr, a_int(i), a_int(end)));
    }
    return result;
}

/* --- Garbage collector --- */

static GCNode* gc_alloc_list = NULL;
static int gc_alloc_count = 0;
static int gc_threshold = 4096;

#define GC_ROOT_STACK_MAX 8192
static AValue* gc_root_stack[GC_ROOT_STACK_MAX];
static int gc_root_sp = 0;

static void gc_register(GCType type, void* obj) {
    GCNode* node = malloc(sizeof(GCNode));
    node->next = gc_alloc_list;
    node->type = type;
    node->mark = 0;
    node->obj = obj;
    gc_alloc_list = node;
    gc_alloc_count++;
}

void a_gc_push_root(AValue* root) {
    if (gc_root_sp < GC_ROOT_STACK_MAX)
        gc_root_stack[gc_root_sp++] = root;
}

void a_gc_pop_roots(int n) {
    gc_root_sp -= n;
    if (gc_root_sp < 0) gc_root_sp = 0;
}

static GCNode* gc_find_node(void* obj) {
    for (GCNode* n = gc_alloc_list; n; n = n->next)
        if (n->obj == obj) return n;
    return NULL;
}

static void gc_mark_value(AValue v) {
    void* obj = NULL;
    if (v.tag == TAG_STRING && v.sval) obj = v.sval;
    else if (v.tag == TAG_ARRAY && v.aval) obj = v.aval;
    else if (v.tag == TAG_MAP && v.mval) obj = v.mval;
    else if (v.tag == TAG_CLOSURE && v.cval) obj = v.cval;
    else return;

    GCNode* node = gc_find_node(obj);
    if (!node || node->mark) return;
    node->mark = 1;

    if (v.tag == TAG_ARRAY) {
        for (int i = 0; i < v.aval->len; i++)
            gc_mark_value(v.aval->items[i]);
    } else if (v.tag == TAG_MAP) {
        for (int i = 0; i < v.mval->len; i++)
            gc_mark_value(v.mval->vals[i]);
    } else if (v.tag == TAG_CLOSURE) {
        gc_mark_value(v.cval->env);
    }
}

static void gc_mark_roots(void) {
    for (int i = 0; i < gc_root_sp; i++)
        gc_mark_value(*gc_root_stack[i]);
}

static void gc_free_object(GCNode* node) {
    switch (node->type) {
        case GC_STRING:
            free(node->obj);
            break;
        case GC_ARRAY: {
            AArray* a = (AArray*)node->obj;
            free(a->items);
            free(a);
            break;
        }
        case GC_MAP: {
            AMap* m = (AMap*)node->obj;
            for (int i = 0; i < m->len; i++) free(m->keys[i]);
            free(m->keys);
            free(m->vals);
            free(m);
            break;
        }
        case GC_CLOSURE:
            free(node->obj);
            break;
    }
}

static void gc_sweep(void) {
    GCNode** p = &gc_alloc_list;
    while (*p) {
        if (!(*p)->mark) {
            GCNode* dead = *p;
            *p = dead->next;
            gc_alloc_count--;
            gc_free_object(dead);
            free(dead);
        } else {
            (*p)->mark = 0;
            p = &(*p)->next;
        }
    }
}

void a_gc_collect(void) {
    gc_mark_roots();
    gc_sweep();
    if (gc_alloc_count > gc_threshold / 2)
        gc_threshold *= 2;
}

/* --- Arena allocator --- */

AArena* a_arena_new(int initial_size) {
    AArena* a = malloc(sizeof(AArena));
    a->size = initial_size > 64 ? initial_size : 64;
    a->buf = malloc(a->size);
    a->pos = 0;
    return a;
}

void a_arena_free(AArena* arena) {
    if (!arena) return;
    free(arena->buf);
    free(arena);
}

void* a_arena_alloc(AArena* arena, int bytes) {
    int aligned = (bytes + 7) & ~7;
    if (arena->pos + aligned > arena->size) {
        while (arena->pos + aligned > arena->size) arena->size *= 2;
        arena->buf = realloc(arena->buf, arena->size);
    }
    void* ptr = arena->buf + arena->pos;
    arena->pos += aligned;
    return ptr;
}

int a_arena_save(AArena* arena) {
    return arena->pos;
}

void a_arena_restore(AArena* arena, int saved_pos) {
    arena->pos = saved_pos;
}

/* --- Compression (miniz) --- */

AValue a_compress_deflate(AValue data) {
    if (data.tag != TAG_STRING) return a_err(a_string("compress.deflate: expected string"));
    if (data.sval->len == 0) {
        size_t out_len = 0;
        void* out = tdefl_compress_mem_to_heap("", 0, &out_len, TDEFL_DEFAULT_MAX_PROBES);
        if (!out) return a_string_len("", 0);
        AValue result = a_string_len((const char*)out, (int)out_len);
        mz_free(out);
        return result;
    }
    size_t out_len = 0;
    void* out = tdefl_compress_mem_to_heap(
        data.sval->data, (size_t)data.sval->len, &out_len,
        TDEFL_DEFAULT_MAX_PROBES
    );
    if (!out) return a_err(a_string("compress.deflate: compression failed"));
    AValue result = a_string_len((const char*)out, (int)out_len);
    mz_free(out);
    return result;
}

AValue a_compress_inflate(AValue data) {
    if (data.tag != TAG_STRING) return a_err(a_string("compress.inflate: expected string"));
    if (data.sval->len == 0) return a_string_len("", 0);
    size_t out_len = 0;
    void* out = tinfl_decompress_mem_to_heap(
        data.sval->data, (size_t)data.sval->len, &out_len, 0
    );
    if (!out && out_len == 0) return a_string_len("", 0);
    if (!out) return a_err(a_string("compress.inflate: decompression failed"));
    AValue result = a_string_len((const char*)out, (int)out_len);
    mz_free(out);
    return result;
}

AValue a_compress_gzip(AValue data) {
    if (data.tag != TAG_STRING) return a_err(a_string("compress.gzip: expected string"));
    if (data.sval->len == 0) {
        size_t dl = 0;
        void* d = tdefl_compress_mem_to_heap("", 0, &dl, TDEFL_DEFAULT_MAX_PROBES);
        size_t gz_len = 10 + (d ? dl : 0) + 8;
        uint8_t* gz = (uint8_t*)malloc(gz_len);
        gz[0]=0x1f; gz[1]=0x8b; gz[2]=8; gz[3]=0;
        gz[4]=gz[5]=gz[6]=gz[7]=0; gz[8]=0; gz[9]=0xff;
        if (d) { memcpy(gz+10, d, dl); mz_free(d); }
        memset(gz+10+(d?dl:0), 0, 8);
        AValue result = a_string_len((const char*)gz, (int)gz_len);
        free(gz);
        return result;
    }
    const uint8_t* src = (const uint8_t*)data.sval->data;
    size_t src_len = (size_t)data.sval->len;

    size_t deflated_len = 0;
    void* deflated = tdefl_compress_mem_to_heap(
        src, src_len, &deflated_len, TDEFL_DEFAULT_MAX_PROBES
    );
    if (!deflated) return a_err(a_string("compress.gzip: compression failed"));

    mz_ulong crc = mz_crc32(MZ_CRC32_INIT, src, src_len);

    /* gzip = 10-byte header + raw deflate + 4-byte CRC32 + 4-byte size */
    size_t gz_len = 10 + deflated_len + 8;
    uint8_t* gz = (uint8_t*)malloc(gz_len);
    if (!gz) { mz_free(deflated); return a_err(a_string("compress.gzip: out of memory")); }

    gz[0] = 0x1f; gz[1] = 0x8b;   /* magic */
    gz[2] = 8;                      /* method: deflate */
    gz[3] = 0;                      /* flags */
    gz[4] = gz[5] = gz[6] = gz[7] = 0; /* mtime */
    gz[8] = 0;                      /* xfl */
    gz[9] = 0xff;                   /* os: unknown */

    memcpy(gz + 10, deflated, deflated_len);
    mz_free(deflated);

    gz[10 + deflated_len + 0] = (uint8_t)(crc);
    gz[10 + deflated_len + 1] = (uint8_t)(crc >> 8);
    gz[10 + deflated_len + 2] = (uint8_t)(crc >> 16);
    gz[10 + deflated_len + 3] = (uint8_t)(crc >> 24);
    gz[10 + deflated_len + 4] = (uint8_t)(src_len);
    gz[10 + deflated_len + 5] = (uint8_t)(src_len >> 8);
    gz[10 + deflated_len + 6] = (uint8_t)(src_len >> 16);
    gz[10 + deflated_len + 7] = (uint8_t)(src_len >> 24);

    AValue result = a_string_len((const char*)gz, (int)gz_len);
    free(gz);
    return result;
}

AValue a_compress_gunzip(AValue data) {
    if (data.tag != TAG_STRING) return a_err(a_string("compress.gunzip: expected string"));
    const uint8_t* src = (const uint8_t*)data.sval->data;
    size_t src_len = (size_t)data.sval->len;

    if (src_len < 18 || src[0] != 0x1f || src[1] != 0x8b)
        return a_err(a_string("compress.gunzip: not gzip data"));
    if (src[2] != 8)
        return a_err(a_string("compress.gunzip: unsupported compression method"));

    /* Skip past the 10-byte header + optional extra fields */
    size_t pos = 10;
    uint8_t flags = src[3];
    if (flags & 0x04) { /* FEXTRA */
        if (pos + 2 > src_len) return a_err(a_string("compress.gunzip: truncated"));
        size_t xlen = src[pos] | ((size_t)src[pos+1] << 8);
        pos += 2 + xlen;
    }
    if (flags & 0x08) { /* FNAME */
        while (pos < src_len && src[pos]) pos++;
        pos++;
    }
    if (flags & 0x10) { /* FCOMMENT */
        while (pos < src_len && src[pos]) pos++;
        pos++;
    }
    if (flags & 0x02) pos += 2; /* FHCRC */

    if (pos >= src_len - 8)
        return a_err(a_string("compress.gunzip: truncated"));

    size_t deflated_len = src_len - pos - 8;
    uint32_t expected_crc = (uint32_t)src[src_len-8] | ((uint32_t)src[src_len-7]<<8) |
                            ((uint32_t)src[src_len-6]<<16) | ((uint32_t)src[src_len-5]<<24);
    uint32_t expected_size = (uint32_t)src[src_len-4] | ((uint32_t)src[src_len-3]<<8) |
                             ((uint32_t)src[src_len-2]<<16) | ((uint32_t)src[src_len-1]<<24);

    size_t out_len = 0;
    void* out = tinfl_decompress_mem_to_heap(
        src + pos, deflated_len, &out_len, 0
    );
    if (!out && out_len != 0) return a_err(a_string("compress.gunzip: decompression failed"));

    uint32_t actual_crc = out ? (uint32_t)mz_crc32(MZ_CRC32_INIT, (const uint8_t*)out, out_len)
                              : (uint32_t)mz_crc32(MZ_CRC32_INIT, NULL, 0);
    if (actual_crc != expected_crc || ((uint32_t)out_len != expected_size)) {
        if (out) mz_free(out);
        return a_err(a_string("compress.gunzip: CRC or size mismatch"));
    }

    AValue result = a_string_len(out ? (const char*)out : "", (int)out_len);
    if (out) mz_free(out);
    return result;
}
