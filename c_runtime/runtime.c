#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

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
        if (--v.sval->rc <= 0) free(v.sval);
    } else if (v.tag == TAG_ARRAY && v.aval) {
        if (--v.aval->rc <= 0) {
            for (int i = 0; i < v.aval->len; i++) a_release(v.aval->items[i]);
            free(v.aval->items);
            free(v.aval);
        }
    } else if (v.tag == TAG_CLOSURE && v.cval) {
        if (--v.cval->rc <= 0) {
            a_release(v.cval->env);
            free(v.cval);
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
    if (v.tag == TAG_MAP && v.mval) return v.mval->len;
    return 0;
}

AValue a_iterable(AValue v) {
    if (v.tag == TAG_MAP) return a_map_entries(v);
    return v;
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

AValue a_index_set(AValue coll, AValue idx, AValue val) {
    if (coll.tag == TAG_MAP) return a_map_set(coll, idx, val);
    if (coll.tag != TAG_ARRAY || idx.tag != TAG_INT) return coll;
    int i = (int)idx.ival;
    int n = coll.aval->len;
    if (i < 0 || i >= n) return coll;
    AArray* na = malloc(sizeof(AArray));
    na->rc = 1; na->len = n; na->cap = n;
    na->items = malloc(sizeof(AValue) * n);
    for (int j = 0; j < n; j++) na->items[j] = coll.aval->items[j];
    na->items[i] = val;
    return (AValue){.tag = TAG_ARRAY, .aval = na};
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
        nm->vals[j] = m.mval->vals[i];
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
        arr->items[i] = a_array_new(2, a_string(m.mval->keys[i]), m.mval->vals[i]);
    return (AValue){.tag = TAG_ARRAY, .aval = arr};
}

AValue a_map_from_entries(AValue arr) {
    if (arr.tag != TAG_ARRAY) return a_map_new(0);
    AValue m = a_map_new(0);
    for (int i = 0; i < arr.aval->len; i++) {
        AValue entry = arr.aval->items[i];
        if (entry.tag == TAG_ARRAY && entry.aval->len >= 2)
            m = a_map_set(m, entry.aval->items[0], entry.aval->items[1]);
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
        if (v.rval.is_ok) return *v.rval.inner;
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
