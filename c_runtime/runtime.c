#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int g_argc = 0;
char** g_argv = NULL;

/* --- Constructors --- */

AValue a_int(int64_t v) { return (AValue){.tag = TAG_INT, .ival = v}; }
AValue a_float(double v) { return (AValue){.tag = TAG_FLOAT, .fval = v}; }
AValue a_bool(int v) { return (AValue){.tag = TAG_BOOL, .bval = v != 0}; }
AValue a_void(void) { return (AValue){.tag = TAG_VOID}; }

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
    return v;
}

void a_release(AValue v) {
    if (v.tag == TAG_STRING && v.sval) {
        if (--v.sval->rc <= 0) free(v.sval);
    } else if (v.tag == TAG_ARRAY && v.aval) {
        if (--v.aval->rc <= 0) {
            for (int i = 0; i < v.aval->len; i++) a_release(v.aval->items[i]);
            free(v.aval->items);
            free(v.aval);
        }
    } else if (v.tag == TAG_MAP && v.mval) {
        if (--v.mval->rc <= 0) {
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
    return 0;
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
    double fa = (a.tag == TAG_INT) ? (double)a.ival : a.fval;
    double fb = (b.tag == TAG_INT) ? (double)b.ival : b.fval;
    return a_bool(fa < fb);
}
AValue a_gt(AValue a, AValue b) { return a_lt(b, a); }
AValue a_lteq(AValue a, AValue b) {
    if (a.tag == TAG_INT && b.tag == TAG_INT) return a_bool(a.ival <= b.ival);
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
    return (AValue){.tag = TAG_STRING, .sval = ns};
}

AValue a_concat_n(int n, ...) {
    va_list ap;
    va_start(ap, n);
    AValue parts[64];
    int total_len = 0;
    for (int i = 0; i < n && i < 64; i++) {
        parts[i] = a_to_str(va_arg(ap, AValue));
        total_len += parts[i].sval->len;
    }
    va_end(ap);
    AString* ns = malloc(sizeof(AString) + total_len + 1);
    ns->rc = 1;
    ns->len = total_len;
    int pos = 0;
    for (int i = 0; i < n && i < 64; i++) {
        memcpy(ns->data + pos, parts[i].sval->data, parts[i].sval->len);
        pos += parts[i].sval->len;
    }
    ns->data[total_len] = '\0';
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
    int total = 0;
    for (int i = 0; i < arr.aval->len; i++) {
        AValue s = a_to_str(arr.aval->items[i]);
        total += s.sval->len;
        if (i > 0) total += sep.sval->len;
    }
    AString* ns = malloc(sizeof(AString) + total + 1);
    ns->rc = 1; ns->len = total;
    int pos = 0;
    for (int i = 0; i < arr.aval->len; i++) {
        if (i > 0) { memcpy(ns->data + pos, sep.sval->data, sep.sval->len); pos += sep.sval->len; }
        AValue s = a_to_str(arr.aval->items[i]);
        memcpy(ns->data + pos, s.sval->data, s.sval->len); pos += s.sval->len;
    }
    ns->data[total] = '\0';
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

/* --- Arrays --- */

AValue a_array_new(int n, ...) {
    AArray* arr = malloc(sizeof(AArray));
    arr->rc = 1; arr->len = n; arr->cap = n > 0 ? n : 1;
    arr->items = malloc(sizeof(AValue) * arr->cap);
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) arr->items[i] = va_arg(ap, AValue);
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
    return arr.aval->items[i];
}

AValue a_array_push(AValue arr, AValue val) {
    int olen = (arr.tag == TAG_ARRAY) ? arr.aval->len : 0;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = olen + 1; na->cap = olen + 1;
    na->items = malloc(sizeof(AValue) * na->cap);
    if (arr.tag == TAG_ARRAY)
        for (int i = 0; i < olen; i++) na->items[i] = arr.aval->items[i];
    na->items[olen] = val;
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
    for (int i = 0; i < n; i++) na->items[i] = arr.aval->items[st + i];
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
    memcpy(na->items, arr.aval->items, sizeof(AValue) * n);
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
    for (int i = 0; i < n; i++) na->items[i] = arr.aval->items[n - 1 - i];
    return (AValue){.tag = TAG_ARRAY, .aval = na};
}

AValue a_concat_arr(AValue a, AValue b) {
    if (a.tag != TAG_ARRAY || b.tag != TAG_ARRAY) return a_array_new(0);
    int n = a.aval->len + b.aval->len;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * (n > 0 ? n : 1));
    memcpy(na->items, a.aval->items, sizeof(AValue) * a.aval->len);
    memcpy(na->items + a.aval->len, b.aval->items, sizeof(AValue) * b.aval->len);
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
        m->vals[i] = va_arg(ap, AValue);
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
    return idx >= 0 ? m.mval->vals[idx] : a_void();
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
        nm->vals[i] = old->vals[i];
    }
    if (existing >= 0) {
        nm->vals[existing] = val;
    } else {
        nm->keys[olen] = strdup(k);
        nm->vals[olen] = val;
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
    for (int i = 0; i < m.mval->len; i++) arr->items[i] = m.mval->vals[i];
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_map_merge(AValue a, AValue b) {
    if (a.tag != TAG_MAP || b.tag != TAG_MAP) return a;
    AValue result = a;
    for (int i = 0; i < b.mval->len; i++)
        result = a_map_set(result, a_string(b.mval->keys[i]), b.mval->vals[i]);
    return result;
}

/* --- I/O --- */

static void print_val(AValue v) {
    if (v.tag == TAG_STRING) { fwrite(v.sval->data, 1, v.sval->len, stdout); return; }
    char buf[4096];
    val_to_buf(v, buf, 4096);
    fputs(buf, stdout);
}

void a_println(AValue v) { print_val(v); putchar('\n'); }
void a_print(AValue v) { print_val(v); }
void a_eprintln(AValue v) {
    char buf[4096];
    val_to_buf(v, buf, 4096);
    fprintf(stderr, "%s\n", buf);
}

AValue a_io_read_file(AValue path) {
    if (path.tag != TAG_STRING) return a_err(a_string("read_file: expected string path"));
    FILE* f = fopen(path.sval->data, "r");
    if (!f) return a_err(a_string("read_file: cannot open file"));
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
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
        if (v.rval.is_ok) return *v.rval.inner;
        fprintf(stderr, "unwrap on Err: ");
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
