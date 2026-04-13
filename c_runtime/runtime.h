#ifndef A_RUNTIME_H
#define A_RUNTIME_H

#include <stdint.h>
#include <stdarg.h>
#include <setjmp.h>

typedef enum {
    TAG_INT, TAG_FLOAT, TAG_BOOL, TAG_VOID,
    TAG_STRING, TAG_ARRAY, TAG_MAP, TAG_RESULT,
    TAG_CLOSURE, TAG_PTR
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
        void* pval;
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

/* Pointers (FFI) */
AValue a_ptr(void* p);
AValue a_ptr_null(void);
AValue a_is_null(AValue v);

/* Refcounting */
AValue a_retain(AValue v);
void a_release(AValue v);

/* Truthiness */
int a_truthy(AValue v);
int a_ilen(AValue v);
AValue a_iterable(AValue v);
AValue a_index_set(AValue coll, AValue idx, AValue val);
AValue a_str_find(AValue s, AValue needle);
AValue a_str_count(AValue s, AValue needle);

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
AValue a_map_delete(AValue m, AValue key);
AValue a_map_entries(AValue m);
AValue a_map_from_entries(AValue arr);

/* I/O */
AValue a_println(AValue v);
AValue a_print(AValue v);
AValue a_eprintln(AValue v);
AValue a_io_read_file(AValue path);
AValue a_io_write_file(AValue path, AValue contents);

/* Filesystem */
AValue a_fs_ls(AValue path);
AValue a_fs_mkdir(AValue path);
AValue a_fs_cwd(void);
AValue a_fs_exists(AValue path);
AValue a_fs_is_dir(AValue path);

/* System */
AValue a_exec(AValue cmd);
AValue a_env_get(AValue key);

/* JSON */
AValue a_json_parse(AValue input);

/* Result */
AValue a_ok(AValue v);
AValue a_err(AValue v);
AValue a_is_ok(AValue v);
AValue a_is_err(AValue v);
AValue a_unwrap(AValue v);

/* Result extras */
AValue a_unwrap_or(AValue v, AValue def);
AValue a_expect(AValue v, AValue msg);

/* Utility */
AValue a_type_of(AValue v);
AValue a_args(void);
AValue a_argv0(void);
void a_fail(AValue v);
AValue a_to_int(AValue v);
AValue a_to_float(AValue v);
AValue a_char_code(AValue v);
AValue a_from_code(AValue v);
AValue a_is_alpha(AValue v);
AValue a_is_digit(AValue v);
AValue a_is_alnum(AValue v);

/* Math */
AValue a_math_sqrt(AValue v);
AValue a_math_abs(AValue v);
AValue a_math_floor(AValue v);
AValue a_math_ceil(AValue v);
AValue a_math_round(AValue v);
AValue a_math_pow(AValue base, AValue exp);
AValue a_math_min(AValue a, AValue b);
AValue a_math_max(AValue a, AValue b);

/* Strings (extras) */
AValue a_str_lines(AValue s);

/* I/O (extras) */
AValue a_io_read_stdin(void);
AValue a_io_read_line(void);
AValue a_io_read_bytes(AValue n);
AValue a_io_flush(void);

/* Environment (extras) */
AValue a_env_set(AValue key, AValue val);
AValue a_env_all(void);

/* Filesystem (extras) */
AValue a_fs_rm(AValue path);
AValue a_fs_mv(AValue src, AValue dst);
AValue a_fs_cp(AValue src, AValue dst);
AValue a_fs_abs(AValue path);
AValue a_fs_is_file(AValue path);

/* Time */
AValue a_time_now(void);
AValue a_time_sleep(AValue ms);

/* Hashing */
AValue a_hash_sha256(AValue data);
AValue a_hash_md5(AValue data);

/* JSON (extras) */
AValue a_json_stringify(AValue v);
AValue a_json_pretty(AValue v);

/* HTTP client */
AValue a_http_get(AValue url, AValue headers);
AValue a_http_post(AValue url, AValue body, AValue headers);

/* HTTP server */
AValue a_http_serve(AValue port, AValue handler);
AValue a_http_serve_static(AValue port, AValue dir);

/* Database (SQLite) */
AValue a_db_open(AValue path);
AValue a_db_close(AValue db);
AValue a_db_exec(AValue db, AValue sql);
AValue a_db_query(AValue db, AValue sql, AValue params);

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

/* Try/catch error handling */
#define A_TRY_STACK_MAX 64
extern jmp_buf a_try_stack[A_TRY_STACK_MAX];
extern AValue a_try_err;
extern int a_try_depth;
AValue a_try_unwrap(AValue v);

/* Garbage collector */
typedef enum { GC_STRING, GC_ARRAY, GC_MAP, GC_CLOSURE } GCType;

typedef struct GCNode {
    struct GCNode* next;
    GCType type;
    uint8_t mark;
    void* obj;
} GCNode;

void a_gc_push_root(AValue* root);
void a_gc_pop_roots(int n);
void a_gc_collect(void);

/* Arena allocator */
typedef struct AArena {
    char* buf;
    int size;
    int pos;
} AArena;

AArena* a_arena_new(int initial_size);
void    a_arena_free(AArena* arena);
void*   a_arena_alloc(AArena* arena, int bytes);
int     a_arena_save(AArena* arena);
void    a_arena_restore(AArena* arena, int saved_pos);

/* Pattern matching helpers (inline for zero overhead) */
static inline int a_is_ok_raw(AValue v) { return v.tag == TAG_RESULT && v.rval.is_ok; }
static inline int a_is_err_raw(AValue v) { return v.tag == TAG_RESULT && !v.rval.is_ok; }
static inline AValue a_unwrap_unsafe(AValue v) { return *v.rval.inner; }
static inline int a_is_array_of_len(AValue v, int n) { return v.tag == TAG_ARRAY && v.aval->len == n; }
static inline int a_is_array_min_len(AValue v, int n) { return v.tag == TAG_ARRAY && v.aval->len >= n; }
static inline int a_is_map_with(AValue v, const char* key) { return v.tag == TAG_MAP && a_truthy(a_map_has(v, a_string(key))); }

#endif
