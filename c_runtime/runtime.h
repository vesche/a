#ifndef A_RUNTIME_H
#define A_RUNTIME_H

#include <stdint.h>
#include <stdarg.h>

typedef enum {
    TAG_INT, TAG_FLOAT, TAG_BOOL, TAG_VOID,
    TAG_STRING, TAG_ARRAY, TAG_MAP, TAG_RESULT,
    TAG_CLOSURE
} ATag;

typedef struct AString {
    int rc;
    int len;
    char data[];
} AString;

typedef struct AValue AValue;

typedef struct AArray {
    int rc;
    int len;
    int cap;
    AValue* items;
} AArray;

typedef struct AMap {
    int rc;
    int len;
    int cap;
    char** keys;
    AValue* vals;
} AMap;

typedef struct AClosure AClosure;

struct AValue {
    ATag tag;
    union {
        int64_t ival;
        double fval;
        int bval;
        AString* sval;
        AArray* aval;
        AMap* mval;
        AClosure* cval;
        struct { int is_ok; AValue* inner; } rval;
    };
};

typedef AValue (*AClosureFn)(AValue env, int argc, AValue* argv);

struct AClosure {
    int rc;
    AClosureFn fn;
    AValue env;
};

extern int g_argc;
extern char** g_argv;

/* Constructors */
AValue a_int(int64_t v);
AValue a_float(double v);
AValue a_bool(int v);
AValue a_void(void);
AValue a_string(const char* s);
AValue a_string_len(const char* s, int len);

/* Refcounting */
AValue a_retain(AValue v);
void a_release(AValue v);

/* Truthiness */
int a_truthy(AValue v);
int a_ilen(AValue v);

/* Arithmetic */
AValue a_add(AValue a, AValue b);
AValue a_sub(AValue a, AValue b);
AValue a_mul(AValue a, AValue b);
AValue a_div(AValue a, AValue b);
AValue a_mod(AValue a, AValue b);
AValue a_neg(AValue a);

/* Comparison */
AValue a_eq(AValue a, AValue b);
AValue a_neq(AValue a, AValue b);
AValue a_lt(AValue a, AValue b);
AValue a_gt(AValue a, AValue b);
AValue a_lteq(AValue a, AValue b);
AValue a_gteq(AValue a, AValue b);
AValue a_not(AValue a);
AValue a_and(AValue a, AValue b);
AValue a_or(AValue a, AValue b);

/* Strings */
AValue a_to_str(AValue v);
AValue a_str_concat(AValue a, AValue b);
AValue a_concat_n(int n, ...);
AValue a_len(AValue v);
AValue a_str_split(AValue s, AValue delim);
AValue a_str_contains(AValue s, AValue sub);
AValue a_str_replace(AValue s, AValue from, AValue to);
AValue a_str_trim(AValue s);
AValue a_str_upper(AValue s);
AValue a_str_lower(AValue s);
AValue a_str_join(AValue arr, AValue sep);
AValue a_str_chars(AValue s);
AValue a_str_slice(AValue s, AValue start, AValue end);
AValue a_str_starts_with(AValue s, AValue pre);
AValue a_str_ends_with(AValue s, AValue suf);

/* Arrays */
AValue a_array_new(int n, ...);
AValue a_array_get(AValue arr, AValue idx);
AValue a_array_push(AValue arr, AValue val);
AValue a_array_slice(AValue arr, AValue start, AValue end);
AValue a_sort(AValue arr);
AValue a_contains(AValue arr, AValue val);
AValue a_reverse_arr(AValue arr);
AValue a_concat_arr(AValue a, AValue b);

/* Maps */
AValue a_map_new(int n, ...);
AValue a_map_get(AValue m, AValue key);
AValue a_map_set(AValue m, AValue key, AValue val);
AValue a_map_has(AValue m, AValue key);
AValue a_map_keys(AValue m);
AValue a_map_values(AValue m);
AValue a_map_merge(AValue a, AValue b);

/* I/O */
void a_println(AValue v);
void a_print(AValue v);
void a_eprintln(AValue v);
AValue a_io_read_file(AValue path);
AValue a_io_write_file(AValue path, AValue contents);

/* Result */
AValue a_ok(AValue v);
AValue a_err(AValue v);
AValue a_is_ok(AValue v);
AValue a_is_err(AValue v);
AValue a_unwrap(AValue v);

/* Utility */
AValue a_type_of(AValue v);
AValue a_args(void);
void a_fail(AValue v);
AValue a_to_int(AValue v);
AValue a_to_float(AValue v);
AValue a_char_code(AValue v);
AValue a_from_code(AValue v);
AValue a_is_alpha(AValue v);
AValue a_is_digit(AValue v);
AValue a_is_alnum(AValue v);

/* Closures */
AValue a_closure(AClosureFn fn, AValue env);
AValue a_closure_call(AValue closure, int argc, ...);
AValue a_closure_call_arr(AValue closure, int argc, AValue* argv);

/* Higher-order functions */
AValue a_hof_map(AValue arr, AValue fn);
AValue a_hof_filter(AValue arr, AValue fn);
AValue a_hof_reduce(AValue arr, AValue init, AValue fn);
AValue a_hof_each(AValue arr, AValue fn);
AValue a_hof_sort_by(AValue arr, AValue fn);
AValue a_hof_find(AValue arr, AValue fn);
AValue a_hof_any(AValue arr, AValue fn);
AValue a_hof_all(AValue arr, AValue fn);
AValue a_hof_flat_map(AValue arr, AValue fn);
AValue a_hof_min_by(AValue arr, AValue fn);
AValue a_hof_max_by(AValue arr, AValue fn);

/* Array utilities */
AValue a_enumerate(AValue arr);
AValue a_zip(AValue a, AValue b);
AValue a_take(AValue arr, AValue n);
AValue a_drop(AValue arr, AValue n);
AValue a_unique(AValue arr);
AValue a_chunk(AValue arr, AValue n);

#endif
