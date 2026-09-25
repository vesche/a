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

/* Insertion-ordered map. keys/vals are parallel arrays so iteration order is
 * stable; `index` is an open-addressing hash table over the entry slots
 * (entry+1, 0 = empty) that exists only once the map has A_MAP_INDEX_MIN
 * entries, since a linear scan wins for the small record-like maps that
 * dominate (AST nodes, options). */
typedef struct AMap {
    int rc;
    int len;
    int cap;
    char** keys;
    AValue* vals;
    uint32_t* hashes;
    int* index;
    int index_cap;
} AMap;
#define A_MAP_INDEX_MIN 8

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
        struct { AValue* inner; } rval;
        void* pval;
    };
};

/* AValue must stay 16 bytes: the x86-64 and AArch64 ABIs pass and return it in
 * two registers; at 24 bytes every call would go through the stack. */
_Static_assert(sizeof(AValue) == 16, "AValue must be 16 bytes");

/* Heap cell behind a Result. `rval.inner` points at `val` (the first member),
 * so `*inner` reads the payload directly and the box is recovered by a cast.
 * The refcount makes Ok/Err values safe to copy, pass, and release like every
 * other heap value; before this they were freed on the first release. The
 * Ok/Err flag lives here rather than in AValue to keep AValue at 16 bytes. */
typedef struct AResultBox {
    AValue val;
    int rc;
    int is_ok;
} AResultBox;
#define A_RESULT_BOX(v) ((AResultBox*)(v).rval.inner)
#define A_RESULT_OK(v) (A_RESULT_BOX(v)->is_ok)

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
AValue a_index_set_move(AValue coll, AValue idx, AValue val);
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
AValue a_array_push_move(AValue arr, AValue val);
AValue a_array_slice(AValue arr, AValue start, AValue end);
AValue a_sort(AValue arr);
AValue a_contains(AValue arr, AValue val);
AValue a_reverse_arr(AValue arr);
AValue a_concat_arr(AValue a, AValue b);

/* Maps */
AValue a_map_new(int n, ...);
AValue a_map_get(AValue m, AValue key);
AValue a_map_get_borrow(AValue m, const char* key);
AValue a_map_get_cstr(AValue m, const char* key);
int a_map_has_cstr(AValue m, const char* key);
AValue a_index_cstr(AValue v, const char* key);
AValue a_variant_new(const char* tag, int n, ...);
int a_is_variant(AValue v, const char* tag);
AValue a_variant_arg(AValue v, int i);
int a_main_exit_code(AValue r);
int a_main_exit_code_int(AValue r);
AValue a_map_set(AValue m, AValue key, AValue val);
AValue a_map_set_move(AValue m, AValue key, AValue val);
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
AValue a_exec_timeout(AValue cmd, AValue ms);
void a_exit_fatal(int code);
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

/* Embedded files (defined in embedded.c) */
AValue a_embedded_file(AValue path);
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
AValue a_fs_stat(AValue path);
AValue a_fs_watch(AValue path, AValue callback);

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
AValue a_http_put(AValue url, AValue body, AValue headers);
AValue a_http_patch(AValue url, AValue body, AValue headers);
AValue a_http_delete(AValue url, AValue headers);

/* HTTP streaming */
AValue a_http_stream(AValue url, AValue body, AValue headers);
AValue a_http_stream_read(AValue handle);
AValue a_http_stream_close(AValue handle);

/* HTTP server */
AValue a_http_serve(AValue port, AValue handler);
AValue a_http_serve_static(AValue port, AValue dir);

/* WebSocket client */
AValue a_ws_connect(AValue url);
AValue a_ws_send(AValue handle, AValue msg);
AValue a_ws_recv(AValue handle);
AValue a_ws_close(AValue handle);

/* Subprocess pipes */
AValue a_proc_spawn(AValue cmd);
AValue a_proc_exec(AValue path, AValue args);
AValue a_proc_write(AValue handle, AValue data);
AValue a_proc_read_line(AValue handle);
AValue a_proc_kill(AValue handle);
AValue a_proc_wait(AValue handle);
AValue a_proc_is_running(AValue handle);

/* Concurrency (fork-based) */
AValue a_spawn(AValue closure);
AValue a_await(AValue handle);
AValue a_await_all(AValue handles);
AValue a_parallel_map(AValue arr, AValue func);
AValue a_parallel_each(AValue arr, AValue func);
AValue a_timeout(AValue ms, AValue func);

/* Async event loop */
AValue a_async_http_get(AValue url, AValue headers);
AValue a_async_http_post(AValue url, AValue body, AValue headers);
AValue a_async_http_put(AValue url, AValue body, AValue headers);
AValue a_async_http_patch(AValue url, AValue body, AValue headers);
AValue a_async_http_delete(AValue url, AValue headers);
AValue a_async_await(AValue handle);
AValue a_async_gather(AValue handles);

/* Database (SQLite) */
AValue a_db_open(AValue path);
AValue a_db_close(AValue db);
AValue a_db_exec(AValue db, AValue sql);
AValue a_db_query(AValue db, AValue sql, AValue params);

/* Compression (miniz) */
AValue a_compress_deflate(AValue data);
AValue a_compress_inflate(AValue data);
AValue a_compress_gzip(AValue data);
AValue a_compress_gunzip(AValue data);

/* UUID */
AValue a_uuid_v4(void);

/* Signal handling */
AValue a_signal_on(AValue name, AValue handler);
void a_signal_check(void);

/* Reflect / introspection */
AValue a_reflect_uptime_ms(void);
AValue a_reflect_memory_usage(void);
AValue a_reflect_pid(void);

/* Image processing */
AValue a_image_load(AValue path);
AValue a_image_decode(AValue bytes);
AValue a_image_save(AValue image, AValue path);
AValue a_image_encode(AValue image, AValue format);
AValue a_image_width(AValue image);
AValue a_image_height(AValue image);
AValue a_image_resize(AValue image, AValue w, AValue h);
AValue a_image_pixels(AValue image);

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
void a_try_raise(AValue err_result);

/* Runtime failure: `fail`, unwrap on Err, division by zero, expect. Inside a
 * `try { }` block the payload becomes the block's Err; otherwise the message
 * and the a-level stack trace are printed and the process exits 1. */
void a_fatal(const char* prefix, AValue payload);

/* Shadow call stack for a-level stack traces. Generated functions open a
 * frame with A_FRAME (popped on every exit path by the cleanup attribute) and
 * record the current source line with A_LINE before each statement. Both are
 * compiled out with -DA_RELEASE. */
typedef struct { const char* fn; const char* file; int line; } AFrame;
#define A_FRAME_MAX 1024
extern AFrame a_frames[A_FRAME_MAX];
extern int a_frame_depth;
extern int a_cur_line;
int a_frame_push(const char* fn, const char* file);
void a_frame_pop_cb(int* unused);
void a_frame_unwind_to(int depth);
void a_print_trace(void);
#ifdef A_RELEASE
#define A_FRAME(fn, file) (void)0
#define A_LINE(n) (void)0
#define A_UNWIND(depth) (void)0
#else
#define A_FRAME(fn, file) int __a_frame __attribute__((cleanup(a_frame_pop_cb))) = a_frame_push(fn, file)
#define A_LINE(n) (a_cur_line = (n))
#define A_UNWIND(depth) a_frame_unwind_to(depth)
#endif

/* Profiling */
void a_profile_init(void);
int  a_profile_register(const char* label);
void a_profile_hit(int id);
AValue a_profile_dump_json(AValue path);
AValue a_profile_get_counters(void);
AValue a_profile_reset(void);

/* Local LLM inference (GGUF) */
AValue a_llm_load(AValue path);
AValue a_llm_generate(AValue handle, AValue prompt, AValue opts);
AValue a_llm_embed(AValue handle, AValue text);
AValue a_llm_unload(AValue handle);
AValue a_llm_info(AValue handle);
AValue a_llm_tokenize(AValue handle, AValue text);
AValue a_llm_detokenize(AValue handle, AValue tokens);
AValue a_llm_vocab_size(AValue handle);

/* Pattern matching helpers (inline for zero overhead) */
static inline int a_is_ok_raw(AValue v) { return v.tag == TAG_RESULT && A_RESULT_OK(v); }
static inline int a_is_err_raw(AValue v) { return v.tag == TAG_RESULT && !A_RESULT_OK(v); }
static inline AValue a_unwrap_unsafe(AValue v) { return *v.rval.inner; }
static inline int a_is_array_of_len(AValue v, int n) { return v.tag == TAG_ARRAY && v.aval->len == n; }
static inline int a_is_array_min_len(AValue v, int n) { return v.tag == TAG_ARRAY && v.aval->len >= n; }
static inline int a_is_map_with(AValue v, const char* key) { return v.tag == TAG_MAP && a_map_has_cstr(v, key); }

/* Statement-scoped temporaries. Every runtime call returns an owned reference
 * and borrows its arguments, so a call result that is only ever passed on
 * (`f(g(x))`, `a + str(x)`, `[h()]`) would leak. cgen wraps such
 * sub-expressions in A_T(), which parks the reference on this stack, and
 * emits a_tmp_release(mark) at the end of each statement, each loop
 * iteration and each function exit. Scalars are skipped.
 * Known gap: a `try { }` landing releases the temporaries of the frames it
 * longjmp'd over, but not their local variables (there is no per-frame
 * unwinding), so each caught error can leak what those frames still held. */
extern AValue* a_tmp_stack;
extern int a_tmp_sp;
extern int a_tmp_cap;
void a_tmp_grow(void);
static inline AValue a_tmp(AValue v) {
    if (v.tag >= TAG_STRING && v.tag <= TAG_CLOSURE) {
        if (a_tmp_sp == a_tmp_cap) a_tmp_grow();
        a_tmp_stack[a_tmp_sp++] = v;
    }
    return v;
}
static inline void a_tmp_release(int mark) {
    while (a_tmp_sp > mark) a_release(a_tmp_stack[--a_tmp_sp]);
}
#define A_T(x) a_tmp(x)

#endif
