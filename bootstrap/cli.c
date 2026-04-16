#include "runtime.h"
#include <stdlib.h>

/* Forward declarations */
AValue fn_lexer_is_ws(AValue c);
AValue fn_lexer_is_id_start(AValue c);
AValue fn_lexer_is_id_char(AValue c);
AValue fn_lexer_keyword_or_ident(AValue word);
AValue fn_lexer_process_escape(AValue c);
AValue fn_lexer_lex(AValue src);
AValue fn_lexer_tk(AValue toks, AValue i);
AValue fn_lexer_tv(AValue toks, AValue i);
AValue fn_lexer_tcount(AValue toks);
AValue fn_lexer_skip_nl(AValue toks, AValue p);
AValue fn_ast_mk_program(AValue items);
AValue fn_ast_mk_fn_decl(AValue name, AValue params, AValue ret_type, AValue effs, AValue precond, AValue postcond, AValue body);
AValue fn_ast_mk_type_decl(AValue name, AValue type_params, AValue body);
AValue fn_ast_mk_mod_decl(AValue name, AValue items);
AValue fn_ast_mk_use_decl(AValue path);
AValue fn_ast_mk_extern_fn(AValue name, AValue params, AValue ret_type);
AValue fn_ast_mk_type_record(AValue fields);
AValue fn_ast_mk_type_sum(AValue variants);
AValue fn_ast_mk_type_alias(AValue typ, AValue where_expr);
AValue fn_ast_mk_variant(AValue name, AValue fields);
AValue fn_ast_mk_field(AValue name, AValue typ);
AValue fn_ast_mk_ty_named(AValue name, AValue args);
AValue fn_ast_mk_ty_array(AValue elem);
AValue fn_ast_mk_ty_tuple(AValue elems);
AValue fn_ast_mk_ty_record(AValue fields);
AValue fn_ast_mk_ty_fn(AValue params, AValue ret_ty);
AValue fn_ast_mk_ty_map(AValue key, AValue val);
AValue fn_ast_mk_ty_prim(AValue name);
AValue fn_ast_mk_ty_infer(void);
AValue fn_ast_mk_param(AValue name, AValue typ);
AValue fn_ast_mk_block(AValue stmts);
AValue fn_ast_mk_let(AValue mutable, AValue name, AValue typ, AValue value);
AValue fn_ast_mk_assign(AValue target, AValue value);
AValue fn_ast_mk_return(AValue expr);
AValue fn_ast_mk_expr_stmt(AValue expr);
AValue fn_ast_mk_if_stmt(AValue cond, AValue then_block, AValue else_branch);
AValue fn_ast_mk_match_stmt(AValue expr, AValue arms);
AValue fn_ast_mk_for_stmt(AValue var, AValue typ, AValue iter, AValue body);
AValue fn_ast_mk_while_stmt(AValue cond, AValue body);
AValue fn_ast_mk_let_destructure(AValue bindings, AValue rest, AValue value);
AValue fn_ast_mk_for_destructure(AValue bindings, AValue iter, AValue body);
AValue fn_ast_mk_break(void);
AValue fn_ast_mk_continue(void);
AValue fn_ast_mk_else_block(AValue block);
AValue fn_ast_mk_else_if(AValue if_stmt);
AValue fn_ast_mk_match_arm(AValue pattern, AValue guard, AValue body);
AValue fn_ast_mk_pat_ident(AValue name);
AValue fn_ast_mk_pat_constructor(AValue name, AValue args);
AValue fn_ast_mk_pat_literal(AValue lit);
AValue fn_ast_mk_pat_wildcard(void);
AValue fn_ast_mk_pat_array(AValue elems);
AValue fn_ast_mk_pat_array_elem(AValue pat);
AValue fn_ast_mk_pat_rest(AValue name);
AValue fn_ast_mk_pat_map(AValue entries);
AValue fn_ast_mk_pat_map_entry(AValue key, AValue pat);
AValue fn_ast_mk_int(AValue val);
AValue fn_ast_mk_float(AValue val);
AValue fn_ast_mk_string(AValue val);
AValue fn_ast_mk_bool_lit(AValue val);
AValue fn_ast_mk_ident(AValue name);
AValue fn_ast_mk_binop(AValue op, AValue left, AValue right);
AValue fn_ast_mk_unary(AValue op, AValue expr);
AValue fn_ast_mk_call(AValue func, AValue args);
AValue fn_ast_mk_field_access(AValue expr, AValue field);
AValue fn_ast_mk_index(AValue expr, AValue index);
AValue fn_ast_mk_try(AValue expr);
AValue fn_ast_mk_try_block(AValue block);
AValue fn_ast_mk_if_expr(AValue cond, AValue then_block, AValue else_block);
AValue fn_ast_mk_match_expr(AValue expr, AValue arms);
AValue fn_ast_mk_block_expr(AValue block);
AValue fn_ast_mk_array(AValue elems);
AValue fn_ast_mk_record(AValue fields);
AValue fn_ast_mk_record_field(AValue name, AValue value);
AValue fn_ast_mk_lambda(AValue params, AValue body);
AValue fn_ast_mk_pipe(AValue left, AValue right);
AValue fn_ast_mk_interpolation(AValue parts);
AValue fn_ast_mk_interp_lit(AValue text);
AValue fn_ast_mk_interp_expr(AValue expr);
AValue fn_ast_mk_map_literal(AValue entries);
AValue fn_ast_mk_map_entry(AValue key, AValue value);
AValue fn_ast_mk_spread(AValue expr);
AValue fn_ast_mk_void_lit(void);
AValue fn_parser_tk(AValue toks, AValue i);
AValue fn_parser_tv(AValue toks, AValue i);
AValue fn_parser_err(AValue msg, AValue pos);
AValue fn_parser__is_perr(AValue r);
AValue fn_parser_skip_nl(AValue toks, AValue pos);
AValue fn_parser_expect(AValue toks, AValue pos, AValue kind);
AValue fn_parser_expect_ident(AValue toks, AValue pos);
AValue fn_parser_expect_nl_or_eof(AValue toks, AValue pos);
AValue fn_parser_parse(AValue src);
AValue fn_parser_parse_program(AValue toks, AValue pos);
AValue fn_parser_parse_top_level(AValue toks, AValue pos);
AValue fn_parser_parse_fn_decl(AValue toks, AValue pos);
AValue fn_parser_parse_extern_fn(AValue toks, AValue pos);
AValue fn_parser_parse_param_list(AValue toks, AValue pos);
AValue fn_parser_parse_param(AValue toks, AValue pos);
AValue fn_parser_parse_effects(AValue toks, AValue pos);
AValue fn_parser_parse_mod_decl(AValue toks, AValue pos);
AValue fn_parser_parse_use_decl(AValue toks, AValue pos);
AValue fn_parser_parse_type_decl(AValue toks, AValue pos);
AValue fn_parser_parse_type_params(AValue toks, AValue pos);
AValue fn_parser_looks_like_sum(AValue toks, AValue pos);
AValue fn_parser_parse_type_body(AValue toks, AValue pos);
AValue fn_parser_parse_variant(AValue toks, AValue pos);
AValue fn_parser_parse_record_fields(AValue toks, AValue pos);
AValue fn_parser_parse_field(AValue toks, AValue pos);
AValue fn_parser_parse_type_expr(AValue toks, AValue pos);
AValue fn_parser_parse_block(AValue toks, AValue pos);
AValue fn_parser_parse_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_let_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_let_destructure(AValue toks, AValue pos);
AValue fn_parser_parse_ret_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_if_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_match_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_match_arm(AValue toks, AValue pos);
AValue fn_parser_parse_for_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_for_destructure(AValue toks, AValue pos);
AValue fn_parser_parse_while_stmt(AValue toks, AValue pos);
AValue fn_parser_parse_pattern(AValue toks, AValue pos);
AValue fn_parser_parse_expr(AValue toks, AValue pos);
AValue fn_parser_parse_pipe_expr(AValue toks, AValue pos);
AValue fn_parser_parse_or_expr(AValue toks, AValue pos);
AValue fn_parser_parse_and_expr(AValue toks, AValue pos);
AValue fn_parser_parse_eq_expr(AValue toks, AValue pos);
AValue fn_parser_parse_cmp_expr(AValue toks, AValue pos);
AValue fn_parser_parse_add_expr(AValue toks, AValue pos);
AValue fn_parser_parse_mul_expr(AValue toks, AValue pos);
AValue fn_parser_parse_unary_expr(AValue toks, AValue pos);
AValue fn_parser_parse_postfix_expr(AValue toks, AValue pos);
AValue fn_parser_parse_primary_expr(AValue toks, AValue pos);
AValue fn_parser_parse_array_element(AValue toks, AValue pos);
AValue fn_parser_parse_interp_string(AValue toks, AValue pos);
AValue fn_cgen__builtin_map(void);
AValue fn_cgen__void_builtins(void);
AValue fn_cgen__c_keywords(void);
AValue fn_cgen__mangle(AValue name);
AValue fn_cgen__bslash(void);
AValue fn_cgen__dquote(void);
AValue fn_cgen__newline_char(void);
AValue fn_cgen__tab_char(void);
AValue fn_cgen__cr_char(void);
AValue fn_cgen__escape_c_str(AValue s);
AValue fn_cgen__indent(AValue depth);
AValue fn_cgen__prefixed_name(AValue name, AValue ctx);
AValue fn_cgen__is_intra_module_call(AValue fname, AValue ctx);
AValue fn_cgen__R(AValue code, AValue li);
AValue fn_cgen__RL(AValue code, AValue li, AValue lifted);
AValue fn_cgen__is_ident(AValue node);
AValue fn_cgen__emit_cleanup(AValue vars, AValue depth);
AValue fn_cgen__ea_collect_idents(AValue expr);
AValue fn_cgen__escape_analysis(AValue stmts, AValue captures);
AValue fn_cgen__collect_idents_in_expr(AValue node);
AValue fn_cgen__collect_idents_in_block(AValue block);
AValue fn_cgen__collect_idents_in_stmt(AValue s);
AValue fn_cgen__compute_captures(AValue lambda_node, AValue enclosing_vars, AValue bm);
AValue fn_cgen__collect_lets_in_block(AValue block);
AValue fn_cgen_emit_expr(AValue node, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__resolve_call_name(AValue func);
AValue fn_cgen__emit_pat_cond(AValue pat, AValue target, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__emit_pat_bind(AValue pat, AValue target, AValue depth, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__emit_arm_body(AValue body, AValue depth, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__emit_match(AValue node, AValue depth, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__emit_match_expr(AValue node, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__emit_match_expr_body(AValue body, AValue bm, AValue ctx, AValue li);
AValue fn_cgen_emit_stmt(AValue node, AValue depth, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__collect_pattern_vars(AValue pat);
AValue fn_cgen__collect_vars_in_stmts(AValue stmts);
AValue fn_cgen_emit_fn(AValue node, AValue bm, AValue ctx, AValue li);
AValue fn_cgen__emit_fwd_decl(AValue name, AValue params);
AValue fn_cgen__use_path_to_file(AValue path_arr);
AValue fn_cgen__use_path_short_name(AValue path_arr);
AValue fn_cgen__collect_fn_names(AValue items);
AValue fn_cgen__load_module(AValue path_arr, AValue bm, AValue loaded, AValue li);
AValue fn_cgen__ffi_c_type(AValue ty_node);
AValue fn_cgen__ffi_extract(AValue ty_node, AValue var_name);
AValue fn_cgen__ffi_wrap(AValue ty_node, AValue expr);
AValue fn_cgen__emit_extern_fn(AValue node);
AValue fn_cgen_emit_program(AValue prog_ast, AValue bm);
AValue fn_cgen_main(void);
AValue fn_wasmgen__indent(AValue n);
AValue fn_wasmgen__mangle(AValue nm);
AValue fn_wasmgen__prefixed(AValue nm, AValue pfx);
AValue fn_wasmgen__wat_type(AValue count);
AValue fn_wasmgen__collect_fns(AValue items);
AValue fn_wasmgen__collect_locals(AValue stmts);
AValue fn_wasmgen__builtin_map(void);
AValue fn_wasmgen__intern_string(AValue ctx, AValue s);
AValue fn_wasmgen_emit_expr(AValue node, AValue dep, AValue ctx);
AValue fn_wasmgen_emit_stmt(AValue node, AValue dep, AValue ctx);
AValue fn_wasmgen__unwrap_body(AValue body_node);
AValue fn_wasmgen_emit_fn(AValue node, AValue pfx, AValue ctx);
AValue fn_wasmgen__emit_imports(void);
AValue fn_wasmgen__emit_data(AValue ctx);
AValue fn_wasmgen_emit_program(AValue prog_ast);
AValue fn_wasmgen_compile(AValue source);
AValue fn_emitter_emit(AValue ast);
AValue fn_emitter_is_present(AValue val);
AValue fn_emitter_mk_indent(AValue level);
AValue fn_emitter_emit_top_level(AValue node, AValue level);
AValue fn_emitter_emit_extern_fn(AValue node, AValue level);
AValue fn_emitter_emit_use_decl(AValue node, AValue level);
AValue fn_emitter_emit_mod_decl(AValue node, AValue level);
AValue fn_emitter_emit_type_decl(AValue node, AValue level);
AValue fn_emitter_emit_type_body(AValue node);
AValue fn_emitter_emit_type(AValue node);
AValue fn_emitter_emit_fn_decl(AValue node, AValue level);
AValue fn_emitter_emit_param(AValue p);
AValue fn_emitter_emit_block(AValue node, AValue level);
AValue fn_emitter_emit_stmt(AValue node, AValue level);
AValue fn_emitter_emit_let(AValue node, AValue level);
AValue fn_emitter_emit_assign(AValue node, AValue level);
AValue fn_emitter_emit_return(AValue node, AValue level);
AValue fn_emitter_emit_expr_stmt(AValue node, AValue level);
AValue fn_emitter_emit_if_stmt(AValue node, AValue level);
AValue fn_emitter_emit_match_stmt(AValue node, AValue level);
AValue fn_emitter_emit_match_arm(AValue arm, AValue level);
AValue fn_emitter_emit_for_stmt(AValue node, AValue level);
AValue fn_emitter_emit_while_stmt(AValue node, AValue level);
AValue fn_emitter_emit_let_destructure(AValue node, AValue level);
AValue fn_emitter_emit_for_destructure(AValue node, AValue level);
AValue fn_emitter_emit_expr(AValue node);
AValue fn_emitter_emit_pattern(AValue node);
AValue fn_emitter_emit_pat_literal(AValue lit);
AValue fn_emitter_escape_str(AValue s);
AValue fn_checker__builtin_arity(void);
AValue fn_checker__is_builtin_or_prefix(AValue name, AValue arity_map);
AValue fn_checker_check(AValue ast);
AValue fn_checker__check_fn_body(AValue fn_node, AValue scope, AValue arity_map, AValue diags);
AValue fn_checker__collect_all_lets(AValue stmts);
AValue fn_checker__collect_idents_expr(AValue node, AValue s);
AValue fn_checker__collect_all_idents_block(AValue stmts, AValue s);
AValue fn_checker__collect_calls_expr(AValue node, AValue out);
AValue fn_checker__collect_all_calls(AValue stmts);
AValue fn_profiler__nl(void);
AValue fn_profiler__lbrace(void);
AValue fn_profiler__collect_fn_names(AValue prog_ast);
AValue fn_profiler__count_branches_inner(AValue stmts, AValue counts);
AValue fn_profiler__analyze_ast(AValue prog_ast);
AValue fn_profiler__mangle_name(AValue name);
AValue fn_profiler__gen_base_c(AValue source_path);
AValue fn_profiler__insert_fn_hits(AValue c_code, AValue fn_names, AValue fn_hit_map);
AValue fn_profiler__insert_branch_hits(AValue c_code, AValue n_ifs, AValue n_whiles, AValue n_fors, AValue hit_base);
AValue fn_profiler_instrument(AValue source_path, AValue profile_out_path);
AValue fn_profiler_analyze_profile(AValue profile_path);
AValue fn_optimizer_load_profile(AValue profile_path);
AValue fn_optimizer__find_hot_functions(AValue counters);
AValue fn_optimizer__find_hot_branches(AValue counters);
AValue fn_optimizer_analyze_profile(AValue profile_path);
AValue fn_optimizer__count_fn_body_stmts(AValue body);
AValue fn_optimizer__is_small_fn(AValue func_node);
AValue fn_optimizer__extract_fn_map(AValue source);
AValue fn_optimizer_suggest(AValue source, AValue profile_report);
AValue fn_optimizer_measure_binary(AValue bin_path);
AValue fn_optimizer_measure_compile_time(AValue source_path);
AValue fn_optimizer_benchmark(AValue source_path, AValue runs);
AValue fn_optimizer_report(AValue source_path, AValue profile_path);
AValue fn_testgen__edge_values_for_type(AValue hint);
AValue fn_testgen__guess_param_type(AValue pname);
AValue fn_testgen__extract_functions(AValue prog_ast);
AValue fn_testgen__gen_arg_combos(AValue params);
AValue fn_testgen__gen_test_for_fn(AValue func_info, AValue test_num);
AValue fn_testgen_gen_tests(AValue source);
AValue fn_testgen_gen_tests_for_file(AValue filepath);
AValue fn_testgen_analyze(AValue source);
AValue fn_path_join(AValue a, AValue b);
AValue fn_path_join3(AValue a, AValue b, AValue c);
AValue fn_path_dirname(AValue p);
AValue fn_path_basename(AValue p);
AValue fn_path_extension(AValue p);
AValue fn_path_stem(AValue p);
AValue fn_path_with_extension(AValue p, AValue ext);
AValue fn_path_is_absolute(AValue p);
AValue fn_path_segments(AValue p);
AValue fn_path_normalize(AValue p);
AValue fn_path__last_slash(AValue s);
AValue fn_path__last_dot(AValue s);
AValue fn_path_home(void);
AValue fn_path_temp(void);
AValue fn_path_relative(AValue from, AValue to);
AValue fn_path_has_extension(AValue p, AValue ext);
AValue fn_path_join_all(AValue parts);
AValue fn_path_is_hidden(AValue p);
AValue fn_cli_esc_code(void);
AValue fn_cli_wrap(AValue s, AValue code);
AValue fn_cli_red(AValue s);
AValue fn_cli_green(AValue s);
AValue fn_cli_yellow(AValue s);
AValue fn_cli_blue(AValue s);
AValue fn_cli_magenta(AValue s);
AValue fn_cli_cyan(AValue s);
AValue fn_cli_gray(AValue s);
AValue fn_cli_bold(AValue s);
AValue fn_cli_dim(AValue s);
AValue fn_cli_underline(AValue s);
AValue fn_toml__t_trim(AValue s);
AValue fn_toml__t_is_int(AValue s);
AValue fn_toml__t_is_float(AValue s);
AValue fn_toml__t_strip_underscores(AValue s);
AValue fn_toml__t_parse_value(AValue s);
AValue fn_toml__t_split_array(AValue s);
AValue fn_toml__t_parse_array(AValue s);
AValue fn_toml__t_parse_inline_table(AValue s);
AValue fn_toml__t_find_eq(AValue s);
AValue fn_toml__t_strip_comment(AValue s);
AValue fn_toml__t_set_nested(AValue root, AValue path, AValue val);
AValue fn_toml__t_get_nested(AValue root, AValue path);
AValue fn_toml__t_parse_key(AValue s);
AValue fn_toml_parse(AValue text);
AValue fn_toml__t_needs_quote(AValue s);
AValue fn_toml__t_quote_key(AValue s);
AValue fn_toml__t_stringify_value(AValue val);
AValue fn_toml__t_stringify_section(AValue val, AValue prefix, AValue lines);
AValue fn_toml_stringify(AValue val);
AValue fn_semver_parse(AValue s);
AValue fn_semver_format(AValue v);
AValue fn_semver_compare(AValue a, AValue b);
AValue fn_semver__gte(AValue v, AValue min);
AValue fn_semver__lt(AValue v, AValue max);
AValue fn_semver_satisfies(AValue version, AValue constraint);
AValue fn_semver_best_match(AValue versions, AValue constraint);
AValue fn_pkg__manifest_path(AValue dir);
AValue fn_pkg__lock_path(AValue dir);
AValue fn_pkg__modules_dir(AValue dir);
AValue fn_pkg_parse_source(AValue source);
AValue fn_pkg__git_url(AValue source);
AValue fn_pkg__fetch_tags(AValue url);
AValue fn_pkg__clone_at_tag(AValue url, AValue tag, AValue dest);
AValue fn_pkg__get_commit(AValue dest);
AValue fn_pkg__copy_a_files(AValue src_dir, AValue dst_dir);
AValue fn_pkg_init(AValue dir);
AValue fn_pkg_read_manifest(AValue dir);
AValue fn_pkg_write_manifest(AValue dir, AValue manifest);
AValue fn_pkg_add_dep(AValue dir, AValue name, AValue source);
AValue fn_pkg__read_lock(AValue dir);
AValue fn_pkg__write_lock(AValue dir, AValue packages);
AValue fn_pkg__find_locked(AValue lock_entries, AValue name);
AValue fn_pkg_install(AValue dir);
AValue fn_plugin__plugins_dir(void);
AValue fn_plugin__ensure_plugins_dir(void);
AValue fn_plugin__registry_path(void);
AValue fn_plugin__load_registry(void);
AValue fn_plugin__save_registry(AValue reg);
AValue fn_plugin__parse_manifest(AValue dir);
AValue fn_plugin_install(AValue source_dir);
AValue fn_plugin_install_git(AValue repo);
AValue fn_plugin__entry_point(AValue manifest, AValue dir);
AValue fn_plugin__plugin_caps(AValue manifest);
AValue fn_plugin_remove(AValue name);
AValue fn_plugin_list(void);
AValue fn_plugin_get(AValue name);
AValue fn_plugin_is_installed(AValue name);
AValue fn_plugin_load(AValue name);
AValue fn_plugin_run(AValue name);
AValue fn_plugin__find_cli(void);
AValue fn_plugin_create_manifest(AValue name, AValue version, AValue desc, AValue ep);
AValue fn_plugin_init(AValue dir, AValue name);
AValue fn__version(void);
AValue fn__die(AValue msg);
AValue fn__find_runtime_dir(void);
AValue fn__generate_c(AValue source_path);
AValue fn__gcc_flags(void);
AValue fn__sqlite_flags(void);
AValue fn__ensure_runtime_objs(AValue runtime_dir);
AValue fn__gcc_try(AValue c_path, AValue out_path, AValue runtime_dir);
AValue fn__gcc(AValue c_path, AValue out_path, AValue runtime_dir);
AValue fn__tmp_path(AValue suffix);
AValue fn_cmd_cc(AValue source_path, AValue out_file);
AValue fn_cmd_wat(AValue source_path, AValue out_file);
AValue fn__codegen_subprocess(AValue source_path, AValue c_path);
AValue fn_cmd_build(AValue source_path, AValue out_path);
AValue fn__known_targets(void);
AValue fn__target_compiler(AValue tgt);
AValue fn__target_flags(AValue tgt);
AValue fn__target_ext(AValue tgt);
AValue fn__target_emcc_flags(void);
AValue fn_cmd_build_target(AValue source_path, AValue out_path, AValue tgt);
AValue fn_cmd_targets(void);
AValue fn__cache_dir(void);
AValue fn__ensure_cache_dir(void);
AValue fn__cached_bin(AValue source_path);
AValue fn__store_cache(AValue source_path, AValue bin_path);
AValue fn_cmd_run(AValue source_path, AValue extra_args);
AValue fn_cmd_test(AValue test_dir);
AValue fn_cmd_cache_clean(void);
AValue fn__profile_insert_fn_hits(AValue c_code, AValue fn_names, AValue fn_hit_map);
AValue fn__profile_insert_branch_hits(AValue c_code, AValue n_ifs, AValue n_whiles, AValue n_fors, AValue hit_base);
AValue fn__profile_count_branches(AValue stmts, AValue counts);
AValue fn_cmd_profile(AValue source_path, AValue profile_out);
AValue fn_cmd_gentests(AValue source_path, AValue out_path);
AValue fn_cmd_optimize_report(AValue source_path, AValue profile_path);
AValue fn_cmd_eval(AValue expr, AValue extra_args);
AValue fn_cmd_lsp(void);
AValue fn__parse_source(AValue source_path);
AValue fn__fmt_file(AValue file_path);
AValue fn__is_dir(AValue p);
AValue fn__fmt_dir(AValue target);
AValue fn_cmd_fmt(AValue target);
AValue fn_cmd_ast(AValue source_path);
AValue fn_cmd_check(AValue source_path);
AValue fn__repl_build_source(AValue fns, AValue bindings, AValue expr);
AValue fn_cmd_repl(void);
AValue fn_cmd_pkg_init(void);
AValue fn_cmd_pkg_add(AValue name, AValue source);
AValue fn_cmd_pkg_install(void);
AValue fn__collect_watch_files(AValue source_path);
AValue fn__get_mtimes(AValue files);
AValue fn__mtimes_changed(AValue old_mt, AValue new_mt);
AValue fn_cmd_watch(AValue source_path);
AValue fn_cmd_spawn(AValue source, AValue agent_name);
AValue fn__agent_log(AValue name, AValue msg);
AValue fn_cmd_agent(AValue source, AValue agent_name, AValue health_port, AValue max_restarts, AValue no_restart);
AValue fn_cmd_plugin(AValue argv);
AValue fn__usage(void);
AValue fn_main(void);

AValue fn_lexer_is_ws(AValue c) {
    AValue __ret = a_void();
    c = a_retain(c);
    __ret = a_or(a_or(a_eq(c, a_string(" ")), a_eq(c, a_string("\t"))), a_eq(c, a_string("\r"))); goto __fn_cleanup;
__fn_cleanup:
    a_release(c);
    return __ret;
}

AValue fn_lexer_is_id_start(AValue c) {
    AValue __ret = a_void();
    c = a_retain(c);
    __ret = a_or(a_is_alpha(c), a_eq(c, a_string("_"))); goto __fn_cleanup;
__fn_cleanup:
    a_release(c);
    return __ret;
}

AValue fn_lexer_is_id_char(AValue c) {
    AValue __ret = a_void();
    c = a_retain(c);
    __ret = a_or(a_is_alnum(c), a_eq(c, a_string("_"))); goto __fn_cleanup;
__fn_cleanup:
    a_release(c);
    return __ret;
}

AValue fn_lexer_keyword_or_ident(AValue word) {
    AValue __ret = a_void();
    word = a_retain(word);
    if (a_truthy(a_eq(word, a_string("fn")))) {
        __ret = a_string("KwFn"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("ty")))) {
        __ret = a_string("KwTy"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("mod")))) {
        __ret = a_string("KwMod"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("let")))) {
        __ret = a_string("KwLet"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("mut")))) {
        __ret = a_string("KwMut"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("if")))) {
        __ret = a_string("KwIf"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("else")))) {
        __ret = a_string("KwElse"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("match")))) {
        __ret = a_string("KwMatch"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("for")))) {
        __ret = a_string("KwFor"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("in")))) {
        __ret = a_string("KwIn"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("while")))) {
        __ret = a_string("KwWhile"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("break")))) {
        __ret = a_string("KwBreak"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("continue")))) {
        __ret = a_string("KwContinue"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("ret")))) {
        __ret = a_string("KwRet"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("use")))) {
        __ret = a_string("KwUse"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("try")))) {
        __ret = a_string("KwTry"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("pub")))) {
        __ret = a_string("KwPub"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("effects")))) {
        __ret = a_string("KwEffects"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("pre")))) {
        __ret = a_string("KwPre"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("post")))) {
        __ret = a_string("KwPost"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("where")))) {
        __ret = a_string("KwWhere"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("extern")))) {
        __ret = a_string("KwExtern"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("true")))) {
        __ret = a_string("KwTrue"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("false")))) {
        __ret = a_string("KwFalse"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("i8")))) {
        __ret = a_string("TyI8"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("i16")))) {
        __ret = a_string("TyI16"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("i32")))) {
        __ret = a_string("TyI32"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("i64")))) {
        __ret = a_string("TyI64"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("u8")))) {
        __ret = a_string("TyU8"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("u16")))) {
        __ret = a_string("TyU16"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("u32")))) {
        __ret = a_string("TyU32"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("u64")))) {
        __ret = a_string("TyU64"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("f32")))) {
        __ret = a_string("TyF32"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("f64")))) {
        __ret = a_string("TyF64"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("bool")))) {
        __ret = a_string("TyBool"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("str")))) {
        __ret = a_string("TyStr"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("bytes")))) {
        __ret = a_string("TyBytes"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("void")))) {
        __ret = a_string("TyVoid"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("ptr")))) {
        __ret = a_string("TyPtr"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(word, a_string("_")))) {
        __ret = a_string("Underscore"); goto __fn_cleanup;
    }
    __ret = a_string("Ident"); goto __fn_cleanup;
__fn_cleanup:
    a_release(word);
    return __ret;
}

AValue fn_lexer_process_escape(AValue c) {
    AValue __ret = a_void();
    c = a_retain(c);
    if (a_truthy(a_eq(c, a_string("n")))) {
        __ret = a_string("\n"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(c, a_string("t")))) {
        __ret = a_string("\t"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(c, a_string("r")))) {
        __ret = a_string("\r"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(c, a_string("\\")))) {
        __ret = a_string("\\"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(c, a_string("\"")))) {
        __ret = a_string("\""); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(c, a_string("{")))) {
        __ret = a_string("{"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(c, a_string("}")))) {
        __ret = a_string("}"); goto __fn_cleanup;
    }
    __ret = a_retain(c); goto __fn_cleanup;
__fn_cleanup:
    a_release(c);
    return __ret;
}

AValue fn_lexer_lex(AValue src) {
    AValue chars = {0}, n = {0}, pos = {0}, toks = {0}, interp_depth = {0}, c = {0}, content = {0}, hash_start = {0}, hashes = {0}, hp = {0}, found = {0}, hi = {0}, ok = {0}, start = {0}, word = {0}, kind = {0}, is_float = {0}, text = {0}, done = {0}, sc = {0}, tlen = {0}, ended = {0};
    AValue __ret = a_void();
    src = a_retain(src);
    { AValue __old = chars; chars = a_str_chars(src); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    { AValue __old = pos; pos = a_int(0); a_release(__old); }
    { AValue __old = toks; toks = a_array_new(0); a_release(__old); }
    { AValue __old = interp_depth; interp_depth = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(pos, n))) {
        { AValue __old = c; c = a_array_get(chars, pos); a_release(__old); }
        if (a_truthy(a_eq(c, a_string("\n")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Newline")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(fn_lexer_is_ws(c))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string(";")))) {
            while (a_truthy(a_lt(pos, n))) {
                if (a_truthy(a_eq(a_array_get(chars, pos), a_string("\n")))) {
                    break;
                }
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("r")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("\"")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    { AValue __old = content; content = a_string(""); a_release(__old); }
                    while (a_truthy(a_lt(pos, n))) {
                        if (a_truthy(a_eq(a_array_get(chars, pos), a_string("\"")))) {
                            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                            break;
                        }
                        { AValue __old = content; content = a_add(content, a_array_get(chars, pos)); a_release(__old); }
                        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("Str")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                    continue;
                }
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("#")))) {
                    { AValue __old = hash_start; hash_start = a_retain(pos); a_release(__old); }
                    { AValue __old = hashes; hashes = a_int(0); a_release(__old); }
                    { AValue __old = hp; hp = a_add(pos, a_int(1)); a_release(__old); }
                    while (a_truthy(a_lt(hp, n))) {
                        if (a_truthy(a_eq(a_array_get(chars, hp), a_string("#")))) {
                            { AValue __old = hashes; hashes = a_add(hashes, a_int(1)); a_release(__old); }
                            { AValue __old = hp; hp = a_add(hp, a_int(1)); a_release(__old); }
                        } else {
                            break;
                        }
                    }
                    if (a_truthy(a_lt(hp, n))) {
                        if (a_truthy(a_eq(a_array_get(chars, hp), a_string("\"")))) {
                            { AValue __old = pos; pos = a_add(hp, a_int(1)); a_release(__old); }
                            { AValue __old = content; content = a_string(""); a_release(__old); }
                            { AValue __old = found; found = a_bool(0); a_release(__old); }
                            while (a_truthy(a_lt(pos, n))) {
                                if (a_truthy(a_eq(a_array_get(chars, pos), a_string("\"")))) {
                                    { AValue __old = hi; hi = a_int(0); a_release(__old); }
                                    { AValue __old = ok; ok = a_bool(1); a_release(__old); }
                                    while (a_truthy(a_lt(hi, hashes))) {
                                        if (a_truthy(a_gteq(a_add(a_add(pos, a_int(1)), hi), n))) {
                                            { AValue __old = ok; ok = a_bool(0); a_release(__old); }
                                            break;
                                        }
                                        if (a_truthy(a_neq(a_array_get(chars, a_add(a_add(pos, a_int(1)), hi)), a_string("#")))) {
                                            { AValue __old = ok; ok = a_bool(0); a_release(__old); }
                                            break;
                                        }
                                        { AValue __old = hi; hi = a_add(hi, a_int(1)); a_release(__old); }
                                    }
                                    if (a_truthy(ok)) {
                                        { AValue __old = pos; pos = a_add(a_add(pos, a_int(1)), hashes); a_release(__old); }
                                        { AValue __old = found; found = a_bool(1); a_release(__old); }
                                        break;
                                    }
                                }
                                { AValue __old = content; content = a_add(content, a_array_get(chars, pos)); a_release(__old); }
                                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                            }
                            { AValue __old = toks; toks = a_array_push(toks, a_string("Str")); a_release(__old); }
                            { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                            continue;
                        }
                    }
                    { AValue __old = pos; pos = a_retain(hash_start); a_release(__old); }
                }
            }
        }
        if (a_truthy(a_eq(c, a_string("`")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, pos), a_string("`")))) {
                    if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("`")))) {
                        { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                        if (a_truthy(a_lt(pos, n))) {
                            if (a_truthy(a_eq(a_array_get(chars, pos), a_string("\n")))) {
                                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                            }
                        }
                        { AValue __old = content; content = a_string(""); a_release(__old); }
                        while (a_truthy(a_lt(pos, n))) {
                            if (a_truthy(a_eq(a_array_get(chars, pos), a_string("`")))) {
                                if (a_truthy(a_lt(a_add(pos, a_int(2)), n))) {
                                    if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("`")))) {
                                        if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(2))), a_string("`")))) {
                                            { AValue __old = pos; pos = a_add(pos, a_int(3)); a_release(__old); }
                                            if (a_truthy(a_gt(a_len(content), a_int(0)))) {
                                                if (a_truthy(a_eq(a_str_slice(content, a_sub(a_len(content), a_int(1)), a_len(content)), a_string("\n")))) {
                                                    { AValue __old = content; content = a_str_slice(content, a_int(0), a_sub(a_len(content), a_int(1))); a_release(__old); }
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                            { AValue __old = content; content = a_add(content, a_array_get(chars, pos)); a_release(__old); }
                            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        }
                        { AValue __old = toks; toks = a_array_push(toks, a_string("Str")); a_release(__old); }
                        { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                        continue;
                    }
                }
            }
        }
        if (a_truthy(fn_lexer_is_id_start(c))) {
            { AValue __old = start; start = a_retain(pos); a_release(__old); }
            while (a_truthy(a_lt(pos, n))) {
                if (a_truthy(fn_lexer_is_id_char(a_array_get(chars, pos)))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                } else {
                    break;
                }
            }
            { AValue __old = word; word = a_str_slice(src, start, pos); a_release(__old); }
            { AValue __old = kind; kind = fn_lexer_keyword_or_ident(word); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, kind); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, word); a_release(__old); }
            continue;
        }
        if (a_truthy(a_is_digit(c))) {
            { AValue __old = start; start = a_retain(pos); a_release(__old); }
            { AValue __old = is_float; is_float = a_bool(0); a_release(__old); }
            while (a_truthy(a_lt(pos, n))) {
                if (a_truthy(a_is_digit(a_array_get(chars, pos)))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                } else {
                    if (a_truthy(a_eq(a_array_get(chars, pos), a_string(".")))) {
                        if (a_truthy(is_float)) {
                            break;
                        }
                        if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                            if (a_truthy(a_is_digit(a_array_get(chars, a_add(pos, a_int(1)))))) {
                                { AValue __old = is_float; is_float = a_bool(1); a_release(__old); }
                                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            }
            { AValue __old = text; text = a_str_slice(src, start, pos); a_release(__old); }
            if (a_truthy(is_float)) {
                { AValue __old = toks; toks = a_array_push(toks, a_string("Float")); a_release(__old); }
            } else {
                { AValue __old = toks; toks = a_array_push(toks, a_string("Int")); a_release(__old); }
            }
            { AValue __old = toks; toks = a_array_push(toks, text); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("\"")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = content; content = a_string(""); a_release(__old); }
            { AValue __old = done; done = a_bool(0); a_release(__old); }
            while (a_truthy(a_lt(pos, n))) {
                { AValue __old = sc; sc = a_array_get(chars, pos); a_release(__old); }
                if (a_truthy(a_eq(sc, a_string("\"")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    { AValue __old = done; done = a_bool(1); a_release(__old); }
                    break;
                }
                if (a_truthy(a_eq(sc, a_string("{")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("InterpStart")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                    { AValue __old = content; content = a_string(""); a_release(__old); }
                    { AValue __old = interp_depth; interp_depth = a_add(interp_depth, a_int(1)); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    { AValue __old = done; done = a_bool(1); a_release(__old); }
                    break;
                }
                if (a_truthy(a_eq(sc, a_string("\\")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    if (a_truthy(a_lt(pos, n))) {
                        { AValue __old = content; content = a_add(content, fn_lexer_process_escape(a_array_get(chars, pos))); a_release(__old); }
                        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    }
                    continue;
                }
                { AValue __old = content; content = a_add(content, sc); a_release(__old); }
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            }
            if (a_truthy(done)) {
                if (a_truthy(a_eq(interp_depth, a_int(0)))) {
                    { AValue __old = tlen; tlen = a_len(toks); a_release(__old); }
                    if (a_truthy(a_gt(tlen, a_int(0)))) {
                        if (a_truthy(a_eq(a_array_get(toks, a_sub(tlen, a_int(2))), a_string("InterpStart")))) {
                            continue;
                        }
                    }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("Str")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                }
            } else {
                { AValue __old = toks; toks = a_array_push(toks, a_string("Str")); a_release(__old); }
                { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
            }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("}")))) {
            if (a_truthy(a_gt(interp_depth, a_int(0)))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = content; content = a_string(""); a_release(__old); }
                { AValue __old = ended; ended = a_bool(0); a_release(__old); }
                while (a_truthy(a_lt(pos, n))) {
                    { AValue __old = sc; sc = a_array_get(chars, pos); a_release(__old); }
                    if (a_truthy(a_eq(sc, a_string("\"")))) {
                        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        { AValue __old = interp_depth; interp_depth = a_sub(interp_depth, a_int(1)); a_release(__old); }
                        { AValue __old = toks; toks = a_array_push(toks, a_string("InterpEnd")); a_release(__old); }
                        { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                        { AValue __old = ended; ended = a_bool(1); a_release(__old); }
                        break;
                    }
                    if (a_truthy(a_eq(sc, a_string("{")))) {
                        { AValue __old = toks; toks = a_array_push(toks, a_string("InterpMid")); a_release(__old); }
                        { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                        { AValue __old = content; content = a_string(""); a_release(__old); }
                        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        { AValue __old = ended; ended = a_bool(1); a_release(__old); }
                        break;
                    }
                    if (a_truthy(a_eq(sc, a_string("\\")))) {
                        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        if (a_truthy(a_lt(pos, n))) {
                            { AValue __old = content; content = a_add(content, fn_lexer_process_escape(a_array_get(chars, pos))); a_release(__old); }
                            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        }
                        continue;
                    }
                    { AValue __old = content; content = a_add(content, sc); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                }
                if (a_truthy(ended)) {
                    continue;
                }
                { AValue __old = interp_depth; interp_depth = a_sub(interp_depth, a_int(1)); a_release(__old); }
                { AValue __old = toks; toks = a_array_push(toks, a_string("InterpEnd")); a_release(__old); }
                { AValue __old = toks; toks = a_array_push(toks, content); a_release(__old); }
                continue;
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("RBrace")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("}")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("(")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("LParen")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("(")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string(")")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("RParen")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string(")")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("{")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("LBrace")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("{")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("[")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("LBracket")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("[")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("]")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("RBracket")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("]")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string(",")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Comma")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string(",")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string(":")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Colon")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string(":")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("?")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Question")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("?")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("#")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Hash")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("#")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("+")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Plus")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("+")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("*")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Star")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("*")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("%")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Percent")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("%")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("/")))) {
            { AValue __old = toks; toks = a_array_push(toks, a_string("Slash")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("/")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string(".")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(2)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string(".")))) {
                    if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(2))), a_string(".")))) {
                        { AValue __old = toks; toks = a_array_push(toks, a_string("DotDotDot")); a_release(__old); }
                        { AValue __old = toks; toks = a_array_push(toks, a_string("...")); a_release(__old); }
                        { AValue __old = pos; pos = a_add(pos, a_int(3)); a_release(__old); }
                        continue;
                    }
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Dot")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string(".")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("-")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string(">")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("Arrow")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("->")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Minus")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("-")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("=")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("=")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("EqEq")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("==")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string(">")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("FatArrow")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("=>")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Eq")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("=")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("!")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("=")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("NotEq")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("!=")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Bang")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("!")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("<")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("=")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("LtEq")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("<=")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Lt")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("<")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string(">")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("=")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("GtEq")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string(">=")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Gt")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string(">")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("&")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("&")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("AmpAmp")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("&&")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Amp")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("&")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("|")))) {
            if (a_truthy(a_lt(a_add(pos, a_int(1)), n))) {
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string("|")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("PipePipe")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("||")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
                if (a_truthy(a_eq(a_array_get(chars, a_add(pos, a_int(1))), a_string(">")))) {
                    { AValue __old = toks; toks = a_array_push(toks, a_string("PipeArrow")); a_release(__old); }
                    { AValue __old = toks; toks = a_array_push(toks, a_string("|>")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(2)); a_release(__old); }
                    continue;
                }
            }
            { AValue __old = toks; toks = a_array_push(toks, a_string("Pipe")); a_release(__old); }
            { AValue __old = toks; toks = a_array_push(toks, a_string("|")); a_release(__old); }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            continue;
        }
        { AValue __old = toks; toks = a_array_push(toks, a_string("Unknown")); a_release(__old); }
        { AValue __old = toks; toks = a_array_push(toks, c); a_release(__old); }
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
    }
    { AValue __old = toks; toks = a_array_push(toks, a_string("Eof")); a_release(__old); }
    { AValue __old = toks; toks = a_array_push(toks, a_string("")); a_release(__old); }
    __ret = a_retain(toks); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(n);
    a_release(pos);
    a_release(toks);
    a_release(interp_depth);
    a_release(c);
    a_release(content);
    a_release(hash_start);
    a_release(hashes);
    a_release(hp);
    a_release(found);
    a_release(hi);
    a_release(ok);
    a_release(start);
    a_release(word);
    a_release(kind);
    a_release(is_float);
    a_release(text);
    a_release(done);
    a_release(sc);
    a_release(tlen);
    a_release(ended);
    a_release(src);
    return __ret;
}

AValue fn_lexer_tk(AValue toks, AValue i) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    i = a_retain(i);
    __ret = a_array_get(toks, a_mul(i, a_int(2))); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(i);
    return __ret;
}

AValue fn_lexer_tv(AValue toks, AValue i) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    i = a_retain(i);
    __ret = a_array_get(toks, a_add(a_mul(i, a_int(2)), a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(i);
    return __ret;
}

AValue fn_lexer_tcount(AValue toks) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    __ret = a_div(a_len(toks), a_int(2)); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    return __ret;
}

AValue fn_lexer_skip_nl(AValue toks, AValue p) {
    AValue i = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    p = a_retain(p);
    { AValue __old = i; i = a_retain(p); a_release(__old); }
    while (a_truthy(a_eq(fn_lexer_tk(toks, i), a_string("Newline")))) {
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(i); goto __fn_cleanup;
__fn_cleanup:
    a_release(i);
    a_release(toks);
    a_release(p);
    return __ret;
}

AValue fn_ast_mk_program(AValue items) {
    AValue __ret = a_void();
    items = a_retain(items);
    __ret = a_map_new(2, "tag", a_string("Program"), "items", items); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    return __ret;
}

AValue fn_ast_mk_fn_decl(AValue name, AValue params, AValue ret_type, AValue effs, AValue precond, AValue postcond, AValue body) {
    AValue __ret = a_void();
    name = a_retain(name);
    params = a_retain(params);
    ret_type = a_retain(ret_type);
    effs = a_retain(effs);
    precond = a_retain(precond);
    postcond = a_retain(postcond);
    body = a_retain(body);
    __ret = a_map_new(8, "tag", a_string("FnDecl"), "name", name, "params", params, "ret_type", ret_type, "effects", effs, "pre", precond, "post", postcond, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(params);
    a_release(ret_type);
    a_release(effs);
    a_release(precond);
    a_release(postcond);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_type_decl(AValue name, AValue type_params, AValue body) {
    AValue __ret = a_void();
    name = a_retain(name);
    type_params = a_retain(type_params);
    body = a_retain(body);
    __ret = a_map_new(4, "tag", a_string("TypeDecl"), "name", name, "params", type_params, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(type_params);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_mod_decl(AValue name, AValue items) {
    AValue __ret = a_void();
    name = a_retain(name);
    items = a_retain(items);
    __ret = a_map_new(3, "tag", a_string("ModDecl"), "name", name, "items", items); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(items);
    return __ret;
}

AValue fn_ast_mk_use_decl(AValue path) {
    AValue __ret = a_void();
    path = a_retain(path);
    __ret = a_map_new(2, "tag", a_string("UseDecl"), "path", path); goto __fn_cleanup;
__fn_cleanup:
    a_release(path);
    return __ret;
}

AValue fn_ast_mk_extern_fn(AValue name, AValue params, AValue ret_type) {
    AValue __ret = a_void();
    name = a_retain(name);
    params = a_retain(params);
    ret_type = a_retain(ret_type);
    __ret = a_map_new(4, "tag", a_string("ExternFn"), "name", name, "params", params, "ret_type", ret_type); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(params);
    a_release(ret_type);
    return __ret;
}

AValue fn_ast_mk_type_record(AValue fields) {
    AValue __ret = a_void();
    fields = a_retain(fields);
    __ret = a_map_new(2, "tag", a_string("TypeRecord"), "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    a_release(fields);
    return __ret;
}

AValue fn_ast_mk_type_sum(AValue variants) {
    AValue __ret = a_void();
    variants = a_retain(variants);
    __ret = a_map_new(2, "tag", a_string("TypeSum"), "variants", variants); goto __fn_cleanup;
__fn_cleanup:
    a_release(variants);
    return __ret;
}

AValue fn_ast_mk_type_alias(AValue typ, AValue where_expr) {
    AValue __ret = a_void();
    typ = a_retain(typ);
    where_expr = a_retain(where_expr);
    __ret = a_map_new(3, "tag", a_string("TypeAlias"), "type", typ, "where", where_expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(typ);
    a_release(where_expr);
    return __ret;
}

AValue fn_ast_mk_variant(AValue name, AValue fields) {
    AValue __ret = a_void();
    name = a_retain(name);
    fields = a_retain(fields);
    __ret = a_map_new(3, "tag", a_string("Variant"), "name", name, "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(fields);
    return __ret;
}

AValue fn_ast_mk_field(AValue name, AValue typ) {
    AValue __ret = a_void();
    name = a_retain(name);
    typ = a_retain(typ);
    __ret = a_map_new(3, "tag", a_string("Field"), "name", name, "type", typ); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(typ);
    return __ret;
}

AValue fn_ast_mk_ty_named(AValue name, AValue args) {
    AValue __ret = a_void();
    name = a_retain(name);
    args = a_retain(args);
    __ret = a_map_new(3, "tag", a_string("TyNamed"), "name", name, "args", args); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(args);
    return __ret;
}

AValue fn_ast_mk_ty_array(AValue elem) {
    AValue __ret = a_void();
    elem = a_retain(elem);
    __ret = a_map_new(2, "tag", a_string("TyArray"), "elem", elem); goto __fn_cleanup;
__fn_cleanup:
    a_release(elem);
    return __ret;
}

AValue fn_ast_mk_ty_tuple(AValue elems) {
    AValue __ret = a_void();
    elems = a_retain(elems);
    __ret = a_map_new(2, "tag", a_string("TyTuple"), "elems", elems); goto __fn_cleanup;
__fn_cleanup:
    a_release(elems);
    return __ret;
}

AValue fn_ast_mk_ty_record(AValue fields) {
    AValue __ret = a_void();
    fields = a_retain(fields);
    __ret = a_map_new(2, "tag", a_string("TyRecord"), "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    a_release(fields);
    return __ret;
}

AValue fn_ast_mk_ty_fn(AValue params, AValue ret_ty) {
    AValue __ret = a_void();
    params = a_retain(params);
    ret_ty = a_retain(ret_ty);
    __ret = a_map_new(3, "tag", a_string("TyFn"), "params", params, "ret", ret_ty); goto __fn_cleanup;
__fn_cleanup:
    a_release(params);
    a_release(ret_ty);
    return __ret;
}

AValue fn_ast_mk_ty_map(AValue key, AValue val) {
    AValue __ret = a_void();
    key = a_retain(key);
    val = a_retain(val);
    __ret = a_map_new(3, "tag", a_string("TyMap"), "key", key, "val", val); goto __fn_cleanup;
__fn_cleanup:
    a_release(key);
    a_release(val);
    return __ret;
}

AValue fn_ast_mk_ty_prim(AValue name) {
    AValue __ret = a_void();
    name = a_retain(name);
    __ret = a_map_new(2, "tag", a_string("TyPrim"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    return __ret;
}

AValue fn_ast_mk_ty_infer(void) {
    AValue __ret = a_void();
    __ret = a_map_new(1, "tag", a_string("TyInfer")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_param(AValue name, AValue typ) {
    AValue __ret = a_void();
    name = a_retain(name);
    typ = a_retain(typ);
    __ret = a_map_new(3, "tag", a_string("Param"), "name", name, "type", typ); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(typ);
    return __ret;
}

AValue fn_ast_mk_block(AValue stmts) {
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    __ret = a_map_new(2, "tag", a_string("Block"), "stmts", stmts); goto __fn_cleanup;
__fn_cleanup:
    a_release(stmts);
    return __ret;
}

AValue fn_ast_mk_let(AValue mutable, AValue name, AValue typ, AValue value) {
    AValue __ret = a_void();
    mutable = a_retain(mutable);
    name = a_retain(name);
    typ = a_retain(typ);
    value = a_retain(value);
    __ret = a_map_new(5, "tag", a_string("Let"), "mutable", mutable, "name", name, "type", typ, "value", value); goto __fn_cleanup;
__fn_cleanup:
    a_release(mutable);
    a_release(name);
    a_release(typ);
    a_release(value);
    return __ret;
}

AValue fn_ast_mk_assign(AValue target, AValue value) {
    AValue __ret = a_void();
    target = a_retain(target);
    value = a_retain(value);
    __ret = a_map_new(3, "tag", a_string("Assign"), "target", target, "value", value); goto __fn_cleanup;
__fn_cleanup:
    a_release(target);
    a_release(value);
    return __ret;
}

AValue fn_ast_mk_return(AValue expr) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    __ret = a_map_new(2, "tag", a_string("Return"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    return __ret;
}

AValue fn_ast_mk_expr_stmt(AValue expr) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    __ret = a_map_new(2, "tag", a_string("ExprStmt"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    return __ret;
}

AValue fn_ast_mk_if_stmt(AValue cond, AValue then_block, AValue else_branch) {
    AValue __ret = a_void();
    cond = a_retain(cond);
    then_block = a_retain(then_block);
    else_branch = a_retain(else_branch);
    __ret = a_map_new(4, "tag", a_string("If"), "cond", cond, "then", then_block, "else", else_branch); goto __fn_cleanup;
__fn_cleanup:
    a_release(cond);
    a_release(then_block);
    a_release(else_branch);
    return __ret;
}

AValue fn_ast_mk_match_stmt(AValue expr, AValue arms) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    arms = a_retain(arms);
    __ret = a_map_new(3, "tag", a_string("Match"), "expr", expr, "arms", arms); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    a_release(arms);
    return __ret;
}

AValue fn_ast_mk_for_stmt(AValue var, AValue typ, AValue iter, AValue body) {
    AValue __ret = a_void();
    var = a_retain(var);
    typ = a_retain(typ);
    iter = a_retain(iter);
    body = a_retain(body);
    __ret = a_map_new(5, "tag", a_string("For"), "var", var, "type", typ, "iter", iter, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(var);
    a_release(typ);
    a_release(iter);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_while_stmt(AValue cond, AValue body) {
    AValue __ret = a_void();
    cond = a_retain(cond);
    body = a_retain(body);
    __ret = a_map_new(3, "tag", a_string("While"), "cond", cond, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(cond);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_let_destructure(AValue bindings, AValue rest, AValue value) {
    AValue __ret = a_void();
    bindings = a_retain(bindings);
    rest = a_retain(rest);
    value = a_retain(value);
    __ret = a_map_new(4, "tag", a_string("LetDestructure"), "bindings", bindings, "rest", rest, "value", value); goto __fn_cleanup;
__fn_cleanup:
    a_release(bindings);
    a_release(rest);
    a_release(value);
    return __ret;
}

AValue fn_ast_mk_for_destructure(AValue bindings, AValue iter, AValue body) {
    AValue __ret = a_void();
    bindings = a_retain(bindings);
    iter = a_retain(iter);
    body = a_retain(body);
    __ret = a_map_new(4, "tag", a_string("ForDestructure"), "bindings", bindings, "iter", iter, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(bindings);
    a_release(iter);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_break(void) {
    AValue __ret = a_void();
    __ret = a_map_new(1, "tag", a_string("Break")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_continue(void) {
    AValue __ret = a_void();
    __ret = a_map_new(1, "tag", a_string("Continue")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_else_block(AValue block) {
    AValue __ret = a_void();
    block = a_retain(block);
    __ret = a_map_new(2, "tag", a_string("ElseBlock"), "block", block); goto __fn_cleanup;
__fn_cleanup:
    a_release(block);
    return __ret;
}

AValue fn_ast_mk_else_if(AValue if_stmt) {
    AValue __ret = a_void();
    if_stmt = a_retain(if_stmt);
    __ret = a_map_new(2, "tag", a_string("ElseIf"), "stmt", if_stmt); goto __fn_cleanup;
__fn_cleanup:
    a_release(if_stmt);
    return __ret;
}

AValue fn_ast_mk_match_arm(AValue pattern, AValue guard, AValue body) {
    AValue __ret = a_void();
    pattern = a_retain(pattern);
    guard = a_retain(guard);
    body = a_retain(body);
    __ret = a_map_new(4, "tag", a_string("MatchArm"), "pattern", pattern, "guard", guard, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(pattern);
    a_release(guard);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_pat_ident(AValue name) {
    AValue __ret = a_void();
    name = a_retain(name);
    __ret = a_map_new(2, "tag", a_string("PatIdent"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    return __ret;
}

AValue fn_ast_mk_pat_constructor(AValue name, AValue args) {
    AValue __ret = a_void();
    name = a_retain(name);
    args = a_retain(args);
    __ret = a_map_new(3, "tag", a_string("PatConstructor"), "name", name, "args", args); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(args);
    return __ret;
}

AValue fn_ast_mk_pat_literal(AValue lit) {
    AValue __ret = a_void();
    lit = a_retain(lit);
    __ret = a_map_new(2, "tag", a_string("PatLiteral"), "value", lit); goto __fn_cleanup;
__fn_cleanup:
    a_release(lit);
    return __ret;
}

AValue fn_ast_mk_pat_wildcard(void) {
    AValue __ret = a_void();
    __ret = a_map_new(1, "tag", a_string("PatWildcard")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_array(AValue elems) {
    AValue __ret = a_void();
    elems = a_retain(elems);
    __ret = a_map_new(2, "tag", a_string("PatArray"), "elems", elems); goto __fn_cleanup;
__fn_cleanup:
    a_release(elems);
    return __ret;
}

AValue fn_ast_mk_pat_array_elem(AValue pat) {
    AValue __ret = a_void();
    pat = a_retain(pat);
    __ret = a_map_new(2, "tag", a_string("PatElem"), "pattern", pat); goto __fn_cleanup;
__fn_cleanup:
    a_release(pat);
    return __ret;
}

AValue fn_ast_mk_pat_rest(AValue name) {
    AValue __ret = a_void();
    name = a_retain(name);
    __ret = a_map_new(2, "tag", a_string("PatRest"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    return __ret;
}

AValue fn_ast_mk_pat_map(AValue entries) {
    AValue __ret = a_void();
    entries = a_retain(entries);
    __ret = a_map_new(2, "tag", a_string("PatMap"), "entries", entries); goto __fn_cleanup;
__fn_cleanup:
    a_release(entries);
    return __ret;
}

AValue fn_ast_mk_pat_map_entry(AValue key, AValue pat) {
    AValue __ret = a_void();
    key = a_retain(key);
    pat = a_retain(pat);
    __ret = a_map_new(3, "tag", a_string("PatMapEntry"), "key", key, "pattern", pat); goto __fn_cleanup;
__fn_cleanup:
    a_release(key);
    a_release(pat);
    return __ret;
}

AValue fn_ast_mk_int(AValue val) {
    AValue __ret = a_void();
    val = a_retain(val);
    __ret = a_map_new(2, "tag", a_string("Int"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    a_release(val);
    return __ret;
}

AValue fn_ast_mk_float(AValue val) {
    AValue __ret = a_void();
    val = a_retain(val);
    __ret = a_map_new(2, "tag", a_string("Float"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    a_release(val);
    return __ret;
}

AValue fn_ast_mk_string(AValue val) {
    AValue __ret = a_void();
    val = a_retain(val);
    __ret = a_map_new(2, "tag", a_string("String"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    a_release(val);
    return __ret;
}

AValue fn_ast_mk_bool_lit(AValue val) {
    AValue __ret = a_void();
    val = a_retain(val);
    __ret = a_map_new(2, "tag", a_string("Bool"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    a_release(val);
    return __ret;
}

AValue fn_ast_mk_ident(AValue name) {
    AValue __ret = a_void();
    name = a_retain(name);
    __ret = a_map_new(2, "tag", a_string("Ident"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    return __ret;
}

AValue fn_ast_mk_binop(AValue op, AValue left, AValue right) {
    AValue __ret = a_void();
    op = a_retain(op);
    left = a_retain(left);
    right = a_retain(right);
    __ret = a_map_new(4, "tag", a_string("BinOp"), "op", op, "left", left, "right", right); goto __fn_cleanup;
__fn_cleanup:
    a_release(op);
    a_release(left);
    a_release(right);
    return __ret;
}

AValue fn_ast_mk_unary(AValue op, AValue expr) {
    AValue __ret = a_void();
    op = a_retain(op);
    expr = a_retain(expr);
    __ret = a_map_new(3, "tag", a_string("UnaryOp"), "op", op, "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(op);
    a_release(expr);
    return __ret;
}

AValue fn_ast_mk_call(AValue func, AValue args) {
    AValue __ret = a_void();
    func = a_retain(func);
    args = a_retain(args);
    __ret = a_map_new(3, "tag", a_string("Call"), "func", func, "args", args); goto __fn_cleanup;
__fn_cleanup:
    a_release(func);
    a_release(args);
    return __ret;
}

AValue fn_ast_mk_field_access(AValue expr, AValue field) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    field = a_retain(field);
    __ret = a_map_new(3, "tag", a_string("FieldAccess"), "expr", expr, "field", field); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    a_release(field);
    return __ret;
}

AValue fn_ast_mk_index(AValue expr, AValue index) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    index = a_retain(index);
    __ret = a_map_new(3, "tag", a_string("Index"), "expr", expr, "index", index); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    a_release(index);
    return __ret;
}

AValue fn_ast_mk_try(AValue expr) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    __ret = a_map_new(2, "tag", a_string("Try"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    return __ret;
}

AValue fn_ast_mk_try_block(AValue block) {
    AValue __ret = a_void();
    block = a_retain(block);
    __ret = a_map_new(2, "tag", a_string("TryBlock"), "block", block); goto __fn_cleanup;
__fn_cleanup:
    a_release(block);
    return __ret;
}

AValue fn_ast_mk_if_expr(AValue cond, AValue then_block, AValue else_block) {
    AValue __ret = a_void();
    cond = a_retain(cond);
    then_block = a_retain(then_block);
    else_block = a_retain(else_block);
    __ret = a_map_new(4, "tag", a_string("IfExpr"), "cond", cond, "then", then_block, "else", else_block); goto __fn_cleanup;
__fn_cleanup:
    a_release(cond);
    a_release(then_block);
    a_release(else_block);
    return __ret;
}

AValue fn_ast_mk_match_expr(AValue expr, AValue arms) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    arms = a_retain(arms);
    __ret = a_map_new(3, "tag", a_string("MatchExpr"), "expr", expr, "arms", arms); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    a_release(arms);
    return __ret;
}

AValue fn_ast_mk_block_expr(AValue block) {
    AValue __ret = a_void();
    block = a_retain(block);
    __ret = a_map_new(2, "tag", a_string("BlockExpr"), "block", block); goto __fn_cleanup;
__fn_cleanup:
    a_release(block);
    return __ret;
}

AValue fn_ast_mk_array(AValue elems) {
    AValue __ret = a_void();
    elems = a_retain(elems);
    __ret = a_map_new(2, "tag", a_string("Array"), "elems", elems); goto __fn_cleanup;
__fn_cleanup:
    a_release(elems);
    return __ret;
}

AValue fn_ast_mk_record(AValue fields) {
    AValue __ret = a_void();
    fields = a_retain(fields);
    __ret = a_map_new(2, "tag", a_string("Record"), "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    a_release(fields);
    return __ret;
}

AValue fn_ast_mk_record_field(AValue name, AValue value) {
    AValue __ret = a_void();
    name = a_retain(name);
    value = a_retain(value);
    __ret = a_map_new(3, "tag", a_string("RecordField"), "name", name, "value", value); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(value);
    return __ret;
}

AValue fn_ast_mk_lambda(AValue params, AValue body) {
    AValue __ret = a_void();
    params = a_retain(params);
    body = a_retain(body);
    __ret = a_map_new(3, "tag", a_string("Lambda"), "params", params, "body", body); goto __fn_cleanup;
__fn_cleanup:
    a_release(params);
    a_release(body);
    return __ret;
}

AValue fn_ast_mk_pipe(AValue left, AValue right) {
    AValue __ret = a_void();
    left = a_retain(left);
    right = a_retain(right);
    __ret = a_map_new(3, "tag", a_string("Pipe"), "left", left, "right", right); goto __fn_cleanup;
__fn_cleanup:
    a_release(left);
    a_release(right);
    return __ret;
}

AValue fn_ast_mk_interpolation(AValue parts) {
    AValue __ret = a_void();
    parts = a_retain(parts);
    __ret = a_map_new(2, "tag", a_string("Interpolation"), "parts", parts); goto __fn_cleanup;
__fn_cleanup:
    a_release(parts);
    return __ret;
}

AValue fn_ast_mk_interp_lit(AValue text) {
    AValue __ret = a_void();
    text = a_retain(text);
    __ret = a_map_new(2, "tag", a_string("InterpLit"), "value", text); goto __fn_cleanup;
__fn_cleanup:
    a_release(text);
    return __ret;
}

AValue fn_ast_mk_interp_expr(AValue expr) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    __ret = a_map_new(2, "tag", a_string("InterpExpr"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    return __ret;
}

AValue fn_ast_mk_map_literal(AValue entries) {
    AValue __ret = a_void();
    entries = a_retain(entries);
    __ret = a_map_new(2, "tag", a_string("MapLiteral"), "entries", entries); goto __fn_cleanup;
__fn_cleanup:
    a_release(entries);
    return __ret;
}

AValue fn_ast_mk_map_entry(AValue key, AValue value) {
    AValue __ret = a_void();
    key = a_retain(key);
    value = a_retain(value);
    __ret = a_map_new(3, "tag", a_string("MapEntry"), "key", key, "value", value); goto __fn_cleanup;
__fn_cleanup:
    a_release(key);
    a_release(value);
    return __ret;
}

AValue fn_ast_mk_spread(AValue expr) {
    AValue __ret = a_void();
    expr = a_retain(expr);
    __ret = a_map_new(2, "tag", a_string("Spread"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(expr);
    return __ret;
}

AValue fn_ast_mk_void_lit(void) {
    AValue __ret = a_void();
    __ret = a_map_new(1, "tag", a_string("Void")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_tk(AValue toks, AValue i) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    i = a_retain(i);
    __ret = a_array_get(toks, a_mul(i, a_int(2))); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(i);
    return __ret;
}

AValue fn_parser_tv(AValue toks, AValue i) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    i = a_retain(i);
    __ret = a_array_get(toks, a_add(a_mul(i, a_int(2)), a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(i);
    return __ret;
}

AValue fn_parser_err(AValue msg, AValue pos) {
    AValue __ret = a_void();
    msg = a_retain(msg);
    pos = a_retain(pos);
    __ret = a_array_new(2, a_map_new(3, "tag", a_string("ParseError"), "msg", msg, "pos", pos), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(msg);
    a_release(pos);
    return __ret;
}

AValue fn_parser__is_perr(AValue r) {
    AValue __ret = a_void();
    r = a_retain(r);
    if (a_truthy(a_eq(a_type_of(a_array_get(r, a_int(0))), a_string("map")))) {
        __ret = a_and(a_map_has(a_array_get(r, a_int(0)), a_string("tag")), a_eq(a_array_get(a_array_get(r, a_int(0)), a_string("tag")), a_string("ParseError"))); goto __fn_cleanup;
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    return __ret;
}

AValue fn_parser_skip_nl(AValue toks, AValue pos) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Newline")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_expect(AValue toks, AValue pos, AValue kind) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    kind = a_retain(kind);
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), kind))) {
        __ret = a_array_new(2, fn_parser_tv(toks, pos), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_add(a_add(a_string("expected "), kind), a_string(", found ")), fn_parser_tk(toks, pos)), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(pos);
    a_release(kind);
    return __ret;
}

AValue fn_parser_expect_ident(AValue toks, AValue pos) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Ident")))) {
        __ret = a_array_new(2, fn_parser_tv(toks, pos), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected identifier, found "), fn_parser_tk(toks, pos)), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_expect_nl_or_eof(AValue toks, AValue pos) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Eof")))) {
        __ret = a_array_new(2, a_bool(1), pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        __ret = a_array_new(2, a_bool(1), pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Newline")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
        __ret = a_array_new(2, a_bool(1), pos); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected newline, found "), fn_parser_tk(toks, pos)), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse(AValue src) {
    AValue toks = {0}, r = {0};
    AValue __ret = a_void();
    src = a_retain(src);
    { AValue __old = toks; toks = fn_lexer_lex(src); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_program(toks, a_int(0)); a_release(__old); }
    __ret = a_array_get(r, a_int(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(r);
    a_release(src);
    return __ret;
}

AValue fn_parser_parse_program(AValue toks, AValue pos) {
    AValue items = {0}, r = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = items; items = a_array_new(0); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("Eof")))) {
        { AValue __old = r; r = fn_parser_parse_top_level(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = items; items = a_array_push(items, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_program(items), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(r);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_top_level(AValue toks, AValue pos) {
    AValue k = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("KwFn")))) {
        __ret = fn_parser_parse_fn_decl(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwTy")))) {
        __ret = fn_parser_parse_type_decl(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwMod")))) {
        __ret = fn_parser_parse_mod_decl(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwUse")))) {
        __ret = fn_parser_parse_use_decl(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwExtern")))) {
        __ret = fn_parser_parse_extern_fn(toks, pos); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected fn, ty, mod, use, or extern; found "), k), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_fn_decl(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, params = {0}, ret_type = {0}, effs = {0}, precond = {0}, postcond = {0}, body = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwFn")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
        { AValue __old = r; r = fn_parser_parse_param_list(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = ret_type; ret_type = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Arrow")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = ret_type; ret_type = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = effs; effs = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwEffects")))) {
        { AValue __old = r; r = fn_parser_parse_effects(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = effs; effs = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = precond; precond = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwPre")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = precond; precond = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = postcond; postcond = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwPost")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = postcond; postcond = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_fn_decl(name, params, ret_type, effs, precond, postcond, body), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(params);
    a_release(ret_type);
    a_release(effs);
    a_release(precond);
    a_release(postcond);
    a_release(body);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_extern_fn(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, params = {0}, ret_type = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwExtern")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwFn")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
        { AValue __old = r; r = fn_parser_parse_param_list(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = ret_type; ret_type = fn_ast_mk_ty_prim(a_string("void")); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Arrow")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = ret_type; ret_type = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_extern_fn(name, params, ret_type), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(params);
    a_release(ret_type);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_param_list(AValue toks, AValue pos) {
    AValue params = {0}, r = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_param(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_param(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, params, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(params);
    a_release(r);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_param(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, typ = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = typ; typ = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Colon")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = typ; typ = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_param(name, typ), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(typ);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_effects(AValue toks, AValue pos) {
    AValue r = {0}, effs = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwEffects")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBracket")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = effs; effs = a_array_new(0); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = effs; effs = a_array_push(effs, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = effs; effs = a_array_push(effs, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, effs, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(effs);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_mod_decl(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, items = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwMod")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = items; items = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        { AValue __old = r; r = fn_parser_parse_top_level(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = items; items = a_array_push(items, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_mod_decl(name, items), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(items);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_use_decl(AValue toks, AValue pos) {
    AValue r = {0}, path = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwUse")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = path; path = a_array_new(1, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Dot")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = path; path = a_array_push(path, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_use_decl(path), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(path);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_type_decl(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, type_params = {0}, body = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwTy")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = type_params; type_params = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Lt")))) {
        { AValue __old = r; r = fn_parser_parse_type_params(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = type_params; type_params = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Eq")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_type_body(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_type_decl(name, type_params, body), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(type_params);
    a_release(body);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_type_params(AValue toks, AValue pos) {
    AValue r = {0}, params = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Lt")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Gt")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, params, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(params);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_looks_like_sum(AValue toks, AValue pos) {
    AValue next = {0}, j = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("Ident")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = next; next = a_add(pos, a_int(1)); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, next), a_string("LParen")))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(fn_parser_tk(toks, next), a_string("Pipe")))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(fn_parser_tk(toks, next), a_string("Newline")))) {
        { AValue __old = j; j = a_retain(next); a_release(__old); }
        while (a_truthy(a_eq(fn_parser_tk(toks, j), a_string("Newline")))) {
            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(fn_parser_tk(toks, j), a_string("Pipe")))) {
            __ret = a_bool(1); goto __fn_cleanup;
        }
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(next);
    a_release(j);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_type_body(AValue toks, AValue pos) {
    AValue r = {0}, variants = {0}, alias_ty = {0}, where_expr = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBrace")))) {
        { AValue __old = r; r = fn_parser_parse_record_fields(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_type_record(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(fn_parser_looks_like_sum(toks, pos))) {
        { AValue __old = variants; variants = a_array_new(0); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_variant(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = variants; variants = a_array_push(variants, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Pipe")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_variant(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = variants; variants = a_array_push(variants, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        __ret = a_array_new(2, fn_ast_mk_type_sum(variants), pos); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = alias_ty; alias_ty = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = where_expr; where_expr = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwWhere")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = where_expr; where_expr = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_type_alias(alias_ty, where_expr), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(variants);
    a_release(alias_ty);
    a_release(where_expr);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_variant(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, fields = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = fields; fields = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LParen")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_variant(name, fields), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(fields);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_record_fields(AValue toks, AValue pos) {
    AValue r = {0}, fields = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = fields; fields = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        { AValue __old = r; r = fn_parser_parse_field(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
                break;
            }
            { AValue __old = r; r = fn_parser_parse_field(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fields, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(fields);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_field(AValue toks, AValue pos) {
    AValue r = {0}, name = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_field(name, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_type_expr(AValue toks, AValue pos) {
    AValue k = {0}, r = {0}, inner = {0}, types = {0}, params = {0}, key_ty = {0}, val_ty = {0}, name = {0}, type_args = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("TyI8")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("i8")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyI16")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("i16")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyI32")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("i32")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyI64")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("i64")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyU8")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("u8")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyU16")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("u16")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyU32")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("u32")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyU64")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("u64")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyF32")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("f32")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyF64")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("f64")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyBool")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("bool")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyStr")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("str")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyBytes")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("bytes")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyVoid")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("void")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyPtr")))) {
        __ret = a_array_new(2, fn_ast_mk_ty_prim(a_string("ptr")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LBracket")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = inner; inner = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_ty_array(inner), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LParen")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = types; types = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = types; types = a_array_push(types, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = types; types = a_array_push(types, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        __ret = a_array_new(2, fn_ast_mk_ty_tuple(types), pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwFn")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = params; params = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Arrow")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_ty_fn(params, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Hash")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = key_ty; key_ty = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = val_ty; val_ty = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_ty_map(key_ty, val_ty), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Ident")))) {
        { AValue __old = name; name = fn_parser_tv(toks, pos); a_release(__old); }
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = type_args; type_args = a_array_new(0); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Lt")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = type_args; type_args = a_array_push(type_args, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = type_args; type_args = a_array_push(type_args, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Gt")); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        __ret = a_array_new(2, fn_ast_mk_ty_named(name, type_args), pos); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected type, found "), k), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(r);
    a_release(inner);
    a_release(types);
    a_release(params);
    a_release(key_ty);
    a_release(val_ty);
    a_release(name);
    a_release(type_args);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_block(AValue toks, AValue pos) {
    AValue r = {0}, stmts = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = stmts; stmts = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Eof")))) {
            __ret = fn_parser_err(a_string("unexpected end of file in block"), pos); goto __fn_cleanup;
        }
        { AValue __old = r; r = fn_parser_parse_stmt(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = stmts; stmts = a_array_push(stmts, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_block(stmts), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(stmts);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_stmt(AValue toks, AValue pos) {
    AValue k = {0}, r = {0}, expr = {0}, value = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("KwLet")))) {
        __ret = fn_parser_parse_let_stmt(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwRet")))) {
        __ret = fn_parser_parse_ret_stmt(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwIf")))) {
        __ret = fn_parser_parse_if_stmt(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwMatch")))) {
        __ret = fn_parser_parse_match_stmt(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwFor")))) {
        __ret = fn_parser_parse_for_stmt(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwWhile")))) {
        __ret = fn_parser_parse_while_stmt(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwBreak")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_break(), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwContinue")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_continue(), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Eq")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = value; value = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_assign(expr, value), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_expr_stmt(expr), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(r);
    a_release(expr);
    a_release(value);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_let_stmt(AValue toks, AValue pos) {
    AValue r = {0}, mutable = {0}, name = {0}, typ = {0}, value = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwLet")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = mutable; mutable = a_bool(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwMut")))) {
        { AValue __old = mutable; mutable = a_bool(1); a_release(__old); }
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
    }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBracket")))) {
        __ret = fn_parser_parse_let_destructure(toks, pos); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = typ; typ = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Colon")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = typ; typ = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Eq")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = value; value = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_let(mutable, name, typ, value), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(mutable);
    a_release(name);
    a_release(typ);
    a_release(value);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_let_destructure(AValue toks, AValue pos) {
    AValue r = {0}, bindings = {0}, rest = {0}, has_rest = {0}, value = {0}, rest_val = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBracket")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = bindings; bindings = a_array_new(0); a_release(__old); }
    { AValue __old = rest; rest = a_string(""); a_release(__old); }
    { AValue __old = has_rest; has_rest = a_bool(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("DotDotDot")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = has_rest; has_rest = a_bool(1); a_release(__old); }
            if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
                if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = rest; rest = a_array_get(r, a_int(0)); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            }
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            }
            break;
        }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Underscore")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = bindings; bindings = a_array_push(bindings, a_string("_")); a_release(__old); }
        } else {
            { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = bindings; bindings = a_array_push(bindings, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        } else {
            break;
        }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Eq")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = value; value = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = rest_val; rest_val = a_string(""); a_release(__old); }
    if (a_truthy(has_rest)) {
        { AValue __old = rest_val; rest_val = a_retain(rest); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_let_destructure(bindings, rest_val, value), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(bindings);
    a_release(rest);
    a_release(has_rest);
    a_release(value);
    a_release(rest_val);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_ret_stmt(AValue toks, AValue pos) {
    AValue r = {0}, k = {0}, expr = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwRet")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_or(a_or(a_eq(k, a_string("Newline")), a_eq(k, a_string("RBrace"))), a_eq(k, a_string("Eof"))))) {
        { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_return(fn_ast_mk_void_lit()), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_return(expr), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(k);
    a_release(expr);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_if_stmt(AValue toks, AValue pos) {
    AValue r = {0}, cond = {0}, then_block = {0}, else_branch = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwIf")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = cond; cond = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = then_block; then_block = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = else_branch; else_branch = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwElse")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwIf")))) {
            { AValue __old = r; r = fn_parser_parse_if_stmt(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = else_branch; else_branch = fn_ast_mk_else_if(a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = else_branch; else_branch = fn_ast_mk_else_block(a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
    }
    __ret = a_array_new(2, fn_ast_mk_if_stmt(cond, then_block, else_branch), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(cond);
    a_release(then_block);
    a_release(else_branch);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_match_stmt(AValue toks, AValue pos) {
    AValue r = {0}, expr = {0}, arms = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwMatch")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = arms; arms = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        { AValue __old = r; r = fn_parser_parse_match_arm(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = arms; arms = a_array_push(arms, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_match_stmt(expr, arms), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(expr);
    a_release(arms);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_match_arm(AValue toks, AValue pos) {
    AValue r = {0}, pattern = {0}, guard = {0}, body = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pattern; pattern = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = guard; guard = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwIf")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = guard; guard = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("FatArrow")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = body; body = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBrace")))) {
        { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    } else {
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_match_arm(pattern, guard, body), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(pattern);
    a_release(guard);
    a_release(body);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_for_stmt(AValue toks, AValue pos) {
    AValue r = {0}, var = {0}, typ = {0}, iter = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwFor")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBracket")))) {
        __ret = fn_parser_parse_for_destructure(toks, pos); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = var; var = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = typ; typ = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Colon")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = typ; typ = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwIn")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = iter; iter = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_for_stmt(var, typ, iter, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(var);
    a_release(typ);
    a_release(iter);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_for_destructure(AValue toks, AValue pos) {
    AValue r = {0}, bindings = {0}, iter = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBracket")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = bindings; bindings = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = bindings; bindings = a_array_push(bindings, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        } else {
            break;
        }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwIn")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = iter; iter = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_for_destructure(bindings, iter, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(bindings);
    a_release(iter);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_while_stmt(AValue toks, AValue pos) {
    AValue r = {0}, cond = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwWhile")); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = cond; cond = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_while_stmt(cond, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(cond);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_pattern(AValue toks, AValue pos) {
    AValue k = {0}, elems = {0}, r = {0}, entries = {0}, key = {0}, name = {0}, pats = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("Underscore")))) {
        __ret = a_array_new(2, fn_ast_mk_pat_wildcard(), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Int")))) {
        __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_int(a_to_int(fn_parser_tv(toks, pos)))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Float")))) {
        __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_float(a_to_float(fn_parser_tv(toks, pos)))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Str")))) {
        __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_string(fn_parser_tv(toks, pos))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwTrue")))) {
        __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_bool_lit(a_bool(1))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwFalse")))) {
        __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_bool_lit(a_bool(0))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LBracket")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = elems; elems = a_array_new(0); a_release(__old); }
        while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("DotDotDot")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = elems; elems = a_array_push(elems, fn_ast_mk_pat_rest(a_array_get(r, a_int(0)))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                break;
            }
            { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = elems; elems = a_array_push(elems, fn_ast_mk_pat_array_elem(a_array_get(r, a_int(0)))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            } else {
                break;
            }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_pat_array(elems), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Hash")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = entries; entries = a_array_new(0); a_release(__old); }
        while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
            { AValue __old = key; key = a_string(""); a_release(__old); }
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Str")))) {
                { AValue __old = key; key = fn_parser_tv(toks, pos); a_release(__old); }
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            } else {
                if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Ident")))) {
                    { AValue __old = key; key = fn_parser_tv(toks, pos); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                } else {
                    __ret = fn_parser_err(a_string("expected string key in map pattern"), pos); goto __fn_cleanup;
                }
            }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = entries; entries = a_array_push(entries, fn_ast_mk_pat_map_entry(key, a_array_get(r, a_int(0)))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            } else {
                break;
            }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_pat_map(entries), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Ident")))) {
        { AValue __old = name; name = fn_parser_tv(toks, pos); a_release(__old); }
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LParen")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = pats; pats = a_array_new(0); a_release(__old); }
            if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
                { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = pats; pats = a_array_push(pats, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = pats; pats = a_array_push(pats, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            __ret = a_array_new(2, fn_ast_mk_pat_constructor(name, pats), a_array_get(r, a_int(1))); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_pat_ident(name), pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Minus")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Int")))) {
            __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_int(a_sub(a_int(0), a_to_int(fn_parser_tv(toks, pos))))), a_add(pos, a_int(1))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Float")))) {
            __ret = a_array_new(2, fn_ast_mk_pat_literal(fn_ast_mk_float(a_sub(a_float(0), a_to_float(fn_parser_tv(toks, pos))))), a_add(pos, a_int(1))); goto __fn_cleanup;
        }
        __ret = fn_parser_err(a_string("expected number after -"), pos); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected pattern, found "), k), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(elems);
    a_release(r);
    a_release(entries);
    a_release(key);
    a_release(name);
    a_release(pats);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_expr(AValue toks, AValue pos) {
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    __ret = fn_parser_parse_pipe_expr(toks, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_pipe_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, saved = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_or_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("PipeArrow")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_or_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_pipe(left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Newline")))) {
                { AValue __old = saved; saved = a_retain(pos); a_release(__old); }
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
                if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("PipeArrow")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_or_expr(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = left; left = fn_ast_mk_pipe(left, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                } else {
                    { AValue __old = pos; pos = a_retain(saved); a_release(__old); }
                    { AValue __old = cont; cont = a_bool(0); a_release(__old); }
                }
            } else {
                { AValue __old = cont; cont = a_bool(0); a_release(__old); }
            }
        }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(cont);
    a_release(saved);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_or_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_and_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("PipePipe")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_and_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = left; left = fn_ast_mk_binop(a_string("||"), left, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_and_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_eq_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("AmpAmp")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_eq_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = left; left = fn_ast_mk_binop(a_string("&&"), left, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_eq_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_cmp_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
        if (a_truthy(a_eq(k, a_string("EqEq")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_cmp_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("=="), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("NotEq")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_cmp_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = left; left = fn_ast_mk_binop(a_string("!="), left, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            } else {
                { AValue __old = cont; cont = a_bool(0); a_release(__old); }
            }
        }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(cont);
    a_release(k);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_cmp_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
        if (a_truthy(a_eq(k, a_string("Lt")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("<"), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Gt")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = left; left = fn_ast_mk_binop(a_string(">"), left, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            } else {
                if (a_truthy(a_eq(k, a_string("LtEq")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = left; left = fn_ast_mk_binop(a_string("<="), left, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                } else {
                    if (a_truthy(a_eq(k, a_string("GtEq")))) {
                        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                        { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
                        if (a_truthy(fn_parser__is_perr(r))) {
                            __ret = a_retain(r); goto __fn_cleanup;
                        }
                        { AValue __old = left; left = fn_ast_mk_binop(a_string(">="), left, a_array_get(r, a_int(0))); a_release(__old); }
                        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                    } else {
                        { AValue __old = cont; cont = a_bool(0); a_release(__old); }
                    }
                }
            }
        }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(cont);
    a_release(k);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_add_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_mul_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
        if (a_truthy(a_eq(k, a_string("Plus")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_mul_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("+"), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Minus")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_mul_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = left; left = fn_ast_mk_binop(a_string("-"), left, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            } else {
                { AValue __old = cont; cont = a_bool(0); a_release(__old); }
            }
        }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(cont);
    a_release(k);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_mul_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
        if (a_truthy(a_eq(k, a_string("Star")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("*"), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Slash")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = left; left = fn_ast_mk_binop(a_string("/"), left, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            } else {
                if (a_truthy(a_eq(k, a_string("Percent")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = left; left = fn_ast_mk_binop(a_string("%"), left, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                } else {
                    { AValue __old = cont; cont = a_bool(0); a_release(__old); }
                }
            }
        }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    a_release(cont);
    a_release(k);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_unary_expr(AValue toks, AValue pos) {
    AValue k = {0}, r = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("Minus")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_unary(a_string("-"), a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Bang")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_unary(a_string("!"), a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwTry")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBrace")))) {
            { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            __ret = a_array_new(2, fn_ast_mk_try_block(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
        }
        { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_try(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_parse_postfix_expr(toks, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(r);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_postfix_expr(AValue toks, AValue pos) {
    AValue r = {0}, expr = {0}, cont = {0}, k = {0}, args = {0}, idx = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = r; r = fn_parser_parse_primary_expr(toks, pos); a_release(__old); }
    if (a_truthy(fn_parser__is_perr(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
        if (a_truthy(a_eq(k, a_string("LParen")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = args; args = a_array_new(0); a_release(__old); }
            if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
                { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = args; args = a_array_push(args, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = args; args = a_array_push(args, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            }
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            { AValue __old = expr; expr = fn_ast_mk_call(expr, args); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Dot")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwPost")))) {
                    { AValue __old = expr; expr = fn_ast_mk_field_access(expr, a_string("post")); a_release(__old); }
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                } else {
                    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = expr; expr = fn_ast_mk_field_access(expr, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            } else {
                if (a_truthy(a_eq(k, a_string("LBracket")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = idx; idx = a_array_get(r, a_int(0)); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
                    if (a_truthy(fn_parser__is_perr(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                    { AValue __old = expr; expr = fn_ast_mk_index(expr, idx); a_release(__old); }
                } else {
                    if (a_truthy(a_eq(k, a_string("Question")))) {
                        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        { AValue __old = expr; expr = fn_ast_mk_try(expr); a_release(__old); }
                    } else {
                        { AValue __old = cont; cont = a_bool(0); a_release(__old); }
                    }
                }
            }
        }
    }
    __ret = a_array_new(2, expr, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(expr);
    a_release(cont);
    a_release(k);
    a_release(args);
    a_release(idx);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_primary_expr(AValue toks, AValue pos) {
    AValue k = {0}, name = {0}, r = {0}, params = {0}, body = {0}, entries = {0}, key = {0}, r2 = {0}, elems = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("Int")))) {
        __ret = a_array_new(2, fn_ast_mk_int(a_to_int(fn_parser_tv(toks, pos))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Float")))) {
        __ret = a_array_new(2, fn_ast_mk_float(a_to_float(fn_parser_tv(toks, pos))), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Str")))) {
        __ret = a_array_new(2, fn_ast_mk_string(fn_parser_tv(toks, pos)), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwTrue")))) {
        __ret = a_array_new(2, fn_ast_mk_bool_lit(a_bool(1)), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwFalse")))) {
        __ret = a_array_new(2, fn_ast_mk_bool_lit(a_bool(0)), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Ident")))) {
        { AValue __old = name; name = fn_parser_tv(toks, pos); a_release(__old); }
        __ret = a_array_new(2, fn_ast_mk_ident(name), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwRet")))) {
        __ret = a_array_new(2, fn_ast_mk_ident(a_string("ret")), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("TyStr")))) {
        if (a_truthy(a_eq(fn_parser_tk(toks, a_add(pos, a_int(1))), a_string("Dot")))) {
            __ret = a_array_new(2, fn_ast_mk_ident(a_string("str")), a_add(pos, a_int(1))); goto __fn_cleanup;
        }
    }
    if (a_truthy(a_eq(k, a_string("KwFn")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = params; params = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
            { AValue __old = r; r = fn_parser_parse_param_list(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = params; params = a_array_get(r, a_int(0)); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = body; body = fn_ast_mk_void_lit(); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("FatArrow")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = body; body = fn_ast_mk_block_expr(a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        __ret = a_array_new(2, fn_ast_mk_lambda(params, body), pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("InterpStart")))) {
        __ret = fn_parser_parse_interp_string(toks, pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Hash")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = entries; entries = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
            { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = key; key = a_array_get(r, a_int(0)); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = entries; entries = a_array_push(entries, fn_ast_mk_map_entry(key, a_array_get(r, a_int(0)))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
                    break;
                }
                { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = key; key = a_array_get(r, a_int(0)); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = entries; entries = a_array_push(entries, fn_ast_mk_map_entry(key, a_array_get(r, a_int(0)))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_map_literal(entries), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LParen")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r2; r2 = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r2))) {
            __ret = a_retain(r2); goto __fn_cleanup;
        }
        __ret = a_array_new(2, a_array_get(r, a_int(0)), a_array_get(r2, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LBracket")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = elems; elems = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
            { AValue __old = r; r = fn_parser_parse_array_element(toks, pos); a_release(__old); }
            if (a_truthy(fn_parser__is_perr(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = elems; elems = a_array_push(elems, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
                    break;
                }
                { AValue __old = r; r = fn_parser_parse_array_element(toks, pos); a_release(__old); }
                if (a_truthy(fn_parser__is_perr(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = elems; elems = a_array_push(elems, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_array(elems), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected expression, found "), k), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(name);
    a_release(r);
    a_release(params);
    a_release(body);
    a_release(entries);
    a_release(key);
    a_release(r2);
    a_release(elems);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_array_element(AValue toks, AValue pos) {
    AValue r = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("DotDotDot")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_spread(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_parse_expr(toks, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_parser_parse_interp_string(AValue toks, AValue pos) {
    AValue parts = {0}, text = {0}, cont = {0}, r = {0}, k = {0};
    AValue __ret = a_void();
    toks = a_retain(toks);
    pos = a_retain(pos);
    { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
    { AValue __old = text; text = fn_parser_tv(toks, pos); a_release(__old); }
    if (a_truthy(a_gt(a_len(a_str_chars(text)), a_int(0)))) {
        { AValue __old = parts; parts = a_array_push(parts, fn_ast_mk_interp_lit(text)); a_release(__old); }
    }
    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(fn_parser__is_perr(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = parts; parts = a_array_push(parts, fn_ast_mk_interp_expr(a_array_get(r, a_int(0)))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
        if (a_truthy(a_eq(k, a_string("InterpMid")))) {
            { AValue __old = text; text = fn_parser_tv(toks, pos); a_release(__old); }
            if (a_truthy(a_gt(a_len(a_str_chars(text)), a_int(0)))) {
                { AValue __old = parts; parts = a_array_push(parts, fn_ast_mk_interp_lit(text)); a_release(__old); }
            }
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("InterpEnd")))) {
                { AValue __old = text; text = fn_parser_tv(toks, pos); a_release(__old); }
                if (a_truthy(a_gt(a_len(a_str_chars(text)), a_int(0)))) {
                    { AValue __old = parts; parts = a_array_push(parts, fn_ast_mk_interp_lit(text)); a_release(__old); }
                }
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = cont; cont = a_bool(0); a_release(__old); }
            } else {
                __ret = fn_parser_err(a_add(a_string("expected interpolation continuation, found "), k), pos); goto __fn_cleanup;
            }
        }
    }
    __ret = a_array_new(2, fn_ast_mk_interpolation(parts), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(parts);
    a_release(text);
    a_release(cont);
    a_release(r);
    a_release(k);
    a_release(toks);
    a_release(pos);
    return __ret;
}

AValue fn_cgen__builtin_map(void) {
    AValue m = {0}, m2 = {0}, m3 = {0}, m4 = {0};
    AValue __ret = a_void();
    { AValue __old = m; m = a_map_new(27, "println", a_string("a_println"), "print", a_string("a_print"), "eprintln", a_string("a_eprintln"), "len", a_string("a_len"), "push", a_string("a_array_push"), "to_str", a_string("a_to_str"), "fail", a_string("a_fail"), "type_of", a_string("a_type_of"), "int", a_string("a_to_int"), "float", a_string("a_to_float"), "sort", a_string("a_sort"), "contains", a_string("a_contains"), "reverse_arr", a_string("a_reverse_arr"), "concat_arr", a_string("a_concat_arr"), "args", a_string("a_args"), "slice", a_string("a_array_slice"), "char_code", a_string("a_char_code"), "from_code", a_string("a_from_code"), "is_alpha", a_string("a_is_alpha"), "is_digit", a_string("a_is_digit"), "is_alnum", a_string("a_is_alnum"), "Ok", a_string("a_ok"), "Err", a_string("a_err"), "unwrap", a_string("a_unwrap"), "is_ok", a_string("a_is_ok"), "is_err", a_string("a_is_err"), "unwrap_or", a_string("a_unwrap_or")); a_release(__old); }
    { AValue __old = m2; m2 = a_map_new(24, "str.concat", a_string("a_str_concat"), "str.split", a_string("a_str_split"), "str.contains", a_string("a_str_contains"), "str.replace", a_string("a_str_replace"), "str.trim", a_string("a_str_trim"), "str.upper", a_string("a_str_upper"), "str.lower", a_string("a_str_lower"), "str.join", a_string("a_str_join"), "str.chars", a_string("a_str_chars"), "str.slice", a_string("a_str_slice"), "str.starts_with", a_string("a_str_starts_with"), "str.ends_with", a_string("a_str_ends_with"), "str.find", a_string("a_str_find"), "str.count", a_string("a_str_count"), "str.lines", a_string("a_str_lines"), "map.get", a_string("a_map_get"), "map.set", a_string("a_map_set"), "map.has", a_string("a_map_has"), "map.keys", a_string("a_map_keys"), "map.values", a_string("a_map_values"), "map.merge", a_string("a_map_merge"), "map.delete", a_string("a_map_delete"), "map.entries", a_string("a_map_entries"), "map.from_entries", a_string("a_map_from_entries")); a_release(__old); }
    { AValue __old = m3; m3 = a_map_new(31, "io.read_file", a_string("a_io_read_file"), "io.write_file", a_string("a_io_write_file"), "io.read_stdin", a_string("a_io_read_stdin"), "io.read_line", a_string("a_io_read_line"), "io.read_bytes", a_string("a_io_read_bytes"), "io.flush", a_string("a_io_flush"), "fs.ls", a_string("a_fs_ls"), "fs.mkdir", a_string("a_fs_mkdir"), "fs.cwd", a_string("a_fs_cwd"), "fs.exists", a_string("a_fs_exists"), "fs.is_dir", a_string("a_fs_is_dir"), "fs.rm", a_string("a_fs_rm"), "fs.mv", a_string("a_fs_mv"), "fs.cp", a_string("a_fs_cp"), "fs.abs", a_string("a_fs_abs"), "fs.is_file", a_string("a_fs_is_file"), "fs.stat", a_string("a_fs_stat"), "fs.watch", a_string("a_fs_watch"), "exec", a_string("a_exec"), "proc.spawn", a_string("a_proc_spawn"), "proc.write", a_string("a_proc_write"), "proc.read_line", a_string("a_proc_read_line"), "proc.kill", a_string("a_proc_kill"), "proc.wait", a_string("a_proc_wait"), "proc.is_running", a_string("a_proc_is_running"), "env.get", a_string("a_env_get"), "env.set", a_string("a_env_set"), "env.all", a_string("a_env_all"), "json.parse", a_string("a_json_parse"), "json.stringify", a_string("a_json_stringify"), "json.pretty", a_string("a_json_pretty")); a_release(__old); }
    { AValue __old = m4; m4 = a_map_new(92, "math.sqrt", a_string("a_math_sqrt"), "math.abs", a_string("a_math_abs"), "math.floor", a_string("a_math_floor"), "math.ceil", a_string("a_math_ceil"), "math.round", a_string("a_math_round"), "math.pow", a_string("a_math_pow"), "math.min", a_string("a_math_min"), "math.max", a_string("a_math_max"), "time.now", a_string("a_time_now"), "time.sleep", a_string("a_time_sleep"), "hash.sha256", a_string("a_hash_sha256"), "hash.md5", a_string("a_hash_md5"), "uuid.v4", a_string("a_uuid_v4"), "signal.on", a_string("a_signal_on"), "image.load", a_string("a_image_load"), "image.decode", a_string("a_image_decode"), "image.save", a_string("a_image_save"), "image.encode", a_string("a_image_encode"), "image.width", a_string("a_image_width"), "image.height", a_string("a_image_height"), "image.resize", a_string("a_image_resize"), "image.pixels", a_string("a_image_pixels"), "http.get", a_string("a_http_get"), "http.post", a_string("a_http_post"), "http.put", a_string("a_http_put"), "http.patch", a_string("a_http_patch"), "http.delete", a_string("a_http_delete"), "http.stream", a_string("a_http_stream"), "http.stream_read", a_string("a_http_stream_read"), "http.stream_close", a_string("a_http_stream_close"), "ws.connect", a_string("a_ws_connect"), "ws.send", a_string("a_ws_send"), "ws.recv", a_string("a_ws_recv"), "ws.close", a_string("a_ws_close"), "http.serve", a_string("a_http_serve"), "http.serve_static", a_string("a_http_serve_static"), "db.open", a_string("a_db_open"), "db.close", a_string("a_db_close"), "db.exec", a_string("a_db_exec"), "db.query", a_string("a_db_query"), "map", a_string("a_hof_map"), "filter", a_string("a_hof_filter"), "reduce", a_string("a_hof_reduce"), "each", a_string("a_hof_each"), "sort_by", a_string("a_hof_sort_by"), "find", a_string("a_hof_find"), "any", a_string("a_hof_any"), "all", a_string("a_hof_all"), "flat_map", a_string("a_hof_flat_map"), "min_by", a_string("a_hof_min_by"), "max_by", a_string("a_hof_max_by"), "enumerate", a_string("a_enumerate"), "zip", a_string("a_zip"), "take", a_string("a_take"), "drop", a_string("a_drop"), "unique", a_string("a_unique"), "chunk", a_string("a_chunk"), "ptr.null", a_string("a_ptr_null"), "ptr.is_null", a_string("a_is_null"), "argv0", a_string("a_argv0"), "embedded_file", a_string("a_embedded_file"), "compress.deflate", a_string("a_compress_deflate"), "compress.inflate", a_string("a_compress_inflate"), "compress.gzip", a_string("a_compress_gzip"), "compress.gunzip", a_string("a_compress_gunzip"), "spawn", a_string("a_spawn"), "await", a_string("a_await"), "await_all", a_string("a_await_all"), "parallel_map", a_string("a_parallel_map"), "parallel_each", a_string("a_parallel_each"), "timeout", a_string("a_timeout"), "async.http_get", a_string("a_async_http_get"), "async.http_post", a_string("a_async_http_post"), "async.http_put", a_string("a_async_http_put"), "async.http_patch", a_string("a_async_http_patch"), "async.http_delete", a_string("a_async_http_delete"), "async.await", a_string("a_async_await"), "async.gather", a_string("a_async_gather"), "reflect.uptime_ms", a_string("a_reflect_uptime_ms"), "reflect.memory_usage", a_string("a_reflect_memory_usage"), "reflect.pid", a_string("a_reflect_pid"), "local_llm.load", a_string("a_llm_load"), "local_llm.generate", a_string("a_llm_generate"), "local_llm.embed", a_string("a_llm_embed"), "local_llm.unload", a_string("a_llm_unload"), "local_llm.info", a_string("a_llm_info"), "local_llm.tokenize", a_string("a_llm_tokenize"), "local_llm.detokenize", a_string("a_llm_detokenize"), "local_llm.vocab_size", a_string("a_llm_vocab_size"), "profile.dump", a_string("a_profile_dump_json"), "profile.get_counters", a_string("a_profile_get_counters"), "profile.reset", a_string("a_profile_reset")); a_release(__old); }
    __ret = a_map_merge(a_map_merge(a_map_merge(m, m2), m3), m4); goto __fn_cleanup;
__fn_cleanup:
    a_release(m);
    a_release(m2);
    a_release(m3);
    a_release(m4);
    return __ret;
}

AValue fn_cgen__void_builtins(void) {
    AValue __ret = a_void();
    __ret = a_array_new(13, a_string("println"), a_string("print"), a_string("eprintln"), a_string("fail"), a_string("each"), a_string("parallel_each"), a_string("env.set"), a_string("time.sleep"), a_string("io.flush"), a_string("http.serve"), a_string("http.serve_static"), a_string("db.close"), a_string("fs.watch")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__c_keywords(void) {
    AValue __ret = a_void();
    __ret = a_array_new(27, a_string("short"), a_string("long"), a_string("int"), a_string("float"), a_string("double"), a_string("void"), a_string("char"), a_string("auto"), a_string("do"), a_string("default"), a_string("register"), a_string("signed"), a_string("unsigned"), a_string("const"), a_string("static"), a_string("extern"), a_string("case"), a_string("switch"), a_string("goto"), a_string("struct"), a_string("union"), a_string("enum"), a_string("typedef"), a_string("sizeof"), a_string("volatile"), a_string("inline"), a_string("restrict")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__mangle(AValue name) {
    AValue out = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    { AValue __old = out; out = a_str_replace(a_str_replace(name, a_string("."), a_string("_")), a_string("-"), a_string("_")); a_release(__old); }
    if (a_truthy(a_contains(fn_cgen__c_keywords(), out))) {
        __ret = a_str_concat(a_string("_"), out); goto __fn_cleanup;
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(out);
    a_release(name);
    return __ret;
}

AValue fn_cgen__bslash(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(92)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__dquote(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(34)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__newline_char(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(10)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__tab_char(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(9)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__cr_char(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(13)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__escape_c_str(AValue s) {
    AValue bs = {0}, out = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = bs; bs = fn_cgen__bslash(); a_release(__old); }
    { AValue __old = out; out = a_str_replace(s, bs, a_str_concat(bs, bs)); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, fn_cgen__dquote(), a_str_concat(bs, fn_cgen__dquote())); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, fn_cgen__newline_char(), a_str_concat(bs, a_string("n"))); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, fn_cgen__cr_char(), a_str_concat(bs, a_string("r"))); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, fn_cgen__tab_char(), a_str_concat(bs, a_string("t"))); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(bs);
    a_release(out);
    a_release(s);
    return __ret;
}

AValue fn_cgen__indent(AValue depth) {
    AValue out = {0}, i = {0};
    AValue __ret = a_void();
    depth = a_retain(depth);
    { AValue __old = out; out = a_string(""); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, depth))) {
        { AValue __old = out; out = a_str_concat(out, a_string("    ")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(out);
    a_release(i);
    a_release(depth);
    return __ret;
}

AValue fn_cgen__prefixed_name(AValue name, AValue ctx) {
    AValue prefix = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    ctx = a_retain(ctx);
    { AValue __old = prefix; prefix = a_array_get(ctx, a_string("prefix")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(prefix), a_string("str")))) {
        if (a_truthy(a_gt(a_len(prefix), a_int(0)))) {
            __ret = a_str_concat(prefix, a_str_concat(a_string("_"), name)); goto __fn_cleanup;
        }
    }
    __ret = a_retain(name); goto __fn_cleanup;
__fn_cleanup:
    a_release(prefix);
    a_release(name);
    a_release(ctx);
    return __ret;
}

AValue fn_cgen__is_intra_module_call(AValue fname, AValue ctx) {
    AValue prefix = {0}, fns = {0};
    AValue __ret = a_void();
    fname = a_retain(fname);
    ctx = a_retain(ctx);
    { AValue __old = prefix; prefix = a_array_get(ctx, a_string("prefix")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(prefix), a_string("str")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_len(prefix), a_int(0)))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_str_contains(fname, a_string(".")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = fns; fns = a_array_get(ctx, a_string("fns")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(fns), a_string("array")))) {
        __ret = a_contains(fns, fname); goto __fn_cleanup;
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(prefix);
    a_release(fns);
    a_release(fname);
    a_release(ctx);
    return __ret;
}

AValue fn_cgen__R(AValue code, AValue li) {
    AValue __ret = a_void();
    code = a_retain(code);
    li = a_retain(li);
    __ret = a_array_new(3, code, li, a_array_new(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(code);
    a_release(li);
    return __ret;
}

AValue fn_cgen__RL(AValue code, AValue li, AValue lifted) {
    AValue __ret = a_void();
    code = a_retain(code);
    li = a_retain(li);
    lifted = a_retain(lifted);
    __ret = a_array_new(3, code, li, lifted); goto __fn_cleanup;
__fn_cleanup:
    a_release(code);
    a_release(li);
    a_release(lifted);
    return __ret;
}

AValue fn_cgen__is_ident(AValue node) {
    AValue n = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    if (a_truthy(a_neq(a_type_of(node), a_string("map")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_neq(a_array_get(node, a_string("tag")), a_string("Ident")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = n; n = a_array_get(node, a_string("name")); a_release(__old); }
    if (a_truthy(a_eq(n, a_string("true")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(n, a_string("false")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    __ret = a_bool(1); goto __fn_cleanup;
__fn_cleanup:
    a_release(n);
    a_release(node);
    return __ret;
}

AValue fn_cgen__emit_cleanup(AValue vars, AValue depth) {
    AValue ind = {0}, out = {0};
    AValue __ret = a_void();
    vars = a_retain(vars);
    depth = a_retain(depth);
    { AValue __old = ind; ind = fn_cgen__indent(depth); a_release(__old); }
    { AValue __old = out; out = a_string(""); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(vars);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue v = {0};
            v = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_str_concat(a_string("a_release("), a_str_concat(v, a_string(");")))))); a_release(__old); }
            a_release(v);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(ind);
    a_release(out);
    a_release(vars);
    a_release(depth);
    return __ret;
}

AValue fn_cgen__ea_collect_idents(AValue expr) {
    AValue tag = {0}, n = {0}, ids = {0};
    AValue __ret = a_void();
    expr = a_retain(expr);
    if (a_truthy(a_neq(a_type_of(expr), a_string("map")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(expr, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Ident")))) {
        { AValue __old = n; n = a_array_get(expr, a_string("name")); a_release(__old); }
        if (a_truthy(a_eq(n, a_string("true")))) {
            __ret = a_array_new(0); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(n, a_string("false")))) {
            __ret = a_array_new(0); goto __fn_cleanup;
        }
        __ret = a_array_new(1, n); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("BinOp")))) {
        __ret = a_concat_arr(fn_cgen__ea_collect_idents(a_array_get(expr, a_string("left"))), fn_cgen__ea_collect_idents(a_array_get(expr, a_string("right")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("UnaryOp")))) {
        __ret = fn_cgen__ea_collect_idents(a_array_get(expr, a_string("expr"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Call")))) {
        { AValue __old = ids; ids = fn_cgen__ea_collect_idents(a_array_get(expr, a_string("func"))); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(expr, a_string("args")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue a = {0};
                a = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__ea_collect_idents(a)); a_release(__old); }
                a_release(a);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Index")))) {
        __ret = a_concat_arr(fn_cgen__ea_collect_idents(a_array_get(expr, a_string("expr"))), fn_cgen__ea_collect_idents(a_array_get(expr, a_string("index")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("FieldAccess")))) {
        __ret = fn_cgen__ea_collect_idents(a_array_get(expr, a_string("expr"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Array")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(expr, a_string("elems")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__ea_collect_idents(e)); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Map")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(expr, a_string("entries")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__ea_collect_idents(a_array_get(e, a_string("value")))); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Spread")))) {
        __ret = fn_cgen__ea_collect_idents(a_array_get(expr, a_string("expr"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Pipe")))) {
        __ret = a_concat_arr(fn_cgen__ea_collect_idents(a_array_get(expr, a_string("left"))), fn_cgen__ea_collect_idents(a_array_get(expr, a_string("right")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("InterpolatedString")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(expr, a_string("parts")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(p, a_string("tag")), a_string("Expr")))) {
                    { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__ea_collect_idents(a_array_get(p, a_string("expr")))); a_release(__old); }
                }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    __ret = a_array_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(n);
    a_release(ids);
    a_release(expr);
    return __ret;
}

AValue fn_cgen__escape_analysis(AValue stmts, AValue captures) {
    AValue escaping = {0}, tag = {0}, ids = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    captures = a_retain(captures);
    { AValue __old = escaping; escaping = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0}, tag = {0}, ids = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = tag; tag = a_array_get(s, a_string("tag")); a_release(__old); }
            if (a_truthy(a_eq(tag, a_string("Return")))) {
                if (a_truthy(a_neq(a_type_of(a_array_get(s, a_string("expr"))), a_string("void")))) {
                    { AValue __old = ids; ids = fn_cgen__ea_collect_idents(a_array_get(s, a_string("expr"))); a_release(__old); }
                    {
                        AValue __iter_arr = a_iterable(ids);
                        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                            AValue id = {0};
                            id = a_array_get(__iter_arr, a_int(__fi));
                            if (a_truthy(a_not(a_contains(escaping, id)))) {
                                { AValue __old = escaping; escaping = a_array_push(escaping, id); a_release(__old); }
                            }
                            a_release(id);
                        }
                        a_release(__iter_arr);
                    }
                }
            }
            a_release(s);
            a_release(tag);
            a_release(ids);
        }
        a_release(__iter_arr);
    }
    {
        AValue __iter_arr = a_iterable(captures);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue cap = {0};
            cap = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_not(a_contains(escaping, cap)))) {
                { AValue __old = escaping; escaping = a_array_push(escaping, cap); a_release(__old); }
            }
            a_release(cap);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(escaping); goto __fn_cleanup;
__fn_cleanup:
    a_release(escaping);
    a_release(tag);
    a_release(ids);
    a_release(stmts);
    a_release(captures);
    return __ret;
}

AValue fn_cgen__collect_idents_in_expr(AValue node) {
    AValue tag = {0}, ids = {0}, func = {0}, body = {0}, bound = {0}, body_ids = {0}, free = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(tag), a_string("str")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Ident")))) {
        __ret = a_array_new(1, a_array_get(node, a_string("name"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Int")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Float")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Bool")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Void")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("String")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("BinOp")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("left"))), fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("right")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("UnaryOp")))) {
        __ret = fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("expr"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Call")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        { AValue __old = func; func = a_array_get(node, a_string("func")); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(func, a_string("tag")), a_string("Ident")))) {
        } else {
            { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(func)); a_release(__old); }
        }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("args")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue arg = {0};
                arg = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(arg)); a_release(__old); }
                a_release(arg);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("FieldAccess")))) {
        __ret = fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("expr"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Index")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("expr"))), fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("index")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Array")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("elems")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(e)); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("MapLiteral")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("entries")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(a_array_get(e, a_string("value")))); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Interpolation")))) {
        { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("parts")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_neq(a_array_get(p, a_string("tag")), a_string("InterpLit")))) {
                    { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(a_array_get(p, a_string("expr")))); a_release(__old); }
                }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("IfExpr")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("cond"))), a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("then"))), fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("else"))))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Pipe")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("left"))), fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("right")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("MatchExpr")))) {
        { AValue __old = ids; ids = fn_cgen__collect_idents_in_expr(a_array_get(node, a_string("expr"))); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("arms")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue arm = {0}, body = {0};
                arm = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_and(a_neq(a_type_of(a_array_get(arm, a_string("guard"))), a_string("void")), a_neq(a_array_get(a_array_get(arm, a_string("guard")), a_string("tag")), a_string("Void"))))) {
                    { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(a_array_get(arm, a_string("guard")))); a_release(__old); }
                }
                { AValue __old = body; body = a_array_get(arm, a_string("body")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(a_array_get(body, a_string("stmts"))), a_string("array")))) {
                    { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_block(body)); a_release(__old); }
                } else {
                    { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_expr(body)); a_release(__old); }
                }
                a_release(arm);
                a_release(body);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Lambda")))) {
        { AValue __old = bound; bound = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("params")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = bound; bound = a_array_push(bound, a_array_get(p, a_string("name"))); a_release(__old); }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        { AValue __old = body_ids; body_ids = fn_cgen__collect_idents_in_block(a_array_get(node, a_string("body"))); a_release(__old); }
        { AValue __old = free; free = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(body_ids);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue id = {0};
                id = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_not(a_contains(bound, id)))) {
                    if (a_truthy(a_not(a_contains(free, id)))) {
                        { AValue __old = free; free = a_array_push(free, id); a_release(__old); }
                    }
                }
                a_release(id);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(free); goto __fn_cleanup;
    }
    __ret = a_array_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(ids);
    a_release(func);
    a_release(body);
    a_release(bound);
    a_release(body_ids);
    a_release(free);
    a_release(node);
    return __ret;
}

AValue fn_cgen__collect_idents_in_block(AValue block) {
    AValue ids = {0}, stmts = {0};
    AValue __ret = a_void();
    block = a_retain(block);
    if (a_truthy(a_eq(a_type_of(a_array_get(block, a_string("tag"))), a_string("str")))) {
        if (a_truthy(a_eq(a_array_get(block, a_string("tag")), a_string("BlockExpr")))) {
            __ret = fn_cgen__collect_idents_in_block(a_array_get(block, a_string("block"))); goto __fn_cleanup;
        }
    }
    { AValue __old = ids; ids = a_array_new(0); a_release(__old); }
    { AValue __old = stmts; stmts = a_array_get(block, a_string("stmts")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(stmts), a_string("array")))) {
        __ret = fn_cgen__collect_idents_in_expr(block); goto __fn_cleanup;
    }
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_stmt(s)); a_release(__old); }
            a_release(s);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(ids); goto __fn_cleanup;
__fn_cleanup:
    a_release(ids);
    a_release(stmts);
    a_release(block);
    return __ret;
}

AValue fn_cgen__collect_idents_in_stmt(AValue s) {
    AValue tag = {0}, ids = {0}, eb = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = tag; tag = a_array_get(s, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Let")))) {
        __ret = fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("value"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Assign")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("target"))), fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("value")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Return")))) {
        if (a_truthy(a_neq(a_type_of(a_array_get(s, a_string("expr"))), a_string("void")))) {
            __ret = fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("expr"))); goto __fn_cleanup;
        }
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ExprStmt")))) {
        __ret = fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("expr"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("If")))) {
        { AValue __old = ids; ids = fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("cond"))); a_release(__old); }
        { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_block(a_array_get(s, a_string("then")))); a_release(__old); }
        { AValue __old = eb; eb = a_array_get(s, a_string("else")); a_release(__old); }
        if (a_truthy(a_neq(a_type_of(eb), a_string("void")))) {
            if (a_truthy(a_eq(a_array_get(eb, a_string("tag")), a_string("ElseBlock")))) {
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_block(a_array_get(eb, a_string("block")))); a_release(__old); }
            }
            if (a_truthy(a_eq(a_array_get(eb, a_string("tag")), a_string("ElseIf")))) {
                { AValue __old = ids; ids = a_concat_arr(ids, fn_cgen__collect_idents_in_stmt(a_array_get(eb, a_string("stmt")))); a_release(__old); }
            }
        }
        __ret = a_retain(ids); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("While")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("cond"))), fn_cgen__collect_idents_in_block(a_array_get(s, a_string("body")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("For")))) {
        __ret = a_concat_arr(fn_cgen__collect_idents_in_expr(a_array_get(s, a_string("iter"))), fn_cgen__collect_idents_in_block(a_array_get(s, a_string("body")))); goto __fn_cleanup;
    }
    __ret = a_array_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(ids);
    a_release(eb);
    a_release(s);
    return __ret;
}

AValue fn_cgen__compute_captures(AValue lambda_node, AValue enclosing_vars, AValue bm) {
    AValue bound = {0}, body = {0}, body_lets = {0}, body_ids = {0}, captures = {0};
    AValue __ret = a_void();
    lambda_node = a_retain(lambda_node);
    enclosing_vars = a_retain(enclosing_vars);
    bm = a_retain(bm);
    { AValue __old = bound; bound = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(lambda_node, a_string("params")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = bound; bound = a_array_push(bound, a_array_get(p, a_string("name"))); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = body; body = a_array_get(lambda_node, a_string("body")); a_release(__old); }
    { AValue __old = body_lets; body_lets = fn_cgen__collect_lets_in_block(body); a_release(__old); }
    { AValue __old = bound; bound = a_concat_arr(bound, body_lets); a_release(__old); }
    { AValue __old = body_ids; body_ids = fn_cgen__collect_idents_in_block(body); a_release(__old); }
    { AValue __old = captures; captures = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(body_ids);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue id = {0};
            id = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_and(a_and(a_not(a_contains(bound, id)), a_not(a_map_has(bm, id))), a_contains(enclosing_vars, id)))) {
                if (a_truthy(a_not(a_contains(captures, id)))) {
                    { AValue __old = captures; captures = a_array_push(captures, id); a_release(__old); }
                }
            }
            a_release(id);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(captures); goto __fn_cleanup;
__fn_cleanup:
    a_release(bound);
    a_release(body);
    a_release(body_lets);
    a_release(body_ids);
    a_release(captures);
    a_release(lambda_node);
    a_release(enclosing_vars);
    a_release(bm);
    return __ret;
}

AValue fn_cgen__collect_lets_in_block(AValue block) {
    AValue names = {0}, stmts = {0};
    AValue __ret = a_void();
    block = a_retain(block);
    if (a_truthy(a_eq(a_type_of(a_array_get(block, a_string("tag"))), a_string("str")))) {
        if (a_truthy(a_eq(a_array_get(block, a_string("tag")), a_string("BlockExpr")))) {
            __ret = fn_cgen__collect_lets_in_block(a_array_get(block, a_string("block"))); goto __fn_cleanup;
        }
    }
    { AValue __old = names; names = a_array_new(0); a_release(__old); }
    { AValue __old = stmts; stmts = a_array_get(block, a_string("stmts")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(stmts), a_string("array")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(s, a_string("tag")), a_string("Let")))) {
                { AValue __old = names; names = a_array_push(names, a_array_get(s, a_string("name"))); a_release(__old); }
            }
            a_release(s);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(names); goto __fn_cleanup;
__fn_cleanup:
    a_release(names);
    a_release(stmts);
    a_release(block);
    return __ret;
}

AValue fn_cgen_emit_expr(AValue node, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, name = {0}, op = {0}, lr = {0}, rr = {0}, l = {0}, r = {0}, lifted = {0}, code = {0}, ir = {0}, func = {0}, a = {0}, fname = {0}, ar = {0}, arg_codes = {0}, cfn = {0}, all_fns = {0}, is_known_fn = {0}, call_args = {0}, mangled = {0}, aliases = {0}, er = {0}, field = {0}, elems = {0}, has_spread = {0}, parts = {0}, entries = {0}, key_node = {0}, key_str = {0}, vr = {0}, cparts = {0}, cr = {0}, tr = {0}, stmts = {0}, last = {0}, right = {0}, new_args = {0}, new_call = {0}, lambda_name = {0}, params = {0}, body = {0}, enclosing_vars = {0}, captures = {0}, fn_code = {0}, ci = {0}, pi = {0}, next_li = {0}, body_lifted = {0}, lambda_vars = {0}, body_tag = {0}, body_vars = {0}, bound = {0}, decl_vars = {0}, body_lets = {0}, lambda_ctx = {0}, init_parts = {0}, lambda_cleanup = {0}, has_return = {0}, si = {0}, s = {0}, is_last = {0}, cleanup = {0}, sr = {0}, env_code = {0}, cap_parts = {0}, closure_code = {0}, all_lifted = {0}, block = {0}, nl = {0}, try_vars = {0}, nstmts = {0}, has_tail = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Int")))) {
        __ret = fn_cgen__R(a_str_concat(a_string("a_int("), a_str_concat(a_to_str(a_array_get(node, a_string("value"))), a_string(")"))), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Float")))) {
        __ret = fn_cgen__R(a_str_concat(a_string("a_float("), a_str_concat(a_to_str(a_array_get(node, a_string("value"))), a_string(")"))), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Bool")))) {
        if (a_truthy(a_array_get(node, a_string("value")))) {
            __ret = fn_cgen__R(a_string("a_bool(1)"), li); goto __fn_cleanup;
        }
        __ret = fn_cgen__R(a_string("a_bool(0)"), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Void")))) {
        __ret = fn_cgen__R(a_string("a_void()"), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("String")))) {
        __ret = fn_cgen__R(a_str_concat(a_string("a_string(\""), a_str_concat(fn_cgen__escape_c_str(a_array_get(node, a_string("value"))), a_string("\")"))), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Ident")))) {
        { AValue __old = name; name = a_array_get(node, a_string("name")); a_release(__old); }
        if (a_truthy(a_eq(name, a_string("true")))) {
            __ret = fn_cgen__R(a_string("a_bool(1)"), li); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("false")))) {
            __ret = fn_cgen__R(a_string("a_bool(0)"), li); goto __fn_cleanup;
        }
        __ret = fn_cgen__R(fn_cgen__mangle(name), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("BinOp")))) {
        { AValue __old = op; op = a_array_get(node, a_string("op")); a_release(__old); }
        { AValue __old = lr; lr = fn_cgen_emit_expr(a_array_get(node, a_string("left")), bm, ctx, li); a_release(__old); }
        { AValue __old = rr; rr = fn_cgen_emit_expr(a_array_get(node, a_string("right")), bm, ctx, a_array_get(lr, a_int(1))); a_release(__old); }
        { AValue __old = l; l = a_array_get(lr, a_int(0)); a_release(__old); }
        { AValue __old = r; r = a_array_get(rr, a_int(0)); a_release(__old); }
        { AValue __old = li; li = a_array_get(rr, a_int(1)); a_release(__old); }
        { AValue __old = lifted; lifted = a_concat_arr(a_array_get(lr, a_int(2)), a_array_get(rr, a_int(2))); a_release(__old); }
        { AValue __old = code; code = a_string(""); a_release(__old); }
        if (a_truthy(a_eq(op, a_string("+")))) {
            { AValue __old = code; code = a_str_concat(a_string("a_add("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("-")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_sub("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("*")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_mul("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("/")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_div("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("%")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_mod("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("==")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_eq("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("!=")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_neq("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("<")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_lt("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string(">")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_gt("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("<=")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_lteq("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string(">=")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_gteq("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("&&")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_and("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(op, a_string("||")), a_eq(code, a_string(""))))) {
            { AValue __old = code; code = a_str_concat(a_string("a_or("), a_str_concat(l, a_str_concat(a_string(", "), a_str_concat(r, a_string(")"))))); a_release(__old); }
        }
        if (a_truthy(a_eq(code, a_string("")))) {
            { AValue __old = code; code = a_str_concat(a_string("/* unknown op: "), a_str_concat(op, a_string(" */a_void()"))); a_release(__old); }
        }
        __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("UnaryOp")))) {
        { AValue __old = ir; ir = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(node, a_string("op")), a_string("!")))) {
            __ret = fn_cgen__RL(a_str_concat(a_string("a_not("), a_str_concat(a_array_get(ir, a_int(0)), a_string(")"))), a_array_get(ir, a_int(1)), a_array_get(ir, a_int(2))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(a_array_get(node, a_string("op")), a_string("-")))) {
            __ret = fn_cgen__RL(a_str_concat(a_string("a_neg("), a_str_concat(a_array_get(ir, a_int(0)), a_string(")"))), a_array_get(ir, a_int(1)), a_array_get(ir, a_int(2))); goto __fn_cleanup;
        }
        __ret = a_retain(ir); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Call")))) {
        { AValue __old = func; func = a_array_get(node, a_string("func")); a_release(__old); }
        { AValue __old = a; a = a_array_get(node, a_string("args")); a_release(__old); }
        { AValue __old = fname; fname = fn_cgen__resolve_call_name(func); a_release(__old); }
        if (a_truthy(a_eq(fname, a_string("fail")))) {
            if (a_truthy(a_gt(a_len(a), a_int(0)))) {
                { AValue __old = ar; ar = fn_cgen_emit_expr(a_array_get(a, a_int(0)), bm, ctx, li); a_release(__old); }
                __ret = fn_cgen__RL(a_str_concat(a_string("(a_fail("), a_str_concat(a_array_get(ar, a_int(0)), a_string("), a_void())"))), a_array_get(ar, a_int(1)), a_array_get(ar, a_int(2))); goto __fn_cleanup;
            }
            __ret = fn_cgen__R(a_string("(a_fail(a_string(\"assertion failed\")), a_void())"), li); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(fname, a_string("exit")))) {
            if (a_truthy(a_gt(a_len(a), a_int(0)))) {
                { AValue __old = ar; ar = fn_cgen_emit_expr(a_array_get(a, a_int(0)), bm, ctx, li); a_release(__old); }
                __ret = fn_cgen__RL(a_str_concat(a_string("(exit((int)"), a_str_concat(a_array_get(ar, a_int(0)), a_string(".ival), a_void())"))), a_array_get(ar, a_int(1)), a_array_get(ar, a_int(2))); goto __fn_cleanup;
            }
            __ret = fn_cgen__R(a_string("(exit(0), a_void())"), li); goto __fn_cleanup;
        }
        { AValue __old = arg_codes; arg_codes = a_array_new(0); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue arg = {0}, ar = {0};
                arg = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = ar; ar = fn_cgen_emit_expr(arg, bm, ctx, li); a_release(__old); }
                { AValue __old = arg_codes; arg_codes = a_array_push(arg_codes, a_array_get(ar, a_int(0))); a_release(__old); }
                { AValue __old = li; li = a_array_get(ar, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(ar, a_int(2))); a_release(__old); }
                a_release(arg);
                a_release(ar);
            }
            a_release(__iter_arr);
        }
        if (a_truthy(a_map_has(bm, fname))) {
            { AValue __old = cfn; cfn = a_array_get(bm, fname); a_release(__old); }
            __ret = fn_cgen__RL(a_str_concat(cfn, a_str_concat(a_string("("), a_str_concat(a_str_join(arg_codes, a_string(", ")), a_string(")")))), li, lifted); goto __fn_cleanup;
        }
        { AValue __old = all_fns; all_fns = a_array_get(ctx, a_string("fns")); a_release(__old); }
        { AValue __old = is_known_fn; is_known_fn = a_bool(0); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(all_fns), a_string("array")))) {
            { AValue __old = is_known_fn; is_known_fn = a_contains(all_fns, fname); a_release(__old); }
        }
        if (a_truthy(a_str_contains(fname, a_string(".")))) {
            { AValue __old = is_known_fn; is_known_fn = a_bool(1); a_release(__old); }
        }
        if (a_truthy(a_and(a_and(a_eq(a_array_get(func, a_string("tag")), a_string("Ident")), a_not(is_known_fn)), a_not(fn_cgen__is_intra_module_call(fname, ctx))))) {
            { AValue __old = call_args; call_args = a_array_new(1, a_to_str(a_len(arg_codes))); a_release(__old); }
            { AValue __old = call_args; call_args = a_concat_arr(call_args, arg_codes); a_release(__old); }
            __ret = fn_cgen__RL(a_str_concat(a_string("a_closure_call("), a_str_concat(fn_cgen__mangle(fname), a_str_concat(a_string(", "), a_str_concat(a_str_join(call_args, a_string(", ")), a_string(")"))))), li, lifted); goto __fn_cleanup;
        }
        { AValue __old = mangled; mangled = a_string(""); a_release(__old); }
        { AValue __old = aliases; aliases = a_array_get(ctx, a_string("import_aliases")); a_release(__old); }
        if (a_truthy(a_and(a_eq(a_type_of(aliases), a_string("map")), a_map_has(aliases, fname)))) {
            { AValue __old = mangled; mangled = a_str_concat(a_string("fn_"), fn_cgen__mangle(a_array_get(aliases, fname))); a_release(__old); }
        } else         if (a_truthy(fn_cgen__is_intra_module_call(fname, ctx))) {
            { AValue __old = mangled; mangled = a_str_concat(a_string("fn_"), fn_cgen__mangle(fn_cgen__prefixed_name(fname, ctx))); a_release(__old); }
        } else {
            { AValue __old = mangled; mangled = a_str_concat(a_string("fn_"), fn_cgen__mangle(fname)); a_release(__old); }
        }
        __ret = fn_cgen__RL(a_str_concat(mangled, a_str_concat(a_string("("), a_str_concat(a_str_join(arg_codes, a_string(", ")), a_string(")")))), li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("FieldAccess")))) {
        { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
        { AValue __old = field; field = a_array_get(node, a_string("field")); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_map_get("), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(", a_string(\""), a_str_concat(field, a_string("\"))"))))), a_array_get(er, a_int(1)), a_array_get(er, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Index")))) {
        { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
        { AValue __old = ir; ir = fn_cgen_emit_expr(a_array_get(node, a_string("index")), bm, ctx, a_array_get(er, a_int(1))); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_array_get("), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(", "), a_str_concat(a_array_get(ir, a_int(0)), a_string(")"))))), a_array_get(ir, a_int(1)), a_concat_arr(a_array_get(er, a_int(2)), a_array_get(ir, a_int(2)))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Array")))) {
        { AValue __old = elems; elems = a_array_get(node, a_string("elems")); a_release(__old); }
        if (a_truthy(a_eq(a_len(elems), a_int(0)))) {
            __ret = fn_cgen__R(a_string("a_array_new(0)"), li); goto __fn_cleanup;
        }
        { AValue __old = has_spread; has_spread = a_bool(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(elems);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("Spread")))) {
                    { AValue __old = has_spread; has_spread = a_bool(1); a_release(__old); }
                }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        if (a_truthy(has_spread)) {
            { AValue __old = code; code = a_string("a_array_new(0)"); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(elems);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue e = {0}, er = {0};
                    e = a_array_get(__iter_arr, a_int(__fi));
                    if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("Spread")))) {
                        { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(e, a_string("expr")), bm, ctx, li); a_release(__old); }
                        { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
                        { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(er, a_int(2))); a_release(__old); }
                        { AValue __old = code; code = a_str_concat(a_string("a_concat_arr("), a_str_concat(code, a_str_concat(a_string(", "), a_str_concat(a_array_get(er, a_int(0)), a_string(")"))))); a_release(__old); }
                    } else {
                        { AValue __old = er; er = fn_cgen_emit_expr(e, bm, ctx, li); a_release(__old); }
                        { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
                        { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(er, a_int(2))); a_release(__old); }
                        { AValue __old = code; code = a_str_concat(a_string("a_array_push("), a_str_concat(code, a_str_concat(a_string(", "), a_str_concat(a_array_get(er, a_int(0)), a_string(")"))))); a_release(__old); }
                    }
                    a_release(e);
                    a_release(er);
                }
                a_release(__iter_arr);
            }
            __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
        }
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(elems);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0}, er = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = er; er = fn_cgen_emit_expr(e, bm, ctx, li); a_release(__old); }
                { AValue __old = parts; parts = a_array_push(parts, a_array_get(er, a_int(0))); a_release(__old); }
                { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(er, a_int(2))); a_release(__old); }
                a_release(e);
                a_release(er);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_array_new("), a_str_concat(a_to_str(a_len(elems)), a_str_concat(a_string(", "), a_str_concat(a_str_join(parts, a_string(", ")), a_string(")"))))), li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("MapLiteral")))) {
        { AValue __old = entries; entries = a_array_get(node, a_string("entries")); a_release(__old); }
        if (a_truthy(a_eq(a_len(entries), a_int(0)))) {
            __ret = fn_cgen__R(a_string("a_map_new(0)"), li); goto __fn_cleanup;
        }
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(entries);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0}, key_node = {0}, key_str = {0}, vr = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = key_node; key_node = a_array_get(e, a_string("key")); a_release(__old); }
                { AValue __old = key_str; key_str = a_string(""); a_release(__old); }
                if (a_truthy(a_eq(a_array_get(key_node, a_string("tag")), a_string("String")))) {
                    { AValue __old = key_str; key_str = a_array_get(key_node, a_string("value")); a_release(__old); }
                }
                { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(e, a_string("value")), bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(vr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(vr, a_int(2))); a_release(__old); }
                { AValue __old = parts; parts = a_array_push(parts, a_str_concat(a_string("\""), a_str_concat(fn_cgen__escape_c_str(key_str), a_str_concat(a_string("\", "), a_array_get(vr, a_int(0)))))); a_release(__old); }
                a_release(e);
                a_release(key_node);
                a_release(key_str);
                a_release(vr);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_map_new("), a_str_concat(a_to_str(a_len(entries)), a_str_concat(a_string(", "), a_str_concat(a_str_join(parts, a_string(", ")), a_string(")"))))), li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Interpolation")))) {
        { AValue __old = parts; parts = a_array_get(node, a_string("parts")); a_release(__old); }
        { AValue __old = cparts; cparts = a_array_new(0); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(parts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0}, er = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(p, a_string("tag")), a_string("InterpLit")))) {
                    { AValue __old = cparts; cparts = a_array_push(cparts, a_str_concat(a_string("a_string(\""), a_str_concat(fn_cgen__escape_c_str(a_array_get(p, a_string("value"))), a_string("\")")))); a_release(__old); }
                } else {
                    { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(p, a_string("expr")), bm, ctx, li); a_release(__old); }
                    { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
                    { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(er, a_int(2))); a_release(__old); }
                    { AValue __old = cparts; cparts = a_array_push(cparts, a_str_concat(a_string("a_to_str("), a_str_concat(a_array_get(er, a_int(0)), a_string(")")))); a_release(__old); }
                }
                a_release(p);
                a_release(er);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_concat_n("), a_str_concat(a_to_str(a_len(cparts)), a_str_concat(a_string(", "), a_str_concat(a_str_join(cparts, a_string(", ")), a_string(")"))))), li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("IfExpr")))) {
        { AValue __old = cr; cr = fn_cgen_emit_expr(a_array_get(node, a_string("cond")), bm, ctx, li); a_release(__old); }
        { AValue __old = tr; tr = fn_cgen_emit_expr(a_array_get(node, a_string("then")), bm, ctx, a_array_get(cr, a_int(1))); a_release(__old); }
        { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("else")), bm, ctx, a_array_get(tr, a_int(1))); a_release(__old); }
        { AValue __old = lifted; lifted = a_concat_arr(a_array_get(cr, a_int(2)), a_concat_arr(a_array_get(tr, a_int(2)), a_array_get(er, a_int(2)))); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(a_string("(a_truthy("), a_str_concat(a_array_get(cr, a_int(0)), a_str_concat(a_string(") ? "), a_str_concat(a_array_get(tr, a_int(0)), a_str_concat(a_string(" : "), a_str_concat(a_array_get(er, a_int(0)), a_string(")"))))))), a_array_get(er, a_int(1)), lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("MatchExpr")))) {
        __ret = fn_cgen__emit_match_expr(node, bm, ctx, li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("BlockExpr")))) {
        { AValue __old = stmts; stmts = a_array_get(a_array_get(node, a_string("block")), a_string("stmts")); a_release(__old); }
        if (a_truthy(a_gt(a_len(stmts), a_int(0)))) {
            { AValue __old = last; last = a_array_get(stmts, a_sub(a_len(stmts), a_int(1))); a_release(__old); }
            if (a_truthy(a_eq(a_array_get(last, a_string("tag")), a_string("ExprStmt")))) {
                __ret = fn_cgen_emit_expr(a_array_get(last, a_string("expr")), bm, ctx, li); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(a_array_get(last, a_string("tag")), a_string("Return")))) {
                if (a_truthy(a_neq(a_type_of(a_array_get(last, a_string("expr"))), a_string("void")))) {
                    __ret = fn_cgen_emit_expr(a_array_get(last, a_string("expr")), bm, ctx, li); goto __fn_cleanup;
                }
            }
        }
        __ret = fn_cgen__R(a_string("a_void()"), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Pipe")))) {
        { AValue __old = right; right = a_array_get(node, a_string("right")); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(right, a_string("tag")), a_string("Call")))) {
            { AValue __old = new_args; new_args = a_concat_arr(a_array_new(1, a_array_get(node, a_string("left"))), a_array_get(right, a_string("args"))); a_release(__old); }
            { AValue __old = new_call; new_call = a_map_new(3, "tag", a_string("Call"), "func", a_array_get(right, a_string("func")), "args", new_args); a_release(__old); }
            __ret = fn_cgen_emit_expr(new_call, bm, ctx, li); goto __fn_cleanup;
        }
        { AValue __old = lr; lr = fn_cgen_emit_expr(a_array_get(node, a_string("left")), bm, ctx, li); a_release(__old); }
        { AValue __old = rr; rr = fn_cgen_emit_expr(a_array_get(node, a_string("right")), bm, ctx, a_array_get(lr, a_int(1))); a_release(__old); }
        { AValue __old = lifted; lifted = a_concat_arr(a_array_get(lr, a_int(2)), a_array_get(rr, a_int(2))); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_closure_call("), a_str_concat(a_array_get(rr, a_int(0)), a_str_concat(a_string(", 1, "), a_str_concat(a_array_get(lr, a_int(0)), a_string(")"))))), a_array_get(rr, a_int(1)), lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Lambda")))) {
        { AValue __old = lambda_name; lambda_name = a_str_concat(a_string("__lambda_"), a_to_str(li)); a_release(__old); }
        { AValue __old = params; params = a_array_get(node, a_string("params")); a_release(__old); }
        { AValue __old = body; body = a_array_get(node, a_string("body")); a_release(__old); }
        { AValue __old = enclosing_vars; enclosing_vars = a_array_get(ctx, a_string("enclosing_vars")); a_release(__old); }
        { AValue __old = captures; captures = a_array_new(0); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(enclosing_vars), a_string("array")))) {
            { AValue __old = captures; captures = fn_cgen__compute_captures(node, enclosing_vars, bm); a_release(__old); }
        }
        { AValue __old = fn_code; fn_code = a_str_concat(a_string("AValue "), a_str_concat(lambda_name, a_string("(AValue __env, int __argc, AValue* __argv) {"))); a_release(__old); }
        { AValue __old = ci; ci = a_int(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(captures);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue cap = {0};
                cap = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(a_string("\n    AValue "), a_str_concat(fn_cgen__mangle(cap), a_str_concat(a_string(" = a_array_get(__env, a_int("), a_str_concat(a_to_str(ci), a_string("));")))))); a_release(__old); }
                { AValue __old = ci; ci = a_add(ci, a_int(1)); a_release(__old); }
                a_release(cap);
            }
            a_release(__iter_arr);
        }
        { AValue __old = pi; pi = a_int(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(params);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(a_string("\n    AValue "), a_str_concat(fn_cgen__mangle(a_array_get(p, a_string("name"))), a_str_concat(a_string(" = __argv["), a_str_concat(a_to_str(pi), a_string("];")))))); a_release(__old); }
                { AValue __old = pi; pi = a_add(pi, a_int(1)); a_release(__old); }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        { AValue __old = next_li; next_li = a_add(li, a_int(1)); a_release(__old); }
        { AValue __old = body_lifted; body_lifted = a_array_new(0); a_release(__old); }
        { AValue __old = lambda_vars; lambda_vars = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(captures);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue cap = {0};
                cap = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = lambda_vars; lambda_vars = a_array_push(lambda_vars, cap); a_release(__old); }
                a_release(cap);
            }
            a_release(__iter_arr);
        }
        {
            AValue __iter_arr = a_iterable(params);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = lambda_vars; lambda_vars = a_array_push(lambda_vars, a_array_get(p, a_string("name"))); a_release(__old); }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        { AValue __old = body_tag; body_tag = a_array_get(body, a_string("tag")); a_release(__old); }
        if (a_truthy(a_eq(body_tag, a_string("BlockExpr")))) {
            { AValue __old = stmts; stmts = a_array_get(a_array_get(body, a_string("block")), a_string("stmts")); a_release(__old); }
            { AValue __old = body_vars; body_vars = fn_cgen__collect_vars_in_stmts(stmts); a_release(__old); }
            { AValue __old = bound; bound = a_array_new(0); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(captures);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue cap = {0};
                    cap = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = bound; bound = a_array_push(bound, fn_cgen__mangle(cap)); a_release(__old); }
                    a_release(cap);
                }
                a_release(__iter_arr);
            }
            {
                AValue __iter_arr = a_iterable(params);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue p = {0};
                    p = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = bound; bound = a_array_push(bound, fn_cgen__mangle(a_array_get(p, a_string("name")))); a_release(__old); }
                    a_release(p);
                }
                a_release(__iter_arr);
            }
            { AValue __old = decl_vars; decl_vars = a_array_new(0); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(body_vars);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue v = {0};
                    v = a_array_get(__iter_arr, a_int(__fi));
                    if (a_truthy(a_not(a_contains(bound, v)))) {
                        if (a_truthy(a_not(a_contains(decl_vars, v)))) {
                            { AValue __old = decl_vars; decl_vars = a_array_push(decl_vars, v); a_release(__old); }
                        }
                    }
                    a_release(v);
                }
                a_release(__iter_arr);
            }
            { AValue __old = body_lets; body_lets = fn_cgen__collect_lets_in_block(a_array_get(body, a_string("block"))); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(body_lets);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue bl = {0};
                    bl = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = lambda_vars; lambda_vars = a_array_push(lambda_vars, bl); a_release(__old); }
                    a_release(bl);
                }
                a_release(__iter_arr);
            }
            { AValue __old = lambda_ctx; lambda_ctx = a_map_set(ctx, a_string("enclosing_vars"), lambda_vars); a_release(__old); }
            if (a_truthy(a_gt(a_len(decl_vars), a_int(0)))) {
                { AValue __old = init_parts; init_parts = a_array_new(0); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(decl_vars);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v = {0};
                        v = a_array_get(__iter_arr, a_int(__fi));
                        { AValue __old = init_parts; init_parts = a_array_push(init_parts, a_str_concat(v, a_string(" = {0}"))); a_release(__old); }
                        a_release(v);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(a_string("\n    AValue "), a_str_concat(a_str_join(init_parts, a_string(", ")), a_string(";")))); a_release(__old); }
            }
            { AValue __old = lambda_cleanup; lambda_cleanup = a_array_new(0); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(captures);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue cap = {0};
                    cap = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = lambda_cleanup; lambda_cleanup = a_array_push(lambda_cleanup, fn_cgen__mangle(cap)); a_release(__old); }
                    a_release(cap);
                }
                a_release(__iter_arr);
            }
            { AValue __old = lambda_cleanup; lambda_cleanup = a_concat_arr(lambda_cleanup, decl_vars); a_release(__old); }
            { AValue __old = lambda_ctx; lambda_ctx = a_map_set(lambda_ctx, a_string("cleanup_vars"), lambda_cleanup); a_release(__old); }
            { AValue __old = lambda_ctx; lambda_ctx = a_map_set(lambda_ctx, a_string("goto_cleanup"), a_bool(0)); a_release(__old); }
            { AValue __old = has_return; has_return = a_bool(0); a_release(__old); }
            { AValue __old = si; si = a_int(0); a_release(__old); }
            while (a_truthy(a_lt(si, a_len(stmts)))) {
                { AValue __old = s; s = a_array_get(stmts, si); a_release(__old); }
                { AValue __old = is_last; is_last = a_eq(si, a_sub(a_len(stmts), a_int(1))); a_release(__old); }
                if (a_truthy(a_and(is_last, a_eq(a_array_get(s, a_string("tag")), a_string("ExprStmt"))))) {
                    { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(s, a_string("expr")), bm, lambda_ctx, next_li); a_release(__old); }
                    { AValue __old = cleanup; cleanup = fn_cgen__emit_cleanup(lambda_cleanup, a_int(1)); a_release(__old); }
                    { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(a_string("\n    { AValue __ret = "), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(";"), a_str_concat(cleanup, a_string("\n    return __ret; }")))))); a_release(__old); }
                    { AValue __old = next_li; next_li = a_array_get(er, a_int(1)); a_release(__old); }
                    { AValue __old = body_lifted; body_lifted = a_concat_arr(body_lifted, a_array_get(er, a_int(2))); a_release(__old); }
                    { AValue __old = has_return; has_return = a_bool(1); a_release(__old); }
                } else {
                    { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_int(1), bm, lambda_ctx, next_li); a_release(__old); }
                    { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
                    { AValue __old = next_li; next_li = a_array_get(sr, a_int(1)); a_release(__old); }
                    { AValue __old = body_lifted; body_lifted = a_concat_arr(body_lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                    if (a_truthy(a_and(is_last, a_eq(a_array_get(s, a_string("tag")), a_string("Return"))))) {
                        { AValue __old = has_return; has_return = a_bool(1); a_release(__old); }
                    }
                }
                { AValue __old = si; si = a_add(si, a_int(1)); a_release(__old); }
            }
            if (a_truthy(a_not(has_return))) {
                { AValue __old = cleanup; cleanup = fn_cgen__emit_cleanup(lambda_cleanup, a_int(1)); a_release(__old); }
                { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(cleanup, a_string("\n    return a_void();"))); a_release(__old); }
            }
        } else {
            { AValue __old = lambda_ctx; lambda_ctx = a_map_set(ctx, a_string("enclosing_vars"), lambda_vars); a_release(__old); }
            { AValue __old = lambda_cleanup; lambda_cleanup = a_array_new(0); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(captures);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue cap = {0};
                    cap = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = lambda_cleanup; lambda_cleanup = a_array_push(lambda_cleanup, fn_cgen__mangle(cap)); a_release(__old); }
                    a_release(cap);
                }
                a_release(__iter_arr);
            }
            { AValue __old = lambda_ctx; lambda_ctx = a_map_set(lambda_ctx, a_string("cleanup_vars"), lambda_cleanup); a_release(__old); }
            { AValue __old = lambda_ctx; lambda_ctx = a_map_set(lambda_ctx, a_string("goto_cleanup"), a_bool(0)); a_release(__old); }
            { AValue __old = er; er = fn_cgen_emit_expr(body, bm, lambda_ctx, next_li); a_release(__old); }
            { AValue __old = cleanup; cleanup = fn_cgen__emit_cleanup(lambda_cleanup, a_int(1)); a_release(__old); }
            { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_str_concat(a_string("\n    { AValue __ret = "), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(";"), a_str_concat(cleanup, a_string("\n    return __ret; }")))))); a_release(__old); }
            { AValue __old = next_li; next_li = a_array_get(er, a_int(1)); a_release(__old); }
            { AValue __old = body_lifted; body_lifted = a_concat_arr(body_lifted, a_array_get(er, a_int(2))); a_release(__old); }
        }
        { AValue __old = fn_code; fn_code = a_str_concat(fn_code, a_string("\n}\n")); a_release(__old); }
        { AValue __old = env_code; env_code = a_string("a_void()"); a_release(__old); }
        if (a_truthy(a_gt(a_len(captures), a_int(0)))) {
            { AValue __old = cap_parts; cap_parts = a_array_new(0); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(captures);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue cap = {0};
                    cap = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = cap_parts; cap_parts = a_array_push(cap_parts, a_str_concat(a_string("a_retain("), a_str_concat(fn_cgen__mangle(cap), a_string(")")))); a_release(__old); }
                    a_release(cap);
                }
                a_release(__iter_arr);
            }
            { AValue __old = env_code; env_code = a_str_concat(a_string("a_array_new("), a_str_concat(a_to_str(a_len(captures)), a_str_concat(a_string(", "), a_str_concat(a_str_join(cap_parts, a_string(", ")), a_string(")"))))); a_release(__old); }
        }
        { AValue __old = closure_code; closure_code = a_str_concat(a_string("a_closure("), a_str_concat(lambda_name, a_str_concat(a_string(", "), a_str_concat(env_code, a_string(")"))))); a_release(__old); }
        { AValue __old = all_lifted; all_lifted = a_concat_arr(body_lifted, a_array_new(1, fn_code)); a_release(__old); }
        __ret = fn_cgen__RL(closure_code, next_li, all_lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Try")))) {
        { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_try_unwrap("), a_str_concat(a_array_get(er, a_int(0)), a_string(")"))), a_array_get(er, a_int(1)), a_array_get(er, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TryBlock")))) {
        { AValue __old = block; block = a_array_get(node, a_string("block")); a_release(__old); }
        { AValue __old = stmts; stmts = a_array_get(block, a_string("stmts")); a_release(__old); }
        { AValue __old = nl; nl = fn_cgen__newline_char(); a_release(__old); }
        { AValue __old = code; code = a_str_concat(a_string("({"), nl); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        AValue __try_result;"), nl)); a_release(__old); }
        { AValue __old = try_vars; try_vars = fn_cgen__collect_vars_in_stmts(stmts); a_release(__old); }
        if (a_truthy(a_gt(a_len(try_vars), a_int(0)))) {
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        AValue "), a_str_concat(a_str_join(try_vars, a_string(", ")), a_str_concat(a_string(";"), nl)))); a_release(__old); }
        }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        a_try_depth++;"), nl)); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        if (setjmp(a_try_stack[a_try_depth - 1]) == 0) {"), nl)); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        { AValue __old = nstmts; nstmts = a_len(stmts); a_release(__old); }
        { AValue __old = has_tail; has_tail = a_and(a_gt(nstmts, a_int(0)), a_eq(a_array_get(a_array_get(stmts, a_sub(nstmts, a_int(1))), a_string("tag")), a_string("ExprStmt"))); a_release(__old); }
        if (a_truthy(has_tail)) {
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        AValue __try_tail = a_void();"), nl)); a_release(__old); }
        }
        { AValue __old = si; si = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(si, nstmts))) {
            { AValue __old = s; s = a_array_get(stmts, si); a_release(__old); }
            { AValue __old = is_last; is_last = a_eq(si, a_sub(nstmts, a_int(1))); a_release(__old); }
            if (a_truthy(a_and(is_last, has_tail))) {
                { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(s, a_string("expr")), bm, ctx, li); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            __try_tail = "), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(";"), nl)))); a_release(__old); }
                { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(er, a_int(2))); a_release(__old); }
            } else {
                { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_int(3), bm, ctx, li); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_array_get(sr, a_int(0)), nl)); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
            }
            { AValue __old = si; si = a_add(si, a_int(1)); a_release(__old); }
        }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            a_try_depth--;"), nl)); a_release(__old); }
        if (a_truthy(has_tail)) {
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            __try_result = a_ok(__try_tail);"), nl)); a_release(__old); }
        } else {
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            __try_result = a_ok(a_void());"), nl)); a_release(__old); }
        }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        } else {"), nl)); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            a_try_depth--;"), nl)); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            __try_result = a_err(a_try_err);"), nl)); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        }"), nl)); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        __try_result;"), nl)); a_release(__old); }
        { AValue __old = code; code = a_str_concat(code, a_string("    })")); a_release(__old); }
        __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
    }
    __ret = fn_cgen__R(a_str_concat(a_string("/* unhandled expr: "), a_str_concat(tag, a_string(" */a_void()"))), li); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(name);
    a_release(op);
    a_release(lr);
    a_release(rr);
    a_release(l);
    a_release(r);
    a_release(lifted);
    a_release(code);
    a_release(ir);
    a_release(func);
    a_release(a);
    a_release(fname);
    a_release(ar);
    a_release(arg_codes);
    a_release(cfn);
    a_release(all_fns);
    a_release(is_known_fn);
    a_release(call_args);
    a_release(mangled);
    a_release(aliases);
    a_release(er);
    a_release(field);
    a_release(elems);
    a_release(has_spread);
    a_release(parts);
    a_release(entries);
    a_release(key_node);
    a_release(key_str);
    a_release(vr);
    a_release(cparts);
    a_release(cr);
    a_release(tr);
    a_release(stmts);
    a_release(last);
    a_release(right);
    a_release(new_args);
    a_release(new_call);
    a_release(lambda_name);
    a_release(params);
    a_release(body);
    a_release(enclosing_vars);
    a_release(captures);
    a_release(fn_code);
    a_release(ci);
    a_release(pi);
    a_release(next_li);
    a_release(body_lifted);
    a_release(lambda_vars);
    a_release(body_tag);
    a_release(body_vars);
    a_release(bound);
    a_release(decl_vars);
    a_release(body_lets);
    a_release(lambda_ctx);
    a_release(init_parts);
    a_release(lambda_cleanup);
    a_release(has_return);
    a_release(si);
    a_release(s);
    a_release(is_last);
    a_release(cleanup);
    a_release(sr);
    a_release(env_code);
    a_release(cap_parts);
    a_release(closure_code);
    a_release(all_lifted);
    a_release(block);
    a_release(nl);
    a_release(try_vars);
    a_release(nstmts);
    a_release(has_tail);
    a_release(node);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__resolve_call_name(AValue func) {
    AValue obj = {0};
    AValue __ret = a_void();
    func = a_retain(func);
    if (a_truthy(a_eq(a_array_get(func, a_string("tag")), a_string("Ident")))) {
        __ret = a_array_get(func, a_string("name")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_array_get(func, a_string("tag")), a_string("FieldAccess")))) {
        { AValue __old = obj; obj = a_array_get(func, a_string("expr")); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(obj, a_string("tag")), a_string("Ident")))) {
            __ret = a_str_concat(a_array_get(obj, a_string("name")), a_str_concat(a_string("."), a_array_get(func, a_string("field")))); goto __fn_cleanup;
        }
    }
    __ret = a_string("__unknown__"); goto __fn_cleanup;
__fn_cleanup:
    a_release(obj);
    a_release(func);
    return __ret;
}

AValue fn_cgen__emit_pat_cond(AValue pat, AValue target, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, val = {0}, vr = {0}, name = {0}, elems = {0}, has_rest = {0}, fixed_count = {0}, cond = {0}, idx = {0}, lifted = {0}, sub = {0}, elem_var = {0}, sc = {0}, entries = {0}, key = {0}, val_expr = {0};
    AValue __ret = a_void();
    pat = a_retain(pat);
    target = a_retain(target);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = tag; tag = a_array_get(pat, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("PatWildcard")))) {
        __ret = fn_cgen__R(a_string("1"), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatIdent")))) {
        __ret = fn_cgen__R(a_string("1"), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatLiteral")))) {
        { AValue __old = val; val = a_array_get(pat, a_string("value")); a_release(__old); }
        { AValue __old = vr; vr = fn_cgen_emit_expr(val, bm, ctx, li); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(a_string("a_truthy(a_eq("), a_str_concat(target, a_str_concat(a_string(", "), a_str_concat(a_array_get(vr, a_int(0)), a_string("))"))))), a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatConstructor")))) {
        { AValue __old = name; name = a_array_get(pat, a_string("name")); a_release(__old); }
        if (a_truthy(a_eq(name, a_string("Ok")))) {
            __ret = fn_cgen__R(a_str_concat(a_string("a_is_ok_raw("), a_str_concat(target, a_string(")"))), li); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("Err")))) {
            __ret = fn_cgen__R(a_str_concat(a_string("a_is_err_raw("), a_str_concat(target, a_string(")"))), li); goto __fn_cleanup;
        }
        __ret = fn_cgen__R(a_string("0 /* unknown constructor */"), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatArray")))) {
        { AValue __old = elems; elems = a_array_get(pat, a_string("elems")); a_release(__old); }
        { AValue __old = has_rest; has_rest = a_bool(0); a_release(__old); }
        { AValue __old = fixed_count; fixed_count = a_int(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(elems);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatRest")))) {
                    { AValue __old = has_rest; has_rest = a_bool(1); a_release(__old); }
                }
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatElem")))) {
                    { AValue __old = fixed_count; fixed_count = a_add(fixed_count, a_int(1)); a_release(__old); }
                }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        { AValue __old = cond; cond = a_string(""); a_release(__old); }
        if (a_truthy(has_rest)) {
            { AValue __old = cond; cond = a_str_concat(a_string("a_is_array_min_len("), a_str_concat(target, a_str_concat(a_string(", "), a_str_concat(a_to_str(fixed_count), a_string(")"))))); a_release(__old); }
        } else {
            { AValue __old = cond; cond = a_str_concat(a_string("a_is_array_of_len("), a_str_concat(target, a_str_concat(a_string(", "), a_str_concat(a_to_str(fixed_count), a_string(")"))))); a_release(__old); }
        }
        { AValue __old = idx; idx = a_int(0); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(elems);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0}, sub = {0}, elem_var = {0}, sc = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatElem")))) {
                    { AValue __old = sub; sub = a_array_get(e, a_string("pattern")); a_release(__old); }
                    if (a_truthy(a_and(a_neq(a_array_get(sub, a_string("tag")), a_string("PatIdent")), a_neq(a_array_get(sub, a_string("tag")), a_string("PatWildcard"))))) {
                        { AValue __old = elem_var; elem_var = a_str_concat(target, a_str_concat(a_string(".aval->items["), a_str_concat(a_to_str(idx), a_string("]")))); a_release(__old); }
                        { AValue __old = sc; sc = fn_cgen__emit_pat_cond(sub, elem_var, bm, ctx, li); a_release(__old); }
                        { AValue __old = li; li = a_array_get(sc, a_int(1)); a_release(__old); }
                        { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sc, a_int(2))); a_release(__old); }
                        { AValue __old = cond; cond = a_str_concat(cond, a_str_concat(a_string(" && "), a_array_get(sc, a_int(0)))); a_release(__old); }
                    }
                    { AValue __old = idx; idx = a_add(idx, a_int(1)); a_release(__old); }
                }
                a_release(e);
                a_release(sub);
                a_release(elem_var);
                a_release(sc);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(cond, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatMap")))) {
        { AValue __old = entries; entries = a_array_get(pat, a_string("entries")); a_release(__old); }
        { AValue __old = cond; cond = a_str_concat(target, a_string(".tag == TAG_MAP")); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(entries);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0}, key = {0}, sub = {0}, val_expr = {0}, sc = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = key; key = a_array_get(e, a_string("key")); a_release(__old); }
                { AValue __old = cond; cond = a_str_concat(cond, a_str_concat(a_string(" && a_is_map_with("), a_str_concat(target, a_str_concat(a_string(", \""), a_str_concat(fn_cgen__escape_c_str(key), a_string("\")")))))); a_release(__old); }
                { AValue __old = sub; sub = a_array_get(e, a_string("pattern")); a_release(__old); }
                if (a_truthy(a_and(a_neq(a_array_get(sub, a_string("tag")), a_string("PatIdent")), a_neq(a_array_get(sub, a_string("tag")), a_string("PatWildcard"))))) {
                    { AValue __old = val_expr; val_expr = a_str_concat(a_string("a_map_get("), a_str_concat(target, a_str_concat(a_string(", a_string(\""), a_str_concat(fn_cgen__escape_c_str(key), a_string("\"))"))))); a_release(__old); }
                    { AValue __old = sc; sc = fn_cgen__emit_pat_cond(sub, val_expr, bm, ctx, li); a_release(__old); }
                    { AValue __old = li; li = a_array_get(sc, a_int(1)); a_release(__old); }
                    { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sc, a_int(2))); a_release(__old); }
                    { AValue __old = cond; cond = a_str_concat(cond, a_str_concat(a_string(" && "), a_array_get(sc, a_int(0)))); a_release(__old); }
                }
                a_release(e);
                a_release(key);
                a_release(sub);
                a_release(val_expr);
                a_release(sc);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(cond, li, lifted); goto __fn_cleanup;
    }
    __ret = fn_cgen__R(a_string("0 /* unknown pattern */"), li); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(val);
    a_release(vr);
    a_release(name);
    a_release(elems);
    a_release(has_rest);
    a_release(fixed_count);
    a_release(cond);
    a_release(idx);
    a_release(lifted);
    a_release(sub);
    a_release(elem_var);
    a_release(sc);
    a_release(entries);
    a_release(key);
    a_release(val_expr);
    a_release(pat);
    a_release(target);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__emit_pat_bind(AValue pat, AValue target, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, ind = {0}, name = {0}, args = {0}, code = {0}, inner = {0}, inner_expr = {0}, br = {0}, elems = {0}, idx = {0}, lifted = {0}, elem_var = {0}, rest_expr = {0}, entries = {0}, key = {0}, val_expr = {0};
    AValue __ret = a_void();
    pat = a_retain(pat);
    target = a_retain(target);
    depth = a_retain(depth);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = tag; tag = a_array_get(pat, a_string("tag")); a_release(__old); }
    { AValue __old = ind; ind = fn_cgen__indent(depth); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("PatWildcard")))) {
        __ret = fn_cgen__R(a_string(""), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatIdent")))) {
        __ret = fn_cgen__R(a_str_concat(ind, a_str_concat(fn_cgen__mangle(a_array_get(pat, a_string("name"))), a_str_concat(a_string(" = "), a_str_concat(target, a_string(";"))))), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatLiteral")))) {
        __ret = fn_cgen__R(a_string(""), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatConstructor")))) {
        { AValue __old = name; name = a_array_get(pat, a_string("name")); a_release(__old); }
        { AValue __old = args; args = a_array_get(pat, a_string("args")); a_release(__old); }
        { AValue __old = code; code = a_string(""); a_release(__old); }
        if (a_truthy(a_and(a_or(a_eq(name, a_string("Ok")), a_eq(name, a_string("Err"))), a_gt(a_len(args), a_int(0))))) {
            { AValue __old = inner; inner = a_array_get(args, a_int(0)); a_release(__old); }
            { AValue __old = inner_expr; inner_expr = a_str_concat(a_string("a_unwrap_unsafe("), a_str_concat(target, a_string(")"))); a_release(__old); }
            { AValue __old = br; br = fn_cgen__emit_pat_bind(inner, inner_expr, depth, bm, ctx, li); a_release(__old); }
            { AValue __old = code; code = a_array_get(br, a_int(0)); a_release(__old); }
            { AValue __old = li; li = a_array_get(br, a_int(1)); a_release(__old); }
        }
        __ret = fn_cgen__R(code, li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatArray")))) {
        { AValue __old = elems; elems = a_array_get(pat, a_string("elems")); a_release(__old); }
        { AValue __old = code; code = a_string(""); a_release(__old); }
        { AValue __old = idx; idx = a_int(0); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(elems);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0}, elem_var = {0}, br = {0}, rest_expr = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatElem")))) {
                    { AValue __old = elem_var; elem_var = a_str_concat(target, a_str_concat(a_string(".aval->items["), a_str_concat(a_to_str(idx), a_string("]")))); a_release(__old); }
                    { AValue __old = br; br = fn_cgen__emit_pat_bind(a_array_get(e, a_string("pattern")), elem_var, depth, bm, ctx, li); a_release(__old); }
                    if (a_truthy(a_gt(a_len(a_array_get(br, a_int(0))), a_int(0)))) {
                        if (a_truthy(a_gt(a_len(code), a_int(0)))) {
                            { AValue __old = code; code = a_str_concat(code, a_string("\n")); a_release(__old); }
                        }
                        { AValue __old = code; code = a_str_concat(code, a_array_get(br, a_int(0))); a_release(__old); }
                    }
                    { AValue __old = li; li = a_array_get(br, a_int(1)); a_release(__old); }
                    { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(br, a_int(2))); a_release(__old); }
                    { AValue __old = idx; idx = a_add(idx, a_int(1)); a_release(__old); }
                }
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatRest")))) {
                    { AValue __old = rest_expr; rest_expr = a_str_concat(a_string("a_drop("), a_str_concat(target, a_str_concat(a_string(", a_int("), a_str_concat(a_to_str(idx), a_string("))"))))); a_release(__old); }
                    if (a_truthy(a_gt(a_len(code), a_int(0)))) {
                        { AValue __old = code; code = a_str_concat(code, a_string("\n")); a_release(__old); }
                    }
                    { AValue __old = code; code = a_str_concat(code, a_str_concat(ind, a_str_concat(fn_cgen__mangle(a_array_get(e, a_string("name"))), a_str_concat(a_string(" = "), a_str_concat(rest_expr, a_string(";")))))); a_release(__old); }
                }
                a_release(e);
                a_release(elem_var);
                a_release(br);
                a_release(rest_expr);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatMap")))) {
        { AValue __old = entries; entries = a_array_get(pat, a_string("entries")); a_release(__old); }
        { AValue __old = code; code = a_string(""); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(entries);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0}, key = {0}, val_expr = {0}, br = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = key; key = a_array_get(e, a_string("key")); a_release(__old); }
                { AValue __old = val_expr; val_expr = a_str_concat(a_string("a_map_get("), a_str_concat(target, a_str_concat(a_string(", a_string(\""), a_str_concat(fn_cgen__escape_c_str(key), a_string("\"))"))))); a_release(__old); }
                { AValue __old = br; br = fn_cgen__emit_pat_bind(a_array_get(e, a_string("pattern")), val_expr, depth, bm, ctx, li); a_release(__old); }
                if (a_truthy(a_gt(a_len(a_array_get(br, a_int(0))), a_int(0)))) {
                    if (a_truthy(a_gt(a_len(code), a_int(0)))) {
                        { AValue __old = code; code = a_str_concat(code, a_string("\n")); a_release(__old); }
                    }
                    { AValue __old = code; code = a_str_concat(code, a_array_get(br, a_int(0))); a_release(__old); }
                }
                { AValue __old = li; li = a_array_get(br, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(br, a_int(2))); a_release(__old); }
                a_release(e);
                a_release(key);
                a_release(val_expr);
                a_release(br);
            }
            a_release(__iter_arr);
        }
        __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
    }
    __ret = fn_cgen__R(a_string(""), li); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(ind);
    a_release(name);
    a_release(args);
    a_release(code);
    a_release(inner);
    a_release(inner_expr);
    a_release(br);
    a_release(elems);
    a_release(idx);
    a_release(lifted);
    a_release(elem_var);
    a_release(rest_expr);
    a_release(entries);
    a_release(key);
    a_release(val_expr);
    a_release(pat);
    a_release(target);
    a_release(depth);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__emit_arm_body(AValue body, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue code = {0}, lifted = {0}, sr = {0}, er = {0};
    AValue __ret = a_void();
    body = a_retain(body);
    depth = a_retain(depth);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = code; code = a_string(""); a_release(__old); }
    { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(a_array_get(body, a_string("stmts"))), a_string("array")))) {
        {
            AValue __iter_arr = a_iterable(a_array_get(body, a_string("stmts")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue s = {0}, sr = {0};
                s = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sr; sr = fn_cgen_emit_stmt(s, depth, bm, ctx, li); a_release(__old); }
                if (a_truthy(a_gt(a_len(code), a_int(0)))) {
                    { AValue __old = code; code = a_str_concat(code, a_string("\n")); a_release(__old); }
                }
                { AValue __old = code; code = a_str_concat(code, a_array_get(sr, a_int(0))); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                a_release(s);
                a_release(sr);
            }
            a_release(__iter_arr);
        }
    } else {
        { AValue __old = er; er = fn_cgen_emit_expr(body, bm, ctx, li); a_release(__old); }
        { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
        { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(er, a_int(2))); a_release(__old); }
        { AValue __old = code; code = a_str_concat(fn_cgen__indent(depth), a_str_concat(a_array_get(er, a_int(0)), a_string(";"))); a_release(__old); }
    }
    __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
__fn_cleanup:
    a_release(code);
    a_release(lifted);
    a_release(sr);
    a_release(er);
    a_release(body);
    a_release(depth);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__emit_match(AValue node, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue ind = {0}, d1 = {0}, d2 = {0}, er = {0}, lifted = {0}, arms = {0}, out = {0}, pat = {0}, guard = {0}, body = {0}, cr = {0}, has_guard = {0}, br = {0}, gr = {0}, br2 = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    depth = a_retain(depth);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = ind; ind = fn_cgen__indent(depth); a_release(__old); }
    { AValue __old = d1; d1 = fn_cgen__indent(a_add(depth, a_int(1))); a_release(__old); }
    { AValue __old = d2; d2 = fn_cgen__indent(a_add(depth, a_int(2))); a_release(__old); }
    { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
    { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
    { AValue __old = lifted; lifted = a_array_get(er, a_int(2)); a_release(__old); }
    { AValue __old = arms; arms = a_array_get(node, a_string("arms")); a_release(__old); }
    { AValue __old = out; out = a_str_concat(ind, a_string("{")); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d1, a_str_concat(a_string("AValue __match = "), a_str_concat(a_array_get(er, a_int(0)), a_string(";")))))); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d1, a_string("int __matched = 0;")))); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(arms);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue arm = {0}, pat = {0}, guard = {0}, body = {0}, cr = {0}, has_guard = {0}, br = {0}, gr = {0}, br2 = {0};
            arm = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = pat; pat = a_array_get(arm, a_string("pattern")); a_release(__old); }
            { AValue __old = guard; guard = a_array_get(arm, a_string("guard")); a_release(__old); }
            { AValue __old = body; body = a_array_get(arm, a_string("body")); a_release(__old); }
            { AValue __old = cr; cr = fn_cgen__emit_pat_cond(pat, a_string("__match"), bm, ctx, li); a_release(__old); }
            { AValue __old = li; li = a_array_get(cr, a_int(1)); a_release(__old); }
            { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(cr, a_int(2))); a_release(__old); }
            { AValue __old = has_guard; has_guard = a_neq(a_type_of(guard), a_string("void")); a_release(__old); }
            if (a_truthy(has_guard)) {
                if (a_truthy(a_eq(a_array_get(guard, a_string("tag")), a_string("Void")))) {
                    { AValue __old = has_guard; has_guard = a_bool(0); a_release(__old); }
                }
            }
            { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d1, a_str_concat(a_string("if (!__matched && "), a_str_concat(a_array_get(cr, a_int(0)), a_string(") {")))))); a_release(__old); }
            { AValue __old = br; br = fn_cgen__emit_pat_bind(pat, a_string("__match"), a_add(depth, a_int(2)), bm, ctx, li); a_release(__old); }
            { AValue __old = li; li = a_array_get(br, a_int(1)); a_release(__old); }
            { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(br, a_int(2))); a_release(__old); }
            if (a_truthy(a_gt(a_len(a_array_get(br, a_int(0))), a_int(0)))) {
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(br, a_int(0)))); a_release(__old); }
            }
            if (a_truthy(has_guard)) {
                { AValue __old = gr; gr = fn_cgen_emit_expr(guard, bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(gr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(gr, a_int(2))); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d2, a_str_concat(a_string("if (a_truthy("), a_str_concat(a_array_get(gr, a_int(0)), a_string(")) {")))))); a_release(__old); }
                { AValue __old = br2; br2 = fn_cgen__emit_arm_body(body, a_add(depth, a_int(3)), bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(br2, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(br2, a_int(2))); a_release(__old); }
                if (a_truthy(a_gt(a_len(a_array_get(br2, a_int(0))), a_int(0)))) {
                    { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(br2, a_int(0)))); a_release(__old); }
                }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(3))), a_string("__matched = 1;")))); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d2, a_string("}")))); a_release(__old); }
            } else {
                { AValue __old = br2; br2 = fn_cgen__emit_arm_body(body, a_add(depth, a_int(2)), bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(br2, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(br2, a_int(2))); a_release(__old); }
                if (a_truthy(a_gt(a_len(a_array_get(br2, a_int(0))), a_int(0)))) {
                    { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(br2, a_int(0)))); a_release(__old); }
                }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d2, a_string("__matched = 1;")))); a_release(__old); }
            }
            { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(d1, a_string("}")))); a_release(__old); }
            a_release(arm);
            a_release(pat);
            a_release(guard);
            a_release(body);
            a_release(cr);
            a_release(has_guard);
            a_release(br);
            a_release(gr);
            a_release(br2);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("}")))); a_release(__old); }
    __ret = fn_cgen__RL(out, li, lifted); goto __fn_cleanup;
__fn_cleanup:
    a_release(ind);
    a_release(d1);
    a_release(d2);
    a_release(er);
    a_release(lifted);
    a_release(arms);
    a_release(out);
    a_release(pat);
    a_release(guard);
    a_release(body);
    a_release(cr);
    a_release(has_guard);
    a_release(br);
    a_release(gr);
    a_release(br2);
    a_release(node);
    a_release(depth);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__emit_match_expr(AValue node, AValue bm, AValue ctx, AValue li) {
    AValue nl = {0}, er = {0}, lifted = {0}, arms = {0}, code = {0}, arm_vars = {0}, pv = {0}, pat = {0}, guard = {0}, body = {0}, cr = {0}, has_guard = {0}, br = {0}, gr = {0}, body_r = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = nl; nl = fn_cgen__newline_char(); a_release(__old); }
    { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
    { AValue __old = li; li = a_array_get(er, a_int(1)); a_release(__old); }
    { AValue __old = lifted; lifted = a_array_get(er, a_int(2)); a_release(__old); }
    { AValue __old = arms; arms = a_array_get(node, a_string("arms")); a_release(__old); }
    { AValue __old = code; code = a_str_concat(a_string("({"), nl); a_release(__old); }
    { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        AValue __match = "), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(";"), nl)))); a_release(__old); }
    { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        AValue __match_result = a_void();"), nl)); a_release(__old); }
    { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        int __matched = 0;"), nl)); a_release(__old); }
    { AValue __old = arm_vars; arm_vars = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(arms);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue arm = {0}, pv = {0};
            arm = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = pv; pv = fn_cgen__collect_pattern_vars(a_array_get(arm, a_string("pattern"))); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(pv);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue v = {0};
                    v = a_array_get(__iter_arr, a_int(__fi));
                    if (a_truthy(a_not(a_contains(arm_vars, v)))) {
                        { AValue __old = arm_vars; arm_vars = a_array_push(arm_vars, v); a_release(__old); }
                    }
                    a_release(v);
                }
                a_release(__iter_arr);
            }
            a_release(arm);
            a_release(pv);
        }
        a_release(__iter_arr);
    }
    if (a_truthy(a_gt(a_len(arm_vars), a_int(0)))) {
        { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        AValue "), a_str_concat(a_str_join(arm_vars, a_string(", ")), a_str_concat(a_string(";"), nl)))); a_release(__old); }
    }
    {
        AValue __iter_arr = a_iterable(arms);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue arm = {0}, pat = {0}, guard = {0}, body = {0}, cr = {0}, has_guard = {0}, br = {0}, gr = {0}, body_r = {0};
            arm = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = pat; pat = a_array_get(arm, a_string("pattern")); a_release(__old); }
            { AValue __old = guard; guard = a_array_get(arm, a_string("guard")); a_release(__old); }
            { AValue __old = body; body = a_array_get(arm, a_string("body")); a_release(__old); }
            { AValue __old = cr; cr = fn_cgen__emit_pat_cond(pat, a_string("__match"), bm, ctx, li); a_release(__old); }
            { AValue __old = li; li = a_array_get(cr, a_int(1)); a_release(__old); }
            { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(cr, a_int(2))); a_release(__old); }
            { AValue __old = has_guard; has_guard = a_neq(a_type_of(guard), a_string("void")); a_release(__old); }
            if (a_truthy(has_guard)) {
                if (a_truthy(a_eq(a_array_get(guard, a_string("tag")), a_string("Void")))) {
                    { AValue __old = has_guard; has_guard = a_bool(0); a_release(__old); }
                }
            }
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        if (!__matched && "), a_str_concat(a_array_get(cr, a_int(0)), a_str_concat(a_string(") {"), nl)))); a_release(__old); }
            { AValue __old = br; br = fn_cgen__emit_pat_bind(pat, a_string("__match"), a_int(3), bm, ctx, li); a_release(__old); }
            { AValue __old = li; li = a_array_get(br, a_int(1)); a_release(__old); }
            { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(br, a_int(2))); a_release(__old); }
            if (a_truthy(a_gt(a_len(a_array_get(br, a_int(0))), a_int(0)))) {
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_array_get(br, a_int(0)), nl)); a_release(__old); }
            }
            if (a_truthy(has_guard)) {
                { AValue __old = gr; gr = fn_cgen_emit_expr(guard, bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(gr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(gr, a_int(2))); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            if (a_truthy("), a_str_concat(a_array_get(gr, a_int(0)), a_str_concat(a_string(")) {"), nl)))); a_release(__old); }
                { AValue __old = body_r; body_r = fn_cgen__emit_match_expr_body(body, bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(body_r, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(body_r, a_int(2))); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("                __match_result = "), a_str_concat(a_array_get(body_r, a_int(0)), a_str_concat(a_string(";"), nl)))); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("                __matched = 1;"), nl)); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            }"), nl)); a_release(__old); }
            } else {
                { AValue __old = body_r; body_r = fn_cgen__emit_match_expr_body(body, bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(body_r, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(body_r, a_int(2))); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            __match_result = "), a_str_concat(a_array_get(body_r, a_int(0)), a_str_concat(a_string(";"), nl)))); a_release(__old); }
                { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("            __matched = 1;"), nl)); a_release(__old); }
            }
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        }"), nl)); a_release(__old); }
            a_release(arm);
            a_release(pat);
            a_release(guard);
            a_release(body);
            a_release(cr);
            a_release(has_guard);
            a_release(br);
            a_release(gr);
            a_release(body_r);
        }
        a_release(__iter_arr);
    }
    { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("        __match_result;"), nl)); a_release(__old); }
    { AValue __old = code; code = a_str_concat(code, a_string("    })")); a_release(__old); }
    __ret = fn_cgen__RL(code, li, lifted); goto __fn_cleanup;
__fn_cleanup:
    a_release(nl);
    a_release(er);
    a_release(lifted);
    a_release(arms);
    a_release(code);
    a_release(arm_vars);
    a_release(pv);
    a_release(pat);
    a_release(guard);
    a_release(body);
    a_release(cr);
    a_release(has_guard);
    a_release(br);
    a_release(gr);
    a_release(body_r);
    a_release(node);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__emit_match_expr_body(AValue body, AValue bm, AValue ctx, AValue li) {
    AValue lifted = {0}, stmts = {0}, last = {0}, nl = {0}, preamble = {0}, i = {0}, sr = {0}, er = {0};
    AValue __ret = a_void();
    body = a_retain(body);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(a_array_get(body, a_string("stmts"))), a_string("array")))) {
        { AValue __old = stmts; stmts = a_array_get(body, a_string("stmts")); a_release(__old); }
        if (a_truthy(a_eq(a_len(stmts), a_int(0)))) {
            __ret = fn_cgen__R(a_string("a_void()"), li); goto __fn_cleanup;
        }
        { AValue __old = last; last = a_array_get(stmts, a_sub(a_len(stmts), a_int(1))); a_release(__old); }
        { AValue __old = nl; nl = fn_cgen__newline_char(); a_release(__old); }
        { AValue __old = preamble; preamble = a_string(""); a_release(__old); }
        { AValue __old = i; i = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(i, a_sub(a_len(stmts), a_int(1))))) {
            { AValue __old = sr; sr = fn_cgen_emit_stmt(a_array_get(stmts, i), a_int(4), bm, ctx, li); a_release(__old); }
            { AValue __old = preamble; preamble = a_str_concat(preamble, a_str_concat(a_array_get(sr, a_int(0)), nl)); a_release(__old); }
            { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
            { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(a_array_get(last, a_string("tag")), a_string("ExprStmt")))) {
            { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(last, a_string("expr")), bm, ctx, li); a_release(__old); }
            if (a_truthy(a_gt(a_len(preamble), a_int(0)))) {
                __ret = fn_cgen__RL(a_str_concat(a_string("({"), a_str_concat(nl, a_str_concat(preamble, a_str_concat(a_string("                "), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(";"), a_str_concat(nl, a_string("            })")))))))), a_array_get(er, a_int(1)), a_concat_arr(lifted, a_array_get(er, a_int(2)))); goto __fn_cleanup;
            }
            __ret = fn_cgen__RL(a_array_get(er, a_int(0)), a_array_get(er, a_int(1)), a_concat_arr(lifted, a_array_get(er, a_int(2)))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(a_array_get(last, a_string("tag")), a_string("Return")))) {
            if (a_truthy(a_neq(a_type_of(a_array_get(last, a_string("expr"))), a_string("void")))) {
                { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(last, a_string("expr")), bm, ctx, li); a_release(__old); }
                if (a_truthy(a_gt(a_len(preamble), a_int(0)))) {
                    __ret = fn_cgen__RL(a_str_concat(a_string("({"), a_str_concat(nl, a_str_concat(preamble, a_str_concat(a_string("                "), a_str_concat(a_array_get(er, a_int(0)), a_str_concat(a_string(";"), a_str_concat(nl, a_string("            })")))))))), a_array_get(er, a_int(1)), a_concat_arr(lifted, a_array_get(er, a_int(2)))); goto __fn_cleanup;
                }
                __ret = fn_cgen__RL(a_array_get(er, a_int(0)), a_array_get(er, a_int(1)), a_concat_arr(lifted, a_array_get(er, a_int(2)))); goto __fn_cleanup;
            }
        }
        __ret = fn_cgen__R(a_string("a_void()"), li); goto __fn_cleanup;
    }
    __ret = fn_cgen_emit_expr(body, bm, ctx, li); goto __fn_cleanup;
__fn_cleanup:
    a_release(lifted);
    a_release(stmts);
    a_release(last);
    a_release(nl);
    a_release(preamble);
    a_release(i);
    a_release(sr);
    a_release(er);
    a_release(body);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen_emit_stmt(AValue node, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, ind = {0}, vr = {0}, vname = {0}, assign_expr = {0}, code = {0}, bindings = {0}, i = {0}, rest_name = {0}, target = {0}, tname = {0}, tr = {0}, ir = {0}, use_goto = {0}, cleanup_vars = {0}, has_cleanup = {0}, cleanup = {0}, val_expr = {0}, ret_expr = {0}, er = {0}, cr = {0}, lifted = {0}, out = {0}, then_stmts = {0}, sr = {0}, else_branch = {0}, else_stmts = {0}, body_stmts = {0}, var_name = {0}, body_vars = {0}, for_decls = {0}, init_parts = {0}, for_cleanup = {0}, decls = {0}, bi = {0}, fd_cleanup = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    depth = a_retain(depth);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    { AValue __old = ind; ind = fn_cgen__indent(depth); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Let")))) {
        { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(node, a_string("value")), bm, ctx, li); a_release(__old); }
        { AValue __old = vname; vname = fn_cgen__mangle(a_array_get(node, a_string("name"))); a_release(__old); }
        { AValue __old = assign_expr; assign_expr = a_array_get(vr, a_int(0)); a_release(__old); }
        { AValue __old = code; code = a_string(""); a_release(__old); }
        if (a_truthy(fn_cgen__is_ident(a_array_get(node, a_string("value"))))) {
            { AValue __old = code; code = a_str_concat(ind, a_str_concat(a_string("{ AValue __old = "), a_str_concat(vname, a_str_concat(a_string("; "), a_str_concat(vname, a_str_concat(a_string(" = a_retain("), a_str_concat(assign_expr, a_string("); a_release(__old); }")))))))); a_release(__old); }
        } else {
            { AValue __old = code; code = a_str_concat(ind, a_str_concat(a_string("{ AValue __old = "), a_str_concat(vname, a_str_concat(a_string("; "), a_str_concat(vname, a_str_concat(a_string(" = "), a_str_concat(assign_expr, a_string("; a_release(__old); }")))))))); a_release(__old); }
        }
        __ret = fn_cgen__RL(code, a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("LetDestructure")))) {
        { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(node, a_string("value")), bm, ctx, li); a_release(__old); }
        { AValue __old = code; code = a_str_concat(ind, a_str_concat(a_string("__dest = "), a_str_concat(a_array_get(vr, a_int(0)), a_string(";")))); a_release(__old); }
        { AValue __old = bindings; bindings = a_array_get(node, a_string("bindings")); a_release(__old); }
        { AValue __old = i; i = a_int(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(bindings);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue b = {0};
                b = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_and(a_neq(b, a_string("")), a_neq(b, a_string("_"))))) {
                    { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("\n"), a_str_concat(ind, a_str_concat(fn_cgen__mangle(b), a_str_concat(a_string(" = a_array_get(__dest, a_int("), a_str_concat(a_to_str(i), a_string("));"))))))); a_release(__old); }
                }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
                a_release(b);
            }
            a_release(__iter_arr);
        }
        { AValue __old = rest_name; rest_name = a_array_get(node, a_string("rest")); a_release(__old); }
        if (a_truthy(a_neq(rest_name, a_string("")))) {
            { AValue __old = code; code = a_str_concat(code, a_str_concat(a_string("\n"), a_str_concat(ind, a_str_concat(fn_cgen__mangle(rest_name), a_str_concat(a_string(" = a_drop(__dest, a_int("), a_str_concat(a_to_str(a_len(bindings)), a_string("));"))))))); a_release(__old); }
        }
        __ret = fn_cgen__RL(code, a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Assign")))) {
        { AValue __old = target; target = a_array_get(node, a_string("target")); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(target, a_string("tag")), a_string("Ident")))) {
            { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(node, a_string("value")), bm, ctx, li); a_release(__old); }
            { AValue __old = tname; tname = fn_cgen__mangle(a_array_get(target, a_string("name"))); a_release(__old); }
            { AValue __old = assign_expr; assign_expr = a_array_get(vr, a_int(0)); a_release(__old); }
            { AValue __old = code; code = a_string(""); a_release(__old); }
            if (a_truthy(fn_cgen__is_ident(a_array_get(node, a_string("value"))))) {
                { AValue __old = code; code = a_str_concat(ind, a_str_concat(a_string("{ AValue __old = "), a_str_concat(tname, a_str_concat(a_string("; "), a_str_concat(tname, a_str_concat(a_string(" = a_retain("), a_str_concat(assign_expr, a_string("); a_release(__old); }")))))))); a_release(__old); }
            } else {
                { AValue __old = code; code = a_str_concat(ind, a_str_concat(a_string("{ AValue __old = "), a_str_concat(tname, a_str_concat(a_string("; "), a_str_concat(tname, a_str_concat(a_string(" = "), a_str_concat(assign_expr, a_string("; a_release(__old); }")))))))); a_release(__old); }
            }
            __ret = fn_cgen__RL(code, a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(a_array_get(target, a_string("tag")), a_string("Index")))) {
            { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(node, a_string("value")), bm, ctx, li); a_release(__old); }
            { AValue __old = tr; tr = fn_cgen_emit_expr(a_array_get(target, a_string("expr")), bm, ctx, a_array_get(vr, a_int(1))); a_release(__old); }
            { AValue __old = ir; ir = fn_cgen_emit_expr(a_array_get(target, a_string("index")), bm, ctx, a_array_get(tr, a_int(1))); a_release(__old); }
            __ret = fn_cgen__RL(a_str_concat(ind, a_str_concat(a_array_get(tr, a_int(0)), a_str_concat(a_string(" = a_index_set("), a_str_concat(a_array_get(tr, a_int(0)), a_str_concat(a_string(", "), a_str_concat(a_array_get(ir, a_int(0)), a_str_concat(a_string(", "), a_str_concat(a_array_get(vr, a_int(0)), a_string(");"))))))))), a_array_get(ir, a_int(1)), a_concat_arr(a_array_get(vr, a_int(2)), a_concat_arr(a_array_get(tr, a_int(2)), a_array_get(ir, a_int(2))))); goto __fn_cleanup;
        }
        { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(node, a_string("value")), bm, ctx, li); a_release(__old); }
        { AValue __old = tr; tr = fn_cgen_emit_expr(target, bm, ctx, a_array_get(vr, a_int(1))); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(ind, a_str_concat(a_array_get(tr, a_int(0)), a_str_concat(a_string(" = "), a_str_concat(a_array_get(vr, a_int(0)), a_string(";"))))), a_array_get(tr, a_int(1)), a_concat_arr(a_array_get(vr, a_int(2)), a_array_get(tr, a_int(2)))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Return")))) {
        { AValue __old = use_goto; use_goto = a_and(a_eq(a_type_of(a_array_get(ctx, a_string("goto_cleanup"))), a_string("bool")), a_array_get(ctx, a_string("goto_cleanup"))); a_release(__old); }
        { AValue __old = cleanup_vars; cleanup_vars = a_array_get(ctx, a_string("cleanup_vars")); a_release(__old); }
        { AValue __old = has_cleanup; has_cleanup = a_and(a_eq(a_type_of(cleanup_vars), a_string("array")), a_gt(a_len(cleanup_vars), a_int(0))); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(a_array_get(node, a_string("expr"))), a_string("void")))) {
            if (a_truthy(use_goto)) {
                __ret = fn_cgen__R(a_str_concat(ind, a_string("goto __fn_cleanup;")), li); goto __fn_cleanup;
            }
            if (a_truthy(has_cleanup)) {
                { AValue __old = cleanup; cleanup = fn_cgen__emit_cleanup(cleanup_vars, depth); a_release(__old); }
                __ret = fn_cgen__R(a_str_concat(ind, a_str_concat(a_string("{"), a_str_concat(cleanup, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("return a_void(); }")))))), li); goto __fn_cleanup;
            }
            __ret = fn_cgen__R(a_str_concat(ind, a_string("return a_void();")), li); goto __fn_cleanup;
        }
        { AValue __old = vr; vr = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
        if (a_truthy(use_goto)) {
            { AValue __old = val_expr; val_expr = a_array_get(vr, a_int(0)); a_release(__old); }
            if (a_truthy(fn_cgen__is_ident(a_array_get(node, a_string("expr"))))) {
                { AValue __old = val_expr; val_expr = a_str_concat(a_string("a_retain("), a_str_concat(a_array_get(vr, a_int(0)), a_string(")"))); a_release(__old); }
            }
            __ret = fn_cgen__RL(a_str_concat(ind, a_str_concat(a_string("__ret = "), a_str_concat(val_expr, a_string("; goto __fn_cleanup;")))), a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
        }
        if (a_truthy(has_cleanup)) {
            { AValue __old = val_expr; val_expr = a_array_get(vr, a_int(0)); a_release(__old); }
            { AValue __old = ret_expr; ret_expr = a_retain(val_expr); a_release(__old); }
            if (a_truthy(fn_cgen__is_ident(a_array_get(node, a_string("expr"))))) {
                { AValue __old = ret_expr; ret_expr = a_str_concat(a_string("a_retain("), a_str_concat(val_expr, a_string(")"))); a_release(__old); }
            }
            { AValue __old = cleanup; cleanup = fn_cgen__emit_cleanup(cleanup_vars, depth); a_release(__old); }
            { AValue __old = code; code = a_str_concat(ind, a_str_concat(a_string("{ AValue __ret = "), a_str_concat(ret_expr, a_str_concat(a_string(";"), a_str_concat(cleanup, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("return __ret; }")))))))); a_release(__old); }
            __ret = fn_cgen__RL(code, a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
        }
        __ret = fn_cgen__RL(a_str_concat(ind, a_str_concat(a_string("return "), a_str_concat(a_array_get(vr, a_int(0)), a_string(";")))), a_array_get(vr, a_int(1)), a_array_get(vr, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ExprStmt")))) {
        { AValue __old = er; er = fn_cgen_emit_expr(a_array_get(node, a_string("expr")), bm, ctx, li); a_release(__old); }
        __ret = fn_cgen__RL(a_str_concat(ind, a_str_concat(a_array_get(er, a_int(0)), a_string(";"))), a_array_get(er, a_int(1)), a_array_get(er, a_int(2))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("If")))) {
        { AValue __old = cr; cr = fn_cgen_emit_expr(a_array_get(node, a_string("cond")), bm, ctx, li); a_release(__old); }
        { AValue __old = li; li = a_array_get(cr, a_int(1)); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_get(cr, a_int(2)); a_release(__old); }
        { AValue __old = out; out = a_str_concat(ind, a_str_concat(a_string("if (a_truthy("), a_str_concat(a_array_get(cr, a_int(0)), a_string(")) {")))); a_release(__old); }
        { AValue __old = then_stmts; then_stmts = a_array_get(a_array_get(node, a_string("then")), a_string("stmts")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(then_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue s = {0}, sr = {0};
                s = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_add(depth, a_int(1)), bm, ctx, li); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                a_release(s);
                a_release(sr);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("}")))); a_release(__old); }
        { AValue __old = else_branch; else_branch = a_array_get(node, a_string("else")); a_release(__old); }
        if (a_truthy(a_neq(a_type_of(else_branch), a_string("void")))) {
            if (a_truthy(a_eq(a_array_get(else_branch, a_string("tag")), a_string("ElseBlock")))) {
                { AValue __old = out; out = a_str_concat(out, a_string(" else {")); a_release(__old); }
                { AValue __old = else_stmts; else_stmts = a_array_get(a_array_get(else_branch, a_string("block")), a_string("stmts")); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(else_stmts);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue s = {0}, sr = {0};
                        s = a_array_get(__iter_arr, a_int(__fi));
                        { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_add(depth, a_int(1)), bm, ctx, li); a_release(__old); }
                        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
                        { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                        { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                        a_release(s);
                        a_release(sr);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("}")))); a_release(__old); }
            }
            if (a_truthy(a_eq(a_array_get(else_branch, a_string("tag")), a_string("ElseIf")))) {
                { AValue __old = sr; sr = fn_cgen_emit_stmt(a_array_get(else_branch, a_string("stmt")), depth, bm, ctx, li); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string(" else "), a_array_get(sr, a_int(0)))); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
            }
        }
        __ret = fn_cgen__RL(out, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("While")))) {
        { AValue __old = cr; cr = fn_cgen_emit_expr(a_array_get(node, a_string("cond")), bm, ctx, li); a_release(__old); }
        { AValue __old = li; li = a_array_get(cr, a_int(1)); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_get(cr, a_int(2)); a_release(__old); }
        { AValue __old = out; out = a_str_concat(ind, a_str_concat(a_string("while (a_truthy("), a_str_concat(a_array_get(cr, a_int(0)), a_string(")) {")))); a_release(__old); }
        { AValue __old = body_stmts; body_stmts = a_array_get(a_array_get(node, a_string("body")), a_string("stmts")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(body_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue s = {0}, sr = {0};
                s = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_add(depth, a_int(1)), bm, ctx, li); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                a_release(s);
                a_release(sr);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("}")))); a_release(__old); }
        __ret = fn_cgen__RL(out, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("For")))) {
        { AValue __old = var_name; var_name = fn_cgen__mangle(a_array_get(node, a_string("var"))); a_release(__old); }
        { AValue __old = ir; ir = fn_cgen_emit_expr(a_array_get(node, a_string("iter")), bm, ctx, li); a_release(__old); }
        { AValue __old = li; li = a_array_get(ir, a_int(1)); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_get(ir, a_int(2)); a_release(__old); }
        { AValue __old = out; out = a_str_concat(ind, a_string("{")); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_str_concat(a_string("AValue __iter_arr = a_iterable("), a_str_concat(a_array_get(ir, a_int(0)), a_string(");")))))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_string("for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {")))); a_release(__old); }
        { AValue __old = body_vars; body_vars = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(node, a_string("body")), a_string("stmts"))); a_release(__old); }
        { AValue __old = for_decls; for_decls = a_array_new(1, var_name); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(body_vars);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_not(a_contains(for_decls, v)))) {
                    { AValue __old = for_decls; for_decls = a_array_push(for_decls, v); a_release(__old); }
                }
                a_release(v);
            }
            a_release(__iter_arr);
        }
        { AValue __old = init_parts; init_parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(for_decls);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = init_parts; init_parts = a_array_push(init_parts, a_str_concat(v, a_string(" = {0}"))); a_release(__old); }
                a_release(v);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(2))), a_str_concat(a_string("AValue "), a_str_concat(a_str_join(init_parts, a_string(", ")), a_string(";")))))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(2))), a_str_concat(var_name, a_string(" = a_array_get(__iter_arr, a_int(__fi));"))))); a_release(__old); }
        { AValue __old = body_stmts; body_stmts = a_array_get(a_array_get(node, a_string("body")), a_string("stmts")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(body_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue s = {0}, sr = {0};
                s = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_add(depth, a_int(2)), bm, ctx, li); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                a_release(s);
                a_release(sr);
            }
            a_release(__iter_arr);
        }
        { AValue __old = for_cleanup; for_cleanup = fn_cgen__emit_cleanup(for_decls, a_add(depth, a_int(2))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, for_cleanup); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_string("}")))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_string("a_release(__iter_arr);")))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("}")))); a_release(__old); }
        __ret = fn_cgen__RL(out, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ForDestructure")))) {
        { AValue __old = bindings; bindings = a_array_get(node, a_string("bindings")); a_release(__old); }
        { AValue __old = ir; ir = fn_cgen_emit_expr(a_array_get(node, a_string("iter")), bm, ctx, li); a_release(__old); }
        { AValue __old = li; li = a_array_get(ir, a_int(1)); a_release(__old); }
        { AValue __old = lifted; lifted = a_array_get(ir, a_int(2)); a_release(__old); }
        { AValue __old = out; out = a_str_concat(ind, a_string("{")); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_str_concat(a_string("AValue __iter_arr = a_iterable("), a_str_concat(a_array_get(ir, a_int(0)), a_string(");")))))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_string("for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {")))); a_release(__old); }
        { AValue __old = decls; decls = a_array_new(1, a_string("__elem")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(bindings);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue b = {0};
                b = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = decls; decls = a_array_push(decls, fn_cgen__mangle(b)); a_release(__old); }
                a_release(b);
            }
            a_release(__iter_arr);
        }
        { AValue __old = body_vars; body_vars = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(node, a_string("body")), a_string("stmts"))); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(body_vars);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_not(a_contains(decls, v)))) {
                    { AValue __old = decls; decls = a_array_push(decls, v); a_release(__old); }
                }
                a_release(v);
            }
            a_release(__iter_arr);
        }
        { AValue __old = init_parts; init_parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(decls);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = init_parts; init_parts = a_array_push(init_parts, a_str_concat(v, a_string(" = {0}"))); a_release(__old); }
                a_release(v);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(2))), a_str_concat(a_string("AValue "), a_str_concat(a_str_join(init_parts, a_string(", ")), a_string(";")))))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(2))), a_string("__elem = a_array_get(__iter_arr, a_int(__fi));")))); a_release(__old); }
        { AValue __old = bi; bi = a_int(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(bindings);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue b = {0};
                b = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(2))), a_str_concat(fn_cgen__mangle(b), a_str_concat(a_string(" = a_array_get(__elem, a_int("), a_str_concat(a_to_str(bi), a_string("));"))))))); a_release(__old); }
                { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
                a_release(b);
            }
            a_release(__iter_arr);
        }
        { AValue __old = body_stmts; body_stmts = a_array_get(a_array_get(node, a_string("body")), a_string("stmts")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(body_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue s = {0}, sr = {0};
                s = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_add(depth, a_int(2)), bm, ctx, li); a_release(__old); }
                { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
                { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
                { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
                a_release(s);
                a_release(sr);
            }
            a_release(__iter_arr);
        }
        { AValue __old = fd_cleanup; fd_cleanup = fn_cgen__emit_cleanup(decls, a_add(depth, a_int(2))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, fd_cleanup); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_string("}")))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(fn_cgen__indent(a_add(depth, a_int(1))), a_string("a_release(__iter_arr);")))); a_release(__old); }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_str_concat(ind, a_string("}")))); a_release(__old); }
        __ret = fn_cgen__RL(out, li, lifted); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Match")))) {
        __ret = fn_cgen__emit_match(node, depth, bm, ctx, li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Break")))) {
        __ret = fn_cgen__R(a_str_concat(ind, a_string("break;")), li); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Continue")))) {
        __ret = fn_cgen__R(a_str_concat(ind, a_string("continue;")), li); goto __fn_cleanup;
    }
    __ret = fn_cgen__R(a_str_concat(ind, a_str_concat(a_string("/* unhandled stmt: "), a_str_concat(tag, a_string(" */")))), li); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(ind);
    a_release(vr);
    a_release(vname);
    a_release(assign_expr);
    a_release(code);
    a_release(bindings);
    a_release(i);
    a_release(rest_name);
    a_release(target);
    a_release(tname);
    a_release(tr);
    a_release(ir);
    a_release(use_goto);
    a_release(cleanup_vars);
    a_release(has_cleanup);
    a_release(cleanup);
    a_release(val_expr);
    a_release(ret_expr);
    a_release(er);
    a_release(cr);
    a_release(lifted);
    a_release(out);
    a_release(then_stmts);
    a_release(sr);
    a_release(else_branch);
    a_release(else_stmts);
    a_release(body_stmts);
    a_release(var_name);
    a_release(body_vars);
    a_release(for_decls);
    a_release(init_parts);
    a_release(for_cleanup);
    a_release(decls);
    a_release(bi);
    a_release(fd_cleanup);
    a_release(node);
    a_release(depth);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__collect_pattern_vars(AValue pat) {
    AValue tag = {0}, vs = {0};
    AValue __ret = a_void();
    pat = a_retain(pat);
    { AValue __old = tag; tag = a_array_get(pat, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("PatIdent")))) {
        __ret = a_array_new(1, fn_cgen__mangle(a_array_get(pat, a_string("name")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatRest")))) {
        __ret = a_array_new(1, fn_cgen__mangle(a_array_get(pat, a_string("name")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatWildcard")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatLiteral")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatConstructor")))) {
        { AValue __old = vs; vs = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(pat, a_string("args")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue a = {0};
                a = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = vs; vs = a_concat_arr(vs, fn_cgen__collect_pattern_vars(a)); a_release(__old); }
                a_release(a);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(vs); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatArray")))) {
        { AValue __old = vs; vs = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(pat, a_string("elems")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatElem")))) {
                    { AValue __old = vs; vs = a_concat_arr(vs, fn_cgen__collect_pattern_vars(a_array_get(e, a_string("pattern")))); a_release(__old); }
                }
                if (a_truthy(a_eq(a_array_get(e, a_string("tag")), a_string("PatRest")))) {
                    { AValue __old = vs; vs = a_array_push(vs, fn_cgen__mangle(a_array_get(e, a_string("name")))); a_release(__old); }
                }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(vs); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatMap")))) {
        { AValue __old = vs; vs = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(pat, a_string("entries")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = vs; vs = a_concat_arr(vs, fn_cgen__collect_pattern_vars(a_array_get(e, a_string("pattern")))); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(vs); goto __fn_cleanup;
    }
    __ret = a_array_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(vs);
    a_release(pat);
    return __ret;
}

AValue fn_cgen__collect_vars_in_stmts(AValue stmts) {
    AValue vars = {0}, tag = {0}, name = {0}, tv = {0}, eb = {0}, ev = {0}, eiv = {0}, wv = {0}, fv = {0}, rn = {0}, pv = {0}, body = {0}, bv = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    { AValue __old = vars; vars = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0}, tag = {0}, name = {0}, tv = {0}, eb = {0}, ev = {0}, eiv = {0}, wv = {0}, fv = {0}, rn = {0}, pv = {0}, body = {0}, bv = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = tag; tag = a_array_get(s, a_string("tag")); a_release(__old); }
            if (a_truthy(a_eq(tag, a_string("Let")))) {
                { AValue __old = name; name = fn_cgen__mangle(a_array_get(s, a_string("name"))); a_release(__old); }
                if (a_truthy(a_not(a_contains(vars, name)))) {
                    { AValue __old = vars; vars = a_array_push(vars, name); a_release(__old); }
                }
            }
            if (a_truthy(a_eq(tag, a_string("If")))) {
                { AValue __old = tv; tv = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(s, a_string("then")), a_string("stmts"))); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(tv);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v = {0};
                        v = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_not(a_contains(vars, v)))) {
                            { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                        }
                        a_release(v);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = eb; eb = a_array_get(s, a_string("else")); a_release(__old); }
                if (a_truthy(a_neq(a_type_of(eb), a_string("void")))) {
                    if (a_truthy(a_eq(a_array_get(eb, a_string("tag")), a_string("ElseBlock")))) {
                        { AValue __old = ev; ev = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(eb, a_string("block")), a_string("stmts"))); a_release(__old); }
                        {
                            AValue __iter_arr = a_iterable(ev);
                            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                AValue v = {0};
                                v = a_array_get(__iter_arr, a_int(__fi));
                                if (a_truthy(a_not(a_contains(vars, v)))) {
                                    { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                                }
                                a_release(v);
                            }
                            a_release(__iter_arr);
                        }
                    }
                    if (a_truthy(a_eq(a_array_get(eb, a_string("tag")), a_string("ElseIf")))) {
                        { AValue __old = eiv; eiv = fn_cgen__collect_vars_in_stmts(a_array_new(1, a_array_get(eb, a_string("stmt")))); a_release(__old); }
                        {
                            AValue __iter_arr = a_iterable(eiv);
                            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                AValue v = {0};
                                v = a_array_get(__iter_arr, a_int(__fi));
                                if (a_truthy(a_not(a_contains(vars, v)))) {
                                    { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                                }
                                a_release(v);
                            }
                            a_release(__iter_arr);
                        }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("While")))) {
                { AValue __old = wv; wv = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(s, a_string("body")), a_string("stmts"))); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(wv);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v = {0};
                        v = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_not(a_contains(vars, v)))) {
                            { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                        }
                        a_release(v);
                    }
                    a_release(__iter_arr);
                }
            }
            if (a_truthy(a_eq(tag, a_string("For")))) {
                { AValue __old = fv; fv = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(s, a_string("body")), a_string("stmts"))); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(fv);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v = {0};
                        v = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_not(a_contains(vars, v)))) {
                            { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                        }
                        a_release(v);
                    }
                    a_release(__iter_arr);
                }
            }
            if (a_truthy(a_eq(tag, a_string("LetDestructure")))) {
                if (a_truthy(a_not(a_contains(vars, a_string("__dest"))))) {
                    { AValue __old = vars; vars = a_array_push(vars, a_string("__dest")); a_release(__old); }
                }
                {
                    AValue __iter_arr = a_iterable(a_array_get(s, a_string("bindings")));
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue b = {0}, name = {0};
                        b = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_and(a_neq(b, a_string("")), a_neq(b, a_string("_"))))) {
                            { AValue __old = name; name = fn_cgen__mangle(b); a_release(__old); }
                            if (a_truthy(a_not(a_contains(vars, name)))) {
                                { AValue __old = vars; vars = a_array_push(vars, name); a_release(__old); }
                            }
                        }
                        a_release(b);
                        a_release(name);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = rn; rn = a_array_get(s, a_string("rest")); a_release(__old); }
                if (a_truthy(a_neq(rn, a_string("")))) {
                    { AValue __old = name; name = fn_cgen__mangle(rn); a_release(__old); }
                    if (a_truthy(a_not(a_contains(vars, name)))) {
                        { AValue __old = vars; vars = a_array_push(vars, name); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("ForDestructure")))) {
                { AValue __old = fv; fv = fn_cgen__collect_vars_in_stmts(a_array_get(a_array_get(s, a_string("body")), a_string("stmts"))); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(fv);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v = {0};
                        v = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_not(a_contains(vars, v)))) {
                            { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                        }
                        a_release(v);
                    }
                    a_release(__iter_arr);
                }
            }
            if (a_truthy(a_eq(tag, a_string("Match")))) {
                {
                    AValue __iter_arr = a_iterable(a_array_get(s, a_string("arms")));
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue arm = {0}, pv = {0}, body = {0}, bv = {0};
                        arm = a_array_get(__iter_arr, a_int(__fi));
                        { AValue __old = pv; pv = fn_cgen__collect_pattern_vars(a_array_get(arm, a_string("pattern"))); a_release(__old); }
                        {
                            AValue __iter_arr = a_iterable(pv);
                            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                AValue v = {0};
                                v = a_array_get(__iter_arr, a_int(__fi));
                                if (a_truthy(a_not(a_contains(vars, v)))) {
                                    { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                                }
                                a_release(v);
                            }
                            a_release(__iter_arr);
                        }
                        { AValue __old = body; body = a_array_get(arm, a_string("body")); a_release(__old); }
                        if (a_truthy(a_eq(a_type_of(a_array_get(body, a_string("stmts"))), a_string("array")))) {
                            { AValue __old = bv; bv = fn_cgen__collect_vars_in_stmts(a_array_get(body, a_string("stmts"))); a_release(__old); }
                            {
                                AValue __iter_arr = a_iterable(bv);
                                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                    AValue v = {0};
                                    v = a_array_get(__iter_arr, a_int(__fi));
                                    if (a_truthy(a_not(a_contains(vars, v)))) {
                                        { AValue __old = vars; vars = a_array_push(vars, v); a_release(__old); }
                                    }
                                    a_release(v);
                                }
                                a_release(__iter_arr);
                            }
                        }
                        a_release(arm);
                        a_release(pv);
                        a_release(body);
                        a_release(bv);
                    }
                    a_release(__iter_arr);
                }
            }
            a_release(s);
            a_release(tag);
            a_release(name);
            a_release(tv);
            a_release(eb);
            a_release(ev);
            a_release(eiv);
            a_release(wv);
            a_release(fv);
            a_release(rn);
            a_release(pv);
            a_release(body);
            a_release(bv);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(vars); goto __fn_cleanup;
__fn_cleanup:
    a_release(vars);
    a_release(tag);
    a_release(name);
    a_release(tv);
    a_release(eb);
    a_release(ev);
    a_release(eiv);
    a_release(wv);
    a_release(fv);
    a_release(rn);
    a_release(pv);
    a_release(body);
    a_release(bv);
    a_release(stmts);
    return __ret;
}

AValue fn_cgen_emit_fn(AValue node, AValue bm, AValue ctx, AValue li) {
    AValue name = {0}, full_name = {0}, params = {0}, body = {0}, param_parts = {0}, param_str = {0}, out = {0}, all_vars = {0}, param_names = {0}, decl_vars = {0}, init_parts = {0}, pn = {0}, all_cleanup_vars = {0}, enc_vars = {0}, fn_ctx = {0}, lifted = {0}, stmts = {0}, sr = {0}, cleanup = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    bm = a_retain(bm);
    ctx = a_retain(ctx);
    li = a_retain(li);
    { AValue __old = name; name = a_array_get(node, a_string("name")); a_release(__old); }
    { AValue __old = full_name; full_name = fn_cgen__prefixed_name(name, ctx); a_release(__old); }
    { AValue __old = params; params = a_array_get(node, a_string("params")); a_release(__old); }
    { AValue __old = body; body = a_array_get(node, a_string("body")); a_release(__old); }
    { AValue __old = param_parts; param_parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = param_parts; param_parts = a_array_push(param_parts, a_str_concat(a_string("AValue "), fn_cgen__mangle(a_array_get(p, a_string("name"))))); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = param_str; param_str = a_str_join(param_parts, a_string(", ")); a_release(__old); }
    if (a_truthy(a_eq(a_len(params), a_int(0)))) {
        { AValue __old = param_str; param_str = a_string("void"); a_release(__old); }
    }
    { AValue __old = out; out = a_str_concat(a_string("AValue fn_"), a_str_concat(fn_cgen__mangle(full_name), a_str_concat(a_string("("), a_str_concat(param_str, a_string(") {"))))); a_release(__old); }
    { AValue __old = all_vars; all_vars = fn_cgen__collect_vars_in_stmts(a_array_get(body, a_string("stmts"))); a_release(__old); }
    { AValue __old = param_names; param_names = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = param_names; param_names = a_array_push(param_names, fn_cgen__mangle(a_array_get(p, a_string("name")))); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = decl_vars; decl_vars = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(all_vars);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue v = {0};
            v = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_not(a_contains(param_names, v)))) {
                if (a_truthy(a_not(a_contains(decl_vars, v)))) {
                    { AValue __old = decl_vars; decl_vars = a_array_push(decl_vars, v); a_release(__old); }
                }
            }
            a_release(v);
        }
        a_release(__iter_arr);
    }
    if (a_truthy(a_gt(a_len(decl_vars), a_int(0)))) {
        { AValue __old = init_parts; init_parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(decl_vars);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = init_parts; init_parts = a_array_push(init_parts, a_str_concat(v, a_string(" = {0}"))); a_release(__old); }
                a_release(v);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n    AValue "), a_str_concat(a_str_join(init_parts, a_string(", ")), a_string(";")))); a_release(__old); }
    }
    { AValue __old = out; out = a_str_concat(out, a_string("\n    AValue __ret = a_void();")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0}, pn = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = pn; pn = fn_cgen__mangle(a_array_get(p, a_string("name"))); a_release(__old); }
            { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n    "), a_str_concat(pn, a_str_concat(a_string(" = a_retain("), a_str_concat(pn, a_string(");")))))); a_release(__old); }
            a_release(p);
            a_release(pn);
        }
        a_release(__iter_arr);
    }
    { AValue __old = all_cleanup_vars; all_cleanup_vars = a_concat_arr(decl_vars, param_names); a_release(__old); }
    { AValue __old = enc_vars; enc_vars = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = enc_vars; enc_vars = a_array_push(enc_vars, a_array_get(p, a_string("name"))); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    {
        AValue __iter_arr = a_iterable(all_vars);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue v = {0};
            v = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_not(a_contains(enc_vars, fn_cgen__mangle(v))))) {
                { AValue __old = enc_vars; enc_vars = a_array_push(enc_vars, v); a_release(__old); }
            }
            a_release(v);
        }
        a_release(__iter_arr);
    }
    { AValue __old = fn_ctx; fn_ctx = a_map_set(ctx, a_string("enclosing_vars"), enc_vars); a_release(__old); }
    { AValue __old = fn_ctx; fn_ctx = a_map_set(fn_ctx, a_string("cleanup_vars"), all_cleanup_vars); a_release(__old); }
    { AValue __old = fn_ctx; fn_ctx = a_map_set(fn_ctx, a_string("goto_cleanup"), a_bool(1)); a_release(__old); }
    { AValue __old = fn_ctx; fn_ctx = a_map_set(fn_ctx, a_string("escaping_vars"), a_array_new(0)); a_release(__old); }
    { AValue __old = lifted; lifted = a_array_new(0); a_release(__old); }
    { AValue __old = stmts; stmts = a_array_get(body, a_string("stmts")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0}, sr = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = sr; sr = fn_cgen_emit_stmt(s, a_int(1), bm, fn_ctx, li); a_release(__old); }
            { AValue __old = out; out = a_str_concat(out, a_str_concat(a_string("\n"), a_array_get(sr, a_int(0)))); a_release(__old); }
            { AValue __old = li; li = a_array_get(sr, a_int(1)); a_release(__old); }
            { AValue __old = lifted; lifted = a_concat_arr(lifted, a_array_get(sr, a_int(2))); a_release(__old); }
            a_release(s);
            a_release(sr);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_str_concat(out, a_string("\n__fn_cleanup:")); a_release(__old); }
    { AValue __old = cleanup; cleanup = fn_cgen__emit_cleanup(all_cleanup_vars, a_int(1)); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_str_concat(cleanup, a_string("\n    return __ret;"))); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_string("\n}\n")); a_release(__old); }
    __ret = fn_cgen__RL(out, li, lifted); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(full_name);
    a_release(params);
    a_release(body);
    a_release(param_parts);
    a_release(param_str);
    a_release(out);
    a_release(all_vars);
    a_release(param_names);
    a_release(decl_vars);
    a_release(init_parts);
    a_release(pn);
    a_release(all_cleanup_vars);
    a_release(enc_vars);
    a_release(fn_ctx);
    a_release(lifted);
    a_release(stmts);
    a_release(sr);
    a_release(cleanup);
    a_release(node);
    a_release(bm);
    a_release(ctx);
    a_release(li);
    return __ret;
}

AValue fn_cgen__emit_fwd_decl(AValue name, AValue params) {
    AValue param_parts = {0}, param_str = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    params = a_retain(params);
    { AValue __old = param_parts; param_parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = param_parts; param_parts = a_array_push(param_parts, a_str_concat(a_string("AValue "), fn_cgen__mangle(a_array_get(p, a_string("name"))))); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = param_str; param_str = a_str_join(param_parts, a_string(", ")); a_release(__old); }
    if (a_truthy(a_eq(a_len(params), a_int(0)))) {
        { AValue __old = param_str; param_str = a_string("void"); a_release(__old); }
    }
    __ret = a_str_concat(a_string("AValue fn_"), a_str_concat(fn_cgen__mangle(name), a_str_concat(a_string("("), a_str_concat(param_str, a_string(");"))))); goto __fn_cleanup;
__fn_cleanup:
    a_release(param_parts);
    a_release(param_str);
    a_release(name);
    a_release(params);
    return __ret;
}

AValue fn_cgen__use_path_to_file(AValue path_arr) {
    AValue rel = {0}, mod_path = {0};
    AValue __ret = a_void();
    path_arr = a_retain(path_arr);
    { AValue __old = rel; rel = a_str_concat(a_str_join(path_arr, a_string("/")), a_string(".a")); a_release(__old); }
    if (a_truthy(a_fs_exists(rel))) {
        __ret = a_retain(rel); goto __fn_cleanup;
    }
    { AValue __old = mod_path; mod_path = a_str_concat(a_string("a_modules/"), rel); a_release(__old); }
    if (a_truthy(a_fs_exists(mod_path))) {
        __ret = a_retain(mod_path); goto __fn_cleanup;
    }
    __ret = a_retain(rel); goto __fn_cleanup;
__fn_cleanup:
    a_release(rel);
    a_release(mod_path);
    a_release(path_arr);
    return __ret;
}

AValue fn_cgen__use_path_short_name(AValue path_arr) {
    AValue __ret = a_void();
    path_arr = a_retain(path_arr);
    __ret = a_array_get(path_arr, a_sub(a_len(path_arr), a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(path_arr);
    return __ret;
}

AValue fn_cgen__collect_fn_names(AValue items) {
    AValue names = {0};
    AValue __ret = a_void();
    items = a_retain(items);
    { AValue __old = names; names = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = names; names = a_array_push(names, a_array_get(item, a_string("name"))); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(names); goto __fn_cleanup;
__fn_cleanup:
    a_release(names);
    a_release(items);
    return __ret;
}

AValue fn_cgen__load_module(AValue path_arr, AValue bm, AValue loaded, AValue li) {
    AValue use_key = {0}, file_path = {0}, rel = {0}, source = {0}, mod_ast = {0}, items = {0}, _short = {0}, all_fwd = {0}, all_fns = {0}, sub = {0}, fn_names = {0}, ctx = {0}, aliases = {0}, full_name = {0}, fr = {0};
    AValue __ret = a_void();
    path_arr = a_retain(path_arr);
    bm = a_retain(bm);
    loaded = a_retain(loaded);
    li = a_retain(li);
    { AValue __old = use_key; use_key = a_str_join(path_arr, a_string(".")); a_release(__old); }
    if (a_truthy(a_contains(loaded, use_key))) {
        __ret = a_map_new(4, "fwd", a_array_new(0), "fns", a_array_new(0), "loaded", loaded, "li", li); goto __fn_cleanup;
    }
    { AValue __old = loaded; loaded = a_array_push(loaded, use_key); a_release(__old); }
    { AValue __old = file_path; file_path = fn_cgen__use_path_to_file(path_arr); a_release(__old); }
    { AValue __old = rel; rel = a_str_concat(a_str_join(path_arr, a_string("/")), a_string(".a")); a_release(__old); }
    { AValue __old = source; source = a_embedded_file(rel); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        { AValue __old = source; source = a_io_read_file(file_path); a_release(__old); }
    }
    { AValue __old = mod_ast; mod_ast = fn_parser_parse(source); a_release(__old); }
    { AValue __old = items; items = a_array_get(mod_ast, a_string("items")); a_release(__old); }
    { AValue __old = _short; _short = fn_cgen__use_path_short_name(path_arr); a_release(__old); }
    { AValue __old = all_fwd; all_fwd = a_array_new(0); a_release(__old); }
    { AValue __old = all_fns; all_fns = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, sub = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("UseDecl")))) {
                { AValue __old = sub; sub = fn_cgen__load_module(a_array_get(item, a_string("path")), bm, loaded, li); a_release(__old); }
                { AValue __old = loaded; loaded = a_array_get(sub, a_string("loaded")); a_release(__old); }
                { AValue __old = li; li = a_array_get(sub, a_string("li")); a_release(__old); }
                { AValue __old = all_fwd; all_fwd = a_concat_arr(all_fwd, a_array_get(sub, a_string("fwd"))); a_release(__old); }
                { AValue __old = all_fns; all_fns = a_concat_arr(all_fns, a_array_get(sub, a_string("fns"))); a_release(__old); }
            }
            a_release(item);
            a_release(sub);
        }
        a_release(__iter_arr);
    }
    { AValue __old = fn_names; fn_names = fn_cgen__collect_fn_names(items); a_release(__old); }
    { AValue __old = ctx; ctx = a_map_new(2, "prefix", _short, "fns", fn_names); a_release(__old); }
    { AValue __old = aliases; aliases = a_map_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, full_name = {0}, fr = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = full_name; full_name = fn_cgen__prefixed_name(a_array_get(item, a_string("name")), ctx); a_release(__old); }
                { AValue __old = all_fwd; all_fwd = a_array_push(all_fwd, fn_cgen__emit_fwd_decl(full_name, a_array_get(item, a_string("params")))); a_release(__old); }
                { AValue __old = fr; fr = fn_cgen_emit_fn(item, bm, ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(fr, a_int(1)); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(a_array_get(fr, a_int(2)));
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue lf = {0};
                        lf = a_array_get(__iter_arr, a_int(__fi));
                        { AValue __old = all_fns; all_fns = a_array_push(all_fns, lf); a_release(__old); }
                        a_release(lf);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = all_fns; all_fns = a_array_push(all_fns, a_array_get(fr, a_int(0))); a_release(__old); }
                { AValue __old = aliases; aliases = a_map_set(aliases, a_array_get(item, a_string("name")), full_name); a_release(__old); }
            }
            a_release(item);
            a_release(full_name);
            a_release(fr);
        }
        a_release(__iter_arr);
    }
    __ret = a_map_new(5, "fwd", all_fwd, "fns", all_fns, "loaded", loaded, "li", li, "aliases", aliases); goto __fn_cleanup;
__fn_cleanup:
    a_release(use_key);
    a_release(file_path);
    a_release(rel);
    a_release(source);
    a_release(mod_ast);
    a_release(items);
    a_release(_short);
    a_release(all_fwd);
    a_release(all_fns);
    a_release(sub);
    a_release(fn_names);
    a_release(ctx);
    a_release(aliases);
    a_release(full_name);
    a_release(fr);
    a_release(path_arr);
    a_release(bm);
    a_release(loaded);
    a_release(li);
    return __ret;
}

AValue fn_cgen__ffi_c_type(AValue ty_node) {
    AValue tag = {0}, name = {0};
    AValue __ret = a_void();
    ty_node = a_retain(ty_node);
    { AValue __old = tag; tag = a_array_get(ty_node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("TyPrim")))) {
        { AValue __old = name; name = a_array_get(ty_node, a_string("name")); a_release(__old); }
        if (a_truthy(a_eq(name, a_string("i8")))) {
            __ret = a_string("int8_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("i16")))) {
            __ret = a_string("int16_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("i32")))) {
            __ret = a_string("int32_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("i64")))) {
            __ret = a_string("int64_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("u8")))) {
            __ret = a_string("uint8_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("u16")))) {
            __ret = a_string("uint16_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("u32")))) {
            __ret = a_string("uint32_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("u64")))) {
            __ret = a_string("uint64_t"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("f32")))) {
            __ret = a_string("float"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("f64")))) {
            __ret = a_string("double"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("bool")))) {
            __ret = a_string("int"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("str")))) {
            __ret = a_string("const char*"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("ptr")))) {
            __ret = a_string("void*"); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("void")))) {
            __ret = a_string("void"); goto __fn_cleanup;
        }
    }
    __ret = a_string("AValue"); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(name);
    a_release(ty_node);
    return __ret;
}

AValue fn_cgen__ffi_extract(AValue ty_node, AValue var_name) {
    AValue tag = {0}, name = {0};
    AValue __ret = a_void();
    ty_node = a_retain(ty_node);
    var_name = a_retain(var_name);
    { AValue __old = tag; tag = a_array_get(ty_node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("TyPrim")))) {
        { AValue __old = name; name = a_array_get(ty_node, a_string("name")); a_release(__old); }
        if (a_truthy(a_or(a_or(a_eq(name, a_string("i8")), a_eq(name, a_string("i16"))), a_eq(name, a_string("i32"))))) {
            __ret = a_str_concat(a_string("("), a_str_concat(fn_cgen__ffi_c_type(ty_node), a_str_concat(a_string(")"), a_str_concat(var_name, a_string(".ival"))))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("i64")))) {
            __ret = a_str_concat(var_name, a_string(".ival")); goto __fn_cleanup;
        }
        if (a_truthy(a_or(a_or(a_or(a_eq(name, a_string("u8")), a_eq(name, a_string("u16"))), a_eq(name, a_string("u32"))), a_eq(name, a_string("u64"))))) {
            __ret = a_str_concat(a_string("("), a_str_concat(fn_cgen__ffi_c_type(ty_node), a_str_concat(a_string(")"), a_str_concat(var_name, a_string(".ival"))))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("f32")))) {
            __ret = a_str_concat(a_string("(float)"), a_str_concat(var_name, a_string(".fval"))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("f64")))) {
            __ret = a_str_concat(var_name, a_string(".fval")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("bool")))) {
            __ret = a_str_concat(var_name, a_string(".bval")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("str")))) {
            __ret = a_str_concat(var_name, a_string(".sval->data")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("ptr")))) {
            __ret = a_str_concat(var_name, a_string(".pval")); goto __fn_cleanup;
        }
    }
    __ret = a_retain(var_name); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(name);
    a_release(ty_node);
    a_release(var_name);
    return __ret;
}

AValue fn_cgen__ffi_wrap(AValue ty_node, AValue expr) {
    AValue tag = {0}, name = {0};
    AValue __ret = a_void();
    ty_node = a_retain(ty_node);
    expr = a_retain(expr);
    { AValue __old = tag; tag = a_array_get(ty_node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("TyPrim")))) {
        { AValue __old = name; name = a_array_get(ty_node, a_string("name")); a_release(__old); }
        if (a_truthy(a_or(a_or(a_or(a_or(a_or(a_or(a_or(a_eq(name, a_string("i8")), a_eq(name, a_string("i16"))), a_eq(name, a_string("i32"))), a_eq(name, a_string("i64"))), a_eq(name, a_string("u8"))), a_eq(name, a_string("u16"))), a_eq(name, a_string("u32"))), a_eq(name, a_string("u64"))))) {
            __ret = a_str_concat(a_string("a_int((int64_t)"), a_str_concat(expr, a_string(")"))); goto __fn_cleanup;
        }
        if (a_truthy(a_or(a_eq(name, a_string("f32")), a_eq(name, a_string("f64"))))) {
            __ret = a_str_concat(a_string("a_float((double)"), a_str_concat(expr, a_string(")"))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("bool")))) {
            __ret = a_str_concat(a_string("a_bool("), a_str_concat(expr, a_string(")"))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("str")))) {
            __ret = a_str_concat(a_string("a_string("), a_str_concat(expr, a_string(")"))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("ptr")))) {
            __ret = a_str_concat(a_string("a_ptr("), a_str_concat(expr, a_string(")"))); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(name, a_string("void")))) {
            __ret = a_string("a_void()"); goto __fn_cleanup;
        }
    }
    __ret = a_retain(expr); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(name);
    a_release(ty_node);
    a_release(expr);
    return __ret;
}

AValue fn_cgen__emit_extern_fn(AValue node) {
    AValue name = {0}, params = {0}, ret_type = {0}, c_ret = {0}, is_void = {0}, c_param_parts = {0}, c_param_str = {0}, raw_proto = {0}, shim_params = {0}, pi = {0}, shim_param_str = {0}, shim = {0}, call_args = {0}, p = {0}, extracted = {0}, call_str = {0}, fwd = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    { AValue __old = name; name = a_array_get(node, a_string("name")); a_release(__old); }
    { AValue __old = params; params = a_array_get(node, a_string("params")); a_release(__old); }
    { AValue __old = ret_type; ret_type = a_array_get(node, a_string("ret_type")); a_release(__old); }
    { AValue __old = c_ret; c_ret = fn_cgen__ffi_c_type(ret_type); a_release(__old); }
    { AValue __old = is_void; is_void = a_eq(c_ret, a_string("void")); a_release(__old); }
    { AValue __old = c_param_parts; c_param_parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = c_param_parts; c_param_parts = a_array_push(c_param_parts, fn_cgen__ffi_c_type(a_array_get(p, a_string("type")))); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = c_param_str; c_param_str = a_str_join(c_param_parts, a_string(", ")); a_release(__old); }
    if (a_truthy(a_eq(a_len(params), a_int(0)))) {
        { AValue __old = c_param_str; c_param_str = a_string("void"); a_release(__old); }
    }
    { AValue __old = raw_proto; raw_proto = a_str_concat(a_string("extern "), a_str_concat(c_ret, a_str_concat(a_string(" "), a_str_concat(name, a_str_concat(a_string("("), a_str_concat(c_param_str, a_string(");"))))))); a_release(__old); }
    { AValue __old = shim_params; shim_params = a_array_new(0); a_release(__old); }
    { AValue __old = pi; pi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(pi, a_len(params)))) {
        { AValue __old = shim_params; shim_params = a_array_push(shim_params, a_str_concat(a_string("AValue __p"), a_to_str(pi))); a_release(__old); }
        { AValue __old = pi; pi = a_add(pi, a_int(1)); a_release(__old); }
    }
    { AValue __old = shim_param_str; shim_param_str = a_str_join(shim_params, a_string(", ")); a_release(__old); }
    if (a_truthy(a_eq(a_len(params), a_int(0)))) {
        { AValue __old = shim_param_str; shim_param_str = a_string("void"); a_release(__old); }
    }
    { AValue __old = shim; shim = a_str_concat(a_string("AValue fn_"), a_str_concat(fn_cgen__mangle(name), a_str_concat(a_string("("), a_str_concat(shim_param_str, a_string(") {\n"))))); a_release(__old); }
    { AValue __old = call_args; call_args = a_array_new(0); a_release(__old); }
    { AValue __old = pi; pi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(pi, a_len(params)))) {
        { AValue __old = p; p = a_array_get(params, pi); a_release(__old); }
        { AValue __old = extracted; extracted = fn_cgen__ffi_extract(a_array_get(p, a_string("type")), a_str_concat(a_string("__p"), a_to_str(pi))); a_release(__old); }
        { AValue __old = call_args; call_args = a_array_push(call_args, extracted); a_release(__old); }
        { AValue __old = pi; pi = a_add(pi, a_int(1)); a_release(__old); }
    }
    { AValue __old = call_str; call_str = a_str_concat(name, a_str_concat(a_string("("), a_str_concat(a_str_join(call_args, a_string(", ")), a_string(")")))); a_release(__old); }
    if (a_truthy(is_void)) {
        { AValue __old = shim; shim = a_str_concat(shim, a_str_concat(a_string("    "), a_str_concat(call_str, a_string(";\n")))); a_release(__old); }
        { AValue __old = shim; shim = a_str_concat(shim, a_string("    return a_void();\n")); a_release(__old); }
    } else {
        { AValue __old = shim; shim = a_str_concat(shim, a_str_concat(a_string("    "), a_str_concat(c_ret, a_str_concat(a_string(" __result = "), a_str_concat(call_str, a_string(";\n")))))); a_release(__old); }
        { AValue __old = shim; shim = a_str_concat(shim, a_str_concat(a_string("    return "), a_str_concat(fn_cgen__ffi_wrap(ret_type, a_string("__result")), a_string(";\n")))); a_release(__old); }
    }
    { AValue __old = shim; shim = a_str_concat(shim, a_string("}\n")); a_release(__old); }
    { AValue __old = fwd; fwd = a_str_concat(a_string("AValue fn_"), a_str_concat(fn_cgen__mangle(name), a_str_concat(a_string("("), a_str_concat(shim_param_str, a_string(");"))))); a_release(__old); }
    __ret = a_array_new(3, raw_proto, fwd, shim); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    a_release(params);
    a_release(ret_type);
    a_release(c_ret);
    a_release(is_void);
    a_release(c_param_parts);
    a_release(c_param_str);
    a_release(raw_proto);
    a_release(shim_params);
    a_release(pi);
    a_release(shim_param_str);
    a_release(shim);
    a_release(call_args);
    a_release(p);
    a_release(extracted);
    a_release(call_str);
    a_release(fwd);
    a_release(node);
    return __ret;
}

AValue fn_cgen_emit_program(AValue prog_ast, AValue bm) {
    AValue items = {0}, all_fwd = {0}, all_fns = {0}, loaded = {0}, li = {0}, import_aliases = {0}, result = {0}, al = {0}, main_fn_names = {0}, extern_protos = {0}, extern_shims = {0}, parts = {0}, main_ctx = {0}, fr = {0}, out = {0}, lambda_fwd = {0}, i = {0};
    AValue __ret = a_void();
    prog_ast = a_retain(prog_ast);
    bm = a_retain(bm);
    { AValue __old = items; items = a_array_get(prog_ast, a_string("items")); a_release(__old); }
    { AValue __old = all_fwd; all_fwd = a_array_new(0); a_release(__old); }
    { AValue __old = all_fns; all_fns = a_array_new(0); a_release(__old); }
    { AValue __old = loaded; loaded = a_array_new(0); a_release(__old); }
    { AValue __old = li; li = a_int(0); a_release(__old); }
    { AValue __old = import_aliases; import_aliases = a_map_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, result = {0}, al = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("UseDecl")))) {
                { AValue __old = result; result = fn_cgen__load_module(a_array_get(item, a_string("path")), bm, loaded, li); a_release(__old); }
                { AValue __old = loaded; loaded = a_array_get(result, a_string("loaded")); a_release(__old); }
                { AValue __old = li; li = a_array_get(result, a_string("li")); a_release(__old); }
                { AValue __old = all_fwd; all_fwd = a_concat_arr(all_fwd, a_array_get(result, a_string("fwd"))); a_release(__old); }
                { AValue __old = all_fns; all_fns = a_concat_arr(all_fns, a_array_get(result, a_string("fns"))); a_release(__old); }
                { AValue __old = al; al = a_array_get(result, a_string("aliases")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(al), a_string("map")))) {
                    {
                        AValue __iter_arr = a_iterable(al);
                        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                            AValue __elem = {0}, k = {0}, v = {0};
                            __elem = a_array_get(__iter_arr, a_int(__fi));
                            k = a_array_get(__elem, a_int(0));
                            v = a_array_get(__elem, a_int(1));
                            { AValue __old = import_aliases; import_aliases = a_map_set(import_aliases, k, v); a_release(__old); }
                            a_release(__elem);
                            a_release(k);
                            a_release(v);
                        }
                        a_release(__iter_arr);
                    }
                }
            }
            a_release(item);
            a_release(result);
            a_release(al);
        }
        a_release(__iter_arr);
    }
    { AValue __old = main_fn_names; main_fn_names = fn_cgen__collect_fn_names(items); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(import_aliases);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue __elem = {0}, k = {0}, v = {0};
            __elem = a_array_get(__iter_arr, a_int(__fi));
            k = a_array_get(__elem, a_int(0));
            v = a_array_get(__elem, a_int(1));
            { AValue __old = main_fn_names; main_fn_names = a_array_push(main_fn_names, k); a_release(__old); }
            a_release(__elem);
            a_release(k);
            a_release(v);
        }
        a_release(__iter_arr);
    }
    { AValue __old = extern_protos; extern_protos = a_array_new(0); a_release(__old); }
    { AValue __old = extern_shims; extern_shims = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, parts = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("ExternFn")))) {
                { AValue __old = main_fn_names; main_fn_names = a_array_push(main_fn_names, a_array_get(item, a_string("name"))); a_release(__old); }
                { AValue __old = parts; parts = fn_cgen__emit_extern_fn(item); a_release(__old); }
                { AValue __old = extern_protos; extern_protos = a_array_push(extern_protos, a_array_get(parts, a_int(0))); a_release(__old); }
                { AValue __old = all_fwd; all_fwd = a_array_push(all_fwd, a_array_get(parts, a_int(1))); a_release(__old); }
                { AValue __old = extern_shims; extern_shims = a_array_push(extern_shims, a_array_get(parts, a_int(2))); a_release(__old); }
            }
            a_release(item);
            a_release(parts);
        }
        a_release(__iter_arr);
    }
    { AValue __old = main_ctx; main_ctx = a_map_new(3, "prefix", a_string(""), "fns", main_fn_names, "import_aliases", import_aliases); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, fr = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = all_fwd; all_fwd = a_array_push(all_fwd, fn_cgen__emit_fwd_decl(a_array_get(item, a_string("name")), a_array_get(item, a_string("params")))); a_release(__old); }
                { AValue __old = fr; fr = fn_cgen_emit_fn(item, bm, main_ctx, li); a_release(__old); }
                { AValue __old = li; li = a_array_get(fr, a_int(1)); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(a_array_get(fr, a_int(2)));
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue lf = {0};
                        lf = a_array_get(__iter_arr, a_int(__fi));
                        { AValue __old = all_fns; all_fns = a_array_push(all_fns, lf); a_release(__old); }
                        a_release(lf);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = all_fns; all_fns = a_array_push(all_fns, a_array_get(fr, a_int(0))); a_release(__old); }
            }
            a_release(item);
            a_release(fr);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_string("#include \"runtime.h\"\n#include <stdlib.h>\n\n"); a_release(__old); }
    if (a_truthy(a_gt(a_len(extern_protos), a_int(0)))) {
        { AValue __old = out; out = a_str_concat(out, a_string("/* FFI extern prototypes */\n")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(extern_protos);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = out; out = a_str_concat(out, a_str_concat(p, a_string("\n"))); a_release(__old); }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_string("\n")); a_release(__old); }
    }
    { AValue __old = lambda_fwd; lambda_fwd = a_array_new(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, li))) {
        { AValue __old = lambda_fwd; lambda_fwd = a_array_push(lambda_fwd, a_str_concat(a_string("AValue __lambda_"), a_str_concat(a_to_str(i), a_string("(AValue __env, int __argc, AValue* __argv);")))); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    if (a_truthy(a_gt(a_len(lambda_fwd), a_int(0)))) {
        { AValue __old = out; out = a_str_concat(out, a_string("/* Lambda forward declarations */\n")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(lambda_fwd);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue f = {0};
                f = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = out; out = a_str_concat(out, a_str_concat(f, a_string("\n"))); a_release(__old); }
                a_release(f);
            }
            a_release(__iter_arr);
        }
        { AValue __old = out; out = a_str_concat(out, a_string("\n")); a_release(__old); }
    }
    { AValue __old = out; out = a_str_concat(out, a_string("/* Forward declarations */\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(all_fwd);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue f = {0};
            f = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_str_concat(out, a_str_concat(f, a_string("\n"))); a_release(__old); }
            a_release(f);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_str_concat(out, a_string("\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(extern_shims);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_str_concat(out, a_str_concat(s, a_string("\n"))); a_release(__old); }
            a_release(s);
        }
        a_release(__iter_arr);
    }
    {
        AValue __iter_arr = a_iterable(all_fns);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue f = {0};
            f = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_str_concat(out, a_str_concat(f, a_string("\n"))); a_release(__old); }
            a_release(f);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_str_concat(out, a_string("int main(int argc, char** argv) {\n")); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_string("    g_argc = argc; g_argv = argv;\n")); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_string("    fn_main();\n")); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_string("    return 0;\n")); a_release(__old); }
    { AValue __old = out; out = a_str_concat(out, a_string("}\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(all_fwd);
    a_release(all_fns);
    a_release(loaded);
    a_release(li);
    a_release(import_aliases);
    a_release(result);
    a_release(al);
    a_release(main_fn_names);
    a_release(extern_protos);
    a_release(extern_shims);
    a_release(parts);
    a_release(main_ctx);
    a_release(fr);
    a_release(out);
    a_release(lambda_fwd);
    a_release(i);
    a_release(prog_ast);
    a_release(bm);
    return __ret;
}

AValue fn_cgen_main(void) {
    AValue file_args = {0}, source = {0}, prog_ast = {0}, bm = {0}, c_code = {0};
    AValue __ret = a_void();
    { AValue __old = file_args; file_args = a_args(); a_release(__old); }
    if (a_truthy(a_eq(a_len(file_args), a_int(0)))) {
        a_eprintln(a_string("usage: a run std/compiler/cgen.a -- <input.a>"));
        (exit((int)a_int(1).ival), a_void());
    }
    { AValue __old = source; source = a_io_read_file(a_array_get(file_args, a_int(0))); a_release(__old); }
    { AValue __old = prog_ast; prog_ast = fn_parser_parse(source); a_release(__old); }
    { AValue __old = bm; bm = fn_cgen__builtin_map(); a_release(__old); }
    { AValue __old = c_code; c_code = fn_cgen_emit_program(prog_ast, bm); a_release(__old); }
    a_print(c_code);
__fn_cleanup:
    a_release(file_args);
    a_release(source);
    a_release(prog_ast);
    a_release(bm);
    a_release(c_code);
    return __ret;
}

AValue fn_wasmgen__indent(AValue n) {
    AValue s = {0}, i = {0};
    AValue __ret = a_void();
    n = a_retain(n);
    { AValue __old = s; s = a_string(""); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = s; s = a_add(s, a_string("  ")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(s); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    a_release(i);
    a_release(n);
    return __ret;
}

AValue fn_wasmgen__mangle(AValue nm) {
    AValue __ret = a_void();
    nm = a_retain(nm);
    __ret = a_str_replace(a_str_replace(nm, a_string("."), a_string("_")), a_string("-"), a_string("_")); goto __fn_cleanup;
__fn_cleanup:
    a_release(nm);
    return __ret;
}

AValue fn_wasmgen__prefixed(AValue nm, AValue pfx) {
    AValue __ret = a_void();
    nm = a_retain(nm);
    pfx = a_retain(pfx);
    if (a_truthy(a_gt(a_len(pfx), a_int(0)))) {
        __ret = a_add(a_add(pfx, a_string("_")), nm); goto __fn_cleanup;
    }
    __ret = a_retain(nm); goto __fn_cleanup;
__fn_cleanup:
    a_release(nm);
    a_release(pfx);
    return __ret;
}

AValue fn_wasmgen__wat_type(AValue count) {
    AValue ps = {0}, i = {0};
    AValue __ret = a_void();
    count = a_retain(count);
    if (a_truthy(a_eq(count, a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = ps; ps = a_array_new(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, count))) {
        { AValue __old = ps; ps = a_array_push(ps, a_add(a_add(a_string("(param $p"), a_to_str(i)), a_string(" i64)"))); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_str_join(ps, a_string(" ")); goto __fn_cleanup;
__fn_cleanup:
    a_release(ps);
    a_release(i);
    a_release(count);
    return __ret;
}

AValue fn_wasmgen__collect_fns(AValue items) {
    AValue names = {0};
    AValue __ret = a_void();
    items = a_retain(items);
    { AValue __old = names; names = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = names; names = a_array_push(names, a_array_get(item, a_string("name"))); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(names); goto __fn_cleanup;
__fn_cleanup:
    a_release(names);
    a_release(items);
    return __ret;
}

AValue fn_wasmgen__collect_locals(AValue stmts) {
    AValue locals = {0}, stag = {0}, vn = {0}, then_s = {0}, sub = {0}, else_n = {0}, sub2 = {0}, w_stmts = {0}, sub3 = {0}, iter_var = {0}, f_stmts = {0}, sub4 = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    { AValue __old = locals; locals = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0}, stag = {0}, vn = {0}, then_s = {0}, sub = {0}, else_n = {0}, sub2 = {0}, w_stmts = {0}, sub3 = {0}, iter_var = {0}, f_stmts = {0}, sub4 = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = stag; stag = a_array_get(s, a_string("tag")); a_release(__old); }
            if (a_truthy(a_eq(stag, a_string("Let")))) {
                { AValue __old = vn; vn = a_array_get(s, a_string("name")); a_release(__old); }
                if (a_truthy(a_not(a_contains(locals, vn)))) {
                    { AValue __old = locals; locals = a_array_push(locals, vn); a_release(__old); }
                }
            }
            if (a_truthy(a_eq(stag, a_string("If")))) {
                if (a_truthy(a_map_has(s, a_string("then")))) {
                    { AValue __old = then_s; then_s = fn_wasmgen__unwrap_body(a_array_get(s, a_string("then"))); a_release(__old); }
                    { AValue __old = sub; sub = fn_wasmgen__collect_locals(then_s); a_release(__old); }
                    {
                        AValue __iter_arr = a_iterable(sub);
                        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                            AValue v = {0};
                            v = a_array_get(__iter_arr, a_int(__fi));
                            if (a_truthy(a_not(a_contains(locals, v)))) {
                                { AValue __old = locals; locals = a_array_push(locals, v); a_release(__old); }
                            }
                            a_release(v);
                        }
                        a_release(__iter_arr);
                    }
                }
                if (a_truthy(a_map_has(s, a_string("else")))) {
                    { AValue __old = else_n; else_n = a_array_get(s, a_string("else")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(else_n), a_string("map")), a_map_has(else_n, a_string("stmts"))))) {
                        { AValue __old = sub2; sub2 = fn_wasmgen__collect_locals(a_array_get(else_n, a_string("stmts"))); a_release(__old); }
                        {
                            AValue __iter_arr = a_iterable(sub2);
                            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                AValue v2 = {0};
                                v2 = a_array_get(__iter_arr, a_int(__fi));
                                if (a_truthy(a_not(a_contains(locals, v2)))) {
                                    { AValue __old = locals; locals = a_array_push(locals, v2); a_release(__old); }
                                }
                                a_release(v2);
                            }
                            a_release(__iter_arr);
                        }
                    }
                }
            }
            if (a_truthy(a_and(a_eq(stag, a_string("While")), a_map_has(s, a_string("body"))))) {
                { AValue __old = w_stmts; w_stmts = fn_wasmgen__unwrap_body(a_array_get(s, a_string("body"))); a_release(__old); }
                { AValue __old = sub3; sub3 = fn_wasmgen__collect_locals(w_stmts); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(sub3);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v3 = {0};
                        v3 = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_not(a_contains(locals, v3)))) {
                            { AValue __old = locals; locals = a_array_push(locals, v3); a_release(__old); }
                        }
                        a_release(v3);
                    }
                    a_release(__iter_arr);
                }
            }
            if (a_truthy(a_and(a_eq(stag, a_string("For")), a_map_has(s, a_string("body"))))) {
                { AValue __old = iter_var; iter_var = a_array_get(s, a_string("var")); a_release(__old); }
                if (a_truthy(a_not(a_contains(locals, iter_var)))) {
                    { AValue __old = locals; locals = a_array_push(locals, iter_var); a_release(__old); }
                }
                { AValue __old = f_stmts; f_stmts = fn_wasmgen__unwrap_body(a_array_get(s, a_string("body"))); a_release(__old); }
                { AValue __old = sub4; sub4 = fn_wasmgen__collect_locals(f_stmts); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(sub4);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue v4 = {0};
                        v4 = a_array_get(__iter_arr, a_int(__fi));
                        if (a_truthy(a_not(a_contains(locals, v4)))) {
                            { AValue __old = locals; locals = a_array_push(locals, v4); a_release(__old); }
                        }
                        a_release(v4);
                    }
                    a_release(__iter_arr);
                }
            }
            a_release(s);
            a_release(stag);
            a_release(vn);
            a_release(then_s);
            a_release(sub);
            a_release(else_n);
            a_release(sub2);
            a_release(w_stmts);
            a_release(sub3);
            a_release(iter_var);
            a_release(f_stmts);
            a_release(sub4);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(locals); goto __fn_cleanup;
__fn_cleanup:
    a_release(locals);
    a_release(stag);
    a_release(vn);
    a_release(then_s);
    a_release(sub);
    a_release(else_n);
    a_release(sub2);
    a_release(w_stmts);
    a_release(sub3);
    a_release(iter_var);
    a_release(f_stmts);
    a_release(sub4);
    a_release(stmts);
    return __ret;
}

AValue fn_wasmgen__builtin_map(void) {
    AValue m1 = {0}, m2 = {0}, m3 = {0}, m4 = {0};
    AValue __ret = a_void();
    { AValue __old = m1; m1 = a_map_new(21, "println", a_string("$rt_println"), "print", a_string("$rt_print"), "to_str", a_string("$rt_to_str"), "len", a_string("$rt_len"), "int", a_string("$rt_to_int"), "float", a_string("$rt_to_float"), "type_of", a_string("$rt_type_of"), "push", a_string("$rt_push"), "concat_arr", a_string("$rt_concat"), "Ok", a_string("$rt_ok"), "Err", a_string("$rt_err"), "unwrap", a_string("$rt_unwrap"), "unwrap_or", a_string("$rt_unwrap_or"), "is_ok", a_string("$rt_is_ok"), "is_err", a_string("$rt_is_err"), "fail", a_string("$rt_fail"), "exit", a_string("$rt_exit"), "exec", a_string("$rt_exec"), "from_code", a_string("$rt_from_code"), "char_code", a_string("$rt_char_code"), "contains", a_string("$rt_contains")); a_release(__old); }
    { AValue __old = m2; m2 = a_map_new(14, "str.concat", a_string("$rt_str_concat"), "str.len", a_string("$rt_str_len"), "str.slice", a_string("$rt_str_slice"), "str.find", a_string("$rt_str_find"), "str.contains", a_string("$rt_str_contains"), "str.replace", a_string("$rt_str_replace"), "str.split", a_string("$rt_str_split"), "str.trim", a_string("$rt_str_trim"), "str.starts_with", a_string("$rt_str_starts_with"), "str.ends_with", a_string("$rt_str_ends_with"), "str.join", a_string("$rt_str_join"), "str.lines", a_string("$rt_str_lines"), "str.upper", a_string("$rt_str_upper"), "str.lower", a_string("$rt_str_lower")); a_release(__old); }
    { AValue __old = m3; m3 = a_map_new(10, "map.get", a_string("$rt_map_get"), "map.set", a_string("$rt_map_set"), "map.has", a_string("$rt_map_has"), "map.delete", a_string("$rt_map_delete"), "map.keys", a_string("$rt_map_keys"), "map.values", a_string("$rt_map_values"), "map.merge", a_string("$rt_map_merge"), "json.parse", a_string("$rt_json_parse"), "json.stringify", a_string("$rt_json_stringify"), "json.pretty", a_string("$rt_json_pretty")); a_release(__old); }
    { AValue __old = m4; m4 = a_map_new(11, "io.read_file", a_string("$rt_read_file"), "io.write_file", a_string("$rt_write_file"), "fs.exists", a_string("$rt_fs_exists"), "fs.mkdir", a_string("$rt_fs_mkdir"), "fs.rm", a_string("$rt_fs_rm"), "fs.ls", a_string("$rt_fs_ls"), "time.now", a_string("$rt_time_now"), "hash.sha256", a_string("$rt_sha256"), "hash.md5", a_string("$rt_md5"), "env.get", a_string("$rt_env_get"), "env.set", a_string("$rt_env_set")); a_release(__old); }
    __ret = a_map_merge(a_map_merge(a_map_merge(m1, m2), m3), m4); goto __fn_cleanup;
__fn_cleanup:
    a_release(m1);
    a_release(m2);
    a_release(m3);
    a_release(m4);
    return __ret;
}

AValue fn_wasmgen__intern_string(AValue ctx, AValue s) {
    AValue strs = {0}, i = {0};
    AValue __ret = a_void();
    ctx = a_retain(ctx);
    s = a_retain(s);
    { AValue __old = strs; strs = a_array_get(ctx, a_string("strings")); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(strs)))) {
        if (a_truthy(a_eq(a_array_get(strs, i), s))) {
            __ret = a_retain(i); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    ctx = a_index_set(ctx, a_string("strings"), a_array_push(strs, s));
    __ret = a_len(strs); goto __fn_cleanup;
__fn_cleanup:
    a_release(strs);
    a_release(i);
    a_release(ctx);
    a_release(s);
    return __ret;
}

AValue fn_wasmgen_emit_expr(AValue node, AValue dep, AValue ctx) {
    AValue etag = {0}, raw = {0}, sv = {0}, idx = {0}, iname = {0}, op = {0}, lft = {0}, rgt = {0}, uop = {0}, operand = {0}, func_node = {0}, fname = {0}, ftag = {0}, fa_expr = {0}, cargs = {0}, bm = {0}, arg_strs = {0}, arg_wat = {0}, rt_name = {0}, dotpos = {0}, qual = {0}, obj = {0}, obj2 = {0}, fld = {0}, fi = {0}, elems = {0}, arr_s = {0}, pairs = {0}, ms = {0}, k_e = {0}, v_e = {0}, iparts = {0}, is = {0}, pe = {0}, ic = {0}, it = {0}, ie = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    dep = a_retain(dep);
    ctx = a_retain(ctx);
    { AValue __old = etag; etag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(etag, a_string("Int")))) {
        { AValue __old = raw; raw = a_array_get(node, a_string("value")); a_release(__old); }
        __ret = a_add(a_add(a_string("(call $rt_int (i64.const "), a_to_str(raw)), a_string("))")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Float")))) {
        __ret = a_add(a_add(a_string("(call $rt_float (f64.const "), a_to_str(a_array_get(node, a_string("value")))), a_string("))")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Bool")))) {
        if (a_truthy(a_array_get(node, a_string("value")))) {
            __ret = a_string("(call $rt_bool (i32.const 1))"); goto __fn_cleanup;
        }
        __ret = a_string("(call $rt_bool (i32.const 0))"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("String")))) {
        { AValue __old = sv; sv = a_array_get(node, a_string("value")); a_release(__old); }
        { AValue __old = idx; idx = fn_wasmgen__intern_string(ctx, sv); a_release(__old); }
        __ret = a_add(a_add(a_string("(call $rt_string (i32.const "), a_to_str(idx)), a_string("))")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Void")))) {
        __ret = a_string("(call $rt_void)"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Ident")))) {
        { AValue __old = iname; iname = a_array_get(node, a_string("name")); a_release(__old); }
        __ret = a_add(a_add(a_string("(local.get $"), fn_wasmgen__mangle(iname)), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("BinOp")))) {
        { AValue __old = op; op = a_array_get(node, a_string("op")); a_release(__old); }
        { AValue __old = lft; lft = fn_wasmgen_emit_expr(a_array_get(node, a_string("left")), dep, ctx); a_release(__old); }
        { AValue __old = rgt; rgt = fn_wasmgen_emit_expr(a_array_get(node, a_string("right")), dep, ctx); a_release(__old); }
        if (a_truthy(a_eq(op, a_string("+")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_add "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("-")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_sub "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("*")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_mul "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("/")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_div "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("%")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_mod "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("==")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_eq "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("!=")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_neq "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("<")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_lt "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string(">")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_gt "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("<=")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_lte "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string(">=")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_gte "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("&&")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_and "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(op, a_string("||")))) {
            __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_or "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
        }
        __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_add "), lft), a_string(" ")), rgt), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("UnaryOp")))) {
        { AValue __old = uop; uop = a_array_get(node, a_string("op")); a_release(__old); }
        { AValue __old = operand; operand = fn_wasmgen_emit_expr(a_array_get(node, a_string("expr")), dep, ctx); a_release(__old); }
        if (a_truthy(a_eq(uop, a_string("-")))) {
            __ret = a_add(a_add(a_string("(call $rt_neg "), operand), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(uop, a_string("!")))) {
            __ret = a_add(a_add(a_string("(call $rt_not "), operand), a_string(")")); goto __fn_cleanup;
        }
        __ret = a_retain(operand); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Call")))) {
        { AValue __old = func_node; func_node = a_array_get(node, a_string("func")); a_release(__old); }
        { AValue __old = fname; fname = a_string(""); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(func_node), a_string("map")))) {
            { AValue __old = ftag; ftag = a_array_get(func_node, a_string("tag")); a_release(__old); }
            if (a_truthy(a_eq(ftag, a_string("Ident")))) {
                { AValue __old = fname; fname = a_array_get(func_node, a_string("name")); a_release(__old); }
            }
            if (a_truthy(a_eq(ftag, a_string("FieldAccess")))) {
                { AValue __old = fa_expr; fa_expr = a_array_get(func_node, a_string("expr")); a_release(__old); }
                if (a_truthy(a_and(a_eq(a_type_of(fa_expr), a_string("map")), a_eq(a_array_get(fa_expr, a_string("tag")), a_string("Ident"))))) {
                    { AValue __old = fname; fname = a_add(a_add(a_array_get(fa_expr, a_string("name")), a_string(".")), a_array_get(func_node, a_string("field"))); a_release(__old); }
                }
            }
        }
        { AValue __old = cargs; cargs = a_array_get(node, a_string("args")); a_release(__old); }
        { AValue __old = bm; bm = a_array_get(ctx, a_string("builtins")); a_release(__old); }
        { AValue __old = arg_strs; arg_strs = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(cargs);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue ca = {0};
                ca = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = arg_strs; arg_strs = a_array_push(arg_strs, fn_wasmgen_emit_expr(ca, dep, ctx)); a_release(__old); }
                a_release(ca);
            }
            a_release(__iter_arr);
        }
        { AValue __old = arg_wat; arg_wat = a_str_join(arg_strs, a_string(" ")); a_release(__old); }
        if (a_truthy(a_map_has(bm, fname))) {
            { AValue __old = rt_name; rt_name = a_array_get(bm, fname); a_release(__old); }
            if (a_truthy(a_eq(a_len(arg_strs), a_int(0)))) {
                __ret = a_add(a_add(a_string("(call "), rt_name), a_string(")")); goto __fn_cleanup;
            }
            __ret = a_add(a_add(a_add(a_add(a_string("(call "), rt_name), a_string(" ")), arg_wat), a_string(")")); goto __fn_cleanup;
        }
        { AValue __old = dotpos; dotpos = a_str_find(fname, a_string(".")); a_release(__old); }
        if (a_truthy(a_gt(dotpos, a_int(0)))) {
            { AValue __old = qual; qual = a_str_replace(fname, a_string("."), a_string("_")); a_release(__old); }
            if (a_truthy(a_map_has(bm, fname))) {
                __ret = a_add(a_add(a_add(a_add(a_string("(call "), a_array_get(bm, fname)), a_string(" ")), arg_wat), a_string(")")); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(a_len(arg_strs), a_int(0)))) {
                __ret = a_add(a_add(a_string("(call $fn_"), fn_wasmgen__mangle(qual)), a_string(")")); goto __fn_cleanup;
            }
            __ret = a_add(a_add(a_add(a_add(a_string("(call $fn_"), fn_wasmgen__mangle(qual)), a_string(" ")), arg_wat), a_string(")")); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(a_len(arg_strs), a_int(0)))) {
            __ret = a_add(a_add(a_string("(call $fn_"), fn_wasmgen__mangle(fname)), a_string(")")); goto __fn_cleanup;
        }
        __ret = a_add(a_add(a_add(a_add(a_string("(call $fn_"), fn_wasmgen__mangle(fname)), a_string(" ")), arg_wat), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Index")))) {
        { AValue __old = obj; obj = fn_wasmgen_emit_expr(a_array_get(node, a_string("expr")), dep, ctx); a_release(__old); }
        { AValue __old = idx; idx = fn_wasmgen_emit_expr(a_array_get(node, a_string("index")), dep, ctx); a_release(__old); }
        __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_index "), obj), a_string(" ")), idx), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("FieldAccess")))) {
        { AValue __old = obj2; obj2 = fn_wasmgen_emit_expr(a_array_get(node, a_string("expr")), dep, ctx); a_release(__old); }
        { AValue __old = fld; fld = a_array_get(node, a_string("field")); a_release(__old); }
        { AValue __old = fi; fi = fn_wasmgen__intern_string(ctx, fld); a_release(__old); }
        __ret = a_add(a_add(a_add(a_add(a_string("(call $rt_field "), obj2), a_string(" (i32.const ")), a_to_str(fi)), a_string("))")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Array")))) {
        { AValue __old = elems; elems = a_array_get(node, a_string("elements")); a_release(__old); }
        if (a_truthy(a_eq(a_len(elems), a_int(0)))) {
            __ret = a_string("(call $rt_array_new)"); goto __fn_cleanup;
        }
        { AValue __old = arr_s; arr_s = a_string("(call $rt_array_new)"); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(elems);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue ae = {0};
                ae = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = arr_s; arr_s = a_add(a_add(a_add(a_add(a_string("(call $rt_push "), arr_s), a_string(" ")), fn_wasmgen_emit_expr(ae, dep, ctx)), a_string(")")); a_release(__old); }
                a_release(ae);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(arr_s); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("MapLiteral")))) {
        { AValue __old = pairs; pairs = a_array_get(node, a_string("pairs")); a_release(__old); }
        { AValue __old = ms; ms = a_string("(call $rt_map_new)"); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(pairs);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue mp = {0}, k_e = {0}, v_e = {0};
                mp = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = k_e; k_e = fn_wasmgen_emit_expr(a_array_get(mp, a_string("key")), dep, ctx); a_release(__old); }
                { AValue __old = v_e; v_e = fn_wasmgen_emit_expr(a_array_get(mp, a_string("value")), dep, ctx); a_release(__old); }
                { AValue __old = ms; ms = a_add(a_add(a_add(a_add(a_add(a_add(a_string("(call $rt_map_set "), ms), a_string(" ")), k_e), a_string(" ")), v_e), a_string(")")); a_release(__old); }
                a_release(mp);
                a_release(k_e);
                a_release(v_e);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(ms); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("Interpolation")))) {
        { AValue __old = iparts; iparts = a_array_get(node, a_string("parts")); a_release(__old); }
        { AValue __old = is; is = a_add(a_add(a_string("(call $rt_string (i32.const "), a_to_str(fn_wasmgen__intern_string(ctx, a_string("")))), a_string("))")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(iparts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue ip = {0}, pe = {0};
                ip = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = pe; pe = fn_wasmgen_emit_expr(ip, dep, ctx); a_release(__old); }
                { AValue __old = is; is = a_add(a_add(a_add(a_add(a_string("(call $rt_str_concat "), is), a_string(" (call $rt_to_str ")), pe), a_string("))")); a_release(__old); }
                a_release(ip);
                a_release(pe);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(is); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(etag, a_string("IfExpr")))) {
        { AValue __old = ic; ic = fn_wasmgen_emit_expr(a_array_get(node, a_string("cond")), dep, ctx); a_release(__old); }
        { AValue __old = it; it = fn_wasmgen_emit_expr(a_array_get(node, a_string("then")), dep, ctx); a_release(__old); }
        { AValue __old = ie; ie = fn_wasmgen_emit_expr(a_array_get(node, a_string("else")), dep, ctx); a_release(__old); }
        __ret = a_add(a_add(a_add(a_add(a_add(a_add(a_string("(if (result i64) (call $rt_truthy "), ic), a_string(") (then ")), it), a_string(") (else ")), ie), a_string("))")); goto __fn_cleanup;
    }
    __ret = a_add(a_string("(call $rt_void) ;; unsupported: "), etag); goto __fn_cleanup;
__fn_cleanup:
    a_release(etag);
    a_release(raw);
    a_release(sv);
    a_release(idx);
    a_release(iname);
    a_release(op);
    a_release(lft);
    a_release(rgt);
    a_release(uop);
    a_release(operand);
    a_release(func_node);
    a_release(fname);
    a_release(ftag);
    a_release(fa_expr);
    a_release(cargs);
    a_release(bm);
    a_release(arg_strs);
    a_release(arg_wat);
    a_release(rt_name);
    a_release(dotpos);
    a_release(qual);
    a_release(obj);
    a_release(obj2);
    a_release(fld);
    a_release(fi);
    a_release(elems);
    a_release(arr_s);
    a_release(pairs);
    a_release(ms);
    a_release(k_e);
    a_release(v_e);
    a_release(iparts);
    a_release(is);
    a_release(pe);
    a_release(ic);
    a_release(it);
    a_release(ie);
    a_release(node);
    a_release(dep);
    a_release(ctx);
    return __ret;
}

AValue fn_wasmgen_emit_stmt(AValue node, AValue dep, AValue ctx) {
    AValue stag = {0}, ind = {0}, e = {0}, vn = {0}, val = {0}, tgt = {0}, an = {0}, av = {0}, rv = {0}, cond_e = {0}, si = {0}, then_stmts = {0}, else_node = {0}, else_stmts = {0}, wc = {0}, sw = {0}, w_stmts = {0}, fi_expr = {0}, fvar = {0}, sf_for = {0}, f_stmts = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    dep = a_retain(dep);
    ctx = a_retain(ctx);
    { AValue __old = stag; stag = a_array_get(node, a_string("tag")); a_release(__old); }
    { AValue __old = ind; ind = fn_wasmgen__indent(dep); a_release(__old); }
    if (a_truthy(a_eq(stag, a_string("ExprStmt")))) {
        { AValue __old = e; e = fn_wasmgen_emit_expr(a_array_get(node, a_string("expr")), dep, ctx); a_release(__old); }
        __ret = a_add(a_add(a_add(ind, a_string("(drop ")), e), a_string(")\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("Let")))) {
        { AValue __old = vn; vn = fn_wasmgen__mangle(a_array_get(node, a_string("name"))); a_release(__old); }
        { AValue __old = val; val = fn_wasmgen_emit_expr(a_array_get(node, a_string("value")), dep, ctx); a_release(__old); }
        __ret = a_add(a_add(a_add(a_add(a_add(ind, a_string("(local.set $")), vn), a_string(" ")), val), a_string(")\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("Assign")))) {
        { AValue __old = tgt; tgt = a_array_get(node, a_string("target")); a_release(__old); }
        { AValue __old = an; an = a_string(""); a_release(__old); }
        if (a_truthy(a_and(a_eq(a_type_of(tgt), a_string("map")), a_eq(a_array_get(tgt, a_string("tag")), a_string("Ident"))))) {
            { AValue __old = an; an = fn_wasmgen__mangle(a_array_get(tgt, a_string("name"))); a_release(__old); }
        }
        if (a_truthy(a_eq(a_type_of(tgt), a_string("str")))) {
            { AValue __old = an; an = fn_wasmgen__mangle(tgt); a_release(__old); }
        }
        { AValue __old = av; av = fn_wasmgen_emit_expr(a_array_get(node, a_string("value")), dep, ctx); a_release(__old); }
        __ret = a_add(a_add(a_add(a_add(a_add(ind, a_string("(local.set $")), an), a_string(" ")), av), a_string(")\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("Return")))) {
        if (a_truthy(a_map_has(node, a_string("expr")))) {
            { AValue __old = rv; rv = fn_wasmgen_emit_expr(a_array_get(node, a_string("expr")), dep, ctx); a_release(__old); }
            __ret = a_add(a_add(a_add(ind, a_string("(return ")), rv), a_string(")\n")); goto __fn_cleanup;
        }
        __ret = a_add(ind, a_string("(return (call $rt_void))\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("If")))) {
        { AValue __old = cond_e; cond_e = fn_wasmgen_emit_expr(a_array_get(node, a_string("cond")), dep, ctx); a_release(__old); }
        { AValue __old = si; si = a_add(a_add(a_add(ind, a_string("(if (call $rt_truthy ")), cond_e), a_string(")\n")); a_release(__old); }
        { AValue __old = si; si = a_add(a_add(si, ind), a_string("  (then\n")); a_release(__old); }
        { AValue __old = then_stmts; then_stmts = fn_wasmgen__unwrap_body(a_array_get(node, a_string("then"))); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(then_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue tb = {0};
                tb = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = si; si = a_add(si, fn_wasmgen_emit_stmt(tb, a_add(dep, a_int(2)), ctx)); a_release(__old); }
                a_release(tb);
            }
            a_release(__iter_arr);
        }
        { AValue __old = si; si = a_add(a_add(si, ind), a_string("  )\n")); a_release(__old); }
        if (a_truthy(a_map_has(node, a_string("else")))) {
            { AValue __old = else_node; else_node = a_array_get(node, a_string("else")); a_release(__old); }
            if (a_truthy(a_and(a_eq(a_type_of(else_node), a_string("map")), a_map_has(else_node, a_string("stmts"))))) {
                { AValue __old = si; si = a_add(a_add(si, ind), a_string("  (else\n")); a_release(__old); }
                { AValue __old = else_stmts; else_stmts = a_array_get(else_node, a_string("stmts")); a_release(__old); }
                {
                    AValue __iter_arr = a_iterable(else_stmts);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue eb = {0};
                        eb = a_array_get(__iter_arr, a_int(__fi));
                        { AValue __old = si; si = a_add(si, fn_wasmgen_emit_stmt(eb, a_add(dep, a_int(2)), ctx)); a_release(__old); }
                        a_release(eb);
                    }
                    a_release(__iter_arr);
                }
                { AValue __old = si; si = a_add(a_add(si, ind), a_string("  )\n")); a_release(__old); }
            }
        }
        { AValue __old = si; si = a_add(a_add(si, ind), a_string(")\n")); a_release(__old); }
        __ret = a_retain(si); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("While")))) {
        { AValue __old = wc; wc = fn_wasmgen_emit_expr(a_array_get(node, a_string("cond")), dep, ctx); a_release(__old); }
        { AValue __old = sw; sw = a_add(ind, a_string("(block $break\n")); a_release(__old); }
        { AValue __old = sw; sw = a_add(a_add(sw, ind), a_string("  (loop $cont\n")); a_release(__old); }
        { AValue __old = sw; sw = a_add(a_add(a_add(a_add(sw, ind), a_string("    (br_if $break (i32.eqz (call $rt_truthy ")), wc), a_string(")))\n")); a_release(__old); }
        { AValue __old = w_stmts; w_stmts = fn_wasmgen__unwrap_body(a_array_get(node, a_string("body"))); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(w_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue wb = {0};
                wb = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sw; sw = a_add(sw, fn_wasmgen_emit_stmt(wb, a_add(dep, a_int(2)), ctx)); a_release(__old); }
                a_release(wb);
            }
            a_release(__iter_arr);
        }
        { AValue __old = sw; sw = a_add(a_add(sw, ind), a_string("    (br $cont)\n")); a_release(__old); }
        { AValue __old = sw; sw = a_add(a_add(sw, ind), a_string("  )\n")); a_release(__old); }
        { AValue __old = sw; sw = a_add(a_add(sw, ind), a_string(")\n")); a_release(__old); }
        __ret = a_retain(sw); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("For")))) {
        { AValue __old = fi_expr; fi_expr = fn_wasmgen_emit_expr(a_array_get(node, a_string("iter")), dep, ctx); a_release(__old); }
        { AValue __old = fvar; fvar = fn_wasmgen__mangle(a_array_get(node, a_string("var"))); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(a_add(ind, a_string("(local.set $__iter ")), fi_expr), a_string(")\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("(local.set $__fi (i32.const 0))\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("(block $break\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("  (loop $cont\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("    (br_if $break (i32.ge_s (local.get $__fi) (i32.wrap_i64 (call $rt_len (local.get $__iter)))))\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(a_add(a_add(sf_for, ind), a_string("    (local.set $")), fvar), a_string(" (call $rt_index (local.get $__iter) (call $rt_int (i64.extend_i32_s (local.get $__fi)))))\n")); a_release(__old); }
        { AValue __old = f_stmts; f_stmts = fn_wasmgen__unwrap_body(a_array_get(node, a_string("body"))); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(f_stmts);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue fb = {0};
                fb = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = sf_for; sf_for = a_add(sf_for, fn_wasmgen_emit_stmt(fb, a_add(dep, a_int(2)), ctx)); a_release(__old); }
                a_release(fb);
            }
            a_release(__iter_arr);
        }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("    (local.set $__fi (i32.add (local.get $__fi) (i32.const 1)))\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("    (br $cont)\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string("  )\n")); a_release(__old); }
        { AValue __old = sf_for; sf_for = a_add(a_add(sf_for, ind), a_string(")\n")); a_release(__old); }
        __ret = a_retain(sf_for); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("Break")))) {
        __ret = a_add(ind, a_string("(br $break)\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(stag, a_string("Continue")))) {
        __ret = a_add(ind, a_string("(br $cont)\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_map_has(node, a_string("expr")))) {
        __ret = a_add(a_add(a_add(ind, a_string("(drop ")), fn_wasmgen_emit_expr(a_array_get(node, a_string("expr")), dep, ctx)), a_string(")\n")); goto __fn_cleanup;
    }
    __ret = a_add(a_add(a_add(ind, a_string(";; unsupported stmt: ")), stag), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(stag);
    a_release(ind);
    a_release(e);
    a_release(vn);
    a_release(val);
    a_release(tgt);
    a_release(an);
    a_release(av);
    a_release(rv);
    a_release(cond_e);
    a_release(si);
    a_release(then_stmts);
    a_release(else_node);
    a_release(else_stmts);
    a_release(wc);
    a_release(sw);
    a_release(w_stmts);
    a_release(fi_expr);
    a_release(fvar);
    a_release(sf_for);
    a_release(f_stmts);
    a_release(node);
    a_release(dep);
    a_release(ctx);
    return __ret;
}

AValue fn_wasmgen__unwrap_body(AValue body_node) {
    AValue blk = {0};
    AValue __ret = a_void();
    body_node = a_retain(body_node);
    if (a_truthy(a_eq(a_type_of(body_node), a_string("array")))) {
        __ret = a_retain(body_node); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_type_of(body_node), a_string("map")))) {
        if (a_truthy(a_map_has(body_node, a_string("stmts")))) {
            __ret = a_array_get(body_node, a_string("stmts")); goto __fn_cleanup;
        }
        if (a_truthy(a_map_has(body_node, a_string("block")))) {
            { AValue __old = blk; blk = a_array_get(body_node, a_string("block")); a_release(__old); }
            if (a_truthy(a_map_has(blk, a_string("stmts")))) {
                __ret = a_array_get(blk, a_string("stmts")); goto __fn_cleanup;
            }
        }
    }
    __ret = a_array_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(blk);
    a_release(body_node);
    return __ret;
}

AValue fn_wasmgen_emit_fn(AValue node, AValue pfx, AValue ctx) {
    AValue fname = {0}, full = {0}, fparams = {0}, fbody = {0}, sf = {0}, pnames = {0}, pn = {0}, locs = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    pfx = a_retain(pfx);
    ctx = a_retain(ctx);
    { AValue __old = fname; fname = a_array_get(node, a_string("name")); a_release(__old); }
    { AValue __old = full; full = fn_wasmgen__prefixed(fname, pfx); a_release(__old); }
    { AValue __old = fparams; fparams = a_array_get(node, a_string("params")); a_release(__old); }
    { AValue __old = fbody; fbody = fn_wasmgen__unwrap_body(a_array_get(node, a_string("body"))); a_release(__old); }
    { AValue __old = sf; sf = a_add(a_add(a_string("  (func $fn_"), fn_wasmgen__mangle(full)), a_string(" ")); a_release(__old); }
    { AValue __old = pnames; pnames = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(fparams);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue fp = {0}, pn = {0};
            fp = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = pn; pn = a_array_get(fp, a_string("name")); a_release(__old); }
            { AValue __old = sf; sf = a_add(a_add(a_add(sf, a_string("(param $")), fn_wasmgen__mangle(pn)), a_string(" i64) ")); a_release(__old); }
            { AValue __old = pnames; pnames = a_array_push(pnames, pn); a_release(__old); }
            a_release(fp);
            a_release(pn);
        }
        a_release(__iter_arr);
    }
    { AValue __old = sf; sf = a_add(sf, a_string("(result i64)\n")); a_release(__old); }
    { AValue __old = locs; locs = fn_wasmgen__collect_locals(fbody); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(locs);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue lv = {0};
            lv = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_not(a_contains(pnames, lv)))) {
                { AValue __old = sf; sf = a_add(a_add(a_add(sf, a_string("    (local $")), fn_wasmgen__mangle(lv)), a_string(" i64)\n")); a_release(__old); }
            }
            a_release(lv);
        }
        a_release(__iter_arr);
    }
    { AValue __old = sf; sf = a_add(sf, a_string("    (local $__iter i64)\n")); a_release(__old); }
    { AValue __old = sf; sf = a_add(sf, a_string("    (local $__fi i32)\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(fbody);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue stmt = {0};
            stmt = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = sf; sf = a_add(sf, fn_wasmgen_emit_stmt(stmt, a_int(2), ctx)); a_release(__old); }
            a_release(stmt);
        }
        a_release(__iter_arr);
    }
    { AValue __old = sf; sf = a_add(sf, a_string("    (call $rt_void)\n")); a_release(__old); }
    { AValue __old = sf; sf = a_add(sf, a_string("  )\n")); a_release(__old); }
    __ret = a_retain(sf); goto __fn_cleanup;
__fn_cleanup:
    a_release(fname);
    a_release(full);
    a_release(fparams);
    a_release(fbody);
    a_release(sf);
    a_release(pnames);
    a_release(pn);
    a_release(locs);
    a_release(node);
    a_release(pfx);
    a_release(ctx);
    return __ret;
}

AValue fn_wasmgen__emit_imports(void) {
    AValue si = {0}, q = {0}, imports = {0}, iname = {0}, arity = {0}, pi = {0};
    AValue __ret = a_void();
    { AValue __old = si; si = a_string(""); a_release(__old); }
    { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
    { AValue __old = imports; imports = a_array_new(81, a_array_new(2, a_string("rt_println"), a_int(1)), a_array_new(2, a_string("rt_print"), a_int(1)), a_array_new(2, a_string("rt_to_str"), a_int(1)), a_array_new(2, a_string("rt_len"), a_int(1)), a_array_new(2, a_string("rt_to_int"), a_int(1)), a_array_new(2, a_string("rt_to_float"), a_int(1)), a_array_new(2, a_string("rt_type_of"), a_int(1)), a_array_new(2, a_string("rt_void"), a_int(0)), a_array_new(2, a_string("rt_int"), a_neg(a_int(5))), a_array_new(2, a_string("rt_bool"), a_neg(a_int(1))), a_array_new(2, a_string("rt_float"), a_neg(a_int(2))), a_array_new(2, a_string("rt_string"), a_neg(a_int(1))), a_array_new(2, a_string("rt_add"), a_int(2)), a_array_new(2, a_string("rt_sub"), a_int(2)), a_array_new(2, a_string("rt_mul"), a_int(2)), a_array_new(2, a_string("rt_div"), a_int(2)), a_array_new(2, a_string("rt_mod"), a_int(2)), a_array_new(2, a_string("rt_eq"), a_int(2)), a_array_new(2, a_string("rt_neq"), a_int(2)), a_array_new(2, a_string("rt_lt"), a_int(2)), a_array_new(2, a_string("rt_gt"), a_int(2)), a_array_new(2, a_string("rt_lte"), a_int(2)), a_array_new(2, a_string("rt_gte"), a_int(2)), a_array_new(2, a_string("rt_and"), a_int(2)), a_array_new(2, a_string("rt_or"), a_int(2)), a_array_new(2, a_string("rt_neg"), a_int(1)), a_array_new(2, a_string("rt_not"), a_int(1)), a_array_new(2, a_string("rt_truthy"), a_neg(a_int(3))), a_array_new(2, a_string("rt_push"), a_int(2)), a_array_new(2, a_string("rt_concat"), a_int(2)), a_array_new(2, a_string("rt_index"), a_int(2)), a_array_new(2, a_string("rt_field"), a_neg(a_int(4))), a_array_new(2, a_string("rt_array_new"), a_int(0)), a_array_new(2, a_string("rt_map_new"), a_int(0)), a_array_new(2, a_string("rt_map_set"), a_int(3)), a_array_new(2, a_string("rt_map_get"), a_int(2)), a_array_new(2, a_string("rt_map_has"), a_int(2)), a_array_new(2, a_string("rt_map_delete"), a_int(2)), a_array_new(2, a_string("rt_map_keys"), a_int(1)), a_array_new(2, a_string("rt_map_values"), a_int(1)), a_array_new(2, a_string("rt_map_merge"), a_int(2)), a_array_new(2, a_string("rt_contains"), a_int(2)), a_array_new(2, a_string("rt_str_concat"), a_int(2)), a_array_new(2, a_string("rt_str_len"), a_int(1)), a_array_new(2, a_string("rt_str_slice"), a_int(3)), a_array_new(2, a_string("rt_str_find"), a_int(2)), a_array_new(2, a_string("rt_str_contains"), a_int(2)), a_array_new(2, a_string("rt_str_replace"), a_int(3)), a_array_new(2, a_string("rt_str_split"), a_int(2)), a_array_new(2, a_string("rt_str_trim"), a_int(1)), a_array_new(2, a_string("rt_str_starts_with"), a_int(2)), a_array_new(2, a_string("rt_str_ends_with"), a_int(2)), a_array_new(2, a_string("rt_str_join"), a_int(2)), a_array_new(2, a_string("rt_str_lines"), a_int(1)), a_array_new(2, a_string("rt_str_upper"), a_int(1)), a_array_new(2, a_string("rt_str_lower"), a_int(1)), a_array_new(2, a_string("rt_json_parse"), a_int(1)), a_array_new(2, a_string("rt_json_stringify"), a_int(1)), a_array_new(2, a_string("rt_json_pretty"), a_int(1)), a_array_new(2, a_string("rt_read_file"), a_int(1)), a_array_new(2, a_string("rt_write_file"), a_int(2)), a_array_new(2, a_string("rt_fs_exists"), a_int(1)), a_array_new(2, a_string("rt_fs_mkdir"), a_int(1)), a_array_new(2, a_string("rt_fs_rm"), a_int(1)), a_array_new(2, a_string("rt_fs_ls"), a_int(1)), a_array_new(2, a_string("rt_time_now"), a_int(0)), a_array_new(2, a_string("rt_ok"), a_int(1)), a_array_new(2, a_string("rt_err"), a_int(1)), a_array_new(2, a_string("rt_unwrap"), a_int(1)), a_array_new(2, a_string("rt_unwrap_or"), a_int(2)), a_array_new(2, a_string("rt_is_ok"), a_int(1)), a_array_new(2, a_string("rt_is_err"), a_int(1)), a_array_new(2, a_string("rt_fail"), a_int(1)), a_array_new(2, a_string("rt_exit"), a_int(1)), a_array_new(2, a_string("rt_exec"), a_int(1)), a_array_new(2, a_string("rt_from_code"), a_int(1)), a_array_new(2, a_string("rt_char_code"), a_int(1)), a_array_new(2, a_string("rt_sha256"), a_int(1)), a_array_new(2, a_string("rt_md5"), a_int(1)), a_array_new(2, a_string("rt_env_get"), a_int(1)), a_array_new(2, a_string("rt_env_set"), a_int(2))); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(imports);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue imp = {0}, iname = {0}, arity = {0}, pi = {0};
            imp = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = iname; iname = a_array_get(imp, a_int(0)); a_release(__old); }
            { AValue __old = arity; arity = a_array_get(imp, a_int(1)); a_release(__old); }
            { AValue __old = si; si = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(si, a_string("  (import ")), q), a_string("env")), q), a_string(" ")), q), iname), q), a_string(" (func $")), iname); a_release(__old); }
            if (a_truthy(a_eq(arity, a_neg(a_int(1))))) {
                { AValue __old = si; si = a_add(si, a_string(" (param i32) (result i64)")); a_release(__old); }
            }
            if (a_truthy(a_eq(arity, a_neg(a_int(2))))) {
                { AValue __old = si; si = a_add(si, a_string(" (param f64) (result i64)")); a_release(__old); }
            }
            if (a_truthy(a_eq(arity, a_neg(a_int(3))))) {
                { AValue __old = si; si = a_add(si, a_string(" (param i64) (result i32)")); a_release(__old); }
            }
            if (a_truthy(a_eq(arity, a_neg(a_int(4))))) {
                { AValue __old = si; si = a_add(si, a_string(" (param i64 i32) (result i64)")); a_release(__old); }
            }
            if (a_truthy(a_eq(arity, a_neg(a_int(5))))) {
                { AValue __old = si; si = a_add(si, a_string(" (param i64) (result i64)")); a_release(__old); }
            }
            if (a_truthy(a_gteq(arity, a_int(0)))) {
                { AValue __old = pi; pi = a_int(0); a_release(__old); }
                while (a_truthy(a_lt(pi, arity))) {
                    { AValue __old = si; si = a_add(si, a_string(" (param i64)")); a_release(__old); }
                    { AValue __old = pi; pi = a_add(pi, a_int(1)); a_release(__old); }
                }
                { AValue __old = si; si = a_add(si, a_string(" (result i64)")); a_release(__old); }
            }
            { AValue __old = si; si = a_add(si, a_string("))\n")); a_release(__old); }
            a_release(imp);
            a_release(iname);
            a_release(arity);
            a_release(pi);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(si); goto __fn_cleanup;
__fn_cleanup:
    a_release(si);
    a_release(q);
    a_release(imports);
    a_release(iname);
    a_release(arity);
    a_release(pi);
    return __ret;
}

AValue fn_wasmgen__emit_data(AValue ctx) {
    AValue strs = {0}, sd = {0}, offset = {0}, i = {0}, bslash = {0}, sv = {0};
    AValue __ret = a_void();
    ctx = a_retain(ctx);
    { AValue __old = strs; strs = a_array_get(ctx, a_string("strings")); a_release(__old); }
    if (a_truthy(a_eq(a_len(strs), a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = sd; sd = a_string("  ;; string table\n"); a_release(__old); }
    { AValue __old = offset; offset = a_int(1024); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    { AValue __old = bslash; bslash = a_from_code(a_int(92)); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(strs)))) {
        { AValue __old = sv; sv = a_array_get(strs, i); a_release(__old); }
        { AValue __old = sd; sd = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(sd, a_string("  (data (i32.const ")), a_to_str(offset)), a_string(") ")), a_from_code(a_int(34))), sv), bslash), a_string("00")), a_from_code(a_int(34))), a_string(")\n")); a_release(__old); }
        { AValue __old = offset; offset = a_add(a_add(offset, a_len(sv)), a_int(1)); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(sd); goto __fn_cleanup;
__fn_cleanup:
    a_release(strs);
    a_release(sd);
    a_release(offset);
    a_release(i);
    a_release(bslash);
    a_release(sv);
    a_release(ctx);
    return __ret;
}

AValue fn_wasmgen_emit_program(AValue prog_ast) {
    AValue items = {0}, ctx = {0}, fns_wat = {0}, q = {0}, wat = {0};
    AValue __ret = a_void();
    prog_ast = a_retain(prog_ast);
    { AValue __old = items; items = a_array_get(prog_ast, a_string("items")); a_release(__old); }
    { AValue __old = ctx; ctx = a_map_new(3, "builtins", fn_wasmgen__builtin_map(), "strings", a_array_new(0), "prefix", a_string("")); a_release(__old); }
    { AValue __old = fns_wat; fns_wat = a_string(""); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = fns_wat; fns_wat = a_add(a_add(fns_wat, fn_wasmgen_emit_fn(item, a_string(""), ctx)), a_string("\n")); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
    { AValue __old = wat; wat = a_string("(module\n"); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, a_string("  ;; Memory: 1 page (64KB) minimum\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(a_add(a_add(a_add(a_add(wat, a_string("  (memory (export ")), q), a_string("memory")), q), a_string(") 1)\n\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, a_string("  ;; Runtime imports\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, fn_wasmgen__emit_imports()); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, a_string("\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, a_string("  ;; Functions\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, fns_wat); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, a_string("  ;; Entry point\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(a_add(a_add(a_add(a_add(wat, a_string("  (export ")), q), a_string("_start")), q), a_string(" (func $fn_main))\n\n")); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, fn_wasmgen__emit_data(ctx)); a_release(__old); }
    { AValue __old = wat; wat = a_add(wat, a_string(")\n")); a_release(__old); }
    __ret = a_retain(wat); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(ctx);
    a_release(fns_wat);
    a_release(q);
    a_release(wat);
    a_release(prog_ast);
    return __ret;
}

AValue fn_wasmgen_compile(AValue source) {
    AValue prog_ast = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    { AValue __old = prog_ast; prog_ast = fn_parser_parse(source); a_release(__old); }
    __ret = fn_wasmgen_emit_program(prog_ast); goto __fn_cleanup;
__fn_cleanup:
    a_release(prog_ast);
    a_release(source);
    return __ret;
}

AValue fn_emitter_emit(AValue ast) {
    AValue items = {0}, parts = {0};
    AValue __ret = a_void();
    ast = a_retain(ast);
    { AValue __old = items; items = a_array_get(ast, a_string("items")); a_release(__old); }
    { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_top_level(item, a_int(0))); a_release(__old); }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    __ret = a_str_join(parts, a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(parts);
    a_release(ast);
    return __ret;
}

AValue fn_emitter_is_present(AValue val) {
    AValue __ret = a_void();
    val = a_retain(val);
    if (a_truthy(a_eq(val, a_bool(0)))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_type_of(val), a_string("void")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_type_of(val), a_string("map")))) {
        if (a_truthy(a_eq(a_array_get(val, a_string("tag")), a_string("Void")))) {
            __ret = a_bool(0); goto __fn_cleanup;
        }
    }
    __ret = a_bool(1); goto __fn_cleanup;
__fn_cleanup:
    a_release(val);
    return __ret;
}

AValue fn_emitter_mk_indent(AValue level) {
    AValue pad = {0}, i = {0};
    AValue __ret = a_void();
    level = a_retain(level);
    { AValue __old = pad; pad = a_string(""); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, level))) {
        { AValue __old = pad; pad = a_add(pad, a_string("  ")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(pad); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(i);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_top_level(AValue node, AValue level) {
    AValue tag = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("FnDecl")))) {
        __ret = fn_emitter_emit_fn_decl(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TypeDecl")))) {
        __ret = fn_emitter_emit_type_decl(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ModDecl")))) {
        __ret = fn_emitter_emit_mod_decl(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("UseDecl")))) {
        __ret = fn_emitter_emit_use_decl(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ExternFn")))) {
        __ret = fn_emitter_emit_extern_fn(node, level); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_extern_fn(AValue node, AValue level) {
    AValue pad = {0}, out = {0}, params = {0}, param_parts = {0}, ret_type = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(pad, a_string("extern fn ")), a_array_get(node, a_string("name"))), a_string("(")); a_release(__old); }
    { AValue __old = params; params = a_array_get(node, a_string("params")); a_release(__old); }
    { AValue __old = param_parts; param_parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = param_parts; param_parts = a_array_push(param_parts, fn_emitter_emit_param(p)); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(out, a_str_join(param_parts, a_string(", "))), a_string(")")); a_release(__old); }
    { AValue __old = ret_type; ret_type = a_array_get(node, a_string("ret_type")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(ret_type))) {
        if (a_truthy(a_neq(a_array_get(ret_type, a_string("tag")), a_string("TyInfer")))) {
            { AValue __old = out; out = a_add(a_add(out, a_string(" -> ")), fn_emitter_emit_type(ret_type)); a_release(__old); }
        }
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(params);
    a_release(param_parts);
    a_release(ret_type);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_use_decl(AValue node, AValue level) {
    AValue path = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = path; path = a_array_get(node, a_string("path")); a_release(__old); }
    __ret = a_add(a_add(a_add(fn_emitter_mk_indent(level), a_string("use ")), a_str_join(path, a_string("."))), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(path);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_mod_decl(AValue node, AValue level) {
    AValue pad = {0}, lb = {0}, rb = {0}, out = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(pad, a_string("mod ")), a_array_get(node, a_string("name"))), a_string(" ")), lb), a_string("\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(node, a_string("items")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_add(a_add(out, fn_emitter_emit_top_level(item, a_add(level, a_int(1)))), a_string("\n")); a_release(__old); }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(a_add(out, pad), rb), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(lb);
    a_release(rb);
    a_release(out);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_type_decl(AValue node, AValue level) {
    AValue pad = {0}, out = {0}, params = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(pad, a_string("ty ")), a_array_get(node, a_string("name"))); a_release(__old); }
    { AValue __old = params; params = a_array_get(node, a_string("params")); a_release(__old); }
    if (a_truthy(a_gt(a_len(params), a_int(0)))) {
        { AValue __old = out; out = a_add(a_add(a_add(out, a_string("<")), a_str_join(params, a_string(", "))), a_string(">")); a_release(__old); }
    }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string(" = ")), fn_emitter_emit_type_body(a_array_get(node, a_string("body")))), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(params);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_type_body(AValue node) {
    AValue tag = {0}, parts = {0}, s = {0}, field_parts = {0}, out = {0}, w = {0}, lb = {0}, rb = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("TypeRecord")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("fields")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue f = {0};
                f = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, a_add(a_add(a_array_get(f, a_string("name")), a_string(": ")), fn_emitter_emit_type(a_array_get(f, a_string("type"))))); a_release(__old); }
                a_release(f);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("{ "), a_str_join(parts, a_string(", "))), a_string(" }")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TypeSum")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("variants")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0}, s = {0}, field_parts = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = s; s = a_array_get(v, a_string("name")); a_release(__old); }
                if (a_truthy(a_gt(a_len(a_array_get(v, a_string("fields"))), a_int(0)))) {
                    { AValue __old = field_parts; field_parts = a_array_new(0); a_release(__old); }
                    {
                        AValue __iter_arr = a_iterable(a_array_get(v, a_string("fields")));
                        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                            AValue f = {0};
                            f = a_array_get(__iter_arr, a_int(__fi));
                            { AValue __old = field_parts; field_parts = a_array_push(field_parts, fn_emitter_emit_type(f)); a_release(__old); }
                            a_release(f);
                        }
                        a_release(__iter_arr);
                    }
                    { AValue __old = s; s = a_add(a_add(a_add(s, a_string("(")), a_str_join(field_parts, a_string(", "))), a_string(")")); a_release(__old); }
                }
                { AValue __old = parts; parts = a_array_push(parts, s); a_release(__old); }
                a_release(v);
                a_release(s);
                a_release(field_parts);
            }
            a_release(__iter_arr);
        }
        __ret = a_str_join(parts, a_string(" | ")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TypeAlias")))) {
        { AValue __old = out; out = fn_emitter_emit_type(a_array_get(node, a_string("type"))); a_release(__old); }
        { AValue __old = w; w = a_array_get(node, a_string("where")); a_release(__old); }
        if (a_truthy(fn_emitter_is_present(w))) {
            { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
            { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
            { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(a_add(out, a_string(" where ")), lb), a_string(" ")), fn_emitter_emit_expr(w)), a_string(" ")), rb); a_release(__old); }
        }
        __ret = a_retain(out); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(parts);
    a_release(s);
    a_release(field_parts);
    a_release(out);
    a_release(w);
    a_release(lb);
    a_release(rb);
    a_release(node);
    return __ret;
}

AValue fn_emitter_emit_type(AValue node) {
    AValue tag = {0}, args = {0}, parts = {0}, param_parts = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    if (a_truthy(a_eq(a_type_of(node), a_string("void")))) {
        __ret = a_string("_"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(node, a_bool(0)))) {
        __ret = a_string("_"); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("TyInfer")))) {
        __ret = a_string("_"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyPrim")))) {
        __ret = a_array_get(node, a_string("name")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyNamed")))) {
        { AValue __old = args; args = a_array_get(node, a_string("args")); a_release(__old); }
        if (a_truthy(a_eq(a_len(args), a_int(0)))) {
            __ret = a_array_get(node, a_string("name")); goto __fn_cleanup;
        }
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(args);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue a = {0};
                a = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_type(a)); a_release(__old); }
                a_release(a);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_add(a_array_get(node, a_string("name")), a_string("<")), a_str_join(parts, a_string(", "))), a_string(">")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyArray")))) {
        __ret = a_add(a_add(a_string("["), fn_emitter_emit_type(a_array_get(node, a_string("elem")))), a_string("]")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyTuple")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("elems")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_type(e)); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("("), a_str_join(parts, a_string(", "))), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyRecord")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("fields")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue f = {0};
                f = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, a_add(a_add(a_array_get(f, a_string("name")), a_string(": ")), fn_emitter_emit_type(a_array_get(f, a_string("type"))))); a_release(__old); }
                a_release(f);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("{ "), a_str_join(parts, a_string(", "))), a_string(" }")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyFn")))) {
        { AValue __old = param_parts; param_parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("params")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = param_parts; param_parts = a_array_push(param_parts, fn_emitter_emit_type(p)); a_release(__old); }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_add(a_string("fn("), a_str_join(param_parts, a_string(", "))), a_string(") -> ")), fn_emitter_emit_type(a_array_get(node, a_string("ret")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TyMap")))) {
        __ret = a_add(a_add(a_add(a_add(a_string("#{"), fn_emitter_emit_type(a_array_get(node, a_string("key")))), a_string(": ")), fn_emitter_emit_type(a_array_get(node, a_string("val")))), a_string("}")); goto __fn_cleanup;
    }
    __ret = a_string("_"); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(args);
    a_release(parts);
    a_release(param_parts);
    a_release(node);
    return __ret;
}

AValue fn_emitter_emit_fn_decl(AValue node, AValue level) {
    AValue pad = {0}, out = {0}, params = {0}, param_parts = {0}, ret_type = {0}, effs = {0}, lb = {0}, rb = {0}, precond = {0}, postcond = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(pad, a_string("fn ")), a_array_get(node, a_string("name"))), a_string("(")); a_release(__old); }
    { AValue __old = params; params = a_array_get(node, a_string("params")); a_release(__old); }
    { AValue __old = param_parts; param_parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(params);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = param_parts; param_parts = a_array_push(param_parts, fn_emitter_emit_param(p)); a_release(__old); }
            a_release(p);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(out, a_str_join(param_parts, a_string(", "))), a_string(")")); a_release(__old); }
    { AValue __old = ret_type; ret_type = a_array_get(node, a_string("ret_type")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(ret_type))) {
        if (a_truthy(a_neq(a_array_get(ret_type, a_string("tag")), a_string("TyInfer")))) {
            { AValue __old = out; out = a_add(a_add(out, a_string(" -> ")), fn_emitter_emit_type(ret_type)); a_release(__old); }
        }
    }
    { AValue __old = effs; effs = a_array_get(node, a_string("effects")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(effs))) {
        if (a_truthy(a_gt(a_len(effs), a_int(0)))) {
            { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(out, a_string("\n")), fn_emitter_mk_indent(a_add(level, a_int(1)))), a_string("effects [")), a_str_join(effs, a_string(", "))), a_string("]")); a_release(__old); }
        }
    }
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = precond; precond = a_array_get(node, a_string("pre")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(precond))) {
        { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(out, a_string("\n")), fn_emitter_mk_indent(a_add(level, a_int(1)))), a_string("pre ")), lb), a_string(" ")), fn_emitter_emit_expr(precond)), a_string(" ")), rb); a_release(__old); }
    }
    { AValue __old = postcond; postcond = a_array_get(node, a_string("post")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(postcond))) {
        { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(out, a_string("\n")), fn_emitter_mk_indent(a_add(level, a_int(1)))), a_string("post ")), lb), a_string(" ")), fn_emitter_emit_expr(postcond)), a_string(" ")), rb); a_release(__old); }
    }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string("\n")), fn_emitter_emit_block(a_array_get(node, a_string("body")), level)), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(params);
    a_release(param_parts);
    a_release(ret_type);
    a_release(effs);
    a_release(lb);
    a_release(rb);
    a_release(precond);
    a_release(postcond);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_param(AValue p) {
    AValue pty = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    { AValue __old = pty; pty = a_array_get(p, a_string("type")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(pty), a_string("void")))) {
        __ret = a_array_get(p, a_string("name")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(pty, a_bool(0)))) {
        __ret = a_array_get(p, a_string("name")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_array_get(pty, a_string("tag")), a_string("TyInfer")))) {
        __ret = a_array_get(p, a_string("name")); goto __fn_cleanup;
    }
    __ret = a_add(a_add(a_array_get(p, a_string("name")), a_string(": ")), fn_emitter_emit_type(pty)); goto __fn_cleanup;
__fn_cleanup:
    a_release(pty);
    a_release(p);
    return __ret;
}

AValue fn_emitter_emit_block(AValue node, AValue level) {
    AValue pad = {0}, lb = {0}, rb = {0}, out = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(pad, lb), a_string("\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(node, a_string("stmts")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_add(out, fn_emitter_emit_stmt(s, a_add(level, a_int(1)))); a_release(__old); }
            a_release(s);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(out, pad), rb); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(lb);
    a_release(rb);
    a_release(out);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_stmt(AValue node, AValue level) {
    AValue tag = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Let")))) {
        __ret = fn_emitter_emit_let(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Assign")))) {
        __ret = fn_emitter_emit_assign(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Return")))) {
        __ret = fn_emitter_emit_return(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ExprStmt")))) {
        __ret = fn_emitter_emit_expr_stmt(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("If")))) {
        __ret = fn_emitter_emit_if_stmt(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Match")))) {
        __ret = fn_emitter_emit_match_stmt(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("For")))) {
        __ret = fn_emitter_emit_for_stmt(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("While")))) {
        __ret = fn_emitter_emit_while_stmt(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("LetDestructure")))) {
        __ret = fn_emitter_emit_let_destructure(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("ForDestructure")))) {
        __ret = fn_emitter_emit_for_destructure(node, level); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Break")))) {
        __ret = a_add(fn_emitter_mk_indent(level), a_string("break\n")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Continue")))) {
        __ret = a_add(fn_emitter_mk_indent(level), a_string("continue\n")); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_let(AValue node, AValue level) {
    AValue pad = {0}, out = {0}, lty = {0}, has_type = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(pad, a_string("let ")); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(node, a_string("mutable")), a_bool(1)))) {
        { AValue __old = out; out = a_add(out, a_string("mut ")); a_release(__old); }
    }
    { AValue __old = lty; lty = a_array_get(node, a_string("type")); a_release(__old); }
    { AValue __old = has_type; has_type = a_bool(0); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(lty))) {
        if (a_truthy(a_neq(a_array_get(lty, a_string("tag")), a_string("TyInfer")))) {
            { AValue __old = has_type; has_type = a_bool(1); a_release(__old); }
        }
    }
    if (a_truthy(has_type)) {
        { AValue __old = out; out = a_add(a_add(a_add(a_add(out, a_array_get(node, a_string("name"))), a_string(": ")), fn_emitter_emit_type(lty)), a_string(" = ")); a_release(__old); }
    } else {
        { AValue __old = out; out = a_add(a_add(out, a_array_get(node, a_string("name"))), a_string(" = ")); a_release(__old); }
    }
    { AValue __old = out; out = a_add(a_add(out, fn_emitter_emit_expr(a_array_get(node, a_string("value")))), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(lty);
    a_release(has_type);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_assign(AValue node, AValue level) {
    AValue pad = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    __ret = a_add(a_add(a_add(a_add(pad, fn_emitter_emit_expr(a_array_get(node, a_string("target")))), a_string(" = ")), fn_emitter_emit_expr(a_array_get(node, a_string("value")))), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_return(AValue node, AValue level) {
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    __ret = a_add(a_add(a_add(fn_emitter_mk_indent(level), a_string("ret ")), fn_emitter_emit_expr(a_array_get(node, a_string("expr")))), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_expr_stmt(AValue node, AValue level) {
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    __ret = a_add(a_add(fn_emitter_mk_indent(level), fn_emitter_emit_expr(a_array_get(node, a_string("expr")))), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_if_stmt(AValue node, AValue level) {
    AValue pad = {0}, out = {0}, els = {0}, els_tag = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(pad, a_string("if ")), fn_emitter_emit_expr(a_array_get(node, a_string("cond")))), a_string(" \n")); a_release(__old); }
    { AValue __old = out; out = a_add(out, fn_emitter_emit_block(a_array_get(node, a_string("then")), level)); a_release(__old); }
    { AValue __old = els; els = a_array_get(node, a_string("else")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(els))) {
        { AValue __old = els_tag; els_tag = a_array_get(els, a_string("tag")); a_release(__old); }
        if (a_truthy(a_eq(els_tag, a_string("ElseBlock")))) {
            { AValue __old = out; out = a_add(out, a_string(" else \n")); a_release(__old); }
            { AValue __old = out; out = a_add(out, fn_emitter_emit_block(a_array_get(els, a_string("block")), level)); a_release(__old); }
            { AValue __old = out; out = a_add(out, a_string("\n")); a_release(__old); }
            __ret = a_retain(out); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(els_tag, a_string("ElseIf")))) {
            { AValue __old = out; out = a_add(out, a_string(" else ")); a_release(__old); }
            __ret = a_add(out, fn_emitter_emit_if_stmt(a_array_get(els, a_string("stmt")), level)); goto __fn_cleanup;
        }
    }
    { AValue __old = out; out = a_add(out, a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(els);
    a_release(els_tag);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_match_stmt(AValue node, AValue level) {
    AValue pad = {0}, lb = {0}, rb = {0}, out = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(pad, a_string("match ")), fn_emitter_emit_expr(a_array_get(node, a_string("expr")))), a_string(" ")), lb), a_string("\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(node, a_string("arms")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue arm = {0};
            arm = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_add(out, fn_emitter_emit_match_arm(arm, a_add(level, a_int(1)))); a_release(__old); }
            a_release(arm);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(a_add(out, pad), rb), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(lb);
    a_release(rb);
    a_release(out);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_match_arm(AValue arm, AValue level) {
    AValue pad = {0}, out = {0}, guard = {0}, body = {0};
    AValue __ret = a_void();
    arm = a_retain(arm);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(pad, fn_emitter_emit_pattern(a_array_get(arm, a_string("pattern")))); a_release(__old); }
    { AValue __old = guard; guard = a_array_get(arm, a_string("guard")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(guard))) {
        { AValue __old = out; out = a_add(a_add(out, a_string(" if ")), fn_emitter_emit_expr(guard)); a_release(__old); }
    }
    { AValue __old = out; out = a_add(out, a_string(" => ")); a_release(__old); }
    { AValue __old = body; body = a_array_get(arm, a_string("body")); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(body, a_string("tag")), a_string("Block")))) {
        { AValue __old = out; out = a_add(a_add(a_add(out, a_string("\n")), fn_emitter_emit_block(body, level)), a_string("\n")); a_release(__old); }
    } else {
        { AValue __old = out; out = a_add(a_add(out, fn_emitter_emit_expr(body)), a_string("\n")); a_release(__old); }
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(guard);
    a_release(body);
    a_release(arm);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_for_stmt(AValue node, AValue level) {
    AValue pad = {0}, fty = {0}, has_type = {0}, out = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = fty; fty = a_array_get(node, a_string("type")); a_release(__old); }
    { AValue __old = has_type; has_type = a_bool(0); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(fty))) {
        if (a_truthy(a_neq(a_array_get(fty, a_string("tag")), a_string("TyInfer")))) {
            { AValue __old = has_type; has_type = a_bool(1); a_release(__old); }
        }
    }
    { AValue __old = out; out = a_retain(pad); a_release(__old); }
    if (a_truthy(has_type)) {
        { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(out, a_string("for ")), a_array_get(node, a_string("var"))), a_string(": ")), fn_emitter_emit_type(fty)), a_string(" in ")); a_release(__old); }
    } else {
        { AValue __old = out; out = a_add(a_add(a_add(out, a_string("for ")), a_array_get(node, a_string("var"))), a_string(" in ")); a_release(__old); }
    }
    { AValue __old = out; out = a_add(a_add(out, fn_emitter_emit_expr(a_array_get(node, a_string("iter")))), a_string("\n")); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(out, fn_emitter_emit_block(a_array_get(node, a_string("body")), level)), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(fty);
    a_release(has_type);
    a_release(out);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_while_stmt(AValue node, AValue level) {
    AValue pad = {0}, out = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(pad, a_string("while ")), fn_emitter_emit_expr(a_array_get(node, a_string("cond")))), a_string("\n")); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(out, fn_emitter_emit_block(a_array_get(node, a_string("body")), level)), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(out);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_let_destructure(AValue node, AValue level) {
    AValue pad = {0}, parts = {0}, rest = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(node, a_string("bindings")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue b = {0};
            b = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(b, a_bool(0)))) {
                { AValue __old = parts; parts = a_array_push(parts, a_string("_")); a_release(__old); }
            } else {
                if (a_truthy(a_eq(a_type_of(b), a_string("void")))) {
                    { AValue __old = parts; parts = a_array_push(parts, a_string("_")); a_release(__old); }
                } else {
                    { AValue __old = parts; parts = a_array_push(parts, b); a_release(__old); }
                }
            }
            a_release(b);
        }
        a_release(__iter_arr);
    }
    { AValue __old = rest; rest = a_array_get(node, a_string("rest")); a_release(__old); }
    if (a_truthy(fn_emitter_is_present(rest))) {
        { AValue __old = parts; parts = a_array_push(parts, a_add(a_string("..."), rest)); a_release(__old); }
    }
    __ret = a_add(a_add(a_add(a_add(a_add(pad, a_string("let [")), a_str_join(parts, a_string(", "))), a_string("] = ")), fn_emitter_emit_expr(a_array_get(node, a_string("value")))), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(parts);
    a_release(rest);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_for_destructure(AValue node, AValue level) {
    AValue pad = {0}, lb = {0}, rb = {0}, parts = {0}, out = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    level = a_retain(level);
    { AValue __old = pad; pad = fn_emitter_mk_indent(level); a_release(__old); }
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(node, a_string("bindings")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue b = {0};
            b = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = parts; parts = a_array_push(parts, b); a_release(__old); }
            a_release(b);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(a_add(a_add(pad, a_string("for [")), a_str_join(parts, a_string(", "))), a_string("] in ")), fn_emitter_emit_expr(a_array_get(node, a_string("iter")))), a_string(" ")), lb), a_string("\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(a_array_get(a_array_get(node, a_string("body")), a_string("stmts")));
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue s = {0};
            s = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_add(out, fn_emitter_emit_stmt(s, a_add(level, a_int(1)))); a_release(__old); }
            a_release(s);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(a_add(out, pad), rb), a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(pad);
    a_release(lb);
    a_release(rb);
    a_release(parts);
    a_release(out);
    a_release(node);
    a_release(level);
    return __ret;
}

AValue fn_emitter_emit_expr(AValue node) {
    AValue tag = {0}, arg_parts = {0}, parts = {0}, param_parts = {0}, lbrace = {0}, rbrace = {0}, dq = {0}, out = {0}, lb = {0}, rb = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    if (a_truthy(a_eq(a_type_of(node), a_string("void")))) {
        __ret = a_string("void"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(node, a_bool(0)))) {
        __ret = a_string("false"); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Int")))) {
        __ret = a_to_str(a_array_get(node, a_string("value"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Float")))) {
        __ret = a_to_str(a_array_get(node, a_string("value"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("String")))) {
        __ret = a_add(a_add(a_string("\""), fn_emitter_escape_str(a_array_get(node, a_string("value")))), a_string("\"")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Bool")))) {
        if (a_truthy(a_array_get(node, a_string("value")))) {
            __ret = a_string("true"); goto __fn_cleanup;
        }
        __ret = a_string("false"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Void")))) {
        __ret = a_string("void"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Ident")))) {
        __ret = a_array_get(node, a_string("name")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("BinOp")))) {
        __ret = a_add(a_add(a_add(a_add(a_add(a_add(a_string("("), fn_emitter_emit_expr(a_array_get(node, a_string("left")))), a_string(" ")), a_array_get(node, a_string("op"))), a_string(" ")), fn_emitter_emit_expr(a_array_get(node, a_string("right")))), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("UnaryOp")))) {
        __ret = a_add(a_array_get(node, a_string("op")), fn_emitter_emit_expr(a_array_get(node, a_string("expr")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Call")))) {
        { AValue __old = arg_parts; arg_parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("args")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue a = {0};
                a = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = arg_parts; arg_parts = a_array_push(arg_parts, fn_emitter_emit_expr(a)); a_release(__old); }
                a_release(a);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_add(fn_emitter_emit_expr(a_array_get(node, a_string("func"))), a_string("(")), a_str_join(arg_parts, a_string(", "))), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("FieldAccess")))) {
        __ret = a_add(a_add(fn_emitter_emit_expr(a_array_get(node, a_string("expr"))), a_string(".")), a_array_get(node, a_string("field"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Index")))) {
        __ret = a_add(a_add(a_add(fn_emitter_emit_expr(a_array_get(node, a_string("expr"))), a_string("[")), fn_emitter_emit_expr(a_array_get(node, a_string("index")))), a_string("]")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Try")))) {
        __ret = a_add(fn_emitter_emit_expr(a_array_get(node, a_string("expr"))), a_string("?")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("TryBlock")))) {
        __ret = a_add(a_string("try "), fn_emitter_emit_block(a_array_get(node, a_string("block")), a_int(0))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Array")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("elems")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue e = {0};
                e = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_expr(e)); a_release(__old); }
                a_release(e);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("["), a_str_join(parts, a_string(", "))), a_string("]")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Record")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("fields")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue f = {0};
                f = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, a_add(a_add(a_array_get(f, a_string("name")), a_string(": ")), fn_emitter_emit_expr(a_array_get(f, a_string("value"))))); a_release(__old); }
                a_release(f);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("{ "), a_str_join(parts, a_string(", "))), a_string(" }")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Lambda")))) {
        { AValue __old = param_parts; param_parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("params")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue p = {0};
                p = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = param_parts; param_parts = a_array_push(param_parts, fn_emitter_emit_param(p)); a_release(__old); }
                a_release(p);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_add(a_string("fn("), a_str_join(param_parts, a_string(", "))), a_string(") => ")), fn_emitter_emit_expr(a_array_get(node, a_string("body")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Pipe")))) {
        __ret = a_add(a_add(fn_emitter_emit_expr(a_array_get(node, a_string("left"))), a_string(" |> ")), fn_emitter_emit_expr(a_array_get(node, a_string("right")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Interpolation")))) {
        { AValue __old = lbrace; lbrace = a_from_code(a_int(123)); a_release(__old); }
        { AValue __old = rbrace; rbrace = a_from_code(a_int(125)); a_release(__old); }
        { AValue __old = dq; dq = a_from_code(a_int(34)); a_release(__old); }
        { AValue __old = out; out = a_retain(dq); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("parts")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue part = {0};
                part = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(part, a_string("tag")), a_string("InterpLit")))) {
                    { AValue __old = out; out = a_add(out, fn_emitter_escape_str(a_array_get(part, a_string("value")))); a_release(__old); }
                }
                if (a_truthy(a_eq(a_array_get(part, a_string("tag")), a_string("InterpExpr")))) {
                    { AValue __old = out; out = a_add(a_add(a_add(out, lbrace), fn_emitter_emit_expr(a_array_get(part, a_string("expr")))), rbrace); a_release(__old); }
                }
                a_release(part);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(out, dq); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("MapLiteral")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("entries")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue entry = {0};
                entry = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, a_add(a_add(fn_emitter_emit_expr(a_array_get(entry, a_string("key"))), a_string(": ")), fn_emitter_emit_expr(a_array_get(entry, a_string("value"))))); a_release(__old); }
                a_release(entry);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("#{"), a_str_join(parts, a_string(", "))), a_string("}")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Spread")))) {
        __ret = a_add(a_string("..."), fn_emitter_emit_expr(a_array_get(node, a_string("expr")))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("IfExpr")))) {
        __ret = a_add(a_add(a_add(a_add(a_add(a_string("if "), fn_emitter_emit_expr(a_array_get(node, a_string("cond")))), a_string(" ")), fn_emitter_emit_block(a_array_get(node, a_string("then")), a_int(0))), a_string(" else ")), fn_emitter_emit_block(a_array_get(node, a_string("else")), a_int(0))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("MatchExpr")))) {
        { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
        { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
        { AValue __old = out; out = a_add(a_add(a_add(a_add(a_string("match "), fn_emitter_emit_expr(a_array_get(node, a_string("expr")))), a_string(" ")), lb), a_string("\n")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("arms")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue arm = {0};
                arm = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = out; out = a_add(out, fn_emitter_emit_match_arm(arm, a_int(1))); a_release(__old); }
                a_release(arm);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(out, rb); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("BlockExpr")))) {
        __ret = fn_emitter_emit_block(a_array_get(node, a_string("block")), a_int(0)); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(arg_parts);
    a_release(parts);
    a_release(param_parts);
    a_release(lbrace);
    a_release(rbrace);
    a_release(dq);
    a_release(out);
    a_release(lb);
    a_release(rb);
    a_release(node);
    return __ret;
}

AValue fn_emitter_emit_pattern(AValue node) {
    AValue tag = {0}, args = {0}, parts = {0}, dq = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    if (a_truthy(a_eq(a_type_of(node), a_string("void")))) {
        __ret = a_string("_"); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("PatWildcard")))) {
        __ret = a_string("_"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatIdent")))) {
        __ret = a_array_get(node, a_string("name")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatLiteral")))) {
        __ret = fn_emitter_emit_pat_literal(a_array_get(node, a_string("value"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatConstructor")))) {
        { AValue __old = args; args = a_array_get(node, a_string("args")); a_release(__old); }
        if (a_truthy(a_eq(a_len(args), a_int(0)))) {
            __ret = a_array_get(node, a_string("name")); goto __fn_cleanup;
        }
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(args);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue a = {0};
                a = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_pattern(a)); a_release(__old); }
                a_release(a);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_add(a_array_get(node, a_string("name")), a_string("(")), a_str_join(parts, a_string(", "))), a_string(")")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatArray")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("elems")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue elem = {0};
                elem = a_array_get(__iter_arr, a_int(__fi));
                if (a_truthy(a_eq(a_array_get(elem, a_string("tag")), a_string("PatRest")))) {
                    { AValue __old = parts; parts = a_array_push(parts, a_add(a_string("..."), a_array_get(elem, a_string("name")))); a_release(__old); }
                } else {
                    if (a_truthy(a_eq(a_array_get(elem, a_string("tag")), a_string("PatElem")))) {
                        { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_pattern(a_array_get(elem, a_string("pattern")))); a_release(__old); }
                    } else {
                        { AValue __old = parts; parts = a_array_push(parts, fn_emitter_emit_pattern(elem)); a_release(__old); }
                    }
                }
                a_release(elem);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("["), a_str_join(parts, a_string(", "))), a_string("]")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("PatMap")))) {
        { AValue __old = dq; dq = a_from_code(a_int(34)); a_release(__old); }
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(a_array_get(node, a_string("entries")));
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue entry = {0};
                entry = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, a_add(a_add(a_add(a_add(dq, a_array_get(entry, a_string("key"))), dq), a_string(": ")), fn_emitter_emit_pattern(a_array_get(entry, a_string("pattern"))))); a_release(__old); }
                a_release(entry);
            }
            a_release(__iter_arr);
        }
        __ret = a_add(a_add(a_string("#{"), a_str_join(parts, a_string(", "))), a_string("}")); goto __fn_cleanup;
    }
    __ret = a_string("_"); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(args);
    a_release(parts);
    a_release(dq);
    a_release(node);
    return __ret;
}

AValue fn_emitter_emit_pat_literal(AValue lit) {
    AValue tag = {0};
    AValue __ret = a_void();
    lit = a_retain(lit);
    if (a_truthy(a_eq(a_type_of(lit), a_string("int")))) {
        __ret = a_to_str(lit); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_type_of(lit), a_string("float")))) {
        __ret = a_to_str(lit); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_type_of(lit), a_string("bool")))) {
        if (a_truthy(lit)) {
            __ret = a_string("true"); goto __fn_cleanup;
        }
        __ret = a_string("false"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_type_of(lit), a_string("str")))) {
        __ret = a_add(a_add(a_string("\""), fn_emitter_escape_str(lit)), a_string("\"")); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(lit, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Int")))) {
        __ret = a_to_str(a_array_get(lit, a_string("value"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Float")))) {
        __ret = a_to_str(a_array_get(lit, a_string("value"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("String")))) {
        __ret = a_add(a_add(a_string("\""), fn_emitter_escape_str(a_array_get(lit, a_string("value")))), a_string("\"")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Bool")))) {
        if (a_truthy(a_array_get(lit, a_string("value")))) {
            __ret = a_string("true"); goto __fn_cleanup;
        }
        __ret = a_string("false"); goto __fn_cleanup;
    }
    __ret = a_to_str(lit); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(lit);
    return __ret;
}

AValue fn_emitter_escape_str(AValue s) {
    AValue out = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = out; out = a_str_replace(s, a_string("\\"), a_string("\\\\")); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, a_string("\""), a_string("\\\"")); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, a_string("\n"), a_string("\\n")); a_release(__old); }
    { AValue __old = out; out = a_str_replace(out, a_string("\t"), a_string("\\t")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(out);
    a_release(s);
    return __ret;
}

AValue fn_checker__builtin_arity(void) {
    AValue __ret = a_void();
    __ret = a_map_new(125, "println", a_int(1), "print", a_int(1), "eprintln", a_int(1), "eprint", a_int(1), "len", a_int(1), "push", a_int(2), "pop", a_int(1), "sort", a_int(1), "contains", a_int(2), "slice", a_int(3), "drop", a_int(2), "to_str", a_int(1), "int", a_int(1), "float", a_int(1), "type_of", a_int(1), "is_alpha", a_int(1), "is_digit", a_int(1), "is_alnum", a_int(1), "char_code", a_int(1), "from_code", a_int(1), "range", a_int(2), "exit", a_int(1), "fail", a_int(1), "assert", a_int(1), "error", a_int(1), "Ok", a_int(1), "Err", a_int(1), "is_ok", a_int(1), "is_err", a_int(1), "unwrap", a_int(1), "Some", a_int(1), "is_none", a_int(1), "str.concat", a_int(2), "str.split", a_int(2), "str.join", a_int(2), "str.contains", a_int(2), "str.starts_with", a_int(2), "str.ends_with", a_int(2), "str.replace", a_int(3), "str.trim", a_int(1), "str.upper", a_int(1), "str.lower", a_int(1), "str.chars", a_int(1), "str.lines", a_int(1), "str.slice", a_int(3), "str.find", a_int(2), "str.count", a_int(2), "str.repeat", a_int(2), "json.parse", a_int(1), "json.stringify", a_int(1), "json.pretty", a_int(1), "map.get", a_int(2), "map.set", a_int(3), "map.has", a_int(2), "map.keys", a_int(1), "map.values", a_int(1), "map.entries", a_int(1), "map.remove", a_int(2), "map.from_entries", a_int(1), "io.read_file", a_int(1), "io.write_file", a_int(2), "io.read_line", a_int(0), "io.read_stdin", a_int(0), "io.flush", a_int(0), "fs.exists", a_int(1), "fs.ls", a_int(1), "fs.mkdir", a_int(1), "fs.rm", a_int(1), "fs.cp", a_int(2), "fs.cwd", a_int(0), "http.get", a_int(1), "http.post", a_int(2), "http.put", a_int(2), "http.delete", a_int(1), "http.patch", a_int(2), "http.serve", a_int(2), "http.stream", a_int(2), "http.stream_read", a_int(1), "http.stream_close", a_int(1), "db.open", a_int(1), "db.exec", a_int(2), "db.query", a_int(2), "db.close", a_int(1), "env.get", a_int(1), "env.set", a_int(2), "time.now", a_int(0), "time.sleep", a_int(1), "datetime.now", a_int(0), "datetime.iso", a_int(1), "hash.sha256", a_int(1), "hash.md5", a_int(1), "exec", a_int(1), "argv0", a_int(0), "args", a_int(0), "uuid.v4", a_int(0), "proc.spawn", a_int(1), "proc.write", a_int(2), "proc.read_line", a_int(1), "proc.kill", a_int(1), "proc.wait", a_int(1), "proc.is_running", a_int(1), "ws.connect", a_int(1), "ws.send", a_int(2), "ws.recv", a_int(1), "ws.close", a_int(1), "signal.on", a_int(2), "compress.deflate", a_int(1), "compress.inflate", a_int(1), "compress.gzip", a_int(1), "compress.gunzip", a_int(1), "image.load", a_int(1), "image.decode", a_int(1), "image.save", a_int(2), "image.encode", a_int(2), "image.width", a_int(1), "image.height", a_int(1), "image.resize", a_int(3), "image.pixels", a_int(1), "regex.is_match", a_int(2), "regex.find", a_int(2), "regex.find_all", a_int(2), "regex.replace", a_int(3), "regex.replace_all", a_int(3), "regex.split", a_int(2), "regex.captures", a_int(2)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_checker__is_builtin_or_prefix(AValue name, AValue arity_map) {
    AValue prefixes = {0}, i = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    arity_map = a_retain(arity_map);
    if (a_truthy(a_map_has(arity_map, name))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    { AValue __old = prefixes; prefixes = a_array_new(21, a_string("str."), a_string("json."), a_string("map."), a_string("io."), a_string("fs."), a_string("http."), a_string("db."), a_string("env."), a_string("time."), a_string("datetime."), a_string("hash."), a_string("proc."), a_string("ws."), a_string("signal."), a_string("compress."), a_string("image."), a_string("regex."), a_string("math."), a_string("path."), a_string("toml."), a_string("config.")); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(prefixes)))) {
        if (a_truthy(a_str_starts_with(name, a_array_get(prefixes, i)))) {
            __ret = a_bool(1); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(prefixes);
    a_release(i);
    a_release(name);
    a_release(arity_map);
    return __ret;
}

AValue fn_checker_check(AValue ast) {
    AValue arity_map = {0}, diags = {0}, items = {0}, scope = {0}, fi = {0}, item = {0}, params = {0}, p = {0};
    AValue __ret = a_void();
    ast = a_retain(ast);
    { AValue __old = arity_map; arity_map = fn_checker__builtin_arity(); a_release(__old); }
    { AValue __old = diags; diags = a_array_new(0); a_release(__old); }
    { AValue __old = items; items = a_array_get(ast, a_string("items")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(items), a_string("array")))) {
        __ret = a_retain(diags); goto __fn_cleanup;
    }
    { AValue __old = scope; scope = a_map_new(0); a_release(__old); }
    { AValue __old = fi; fi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(fi, a_len(items)))) {
        { AValue __old = item; item = a_array_get(items, fi); a_release(__old); }
        if (a_truthy(a_and(a_eq(a_type_of(item), a_string("map")), a_map_has(item, a_string("tag"))))) {
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = scope; scope = a_map_set(scope, a_array_get(item, a_string("name")), a_bool(1)); a_release(__old); }
                { AValue __old = params; params = a_array_get(item, a_string("params")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(params), a_string("array")))) {
                    { AValue __old = arity_map; arity_map = a_map_set(arity_map, a_array_get(item, a_string("name")), a_len(params)); a_release(__old); }
                }
            }
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("UseDecl")))) {
                { AValue __old = p; p = a_array_get(item, a_string("path")); a_release(__old); }
                if (a_truthy(a_and(a_eq(a_type_of(p), a_string("array")), a_gt(a_len(p), a_int(0))))) {
                    { AValue __old = scope; scope = a_map_set(scope, a_array_get(p, a_sub(a_len(p), a_int(1))), a_bool(1)); a_release(__old); }
                }
            }
        }
        { AValue __old = fi; fi = a_add(fi, a_int(1)); a_release(__old); }
    }
    { AValue __old = fi; fi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(fi, a_len(items)))) {
        { AValue __old = item; item = a_array_get(items, fi); a_release(__old); }
        if (a_truthy(a_and(a_and(a_eq(a_type_of(item), a_string("map")), a_map_has(item, a_string("tag"))), a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl"))))) {
            { AValue __old = diags; diags = fn_checker__check_fn_body(item, scope, arity_map, diags); a_release(__old); }
        }
        { AValue __old = fi; fi = a_add(fi, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(diags); goto __fn_cleanup;
__fn_cleanup:
    a_release(arity_map);
    a_release(diags);
    a_release(items);
    a_release(scope);
    a_release(fi);
    a_release(item);
    a_release(params);
    a_release(p);
    a_release(ast);
    return __ret;
}

AValue fn_checker__check_fn_body(AValue fn_node, AValue scope, AValue arity_map, AValue diags) {
    AValue fn_scope = {0}, params = {0}, pi = {0}, p = {0}, body = {0}, stmts = {0}, bindings = {0}, bkeys = {0}, bi = {0}, ident_set = {0}, calls = {0}, ident_keys = {0}, ii = {0}, name = {0}, ci = {0}, c = {0}, fn_name = {0}, nargs = {0}, expected = {0}, found_ret = {0}, si = {0}, s = {0}, ref_set = {0}, vname = {0};
    AValue __ret = a_void();
    fn_node = a_retain(fn_node);
    scope = a_retain(scope);
    arity_map = a_retain(arity_map);
    diags = a_retain(diags);
    { AValue __old = fn_scope; fn_scope = a_retain(scope); a_release(__old); }
    { AValue __old = params; params = a_array_get(fn_node, a_string("params")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(params), a_string("array")))) {
        { AValue __old = pi; pi = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(pi, a_len(params)))) {
            { AValue __old = p; p = a_array_get(params, pi); a_release(__old); }
            if (a_truthy(a_and(a_eq(a_type_of(p), a_string("map")), a_map_has(p, a_string("name"))))) {
                { AValue __old = fn_scope; fn_scope = a_map_set(fn_scope, a_array_get(p, a_string("name")), a_bool(1)); a_release(__old); }
            }
            { AValue __old = pi; pi = a_add(pi, a_int(1)); a_release(__old); }
        }
    }
    { AValue __old = body; body = a_array_get(fn_node, a_string("body")); a_release(__old); }
    if (a_truthy(a_or(a_neq(a_type_of(body), a_string("map")), a_neq(a_array_get(body, a_string("tag")), a_string("Block"))))) {
        __ret = a_retain(diags); goto __fn_cleanup;
    }
    { AValue __old = stmts; stmts = a_array_get(body, a_string("stmts")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(stmts), a_string("array")))) {
        __ret = a_retain(diags); goto __fn_cleanup;
    }
    { AValue __old = bindings; bindings = fn_checker__collect_all_lets(stmts); a_release(__old); }
    { AValue __old = bkeys; bkeys = a_map_keys(bindings); a_release(__old); }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, a_len(bkeys)))) {
        { AValue __old = fn_scope; fn_scope = a_map_set(fn_scope, a_array_get(bkeys, bi), a_bool(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = ident_set; ident_set = fn_checker__collect_all_idents_block(stmts, a_map_new(0)); a_release(__old); }
    { AValue __old = calls; calls = fn_checker__collect_all_calls(stmts); a_release(__old); }
    { AValue __old = ident_keys; ident_keys = a_map_keys(ident_set); a_release(__old); }
    { AValue __old = ii; ii = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(ii, a_len(ident_keys)))) {
        { AValue __old = name; name = a_array_get(ident_keys, ii); a_release(__old); }
        if (a_truthy(a_and(a_not(a_map_has(fn_scope, name)), a_not(fn_checker__is_builtin_or_prefix(name, arity_map))))) {
            { AValue __old = diags; diags = a_array_push(diags, a_map_new(3, "severity", a_string("error"), "line", a_int(0), "msg", a_add(a_string("undefined variable: "), name))); a_release(__old); }
        }
        { AValue __old = ii; ii = a_add(ii, a_int(1)); a_release(__old); }
    }
    { AValue __old = ci; ci = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(ci, a_len(calls)))) {
        { AValue __old = c; c = a_array_get(calls, ci); a_release(__old); }
        { AValue __old = fn_name; fn_name = a_array_get(c, a_string("name")); a_release(__old); }
        { AValue __old = nargs; nargs = a_array_get(c, a_string("nargs")); a_release(__old); }
        if (a_truthy(a_map_has(arity_map, fn_name))) {
            { AValue __old = expected; expected = a_map_get(arity_map, fn_name); a_release(__old); }
            if (a_truthy(a_neq(nargs, expected))) {
                { AValue __old = diags; diags = a_array_push(diags, a_map_new(3, "severity", a_string("error"), "line", a_int(0), "msg", a_add(a_add(a_add(a_add(fn_name, a_string(" expects ")), a_to_str(expected)), a_string(" argument(s), got ")), a_to_str(nargs)))); a_release(__old); }
            }
        }
        { AValue __old = ci; ci = a_add(ci, a_int(1)); a_release(__old); }
    }
    { AValue __old = found_ret; found_ret = a_bool(0); a_release(__old); }
    { AValue __old = si; si = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(si, a_len(stmts)))) {
        if (a_truthy(found_ret)) {
            { AValue __old = diags; diags = a_array_push(diags, a_map_new(3, "severity", a_string("warning"), "line", a_int(0), "msg", a_string("unreachable code after return"))); a_release(__old); }
            break;
        }
        { AValue __old = s; s = a_array_get(stmts, si); a_release(__old); }
        if (a_truthy(a_and(a_and(a_eq(a_type_of(s), a_string("map")), a_map_has(s, a_string("tag"))), a_eq(a_array_get(s, a_string("tag")), a_string("Return"))))) {
            { AValue __old = found_ret; found_ret = a_bool(1); a_release(__old); }
        }
        { AValue __old = si; si = a_add(si, a_int(1)); a_release(__old); }
    }
    { AValue __old = ref_set; ref_set = a_retain(ident_set); a_release(__old); }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, a_len(bkeys)))) {
        { AValue __old = vname; vname = a_array_get(bkeys, bi); a_release(__old); }
        if (a_truthy(a_and(a_not(a_str_starts_with(vname, a_string("_"))), a_not(a_map_has(ref_set, vname))))) {
            { AValue __old = diags; diags = a_array_push(diags, a_map_new(3, "severity", a_string("warning"), "line", a_int(0), "msg", a_add(a_string("unused variable: "), vname))); a_release(__old); }
        }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(diags); goto __fn_cleanup;
__fn_cleanup:
    a_release(fn_scope);
    a_release(params);
    a_release(pi);
    a_release(p);
    a_release(body);
    a_release(stmts);
    a_release(bindings);
    a_release(bkeys);
    a_release(bi);
    a_release(ident_set);
    a_release(calls);
    a_release(ident_keys);
    a_release(ii);
    a_release(name);
    a_release(ci);
    a_release(c);
    a_release(fn_name);
    a_release(nargs);
    a_release(expected);
    a_release(found_ret);
    a_release(si);
    a_release(s);
    a_release(ref_set);
    a_release(vname);
    a_release(fn_node);
    a_release(scope);
    a_release(arity_map);
    a_release(diags);
    return __ret;
}

AValue fn_checker__collect_all_lets(AValue stmts) {
    AValue names = {0}, i = {0}, s = {0}, tag = {0}, n = {0}, b = {0}, inner = {0}, sub = {0}, sk = {0}, j = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    { AValue __old = names; names = a_map_new(0); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(stmts), a_string("array")))) {
        __ret = a_retain(names); goto __fn_cleanup;
    }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(stmts)))) {
        { AValue __old = s; s = a_array_get(stmts, i); a_release(__old); }
        if (a_truthy(a_and(a_eq(a_type_of(s), a_string("map")), a_map_has(s, a_string("tag"))))) {
            { AValue __old = tag; tag = a_array_get(s, a_string("tag")); a_release(__old); }
            if (a_truthy(a_and(a_eq(tag, a_string("Let")), a_map_has(s, a_string("name"))))) {
                { AValue __old = n; n = a_array_get(s, a_string("name")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(n), a_string("str")))) {
                    { AValue __old = names; names = a_map_set(names, n, a_bool(1)); a_release(__old); }
                }
            }
            if (a_truthy(a_and(a_eq(tag, a_string("For")), a_map_has(s, a_string("binding"))))) {
                { AValue __old = b; b = a_array_get(s, a_string("binding")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(b), a_string("str")))) {
                    { AValue __old = names; names = a_map_set(names, b, a_bool(1)); a_release(__old); }
                }
                if (a_truthy(a_and(a_eq(a_type_of(b), a_string("map")), a_map_has(b, a_string("name"))))) {
                    { AValue __old = names; names = a_map_set(names, a_array_get(b, a_string("name")), a_bool(1)); a_release(__old); }
                }
            }
            if (a_truthy(a_or(a_eq(tag, a_string("For")), a_eq(tag, a_string("While"))))) {
                if (a_truthy(a_map_has(s, a_string("body")))) {
                    { AValue __old = inner; inner = a_array_get(s, a_string("body")); a_release(__old); }
                    if (a_truthy(a_and(a_and(a_eq(a_type_of(inner), a_string("map")), a_map_has(inner, a_string("tag"))), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = sub; sub = fn_checker__collect_all_lets(a_array_get(inner, a_string("stmts"))); a_release(__old); }
                        { AValue __old = sk; sk = a_map_keys(sub); a_release(__old); }
                        { AValue __old = j; j = a_int(0); a_release(__old); }
                        while (a_truthy(a_lt(j, a_len(sk)))) {
                            { AValue __old = names; names = a_map_set(names, a_array_get(sk, j), a_bool(1)); a_release(__old); }
                            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
                        }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("If")))) {
                if (a_truthy(a_map_has(s, a_string("then")))) {
                    { AValue __old = inner; inner = a_array_get(s, a_string("then")); a_release(__old); }
                    if (a_truthy(a_and(a_and(a_eq(a_type_of(inner), a_string("map")), a_map_has(inner, a_string("tag"))), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = sub; sub = fn_checker__collect_all_lets(a_array_get(inner, a_string("stmts"))); a_release(__old); }
                        { AValue __old = sk; sk = a_map_keys(sub); a_release(__old); }
                        { AValue __old = j; j = a_int(0); a_release(__old); }
                        while (a_truthy(a_lt(j, a_len(sk)))) {
                            { AValue __old = names; names = a_map_set(names, a_array_get(sk, j), a_bool(1)); a_release(__old); }
                            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
                        }
                    }
                }
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(names); goto __fn_cleanup;
__fn_cleanup:
    a_release(names);
    a_release(i);
    a_release(s);
    a_release(tag);
    a_release(n);
    a_release(b);
    a_release(inner);
    a_release(sub);
    a_release(sk);
    a_release(j);
    a_release(stmts);
    return __ret;
}

AValue fn_checker__collect_idents_expr(AValue node, AValue s) {
    AValue tag = {0}, args = {0}, i = {0}, elems = {0}, parts = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    s = a_retain(s);
    if (a_truthy(a_neq(a_type_of(node), a_string("map")))) {
        __ret = a_retain(s); goto __fn_cleanup;
    }
    if (a_truthy(a_not(a_map_has(node, a_string("tag"))))) {
        __ret = a_retain(s); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Ident")))) {
        __ret = a_map_set(s, a_array_get(node, a_string("name")), a_bool(1)); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Call")))) {
        { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(node, a_string("func")), s); a_release(__old); }
        { AValue __old = args; args = a_array_get(node, a_string("args")); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(args), a_string("array")))) {
            { AValue __old = i; i = a_int(0); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(args)))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(args, i), s); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        __ret = a_retain(s); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_eq(tag, a_string("BinOp")), a_eq(tag, a_string("BinaryOp"))))) {
        { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(node, a_string("left")), s); a_release(__old); }
        __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("right")), s); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_or(a_eq(tag, a_string("UnaryOp")), a_eq(tag, a_string("Not"))), a_eq(tag, a_string("Neg"))))) {
        if (a_truthy(a_map_has(node, a_string("operand")))) {
            __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("operand")), s); goto __fn_cleanup;
        }
        if (a_truthy(a_map_has(node, a_string("expr")))) {
            __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("expr")), s); goto __fn_cleanup;
        }
        __ret = a_retain(s); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Index")))) {
        { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(node, a_string("object")), s); a_release(__old); }
        __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("index")), s); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("FieldAccess")))) {
        __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("object")), s); goto __fn_cleanup;
    }
    if (a_truthy(a_and(a_eq(tag, a_string("Array")), a_map_has(node, a_string("elements"))))) {
        { AValue __old = elems; elems = a_array_get(node, a_string("elements")); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(elems), a_string("array")))) {
            { AValue __old = i; i = a_int(0); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(elems)))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(elems, i), s); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        __ret = a_retain(s); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Pipe")))) {
        { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(node, a_string("left")), s); a_release(__old); }
        __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("right")), s); goto __fn_cleanup;
    }
    if (a_truthy(a_and(a_eq(tag, a_string("Interpolation")), a_map_has(node, a_string("parts"))))) {
        { AValue __old = parts; parts = a_array_get(node, a_string("parts")); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(parts), a_string("array")))) {
            { AValue __old = i; i = a_int(0); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(parts)))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(parts, i), s); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        __ret = a_retain(s); goto __fn_cleanup;
    }
    if (a_truthy(a_and(a_eq(tag, a_string("Try")), a_map_has(node, a_string("expr"))))) {
        __ret = fn_checker__collect_idents_expr(a_array_get(node, a_string("expr")), s); goto __fn_cleanup;
    }
    __ret = a_retain(s); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(args);
    a_release(i);
    a_release(elems);
    a_release(parts);
    a_release(node);
    a_release(s);
    return __ret;
}

AValue fn_checker__collect_all_idents_block(AValue stmts, AValue s) {
    AValue i = {0}, st = {0}, tag = {0}, inner = {0}, el = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    s = a_retain(s);
    if (a_truthy(a_neq(a_type_of(stmts), a_string("array")))) {
        __ret = a_retain(s); goto __fn_cleanup;
    }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(stmts)))) {
        { AValue __old = st; st = a_array_get(stmts, i); a_release(__old); }
        if (a_truthy(a_and(a_eq(a_type_of(st), a_string("map")), a_map_has(st, a_string("tag"))))) {
            { AValue __old = tag; tag = a_array_get(st, a_string("tag")); a_release(__old); }
            if (a_truthy(a_and(a_and(a_eq(tag, a_string("Let")), a_map_has(st, a_string("value"))), a_neq(a_type_of(a_array_get(st, a_string("value"))), a_string("void"))))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("value")), s); a_release(__old); }
            }
            if (a_truthy(a_eq(tag, a_string("Assign")))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("target")), s); a_release(__old); }
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("value")), s); a_release(__old); }
            }
            if (a_truthy(a_and(a_and(a_eq(tag, a_string("Return")), a_map_has(st, a_string("value"))), a_neq(a_type_of(a_array_get(st, a_string("value"))), a_string("void"))))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("value")), s); a_release(__old); }
            }
            if (a_truthy(a_eq(tag, a_string("ExprStmt")))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("expr")), s); a_release(__old); }
            }
            if (a_truthy(a_eq(tag, a_string("If")))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("condition")), s); a_release(__old); }
                if (a_truthy(a_map_has(st, a_string("then")))) {
                    { AValue __old = inner; inner = a_array_get(st, a_string("then")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(inner), a_string("map")), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = s; s = fn_checker__collect_all_idents_block(a_array_get(inner, a_string("stmts")), s); a_release(__old); }
                    }
                }
                if (a_truthy(a_and(a_map_has(st, a_string("else")), a_neq(a_type_of(a_array_get(st, a_string("else"))), a_string("void"))))) {
                    { AValue __old = el; el = a_array_get(st, a_string("else")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(el), a_string("map")), a_eq(a_array_get(el, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = s; s = fn_checker__collect_all_idents_block(a_array_get(el, a_string("stmts")), s); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("For")))) {
                if (a_truthy(a_map_has(st, a_string("iterable")))) {
                    { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("iterable")), s); a_release(__old); }
                }
                if (a_truthy(a_map_has(st, a_string("body")))) {
                    { AValue __old = inner; inner = a_array_get(st, a_string("body")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(inner), a_string("map")), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = s; s = fn_checker__collect_all_idents_block(a_array_get(inner, a_string("stmts")), s); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("While")))) {
                { AValue __old = s; s = fn_checker__collect_idents_expr(a_array_get(st, a_string("condition")), s); a_release(__old); }
                if (a_truthy(a_map_has(st, a_string("body")))) {
                    { AValue __old = inner; inner = a_array_get(st, a_string("body")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(inner), a_string("map")), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = s; s = fn_checker__collect_all_idents_block(a_array_get(inner, a_string("stmts")), s); a_release(__old); }
                    }
                }
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(s); goto __fn_cleanup;
__fn_cleanup:
    a_release(i);
    a_release(st);
    a_release(tag);
    a_release(inner);
    a_release(el);
    a_release(stmts);
    a_release(s);
    return __ret;
}

AValue fn_checker__collect_calls_expr(AValue node, AValue out) {
    AValue tag = {0}, callee = {0}, args = {0}, nargs = {0}, obj = {0}, field = {0}, i = {0}, elems = {0};
    AValue __ret = a_void();
    node = a_retain(node);
    out = a_retain(out);
    if (a_truthy(a_neq(a_type_of(node), a_string("map")))) {
        __ret = a_retain(out); goto __fn_cleanup;
    }
    if (a_truthy(a_not(a_map_has(node, a_string("tag"))))) {
        __ret = a_retain(out); goto __fn_cleanup;
    }
    { AValue __old = tag; tag = a_array_get(node, a_string("tag")); a_release(__old); }
    if (a_truthy(a_eq(tag, a_string("Call")))) {
        { AValue __old = callee; callee = a_array_get(node, a_string("func")); a_release(__old); }
        { AValue __old = args; args = a_array_get(node, a_string("args")); a_release(__old); }
        { AValue __old = nargs; nargs = a_int(0); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(args), a_string("array")))) {
            { AValue __old = nargs; nargs = a_len(args); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(a_type_of(callee), a_string("map")), a_eq(a_array_get(callee, a_string("tag")), a_string("Ident"))))) {
            { AValue __old = out; out = a_array_push(out, a_map_new(2, "name", a_array_get(callee, a_string("name")), "nargs", nargs)); a_release(__old); }
        }
        if (a_truthy(a_and(a_eq(a_type_of(callee), a_string("map")), a_eq(a_array_get(callee, a_string("tag")), a_string("FieldAccess"))))) {
            { AValue __old = obj; obj = a_array_get(callee, a_string("object")); a_release(__old); }
            { AValue __old = field; field = a_array_get(callee, a_string("field")); a_release(__old); }
            if (a_truthy(a_and(a_eq(a_type_of(obj), a_string("map")), a_eq(a_array_get(obj, a_string("tag")), a_string("Ident"))))) {
                { AValue __old = out; out = a_array_push(out, a_map_new(2, "name", a_add(a_add(a_array_get(obj, a_string("name")), a_string(".")), field), "nargs", nargs)); a_release(__old); }
            }
        }
        { AValue __old = out; out = fn_checker__collect_calls_expr(callee, out); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(args), a_string("array")))) {
            { AValue __old = i; i = a_int(0); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(args)))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(args, i), out); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        __ret = a_retain(out); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_eq(tag, a_string("BinOp")), a_eq(tag, a_string("BinaryOp"))))) {
        { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(node, a_string("left")), out); a_release(__old); }
        __ret = fn_checker__collect_calls_expr(a_array_get(node, a_string("right")), out); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_or(a_eq(tag, a_string("UnaryOp")), a_eq(tag, a_string("Not"))), a_eq(tag, a_string("Neg"))))) {
        if (a_truthy(a_map_has(node, a_string("operand")))) {
            __ret = fn_checker__collect_calls_expr(a_array_get(node, a_string("operand")), out); goto __fn_cleanup;
        }
        if (a_truthy(a_map_has(node, a_string("expr")))) {
            __ret = fn_checker__collect_calls_expr(a_array_get(node, a_string("expr")), out); goto __fn_cleanup;
        }
        __ret = a_retain(out); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Index")))) {
        { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(node, a_string("object")), out); a_release(__old); }
        __ret = fn_checker__collect_calls_expr(a_array_get(node, a_string("index")), out); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("FieldAccess")))) {
        __ret = fn_checker__collect_calls_expr(a_array_get(node, a_string("object")), out); goto __fn_cleanup;
    }
    if (a_truthy(a_and(a_eq(tag, a_string("Array")), a_map_has(node, a_string("elements"))))) {
        { AValue __old = elems; elems = a_array_get(node, a_string("elements")); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(elems), a_string("array")))) {
            { AValue __old = i; i = a_int(0); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(elems)))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(elems, i), out); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        __ret = a_retain(out); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tag, a_string("Pipe")))) {
        { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(node, a_string("left")), out); a_release(__old); }
        __ret = fn_checker__collect_calls_expr(a_array_get(node, a_string("right")), out); goto __fn_cleanup;
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(callee);
    a_release(args);
    a_release(nargs);
    a_release(obj);
    a_release(field);
    a_release(i);
    a_release(elems);
    a_release(node);
    a_release(out);
    return __ret;
}

AValue fn_checker__collect_all_calls(AValue stmts) {
    AValue out = {0}, i = {0}, s = {0}, tag = {0}, inner = {0}, sub = {0}, j = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    { AValue __old = out; out = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(stmts), a_string("array")))) {
        __ret = a_retain(out); goto __fn_cleanup;
    }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(stmts)))) {
        { AValue __old = s; s = a_array_get(stmts, i); a_release(__old); }
        if (a_truthy(a_and(a_eq(a_type_of(s), a_string("map")), a_map_has(s, a_string("tag"))))) {
            { AValue __old = tag; tag = a_array_get(s, a_string("tag")); a_release(__old); }
            if (a_truthy(a_and(a_and(a_eq(tag, a_string("Let")), a_map_has(s, a_string("value"))), a_neq(a_type_of(a_array_get(s, a_string("value"))), a_string("void"))))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("value")), out); a_release(__old); }
            }
            if (a_truthy(a_eq(tag, a_string("Assign")))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("value")), out); a_release(__old); }
            }
            if (a_truthy(a_and(a_and(a_eq(tag, a_string("Return")), a_map_has(s, a_string("value"))), a_neq(a_type_of(a_array_get(s, a_string("value"))), a_string("void"))))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("value")), out); a_release(__old); }
            }
            if (a_truthy(a_eq(tag, a_string("ExprStmt")))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("expr")), out); a_release(__old); }
            }
            if (a_truthy(a_eq(tag, a_string("If")))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("condition")), out); a_release(__old); }
                if (a_truthy(a_map_has(s, a_string("then")))) {
                    { AValue __old = inner; inner = a_array_get(s, a_string("then")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(inner), a_string("map")), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = sub; sub = fn_checker__collect_all_calls(a_array_get(inner, a_string("stmts"))); a_release(__old); }
                        { AValue __old = j; j = a_int(0); a_release(__old); }
                        while (a_truthy(a_lt(j, a_len(sub)))) {
                            { AValue __old = out; out = a_array_push(out, a_array_get(sub, j)); a_release(__old); }
                            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
                        }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("For")))) {
                if (a_truthy(a_map_has(s, a_string("iterable")))) {
                    { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("iterable")), out); a_release(__old); }
                }
                if (a_truthy(a_map_has(s, a_string("body")))) {
                    { AValue __old = inner; inner = a_array_get(s, a_string("body")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(inner), a_string("map")), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = sub; sub = fn_checker__collect_all_calls(a_array_get(inner, a_string("stmts"))); a_release(__old); }
                        { AValue __old = j; j = a_int(0); a_release(__old); }
                        while (a_truthy(a_lt(j, a_len(sub)))) {
                            { AValue __old = out; out = a_array_push(out, a_array_get(sub, j)); a_release(__old); }
                            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
                        }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("While")))) {
                { AValue __old = out; out = fn_checker__collect_calls_expr(a_array_get(s, a_string("condition")), out); a_release(__old); }
                if (a_truthy(a_map_has(s, a_string("body")))) {
                    { AValue __old = inner; inner = a_array_get(s, a_string("body")); a_release(__old); }
                    if (a_truthy(a_and(a_eq(a_type_of(inner), a_string("map")), a_eq(a_array_get(inner, a_string("tag")), a_string("Block"))))) {
                        { AValue __old = sub; sub = fn_checker__collect_all_calls(a_array_get(inner, a_string("stmts"))); a_release(__old); }
                        { AValue __old = j; j = a_int(0); a_release(__old); }
                        while (a_truthy(a_lt(j, a_len(sub)))) {
                            { AValue __old = out; out = a_array_push(out, a_array_get(sub, j)); a_release(__old); }
                            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
                        }
                    }
                }
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(out);
    a_release(i);
    a_release(s);
    a_release(tag);
    a_release(inner);
    a_release(sub);
    a_release(j);
    a_release(stmts);
    return __ret;
}

AValue fn_profiler__nl(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(10)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_profiler__lbrace(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(123)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_profiler__collect_fn_names(AValue prog_ast) {
    AValue items = {0}, names = {0};
    AValue __ret = a_void();
    prog_ast = a_retain(prog_ast);
    { AValue __old = items; items = a_array_get(prog_ast, a_string("items")); a_release(__old); }
    { AValue __old = names; names = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = names; names = a_array_push(names, a_array_get(item, a_string("name"))); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(names); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(names);
    a_release(prog_ast);
    return __ret;
}

AValue fn_profiler__count_branches_inner(AValue stmts, AValue counts) {
    AValue tag = {0}, then_stmts = {0}, else_br = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    counts = a_retain(counts);
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue st = {0}, tag = {0}, then_stmts = {0}, else_br = {0};
            st = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = tag; tag = a_array_get(st, a_string("tag")); a_release(__old); }
            if (a_truthy(a_eq(tag, a_string("If")))) {
                { AValue __old = counts; counts = a_map_set(counts, a_string("ifs"), a_add(a_array_get(counts, a_string("ifs")), a_int(1))); a_release(__old); }
                { AValue __old = then_stmts; then_stmts = a_array_get(a_array_get(st, a_string("then")), a_string("stmts")); a_release(__old); }
                { AValue __old = counts; counts = fn_profiler__count_branches_inner(then_stmts, counts); a_release(__old); }
                { AValue __old = else_br; else_br = a_array_get(st, a_string("else")); a_release(__old); }
                if (a_truthy(a_neq(a_type_of(else_br), a_string("void")))) {
                    if (a_truthy(a_eq(a_array_get(else_br, a_string("tag")), a_string("ElseBlock")))) {
                        { AValue __old = counts; counts = a_map_set(counts, a_string("elses"), a_add(a_array_get(counts, a_string("elses")), a_int(1))); a_release(__old); }
                        { AValue __old = counts; counts = fn_profiler__count_branches_inner(a_array_get(a_array_get(else_br, a_string("block")), a_string("stmts")), counts); a_release(__old); }
                    }
                    if (a_truthy(a_eq(a_array_get(else_br, a_string("tag")), a_string("ElseIf")))) {
                        { AValue __old = counts; counts = fn_profiler__count_branches_inner(a_array_new(1, a_array_get(else_br, a_string("stmt"))), counts); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("While")))) {
                { AValue __old = counts; counts = a_map_set(counts, a_string("whiles"), a_add(a_array_get(counts, a_string("whiles")), a_int(1))); a_release(__old); }
                { AValue __old = counts; counts = fn_profiler__count_branches_inner(a_array_get(a_array_get(st, a_string("body")), a_string("stmts")), counts); a_release(__old); }
            }
            if (a_truthy(a_or(a_eq(tag, a_string("For")), a_eq(tag, a_string("ForDestructure"))))) {
                { AValue __old = counts; counts = a_map_set(counts, a_string("fors"), a_add(a_array_get(counts, a_string("fors")), a_int(1))); a_release(__old); }
                { AValue __old = counts; counts = fn_profiler__count_branches_inner(a_array_get(a_array_get(st, a_string("body")), a_string("stmts")), counts); a_release(__old); }
            }
            a_release(st);
            a_release(tag);
            a_release(then_stmts);
            a_release(else_br);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(counts); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(then_stmts);
    a_release(else_br);
    a_release(stmts);
    a_release(counts);
    return __ret;
}

AValue fn_profiler__analyze_ast(AValue prog_ast) {
    AValue fns = {0}, items = {0}, total_counts = {0}, body = {0};
    AValue __ret = a_void();
    prog_ast = a_retain(prog_ast);
    { AValue __old = fns; fns = fn_profiler__collect_fn_names(prog_ast); a_release(__old); }
    { AValue __old = items; items = a_array_get(prog_ast, a_string("items")); a_release(__old); }
    { AValue __old = total_counts; total_counts = a_map_new(4, "ifs", a_int(0), "elses", a_int(0), "whiles", a_int(0), "fors", a_int(0)); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, body = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = body; body = a_array_get(item, a_string("body")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(body), a_string("map")))) {
                    if (a_truthy(a_map_has(body, a_string("stmts")))) {
                        { AValue __old = total_counts; total_counts = fn_profiler__count_branches_inner(a_array_get(body, a_string("stmts")), total_counts); a_release(__old); }
                    }
                }
            }
            a_release(item);
            a_release(body);
        }
        a_release(__iter_arr);
    }
    __ret = a_map_new(2, "functions", fns, "branch_counts", total_counts); goto __fn_cleanup;
__fn_cleanup:
    a_release(fns);
    a_release(items);
    a_release(total_counts);
    a_release(body);
    a_release(prog_ast);
    return __ret;
}

AValue fn_profiler__mangle_name(AValue name) {
    AValue __ret = a_void();
    name = a_retain(name);
    __ret = a_str_replace(a_str_replace(a_str_replace(name, a_string("."), a_string("_")), a_string("-"), a_string("_")), a_string("!"), a_string("_bang")); goto __fn_cleanup;
__fn_cleanup:
    a_release(name);
    return __ret;
}

AValue fn_profiler__gen_base_c(AValue source_path) {
    AValue self_path = {0}, tmp_c = {0}, r = {0}, c_code = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = self_path; self_path = a_argv0(); a_release(__old); }
    { AValue __old = tmp_c; tmp_c = a_add(a_add(a_string("/tmp/a_prof_"), a_to_str(a_time_now())), a_string(".c")); a_release(__old); }
    { AValue __old = r; r = a_exec(a_add(a_add(a_add(a_add(self_path, a_string(" cc ")), source_path), a_string(" -o ")), tmp_c)); a_release(__old); }
    if (a_truthy(a_neq(a_array_get(r, a_string("code")), a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = c_code; c_code = a_io_read_file(tmp_c); a_release(__old); }
    a_fs_rm(tmp_c);
    __ret = a_retain(c_code); goto __fn_cleanup;
__fn_cleanup:
    a_release(self_path);
    a_release(tmp_c);
    a_release(r);
    a_release(c_code);
    a_release(source_path);
    return __ret;
}

AValue fn_profiler__insert_fn_hits(AValue c_code, AValue fn_names, AValue fn_hit_map) {
    AValue nl = {0}, lbr = {0}, result = {0}, mangled = {0}, sig_str = {0}, parts = {0}, after_sig = {0}, brace_idx = {0}, hit_id = {0}, after_brace = {0}, before_brace = {0}, hit_line = {0};
    AValue __ret = a_void();
    c_code = a_retain(c_code);
    fn_names = a_retain(fn_names);
    fn_hit_map = a_retain(fn_hit_map);
    { AValue __old = nl; nl = fn_profiler__nl(); a_release(__old); }
    { AValue __old = lbr; lbr = fn_profiler__lbrace(); a_release(__old); }
    { AValue __old = result; result = a_retain(c_code); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(fn_names);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue fn2 = {0}, mangled = {0}, sig_str = {0}, parts = {0}, after_sig = {0}, brace_idx = {0}, hit_id = {0}, after_brace = {0}, before_brace = {0}, hit_line = {0};
            fn2 = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = mangled; mangled = fn_profiler__mangle_name(fn2); a_release(__old); }
            { AValue __old = sig_str; sig_str = a_add(a_add(a_string("AValue fn_"), mangled), a_string("(")); a_release(__old); }
            { AValue __old = parts; parts = a_str_split(result, sig_str); a_release(__old); }
            if (a_truthy(a_gteq(a_len(parts), a_int(2)))) {
                { AValue __old = after_sig; after_sig = a_array_get(parts, a_int(1)); a_release(__old); }
                { AValue __old = brace_idx; brace_idx = a_str_find(after_sig, lbr); a_release(__old); }
                if (a_truthy(a_gteq(brace_idx, a_int(0)))) {
                    { AValue __old = hit_id; hit_id = a_array_get(fn_hit_map, fn2); a_release(__old); }
                    { AValue __old = after_brace; after_brace = a_str_slice(after_sig, a_add(brace_idx, a_int(1)), a_len(after_sig)); a_release(__old); }
                    { AValue __old = before_brace; before_brace = a_str_slice(after_sig, a_int(0), a_add(brace_idx, a_int(1))); a_release(__old); }
                    { AValue __old = hit_line; hit_line = a_add(a_add(a_add(nl, a_string("    a_profile_hit(")), a_to_str(hit_id)), a_string(");")); a_release(__old); }
                    { AValue __old = result; result = a_add(a_add(a_add(a_add(a_array_get(parts, a_int(0)), sig_str), before_brace), hit_line), after_brace); a_release(__old); }
                }
            }
            a_release(fn2);
            a_release(mangled);
            a_release(sig_str);
            a_release(parts);
            a_release(after_sig);
            a_release(brace_idx);
            a_release(hit_id);
            a_release(after_brace);
            a_release(before_brace);
            a_release(hit_line);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(nl);
    a_release(lbr);
    a_release(result);
    a_release(mangled);
    a_release(sig_str);
    a_release(parts);
    a_release(after_sig);
    a_release(brace_idx);
    a_release(hit_id);
    a_release(after_brace);
    a_release(before_brace);
    a_release(hit_line);
    a_release(c_code);
    a_release(fn_names);
    a_release(fn_hit_map);
    return __ret;
}

AValue fn_profiler__insert_branch_hits(AValue c_code, AValue n_ifs, AValue n_whiles, AValue n_fors, AValue hit_base) {
    AValue nl = {0}, src_lines = {0}, new_lines = {0}, idx_if = {0}, idx_wh = {0}, idx_fo = {0}, trimmed = {0}, cid = {0};
    AValue __ret = a_void();
    c_code = a_retain(c_code);
    n_ifs = a_retain(n_ifs);
    n_whiles = a_retain(n_whiles);
    n_fors = a_retain(n_fors);
    hit_base = a_retain(hit_base);
    { AValue __old = nl; nl = fn_profiler__nl(); a_release(__old); }
    { AValue __old = src_lines; src_lines = a_str_split(c_code, nl); a_release(__old); }
    { AValue __old = new_lines; new_lines = a_array_new(0); a_release(__old); }
    { AValue __old = idx_if; idx_if = a_int(0); a_release(__old); }
    { AValue __old = idx_wh; idx_wh = a_int(0); a_release(__old); }
    { AValue __old = idx_fo; idx_fo = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(src_lines);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue ln = {0}, trimmed = {0}, cid = {0};
            ln = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = new_lines; new_lines = a_array_push(new_lines, ln); a_release(__old); }
            { AValue __old = trimmed; trimmed = a_str_trim(ln); a_release(__old); }
            if (a_truthy(a_str_starts_with(trimmed, a_string("if (a_truthy(")))) {
                if (a_truthy(a_str_ends_with(trimmed, a_add(a_string(") "), fn_profiler__lbrace())))) {
                    if (a_truthy(a_lt(idx_if, n_ifs))) {
                        { AValue __old = cid; cid = a_add(hit_base, idx_if); a_release(__old); }
                        { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("        a_profile_hit("), a_to_str(cid)), a_string(");"))); a_release(__old); }
                        { AValue __old = idx_if; idx_if = a_add(idx_if, a_int(1)); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_str_starts_with(trimmed, a_string("while (a_truthy(")))) {
                if (a_truthy(a_str_ends_with(trimmed, a_add(a_string(") "), fn_profiler__lbrace())))) {
                    { AValue __old = cid; cid = a_add(a_add(hit_base, n_ifs), idx_wh); a_release(__old); }
                    { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("        a_profile_hit("), a_to_str(cid)), a_string(");"))); a_release(__old); }
                    { AValue __old = idx_wh; idx_wh = a_add(idx_wh, a_int(1)); a_release(__old); }
                }
            }
            if (a_truthy(a_str_starts_with(trimmed, a_string("for (int __fi")))) {
                { AValue __old = cid; cid = a_add(a_add(a_add(hit_base, n_ifs), n_whiles), idx_fo); a_release(__old); }
                { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("            a_profile_hit("), a_to_str(cid)), a_string(");"))); a_release(__old); }
                { AValue __old = idx_fo; idx_fo = a_add(idx_fo, a_int(1)); a_release(__old); }
            }
            a_release(ln);
            a_release(trimmed);
            a_release(cid);
        }
        a_release(__iter_arr);
    }
    __ret = a_str_join(new_lines, nl); goto __fn_cleanup;
__fn_cleanup:
    a_release(nl);
    a_release(src_lines);
    a_release(new_lines);
    a_release(idx_if);
    a_release(idx_wh);
    a_release(idx_fo);
    a_release(trimmed);
    a_release(cid);
    a_release(c_code);
    a_release(n_ifs);
    a_release(n_whiles);
    a_release(n_fors);
    a_release(hit_base);
    return __ret;
}

AValue fn_profiler_instrument(AValue source_path, AValue profile_out_path) {
    AValue source = {0}, parsed = {0}, analysis = {0}, fn_names = {0}, base_c = {0}, nl = {0}, q = {0}, counter_id = {0}, reg_lines = {0}, fn_hit_map = {0}, result = {0}, bc = {0}, n_ifs = {0}, n_whiles = {0}, n_fors = {0}, bi = {0}, hit_base = {0}, old_main = {0}, new_main = {0}, old_end = {0}, new_end = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    profile_out_path = a_retain(profile_out_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = parsed; parsed = fn_parser_parse(source); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(parsed), a_string("map")))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    if (a_truthy(a_neq(a_array_get(parsed, a_string("tag")), a_string("Program")))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = analysis; analysis = fn_profiler__analyze_ast(parsed); a_release(__old); }
    { AValue __old = fn_names; fn_names = a_array_get(analysis, a_string("functions")); a_release(__old); }
    { AValue __old = base_c; base_c = fn_profiler__gen_base_c(source_path); a_release(__old); }
    if (a_truthy(a_eq(a_len(base_c), a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = nl; nl = fn_profiler__nl(); a_release(__old); }
    { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
    { AValue __old = counter_id; counter_id = a_int(0); a_release(__old); }
    { AValue __old = reg_lines; reg_lines = a_string(""); a_release(__old); }
    { AValue __old = fn_hit_map; fn_hit_map = a_map_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(fn_names);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue fname = {0};
            fname = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("fn:")), fname), q), a_string(");")), nl); a_release(__old); }
            { AValue __old = fn_hit_map; fn_hit_map = a_map_set(fn_hit_map, fname, counter_id); a_release(__old); }
            { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
            a_release(fname);
        }
        a_release(__iter_arr);
    }
    { AValue __old = result; result = fn_profiler__insert_fn_hits(base_c, fn_names, fn_hit_map); a_release(__old); }
    { AValue __old = bc; bc = a_array_get(analysis, a_string("branch_counts")); a_release(__old); }
    { AValue __old = n_ifs; n_ifs = a_array_get(bc, a_string("ifs")); a_release(__old); }
    { AValue __old = n_whiles; n_whiles = a_array_get(bc, a_string("whiles")); a_release(__old); }
    { AValue __old = n_fors; n_fors = a_array_get(bc, a_string("fors")); a_release(__old); }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, n_ifs))) {
        { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("if:")), a_to_str(bi)), q), a_string(");")), nl); a_release(__old); }
        { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, n_whiles))) {
        { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("while:")), a_to_str(bi)), q), a_string(");")), nl); a_release(__old); }
        { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, n_fors))) {
        { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("for:")), a_to_str(bi)), q), a_string(");")), nl); a_release(__old); }
        { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = hit_base; hit_base = a_len(fn_names); a_release(__old); }
    { AValue __old = result; result = fn_profiler__insert_branch_hits(result, n_ifs, n_whiles, n_fors, hit_base); a_release(__old); }
    { AValue __old = old_main; old_main = a_string("g_argc = argc; g_argv = argv;"); a_release(__old); }
    { AValue __old = new_main; new_main = a_add(a_add(a_add(a_add(a_string("g_argc = argc; g_argv = argv;"), nl), a_string("    a_profile_init();")), nl), reg_lines); a_release(__old); }
    { AValue __old = result; result = a_str_replace(result, old_main, new_main); a_release(__old); }
    { AValue __old = old_end; old_end = a_add(a_add(a_string("fn_main();"), nl), a_string("    return 0;")); a_release(__old); }
    { AValue __old = new_end; new_end = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("fn_main();"), nl), a_string("    a_profile_dump_json(a_string(")), q), profile_out_path), q), a_string("));")), nl), a_string("    return 0;")); a_release(__old); }
    { AValue __old = result; result = a_str_replace(result, old_end, new_end); a_release(__old); }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(parsed);
    a_release(analysis);
    a_release(fn_names);
    a_release(base_c);
    a_release(nl);
    a_release(q);
    a_release(counter_id);
    a_release(reg_lines);
    a_release(fn_hit_map);
    a_release(result);
    a_release(bc);
    a_release(n_ifs);
    a_release(n_whiles);
    a_release(n_fors);
    a_release(bi);
    a_release(hit_base);
    a_release(old_main);
    a_release(new_main);
    a_release(old_end);
    a_release(new_end);
    a_release(source_path);
    a_release(profile_out_path);
    return __ret;
}

AValue fn_profiler_analyze_profile(AValue profile_path) {
    AValue raw = {0}, data = {0}, counters = {0}, hot_fns = {0}, hot_branches = {0}, max_count = {0}, threshold = {0}, lbl = {0}, cnt = {0}, fname = {0}, total_hits = {0};
    AValue __ret = a_void();
    profile_path = a_retain(profile_path);
    { AValue __old = raw; raw = a_io_read_file(profile_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(raw), a_string("void")), a_eq(a_len(raw), a_int(0))))) {
        __ret = a_map_new(1, "error", a_string("cannot read profile")); goto __fn_cleanup;
    }
    { AValue __old = data; data = a_json_parse(raw); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(data), a_string("map")))) {
        __ret = a_map_new(1, "error", a_string("invalid profile JSON")); goto __fn_cleanup;
    }
    { AValue __old = counters; counters = a_array_get(data, a_string("counters")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(counters), a_string("array")))) {
        __ret = a_map_new(1, "error", a_string("no counters in profile")); goto __fn_cleanup;
    }
    { AValue __old = hot_fns; hot_fns = a_array_new(0); a_release(__old); }
    { AValue __old = hot_branches; hot_branches = a_array_new(0); a_release(__old); }
    { AValue __old = max_count; max_count = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_gt(a_array_get(c, a_string("count")), max_count))) {
                { AValue __old = max_count; max_count = a_array_get(c, a_string("count")); a_release(__old); }
            }
            a_release(c);
        }
        a_release(__iter_arr);
    }
    { AValue __old = threshold; threshold = a_div(max_count, a_int(10)); a_release(__old); }
    if (a_truthy(a_lt(threshold, a_int(1)))) {
        { AValue __old = threshold; threshold = a_int(1); a_release(__old); }
    }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue ct = {0}, lbl = {0}, cnt = {0}, fname = {0};
            ct = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = lbl; lbl = a_array_get(ct, a_string("label")); a_release(__old); }
            { AValue __old = cnt; cnt = a_array_get(ct, a_string("count")); a_release(__old); }
            if (a_truthy(a_gteq(cnt, threshold))) {
                if (a_truthy(a_str_starts_with(lbl, a_string("fn:")))) {
                    { AValue __old = fname; fname = a_str_slice(lbl, a_int(3), a_len(lbl)); a_release(__old); }
                    { AValue __old = hot_fns; hot_fns = a_array_push(hot_fns, a_map_new(3, "name", fname, "count", cnt, "hot_ratio", a_div(a_mul(cnt, a_int(100)), max_count))); a_release(__old); }
                }
            }
            if (a_truthy(a_gt(cnt, a_int(0)))) {
                if (a_truthy(a_or(a_or(a_str_starts_with(lbl, a_string("if:")), a_str_starts_with(lbl, a_string("while:"))), a_str_starts_with(lbl, a_string("for:"))))) {
                    { AValue __old = hot_branches; hot_branches = a_array_push(hot_branches, a_map_new(2, "label", lbl, "count", cnt)); a_release(__old); }
                }
            }
            a_release(ct);
            a_release(lbl);
            a_release(cnt);
            a_release(fname);
        }
        a_release(__iter_arr);
    }
    { AValue __old = total_hits; total_hits = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue ct2 = {0};
            ct2 = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = total_hits; total_hits = a_add(total_hits, a_array_get(ct2, a_string("count"))); a_release(__old); }
            a_release(ct2);
        }
        a_release(__iter_arr);
    }
    __ret = a_map_new(5, "total_points", a_array_get(data, a_string("total_points")), "total_hits", total_hits, "hot_functions", hot_fns, "hot_branches", hot_branches, "counters", counters); goto __fn_cleanup;
__fn_cleanup:
    a_release(raw);
    a_release(data);
    a_release(counters);
    a_release(hot_fns);
    a_release(hot_branches);
    a_release(max_count);
    a_release(threshold);
    a_release(lbl);
    a_release(cnt);
    a_release(fname);
    a_release(total_hits);
    a_release(profile_path);
    return __ret;
}

AValue fn_optimizer_load_profile(AValue profile_path) {
    AValue raw = {0}, data = {0};
    AValue __ret = a_void();
    profile_path = a_retain(profile_path);
    { AValue __old = raw; raw = a_io_read_file(profile_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(raw), a_string("void")), a_eq(a_len(raw), a_int(0))))) {
        __ret = a_map_new(1, "error", a_string("cannot read profile")); goto __fn_cleanup;
    }
    { AValue __old = data; data = a_json_parse(raw); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(data), a_string("map")))) {
        __ret = a_map_new(1, "error", a_string("invalid profile JSON")); goto __fn_cleanup;
    }
    __ret = a_retain(data); goto __fn_cleanup;
__fn_cleanup:
    a_release(raw);
    a_release(data);
    a_release(profile_path);
    return __ret;
}

AValue fn_optimizer__find_hot_functions(AValue counters) {
    AValue hot = {0}, max_count = {0}, cnt = {0}, threshold = {0}, lbl = {0};
    AValue __ret = a_void();
    counters = a_retain(counters);
    { AValue __old = hot; hot = a_array_new(0); a_release(__old); }
    { AValue __old = max_count; max_count = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0}, cnt = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = cnt; cnt = a_array_get(c, a_string("count")); a_release(__old); }
            if (a_truthy(a_gt(cnt, max_count))) {
                { AValue __old = max_count; max_count = a_retain(cnt); a_release(__old); }
            }
            a_release(c);
            a_release(cnt);
        }
        a_release(__iter_arr);
    }
    if (a_truthy(a_eq(max_count, a_int(0)))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    { AValue __old = threshold; threshold = a_div(max_count, a_int(10)); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0}, lbl = {0}, cnt = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = lbl; lbl = a_array_get(c, a_string("label")); a_release(__old); }
            { AValue __old = cnt; cnt = a_array_get(c, a_string("count")); a_release(__old); }
            if (a_truthy(a_and(a_gteq(cnt, threshold), a_str_starts_with(lbl, a_string("fn:"))))) {
                { AValue __old = hot; hot = a_array_push(hot, a_map_new(4, "label", lbl, "count", cnt, "name", a_str_slice(lbl, a_int(3), a_len(lbl)), "hot_ratio", a_div(a_mul(cnt, a_int(100)), max_count))); a_release(__old); }
            }
            a_release(c);
            a_release(lbl);
            a_release(cnt);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(hot); goto __fn_cleanup;
__fn_cleanup:
    a_release(hot);
    a_release(max_count);
    a_release(cnt);
    a_release(threshold);
    a_release(lbl);
    a_release(counters);
    return __ret;
}

AValue fn_optimizer__find_hot_branches(AValue counters) {
    AValue branches = {0}, lbl = {0}, cnt = {0};
    AValue __ret = a_void();
    counters = a_retain(counters);
    { AValue __old = branches; branches = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0}, lbl = {0}, cnt = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = lbl; lbl = a_array_get(c, a_string("label")); a_release(__old); }
            { AValue __old = cnt; cnt = a_array_get(c, a_string("count")); a_release(__old); }
            if (a_truthy(a_or(a_or(a_or(a_str_starts_with(lbl, a_string("if:")), a_str_starts_with(lbl, a_string("else:"))), a_str_starts_with(lbl, a_string("while:"))), a_str_starts_with(lbl, a_string("for:"))))) {
                if (a_truthy(a_gt(cnt, a_int(0)))) {
                    { AValue __old = branches; branches = a_array_push(branches, a_map_new(2, "label", lbl, "count", cnt)); a_release(__old); }
                }
            }
            a_release(c);
            a_release(lbl);
            a_release(cnt);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(branches); goto __fn_cleanup;
__fn_cleanup:
    a_release(branches);
    a_release(lbl);
    a_release(cnt);
    a_release(counters);
    return __ret;
}

AValue fn_optimizer_analyze_profile(AValue profile_path) {
    AValue data = {0}, counters = {0}, hot_fns = {0}, hot_branches = {0}, total_pts = {0}, total_hits = {0};
    AValue __ret = a_void();
    profile_path = a_retain(profile_path);
    { AValue __old = data; data = fn_optimizer_load_profile(profile_path); a_release(__old); }
    if (a_truthy(a_map_has(data, a_string("error")))) {
        __ret = a_retain(data); goto __fn_cleanup;
    }
    { AValue __old = counters; counters = a_array_get(data, a_string("counters")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(counters), a_string("array")))) {
        __ret = a_map_new(1, "error", a_string("no counters in profile")); goto __fn_cleanup;
    }
    { AValue __old = hot_fns; hot_fns = fn_optimizer__find_hot_functions(counters); a_release(__old); }
    { AValue __old = hot_branches; hot_branches = fn_optimizer__find_hot_branches(counters); a_release(__old); }
    { AValue __old = total_pts; total_pts = a_array_get(data, a_string("total_points")); a_release(__old); }
    { AValue __old = total_hits; total_hits = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(counters);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = total_hits; total_hits = a_add(total_hits, a_array_get(c, a_string("count"))); a_release(__old); }
            a_release(c);
        }
        a_release(__iter_arr);
    }
    __ret = a_map_new(5, "total_points", total_pts, "total_hits", total_hits, "hot_functions", hot_fns, "hot_branches", hot_branches, "counters", counters); goto __fn_cleanup;
__fn_cleanup:
    a_release(data);
    a_release(counters);
    a_release(hot_fns);
    a_release(hot_branches);
    a_release(total_pts);
    a_release(total_hits);
    a_release(profile_path);
    return __ret;
}

AValue fn_optimizer__count_fn_body_stmts(AValue body) {
    AValue __ret = a_void();
    body = a_retain(body);
    if (a_truthy(a_and(a_eq(a_type_of(body), a_string("map")), a_map_has(body, a_string("stmts"))))) {
        __ret = a_len(a_array_get(body, a_string("stmts"))); goto __fn_cleanup;
    }
    __ret = a_int(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(body);
    return __ret;
}

AValue fn_optimizer__is_small_fn(AValue func_node) {
    AValue stmts = {0};
    AValue __ret = a_void();
    func_node = a_retain(func_node);
    { AValue __old = stmts; stmts = fn_optimizer__count_fn_body_stmts(a_array_get(func_node, a_string("body"))); a_release(__old); }
    __ret = a_lteq(stmts, a_int(5)); goto __fn_cleanup;
__fn_cleanup:
    a_release(stmts);
    a_release(func_node);
    return __ret;
}

AValue fn_optimizer__extract_fn_map(AValue source) {
    AValue prog = {0}, fm = {0}, items = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    { AValue __old = prog; prog = fn_parser_parse(source); a_release(__old); }
    if (a_truthy(a_or(a_neq(a_type_of(prog), a_string("map")), a_neq(a_array_get(prog, a_string("tag")), a_string("Program"))))) {
        __ret = a_map_new(0); goto __fn_cleanup;
    }
    { AValue __old = fm; fm = a_map_new(0); a_release(__old); }
    { AValue __old = items; items = a_array_get(prog, a_string("items")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = fm; fm = a_map_set(fm, a_array_get(item, a_string("name")), item); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(fm); goto __fn_cleanup;
__fn_cleanup:
    a_release(prog);
    a_release(fm);
    a_release(items);
    a_release(source);
    return __ret;
}

AValue fn_optimizer_suggest(AValue source, AValue profile_report) {
    AValue suggestions = {0}, fn_map = {0}, hot_fns = {0}, fname = {0}, cnt = {0}, ratio = {0}, fn_node = {0}, is_small = {0}, n_params = {0}, hot_branches = {0}, lbl = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    profile_report = a_retain(profile_report);
    { AValue __old = suggestions; suggestions = a_array_new(0); a_release(__old); }
    if (a_truthy(a_map_has(profile_report, a_string("error")))) {
        __ret = a_retain(suggestions); goto __fn_cleanup;
    }
    { AValue __old = fn_map; fn_map = fn_optimizer__extract_fn_map(source); a_release(__old); }
    { AValue __old = hot_fns; hot_fns = a_array_get(profile_report, a_string("hot_functions")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(hot_fns);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue hf = {0}, fname = {0}, cnt = {0}, ratio = {0}, fn_node = {0}, is_small = {0}, n_params = {0};
            hf = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = fname; fname = a_array_get(hf, a_string("name")); a_release(__old); }
            { AValue __old = cnt; cnt = a_array_get(hf, a_string("count")); a_release(__old); }
            { AValue __old = ratio; ratio = a_array_get(hf, a_string("hot_ratio")); a_release(__old); }
            if (a_truthy(a_map_has(fn_map, fname))) {
                { AValue __old = fn_node; fn_node = a_array_get(fn_map, fname); a_release(__old); }
                { AValue __old = is_small; is_small = fn_optimizer__is_small_fn(fn_node); a_release(__old); }
                { AValue __old = n_params; n_params = a_len(a_array_get(fn_node, a_string("params"))); a_release(__old); }
                if (a_truthy(a_and(a_and(is_small, a_lteq(n_params, a_int(3))), a_gteq(ratio, a_int(50))))) {
                    { AValue __old = suggestions; suggestions = a_array_push(suggestions, a_map_new(4, "type", a_string("inline"), "function", fname, "reason", a_add(a_add(a_add(a_add(a_add(a_add(a_string("hot function ("), a_to_str(cnt)), a_string(" calls, ")), a_to_str(ratio)), a_string("% of max) with small body (")), a_to_str(fn_optimizer__count_fn_body_stmts(a_array_get(fn_node, a_string("body"))))), a_string(" stmts)")), "priority", a_string("high"))); a_release(__old); }
                }
                if (a_truthy(a_and(a_not(is_small), a_gteq(ratio, a_int(80))))) {
                    { AValue __old = suggestions; suggestions = a_array_push(suggestions, a_map_new(4, "type", a_string("optimize"), "function", fname, "reason", a_add(a_add(a_add(a_add(a_string("dominant hot function ("), a_to_str(cnt)), a_string(" calls, ")), a_to_str(ratio)), a_string("% of max), review for algorithmic improvements")), "priority", a_string("high"))); a_release(__old); }
                }
            }
            a_release(hf);
            a_release(fname);
            a_release(cnt);
            a_release(ratio);
            a_release(fn_node);
            a_release(is_small);
            a_release(n_params);
        }
        a_release(__iter_arr);
    }
    { AValue __old = hot_branches; hot_branches = a_array_get(profile_report, a_string("hot_branches")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(hot_branches);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue hb = {0}, lbl = {0}, cnt = {0};
            hb = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = lbl; lbl = a_array_get(hb, a_string("label")); a_release(__old); }
            { AValue __old = cnt; cnt = a_array_get(hb, a_string("count")); a_release(__old); }
            if (a_truthy(a_and(a_str_starts_with(lbl, a_string("while:")), a_gt(cnt, a_int(10000))))) {
                { AValue __old = suggestions; suggestions = a_array_push(suggestions, a_map_new(4, "type", a_string("loop_unroll"), "label", lbl, "reason", a_add(a_add(a_string("hot loop with "), a_to_str(cnt)), a_string(" iterations, consider unrolling")), "priority", a_string("medium"))); a_release(__old); }
            }
            a_release(hb);
            a_release(lbl);
            a_release(cnt);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(suggestions); goto __fn_cleanup;
__fn_cleanup:
    a_release(suggestions);
    a_release(fn_map);
    a_release(hot_fns);
    a_release(fname);
    a_release(cnt);
    a_release(ratio);
    a_release(fn_node);
    a_release(is_small);
    a_release(n_params);
    a_release(hot_branches);
    a_release(lbl);
    a_release(source);
    a_release(profile_report);
    return __ret;
}

AValue fn_optimizer_measure_binary(AValue bin_path) {
    AValue stat_r = {0}, sz = {0};
    AValue __ret = a_void();
    bin_path = a_retain(bin_path);
    if (a_truthy(a_not(a_fs_exists(bin_path)))) {
        __ret = a_map_new(1, "error", a_string("binary not found")); goto __fn_cleanup;
    }
    { AValue __old = stat_r; stat_r = a_exec(a_add(a_string("wc -c < "), bin_path)); a_release(__old); }
    { AValue __old = sz; sz = a_int(0); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(stat_r, a_string("code")), a_int(0)))) {
        { AValue __old = sz; sz = a_to_int(a_str_trim(a_array_get(stat_r, a_string("stdout")))); a_release(__old); }
    }
    __ret = a_map_new(4, "path", bin_path, "size_bytes", sz, "size_kb", a_div(sz, a_int(1024)), "size_mb", a_div(sz, a_int(1048576))); goto __fn_cleanup;
__fn_cleanup:
    a_release(stat_r);
    a_release(sz);
    a_release(bin_path);
    return __ret;
}

AValue fn_optimizer_measure_compile_time(AValue source_path) {
    AValue t0 = {0}, self = {0}, tmp_bin = {0}, r = {0}, t1 = {0}, elapsed = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = t0; t0 = a_time_now(); a_release(__old); }
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = tmp_bin; tmp_bin = a_add(a_string("/tmp/a_opt_bench_"), a_to_str(a_time_now())); a_release(__old); }
    { AValue __old = r; r = a_exec(a_add(a_add(a_add(a_add(a_add(self, a_string(" build ")), source_path), a_string(" -o ")), tmp_bin), a_string(" 2>&1"))); a_release(__old); }
    { AValue __old = t1; t1 = a_time_now(); a_release(__old); }
    { AValue __old = elapsed; elapsed = a_sub(t1, t0); a_release(__old); }
    if (a_truthy(a_fs_exists(tmp_bin))) {
        a_fs_rm(tmp_bin);
    }
    __ret = a_map_new(3, "source", source_path, "compile_ms", elapsed, "success", a_eq(a_array_get(r, a_string("code")), a_int(0))); goto __fn_cleanup;
__fn_cleanup:
    a_release(t0);
    a_release(self);
    a_release(tmp_bin);
    a_release(r);
    a_release(t1);
    a_release(elapsed);
    a_release(source_path);
    return __ret;
}

AValue fn_optimizer_benchmark(AValue source_path, AValue runs) {
    AValue self = {0}, tmp_bin = {0}, build_r = {0}, times = {0}, ri = {0}, t0 = {0}, run_r = {0}, t1 = {0}, total = {0}, min_t = {0}, max_t = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    runs = a_retain(runs);
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = tmp_bin; tmp_bin = a_add(a_string("/tmp/a_opt_bench_"), a_to_str(a_time_now())); a_release(__old); }
    { AValue __old = build_r; build_r = a_exec(a_add(a_add(a_add(a_add(a_add(self, a_string(" build ")), source_path), a_string(" -o ")), tmp_bin), a_string(" 2>&1"))); a_release(__old); }
    if (a_truthy(a_neq(a_array_get(build_r, a_string("code")), a_int(0)))) {
        __ret = a_map_new(1, "error", a_string("build failed")); goto __fn_cleanup;
    }
    { AValue __old = times; times = a_array_new(0); a_release(__old); }
    { AValue __old = ri; ri = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(ri, runs))) {
        { AValue __old = t0; t0 = a_time_now(); a_release(__old); }
        { AValue __old = run_r; run_r = a_exec(a_add(tmp_bin, a_string(" 2>&1"))); a_release(__old); }
        { AValue __old = t1; t1 = a_time_now(); a_release(__old); }
        { AValue __old = times; times = a_array_push(times, a_sub(t1, t0)); a_release(__old); }
        { AValue __old = ri; ri = a_add(ri, a_int(1)); a_release(__old); }
    }
    a_fs_rm(tmp_bin);
    { AValue __old = total; total = a_int(0); a_release(__old); }
    { AValue __old = min_t; min_t = a_array_get(times, a_int(0)); a_release(__old); }
    { AValue __old = max_t; max_t = a_array_get(times, a_int(0)); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(times);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue t = {0};
            t = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = total; total = a_add(total, t); a_release(__old); }
            if (a_truthy(a_lt(t, min_t))) {
                { AValue __old = min_t; min_t = a_retain(t); a_release(__old); }
            }
            if (a_truthy(a_gt(t, max_t))) {
                { AValue __old = max_t; max_t = a_retain(t); a_release(__old); }
            }
            a_release(t);
        }
        a_release(__iter_arr);
    }
    __ret = a_map_new(6, "source", source_path, "runs", runs, "avg_ms", a_div(total, runs), "min_ms", min_t, "max_ms", max_t, "total_ms", total); goto __fn_cleanup;
__fn_cleanup:
    a_release(self);
    a_release(tmp_bin);
    a_release(build_r);
    a_release(times);
    a_release(ri);
    a_release(t0);
    a_release(run_r);
    a_release(t1);
    a_release(total);
    a_release(min_t);
    a_release(max_t);
    a_release(source_path);
    a_release(runs);
    return __ret;
}

AValue fn_optimizer_report(AValue source_path, AValue profile_path) {
    AValue source = {0}, prof = {0}, suggs = {0}, bin_info = {0}, out = {0}, hf = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    profile_path = a_retain(profile_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    { AValue __old = prof; prof = fn_optimizer_analyze_profile(profile_path); a_release(__old); }
    { AValue __old = suggs; suggs = fn_optimizer_suggest(source, prof); a_release(__old); }
    { AValue __old = bin_info; bin_info = fn_optimizer_measure_binary(source_path); a_release(__old); }
    { AValue __old = out; out = a_string("=== Optimization Report ===\n\n"); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string("Profile: ")), profile_path), a_string("\n")); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string("Total instrumentation points: ")), a_to_str(a_array_get(prof, a_string("total_points")))), a_string("\n")); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string("Total hits: ")), a_to_str(a_array_get(prof, a_string("total_hits")))), a_string("\n\n")); a_release(__old); }
    { AValue __old = hf; hf = a_array_get(prof, a_string("hot_functions")); a_release(__old); }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string("Hot Functions (")), a_to_str(a_len(hf))), a_string("):\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(hf);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue f = {0};
            f = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(a_add(a_add(out, a_string("  ")), a_array_get(f, a_string("name"))), a_string(": ")), a_to_str(a_array_get(f, a_string("count")))), a_string(" calls (")), a_to_str(a_array_get(f, a_string("hot_ratio")))), a_string("% of max)\n")); a_release(__old); }
            a_release(f);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(a_add(a_add(out, a_string("\nSuggestions (")), a_to_str(a_len(suggs))), a_string("):\n")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(suggs);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue sg = {0};
            sg = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = out; out = a_add(a_add(a_add(a_add(a_add(a_add(a_add(out, a_string("  [")), a_array_get(sg, a_string("priority"))), a_string("] ")), a_array_get(sg, a_string("type"))), a_string(" -- ")), a_array_get(sg, a_string("reason"))), a_string("\n")); a_release(__old); }
            a_release(sg);
        }
        a_release(__iter_arr);
    }
    { AValue __old = out; out = a_add(out, a_string("\n")); a_release(__old); }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(prof);
    a_release(suggs);
    a_release(bin_info);
    a_release(out);
    a_release(hf);
    a_release(source_path);
    a_release(profile_path);
    return __ret;
}

AValue fn_testgen__edge_values_for_type(AValue hint) {
    AValue q = {0};
    AValue __ret = a_void();
    hint = a_retain(hint);
    if (a_truthy(a_or(a_eq(hint, a_string("int")), a_eq(hint, a_string("number"))))) {
        __ret = a_array_new(5, a_string("0"), a_string("1"), a_string("-1"), a_string("42"), a_string("100")); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_eq(hint, a_string("str")), a_eq(hint, a_string("string"))))) {
        { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
        __ret = a_array_new(4, a_add(q, q), a_add(a_add(q, a_string("hello")), q), a_add(a_add(q, a_string("a")), q), a_add(a_add(q, a_string("test string")), q)); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(hint, a_string("bool")))) {
        __ret = a_array_new(2, a_string("true"), a_string("false")); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_eq(hint, a_string("array")), a_eq(hint, a_string("list"))))) {
        __ret = a_array_new(3, a_string("[]"), a_string("[1]"), a_string("[1, 2, 3]")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(hint, a_string("map")))) {
        __ret = a_array_new(2, a_add(a_add(a_string("#"), a_from_code(a_int(123))), a_from_code(a_int(125))), a_add(a_add(a_add(a_add(a_add(a_add(a_string("#"), a_from_code(a_int(123))), a_from_code(a_int(34))), a_string("a")), a_from_code(a_int(34))), a_string(": 1")), a_from_code(a_int(125)))); goto __fn_cleanup;
    }
    __ret = a_array_new(4, a_string("0"), a_add(a_add(a_from_code(a_int(34)), a_string("test")), a_from_code(a_int(34))), a_string("true"), a_string("[]")); goto __fn_cleanup;
__fn_cleanup:
    a_release(q);
    a_release(hint);
    return __ret;
}

AValue fn_testgen__guess_param_type(AValue pname) {
    AValue nm = {0};
    AValue __ret = a_void();
    pname = a_retain(pname);
    { AValue __old = nm; nm = a_str_lower(pname); a_release(__old); }
    if (a_truthy(a_or(a_or(a_or(a_or(a_or(a_or(a_or(a_or(a_or(a_str_contains(nm, a_string("count")), a_str_contains(nm, a_string("num"))), a_str_contains(nm, a_string("size"))), a_str_contains(nm, a_string("len"))), a_str_contains(nm, a_string("idx"))), a_str_contains(nm, a_string("index"))), a_eq(nm, a_string("n"))), a_eq(nm, a_string("i"))), a_eq(nm, a_string("x"))), a_eq(nm, a_string("y"))))) {
        __ret = a_string("int"); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_or(a_or(a_or(a_or(a_or(a_str_contains(nm, a_string("name")), a_str_contains(nm, a_string("text"))), a_str_contains(nm, a_string("msg"))), a_str_contains(nm, a_string("str"))), a_str_contains(nm, a_string("label"))), a_str_contains(nm, a_string("desc"))), a_eq(nm, a_string("s"))))) {
        __ret = a_string("str"); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_or(a_or(a_or(a_str_contains(nm, a_string("flag")), a_str_contains(nm, a_string("enabled"))), a_str_contains(nm, a_string("active"))), a_str_contains(nm, a_string("is_"))), a_str_contains(nm, a_string("has_"))))) {
        __ret = a_string("bool"); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_or(a_or(a_str_contains(nm, a_string("items")), a_str_contains(nm, a_string("list"))), a_str_contains(nm, a_string("arr"))), a_str_contains(nm, a_string("elems"))))) {
        __ret = a_string("array"); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_or(a_or(a_str_contains(nm, a_string("opts")), a_str_contains(nm, a_string("config"))), a_str_contains(nm, a_string("options"))), a_str_contains(nm, a_string("settings"))))) {
        __ret = a_string("map"); goto __fn_cleanup;
    }
    __ret = a_string("any"); goto __fn_cleanup;
__fn_cleanup:
    a_release(nm);
    a_release(pname);
    return __ret;
}

AValue fn_testgen__extract_functions(AValue prog_ast) {
    AValue items = {0}, fns = {0}, fname = {0}, params = {0}, param_info = {0}, pname = {0}, ptype = {0}, body = {0}, has_return = {0}, stmts = {0};
    AValue __ret = a_void();
    prog_ast = a_retain(prog_ast);
    { AValue __old = items; items = a_array_get(prog_ast, a_string("items")); a_release(__old); }
    { AValue __old = fns; fns = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0}, fname = {0}, params = {0}, param_info = {0}, pname = {0}, ptype = {0}, body = {0}, has_return = {0}, stmts = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = fname; fname = a_array_get(item, a_string("name")); a_release(__old); }
                if (a_truthy(a_str_starts_with(fname, a_string("_")))) {
                } else {
                    if (a_truthy(a_neq(fname, a_string("main")))) {
                        { AValue __old = params; params = a_array_get(item, a_string("params")); a_release(__old); }
                        { AValue __old = param_info; param_info = a_array_new(0); a_release(__old); }
                        {
                            AValue __iter_arr = a_iterable(params);
                            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                AValue p = {0}, pname = {0}, ptype = {0};
                                p = a_array_get(__iter_arr, a_int(__fi));
                                { AValue __old = pname; pname = a_array_get(p, a_string("name")); a_release(__old); }
                                { AValue __old = ptype; ptype = fn_testgen__guess_param_type(pname); a_release(__old); }
                                { AValue __old = param_info; param_info = a_array_push(param_info, a_map_new(2, "name", pname, "type", ptype)); a_release(__old); }
                                a_release(p);
                                a_release(pname);
                                a_release(ptype);
                            }
                            a_release(__iter_arr);
                        }
                        { AValue __old = body; body = a_array_get(item, a_string("body")); a_release(__old); }
                        { AValue __old = has_return; has_return = a_bool(0); a_release(__old); }
                        if (a_truthy(a_and(a_eq(a_type_of(body), a_string("map")), a_map_has(body, a_string("stmts"))))) {
                            { AValue __old = stmts; stmts = a_array_get(body, a_string("stmts")); a_release(__old); }
                            {
                                AValue __iter_arr = a_iterable(stmts);
                                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                                    AValue st = {0};
                                    st = a_array_get(__iter_arr, a_int(__fi));
                                    if (a_truthy(a_eq(a_array_get(st, a_string("tag")), a_string("Return")))) {
                                        { AValue __old = has_return; has_return = a_bool(1); a_release(__old); }
                                    }
                                    a_release(st);
                                }
                                a_release(__iter_arr);
                            }
                        }
                        { AValue __old = fns; fns = a_array_push(fns, a_map_new(4, "name", fname, "params", param_info, "has_return", has_return, "param_count", a_len(params))); a_release(__old); }
                    }
                }
            }
            a_release(item);
            a_release(fname);
            a_release(params);
            a_release(param_info);
            a_release(pname);
            a_release(ptype);
            a_release(body);
            a_release(has_return);
            a_release(stmts);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(fns); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(fns);
    a_release(fname);
    a_release(params);
    a_release(param_info);
    a_release(pname);
    a_release(ptype);
    a_release(body);
    a_release(has_return);
    a_release(stmts);
    a_release(prog_ast);
    return __ret;
}

AValue fn_testgen__gen_arg_combos(AValue params) {
    AValue p = {0}, vals = {0}, combos = {0}, first = {0}, rest_params = {0}, first_vals = {0}, rest_combos = {0}, combo = {0};
    AValue __ret = a_void();
    params = a_retain(params);
    if (a_truthy(a_eq(a_len(params), a_int(0)))) {
        __ret = a_array_new(1, a_array_new(0)); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_len(params), a_int(1)))) {
        { AValue __old = p; p = a_array_get(params, a_int(0)); a_release(__old); }
        { AValue __old = vals; vals = fn_testgen__edge_values_for_type(a_array_get(p, a_string("type"))); a_release(__old); }
        { AValue __old = combos; combos = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(vals);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue v = {0};
                v = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = combos; combos = a_array_push(combos, a_array_new(1, v)); a_release(__old); }
                a_release(v);
            }
            a_release(__iter_arr);
        }
        __ret = a_retain(combos); goto __fn_cleanup;
    }
    { AValue __old = first; first = a_array_get(params, a_int(0)); a_release(__old); }
    { AValue __old = rest_params; rest_params = a_drop(params, a_int(1)); a_release(__old); }
    { AValue __old = first_vals; first_vals = fn_testgen__edge_values_for_type(a_array_get(first, a_string("type"))); a_release(__old); }
    { AValue __old = rest_combos; rest_combos = fn_testgen__gen_arg_combos(rest_params); a_release(__old); }
    { AValue __old = combos; combos = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(first_vals);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue fv = {0}, combo = {0};
            fv = a_array_get(__iter_arr, a_int(__fi));
            {
                AValue __iter_arr = a_iterable(rest_combos);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue rc = {0}, combo = {0};
                    rc = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = combo; combo = a_concat_arr(a_array_new(1, fv), rc); a_release(__old); }
                    if (a_truthy(a_lt(a_len(combos), a_int(20)))) {
                        { AValue __old = combos; combos = a_array_push(combos, combo); a_release(__old); }
                    }
                    a_release(rc);
                    a_release(combo);
                }
                a_release(__iter_arr);
            }
            a_release(fv);
            a_release(combo);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(combos); goto __fn_cleanup;
__fn_cleanup:
    a_release(p);
    a_release(vals);
    a_release(combos);
    a_release(first);
    a_release(rest_params);
    a_release(first_vals);
    a_release(rest_combos);
    a_release(combo);
    a_release(params);
    return __ret;
}

AValue fn_testgen__gen_test_for_fn(AValue func_info, AValue test_num) {
    AValue fname = {0}, params = {0}, has_ret = {0}, combos = {0}, code = {0}, tn = {0}, arg_str = {0};
    AValue __ret = a_void();
    func_info = a_retain(func_info);
    test_num = a_retain(test_num);
    { AValue __old = fname; fname = a_array_get(func_info, a_string("name")); a_release(__old); }
    { AValue __old = params; params = a_array_get(func_info, a_string("params")); a_release(__old); }
    { AValue __old = has_ret; has_ret = a_array_get(func_info, a_string("has_return")); a_release(__old); }
    { AValue __old = combos; combos = fn_testgen__gen_arg_combos(params); a_release(__old); }
    { AValue __old = code; code = a_string(""); a_release(__old); }
    { AValue __old = tn; tn = a_retain(test_num); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(combos);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue combo = {0}, arg_str = {0};
            combo = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = arg_str; arg_str = a_str_join(combo, a_string(", ")); a_release(__old); }
            { AValue __old = code; code = a_add(a_add(a_add(a_add(a_add(a_add(a_add(code, a_string("  ; --- Test ")), a_to_str(tn)), a_string(": ")), fname), a_string("(")), arg_str), a_string(") ---\n")); a_release(__old); }
            if (a_truthy(has_ret)) {
                { AValue __old = code; code = a_add(a_add(a_add(a_add(a_add(a_add(a_add(code, a_string("  let r_")), a_to_str(tn)), a_string(" = ")), fname), a_string("(")), arg_str), a_string(")\n")); a_release(__old); }
                { AValue __old = code; code = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(code, a_string("  println(")), a_from_code(a_int(34))), a_string("PASS: ")), fname), a_string("(")), arg_str), a_string(") = ")), a_from_code(a_int(34))), a_string(" + to_str(r_")), a_to_str(tn)), a_string("))\n")); a_release(__old); }
            } else {
                { AValue __old = code; code = a_add(a_add(a_add(a_add(a_add(code, a_string("  ")), fname), a_string("(")), arg_str), a_string(")\n")); a_release(__old); }
                { AValue __old = code; code = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(code, a_string("  println(")), a_from_code(a_int(34))), a_string("PASS: ")), fname), a_string("(")), arg_str), a_string(")")), a_from_code(a_int(34))), a_string(")\n")); a_release(__old); }
            }
            { AValue __old = code; code = a_add(code, a_string("\n")); a_release(__old); }
            { AValue __old = tn; tn = a_add(tn, a_int(1)); a_release(__old); }
            a_release(combo);
            a_release(arg_str);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(code); goto __fn_cleanup;
__fn_cleanup:
    a_release(fname);
    a_release(params);
    a_release(has_ret);
    a_release(combos);
    a_release(code);
    a_release(tn);
    a_release(arg_str);
    a_release(func_info);
    a_release(test_num);
    return __ret;
}

AValue fn_testgen_gen_tests(AValue source) {
    AValue prog = {0}, fns = {0}, test_code = {0}, test_num = {0}, fn_tests = {0}, combos = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    { AValue __old = prog; prog = fn_parser_parse(source); a_release(__old); }
    if (a_truthy(a_or(a_neq(a_type_of(prog), a_string("map")), a_neq(a_array_get(prog, a_string("tag")), a_string("Program"))))) {
        __ret = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("; error: could not parse source\nfn main() "), a_from_code(a_int(123))), a_string("\n  println(")), a_from_code(a_int(34))), a_string("ERROR: parse failure")), a_from_code(a_int(34))), a_string(")\n")), a_from_code(a_int(125))), a_string("\n")); goto __fn_cleanup;
    }
    { AValue __old = fns; fns = fn_testgen__extract_functions(prog); a_release(__old); }
    if (a_truthy(a_eq(a_len(fns), a_int(0)))) {
        __ret = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("; no testable functions found\nfn main() "), a_from_code(a_int(123))), a_string("\n  println(")), a_from_code(a_int(34))), a_string("no tests generated")), a_from_code(a_int(34))), a_string(")\n")), a_from_code(a_int(125))), a_string("\n")); goto __fn_cleanup;
    }
    { AValue __old = test_code; test_code = a_string("; Auto-generated tests by std/testgen.a\n\n"); a_release(__old); }
    { AValue __old = test_code; test_code = a_add(test_code, a_string("; Source functions under test:\n")); a_release(__old); }
    { AValue __old = test_code; test_code = a_add(a_add(test_code, source), a_string("\n\n")); a_release(__old); }
    { AValue __old = test_code; test_code = a_add(a_add(a_add(test_code, a_string("fn main() ")), a_from_code(a_int(123))), a_string("\n")); a_release(__old); }
    { AValue __old = test_num; test_num = a_int(1); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(fns);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue func = {0}, fn_tests = {0}, combos = {0};
            func = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = fn_tests; fn_tests = fn_testgen__gen_test_for_fn(func, test_num); a_release(__old); }
            { AValue __old = test_code; test_code = a_add(test_code, fn_tests); a_release(__old); }
            { AValue __old = combos; combos = fn_testgen__gen_arg_combos(a_array_get(func, a_string("params"))); a_release(__old); }
            { AValue __old = test_num; test_num = a_add(test_num, a_len(combos)); a_release(__old); }
            a_release(func);
            a_release(fn_tests);
            a_release(combos);
        }
        a_release(__iter_arr);
    }
    { AValue __old = test_code; test_code = a_add(a_add(a_add(a_add(a_add(a_add(a_add(test_code, a_string("  println(")), a_from_code(a_int(34))), a_string("All ")), a_to_str(a_sub(test_num, a_int(1)))), a_string(" generated tests passed")), a_from_code(a_int(34))), a_string(")\n")); a_release(__old); }
    { AValue __old = test_code; test_code = a_add(a_add(test_code, a_from_code(a_int(125))), a_string("\n")); a_release(__old); }
    __ret = a_retain(test_code); goto __fn_cleanup;
__fn_cleanup:
    a_release(prog);
    a_release(fns);
    a_release(test_code);
    a_release(test_num);
    a_release(fn_tests);
    a_release(combos);
    a_release(source);
    return __ret;
}

AValue fn_testgen_gen_tests_for_file(AValue filepath) {
    AValue source = {0};
    AValue __ret = a_void();
    filepath = a_retain(filepath);
    { AValue __old = source; source = a_io_read_file(filepath); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        __ret = a_add(a_add(a_string("; error: could not read "), filepath), a_string("\n")); goto __fn_cleanup;
    }
    __ret = fn_testgen_gen_tests(source); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(filepath);
    return __ret;
}

AValue fn_testgen_analyze(AValue source) {
    AValue prog = {0}, fns = {0}, all_items = {0}, total_fns = {0}, private_fns = {0}, total_params = {0}, src_lines = {0}, code_lines = {0}, comment_lines = {0}, trimmed = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    { AValue __old = prog; prog = fn_parser_parse(source); a_release(__old); }
    if (a_truthy(a_or(a_neq(a_type_of(prog), a_string("map")), a_neq(a_array_get(prog, a_string("tag")), a_string("Program"))))) {
        __ret = a_map_new(3, "error", a_string("parse failure"), "functions", a_int(0), "lines", a_int(0)); goto __fn_cleanup;
    }
    { AValue __old = fns; fns = fn_testgen__extract_functions(prog); a_release(__old); }
    { AValue __old = all_items; all_items = a_array_get(prog, a_string("items")); a_release(__old); }
    { AValue __old = total_fns; total_fns = a_int(0); a_release(__old); }
    { AValue __old = private_fns; private_fns = a_int(0); a_release(__old); }
    { AValue __old = total_params; total_params = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(all_items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = total_fns; total_fns = a_add(total_fns, a_int(1)); a_release(__old); }
                if (a_truthy(a_str_starts_with(a_array_get(item, a_string("name")), a_string("_")))) {
                    { AValue __old = private_fns; private_fns = a_add(private_fns, a_int(1)); a_release(__old); }
                }
                { AValue __old = total_params; total_params = a_add(total_params, a_len(a_array_get(item, a_string("params")))); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    { AValue __old = src_lines; src_lines = a_str_split(source, a_string("\n")); a_release(__old); }
    { AValue __old = code_lines; code_lines = a_int(0); a_release(__old); }
    { AValue __old = comment_lines; comment_lines = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(src_lines);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue ln = {0}, trimmed = {0};
            ln = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = trimmed; trimmed = a_str_trim(ln); a_release(__old); }
            if (a_truthy(a_eq(a_len(trimmed), a_int(0)))) {
            } else {
                if (a_truthy(a_str_starts_with(trimmed, a_string(";")))) {
                    { AValue __old = comment_lines; comment_lines = a_add(comment_lines, a_int(1)); a_release(__old); }
                } else {
                    { AValue __old = code_lines; code_lines = a_add(code_lines, a_int(1)); a_release(__old); }
                }
            }
            a_release(ln);
            a_release(trimmed);
        }
        a_release(__iter_arr);
    }
    __ret = a_map_new(8, "total_functions", total_fns, "public_functions", a_sub(total_fns, private_fns), "private_functions", private_fns, "testable_functions", a_len(fns), "total_parameters", total_params, "total_lines", a_len(src_lines), "code_lines", code_lines, "comment_lines", comment_lines); goto __fn_cleanup;
__fn_cleanup:
    a_release(prog);
    a_release(fns);
    a_release(all_items);
    a_release(total_fns);
    a_release(private_fns);
    a_release(total_params);
    a_release(src_lines);
    a_release(code_lines);
    a_release(comment_lines);
    a_release(trimmed);
    a_release(source);
    return __ret;
}

AValue fn_path_join(AValue a, AValue b) {
    AValue __ret = a_void();
    a = a_retain(a);
    b = a_retain(b);
    if (a_truthy(a_eq(a, a_string("")))) {
        __ret = a_retain(b); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(b, a_string("")))) {
        __ret = a_retain(a); goto __fn_cleanup;
    }
    if (a_truthy(a_str_starts_with(b, a_string("/")))) {
        __ret = a_retain(b); goto __fn_cleanup;
    }
    if (a_truthy(a_str_ends_with(a, a_string("/")))) {
        __ret = a_str_concat(a, b); goto __fn_cleanup;
    }
    __ret = a_str_concat(a_str_concat(a, a_string("/")), b); goto __fn_cleanup;
__fn_cleanup:
    a_release(a);
    a_release(b);
    return __ret;
}

AValue fn_path_join3(AValue a, AValue b, AValue c) {
    AValue __ret = a_void();
    a = a_retain(a);
    b = a_retain(b);
    c = a_retain(c);
    __ret = fn_path_join(fn_path_join(a, b), c); goto __fn_cleanup;
__fn_cleanup:
    a_release(a);
    a_release(b);
    a_release(c);
    return __ret;
}

AValue fn_path_dirname(AValue p) {
    AValue clean = {0}, idx = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    if (a_truthy(a_eq(p, a_string("/")))) {
        __ret = a_string("/"); goto __fn_cleanup;
    }
    { AValue __old = clean; clean = a_retain(p); a_release(__old); }
    if (a_truthy(a_str_ends_with(clean, a_string("/")))) {
        { AValue __old = clean; clean = a_str_slice(clean, a_int(0), a_sub(a_len(clean), a_int(1))); a_release(__old); }
    }
    { AValue __old = idx; idx = fn_path__last_slash(clean); a_release(__old); }
    if (a_truthy(a_lt(idx, a_int(0)))) {
        __ret = a_string("."); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(idx, a_int(0)))) {
        __ret = a_string("/"); goto __fn_cleanup;
    }
    __ret = a_str_slice(clean, a_int(0), idx); goto __fn_cleanup;
__fn_cleanup:
    a_release(clean);
    a_release(idx);
    a_release(p);
    return __ret;
}

AValue fn_path_basename(AValue p) {
    AValue clean = {0}, idx = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    if (a_truthy(a_eq(p, a_string("/")))) {
        __ret = a_string("/"); goto __fn_cleanup;
    }
    { AValue __old = clean; clean = a_retain(p); a_release(__old); }
    if (a_truthy(a_str_ends_with(clean, a_string("/")))) {
        { AValue __old = clean; clean = a_str_slice(clean, a_int(0), a_sub(a_len(clean), a_int(1))); a_release(__old); }
    }
    { AValue __old = idx; idx = fn_path__last_slash(clean); a_release(__old); }
    if (a_truthy(a_lt(idx, a_int(0)))) {
        __ret = a_retain(clean); goto __fn_cleanup;
    }
    __ret = a_str_slice(clean, a_add(idx, a_int(1)), a_len(clean)); goto __fn_cleanup;
__fn_cleanup:
    a_release(clean);
    a_release(idx);
    a_release(p);
    return __ret;
}

AValue fn_path_extension(AValue p) {
    AValue base = {0}, idx = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    { AValue __old = base; base = fn_path_basename(p); a_release(__old); }
    { AValue __old = idx; idx = fn_path__last_dot(base); a_release(__old); }
    if (a_truthy(a_lteq(idx, a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    __ret = a_str_slice(base, a_add(idx, a_int(1)), a_len(base)); goto __fn_cleanup;
__fn_cleanup:
    a_release(base);
    a_release(idx);
    a_release(p);
    return __ret;
}

AValue fn_path_stem(AValue p) {
    AValue base = {0}, idx = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    { AValue __old = base; base = fn_path_basename(p); a_release(__old); }
    { AValue __old = idx; idx = fn_path__last_dot(base); a_release(__old); }
    if (a_truthy(a_lteq(idx, a_int(0)))) {
        __ret = a_retain(base); goto __fn_cleanup;
    }
    __ret = a_str_slice(base, a_int(0), idx); goto __fn_cleanup;
__fn_cleanup:
    a_release(base);
    a_release(idx);
    a_release(p);
    return __ret;
}

AValue fn_path_with_extension(AValue p, AValue ext) {
    AValue dir = {0}, s = {0}, new_base = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    ext = a_retain(ext);
    { AValue __old = dir; dir = fn_path_dirname(p); a_release(__old); }
    { AValue __old = s; s = fn_path_stem(p); a_release(__old); }
    { AValue __old = new_base; new_base = a_retain(s); a_release(__old); }
    if (a_truthy(a_neq(ext, a_string("")))) {
        { AValue __old = new_base; new_base = a_str_concat(a_str_concat(s, a_string(".")), ext); a_release(__old); }
    }
    if (a_truthy(a_eq(dir, a_string(".")))) {
        __ret = a_retain(new_base); goto __fn_cleanup;
    }
    __ret = fn_path_join(dir, new_base); goto __fn_cleanup;
__fn_cleanup:
    a_release(dir);
    a_release(s);
    a_release(new_base);
    a_release(p);
    a_release(ext);
    return __ret;
}

AValue fn_path_is_absolute(AValue p) {
    AValue __ret = a_void();
    p = a_retain(p);
    __ret = a_str_starts_with(p, a_string("/")); goto __fn_cleanup;
__fn_cleanup:
    a_release(p);
    return __ret;
}

AValue fn_path_segments(AValue p) {
    AValue parts = {0}, out = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    if (a_truthy(a_eq(p, a_string("")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    { AValue __old = parts; parts = a_str_split(p, a_string("/")); a_release(__old); }
    { AValue __old = out; out = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(parts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue part = {0};
            part = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_neq(part, a_string("")))) {
                { AValue __old = out; out = a_array_push(out, part); a_release(__old); }
            }
            a_release(part);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(parts);
    a_release(out);
    a_release(p);
    return __ret;
}

AValue fn_path_normalize(AValue p) {
    AValue is_abs = {0}, parts = {0}, stack = {0}, top = {0}, result = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    { AValue __old = is_abs; is_abs = a_str_starts_with(p, a_string("/")); a_release(__old); }
    { AValue __old = parts; parts = a_str_split(p, a_string("/")); a_release(__old); }
    { AValue __old = stack; stack = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(parts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue part = {0}, top = {0};
            part = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(part, a_string("")))) {
                continue;
            }
            if (a_truthy(a_eq(part, a_string(".")))) {
                continue;
            }
            if (a_truthy(a_eq(part, a_string("..")))) {
                if (a_truthy(a_gt(a_len(stack), a_int(0)))) {
                    { AValue __old = top; top = a_array_get(stack, a_sub(a_len(stack), a_int(1))); a_release(__old); }
                    if (a_truthy(a_neq(top, a_string("..")))) {
                        { AValue __old = stack; stack = a_array_slice(stack, a_int(0), a_sub(a_len(stack), a_int(1))); a_release(__old); }
                        continue;
                    }
                }
                if (a_truthy(a_not(is_abs))) {
                    { AValue __old = stack; stack = a_array_push(stack, a_string("..")); a_release(__old); }
                }
                continue;
            }
            { AValue __old = stack; stack = a_array_push(stack, part); a_release(__old); }
            a_release(part);
            a_release(top);
        }
        a_release(__iter_arr);
    }
    { AValue __old = result; result = a_str_join(stack, a_string("/")); a_release(__old); }
    if (a_truthy(is_abs)) {
        __ret = a_str_concat(a_string("/"), result); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(result, a_string("")))) {
        __ret = a_string("."); goto __fn_cleanup;
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(is_abs);
    a_release(parts);
    a_release(stack);
    a_release(top);
    a_release(result);
    a_release(p);
    return __ret;
}

AValue fn_path__last_slash(AValue s) {
    AValue chars = {0}, last = {0}, i = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    { AValue __old = last; last = a_neg(a_int(1)); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(chars);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(c, a_string("/")))) {
                { AValue __old = last; last = a_retain(i); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            a_release(c);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(last); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(last);
    a_release(i);
    a_release(s);
    return __ret;
}

AValue fn_path__last_dot(AValue s) {
    AValue chars = {0}, last = {0}, i = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    { AValue __old = last; last = a_neg(a_int(1)); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(chars);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(c, a_string(".")))) {
                { AValue __old = last; last = a_retain(i); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            a_release(c);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(last); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(last);
    a_release(i);
    a_release(s);
    return __ret;
}

AValue fn_path_home(void) {
    AValue h = {0};
    AValue __ret = a_void();
    { AValue __old = h; h = a_env_get(a_string("HOME")); a_release(__old); }
    if (a_truthy(a_neq(h, a_string("")))) {
        __ret = a_retain(h); goto __fn_cleanup;
    }
    __ret = a_env_get(a_string("USERPROFILE")); goto __fn_cleanup;
__fn_cleanup:
    a_release(h);
    return __ret;
}

AValue fn_path_temp(void) {
    AValue t = {0}, t2 = {0};
    AValue __ret = a_void();
    { AValue __old = t; t = a_env_get(a_string("TMPDIR")); a_release(__old); }
    if (a_truthy(a_neq(t, a_string("")))) {
        __ret = a_retain(t); goto __fn_cleanup;
    }
    { AValue __old = t2; t2 = a_env_get(a_string("TEMP")); a_release(__old); }
    if (a_truthy(a_neq(t2, a_string("")))) {
        __ret = a_retain(t2); goto __fn_cleanup;
    }
    __ret = a_string("/tmp"); goto __fn_cleanup;
__fn_cleanup:
    a_release(t);
    a_release(t2);
    return __ret;
}

AValue fn_path_relative(AValue from, AValue to) {
    AValue f_parts = {0}, t_parts = {0}, common = {0}, result = {0}, ups = {0}, i = {0}, j = {0};
    AValue __ret = a_void();
    from = a_retain(from);
    to = a_retain(to);
    { AValue __old = f_parts; f_parts = fn_path_segments(fn_path_normalize(from)); a_release(__old); }
    { AValue __old = t_parts; t_parts = fn_path_segments(fn_path_normalize(to)); a_release(__old); }
    { AValue __old = common; common = a_int(0); a_release(__old); }
    while (a_truthy(a_and(a_lt(common, a_len(f_parts)), a_lt(common, a_len(t_parts))))) {
        if (a_truthy(a_neq(a_array_get(f_parts, common), a_array_get(t_parts, common)))) {
            break;
        }
        { AValue __old = common; common = a_add(common, a_int(1)); a_release(__old); }
    }
    { AValue __old = result; result = a_string(""); a_release(__old); }
    { AValue __old = ups; ups = a_sub(a_len(f_parts), common); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, ups))) {
        if (a_truthy(a_neq(result, a_string("")))) {
            { AValue __old = result; result = a_str_concat(result, a_string("/")); a_release(__old); }
        }
        { AValue __old = result; result = a_str_concat(result, a_string("..")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    { AValue __old = j; j = a_retain(common); a_release(__old); }
    while (a_truthy(a_lt(j, a_len(t_parts)))) {
        if (a_truthy(a_neq(result, a_string("")))) {
            { AValue __old = result; result = a_str_concat(result, a_string("/")); a_release(__old); }
        }
        { AValue __old = result; result = a_str_concat(result, a_array_get(t_parts, j)); a_release(__old); }
        { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
    }
    if (a_truthy(a_eq(result, a_string("")))) {
        __ret = a_string("."); goto __fn_cleanup;
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(f_parts);
    a_release(t_parts);
    a_release(common);
    a_release(result);
    a_release(ups);
    a_release(i);
    a_release(j);
    a_release(from);
    a_release(to);
    return __ret;
}

AValue fn_path_has_extension(AValue p, AValue ext) {
    AValue __ret = a_void();
    p = a_retain(p);
    ext = a_retain(ext);
    __ret = a_eq(fn_path_extension(p), ext); goto __fn_cleanup;
__fn_cleanup:
    a_release(p);
    a_release(ext);
    return __ret;
}

AValue fn_path_join_all(AValue parts) {
    AValue result = {0}, i = {0};
    AValue __ret = a_void();
    parts = a_retain(parts);
    if (a_truthy(a_eq(a_len(parts), a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = result; result = a_array_get(parts, a_int(0)); a_release(__old); }
    { AValue __old = i; i = a_int(1); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(parts)))) {
        { AValue __old = result; result = fn_path_join(result, a_array_get(parts, i)); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(result);
    a_release(i);
    a_release(parts);
    return __ret;
}

AValue fn_path_is_hidden(AValue p) {
    AValue base = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    { AValue __old = base; base = fn_path_basename(p); a_release(__old); }
    __ret = a_str_starts_with(base, a_string(".")); goto __fn_cleanup;
__fn_cleanup:
    a_release(base);
    a_release(p);
    return __ret;
}

AValue fn_cli_esc_code(void) {
    AValue __ret = a_void();
    __ret = a_from_code(a_int(27)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_wrap(AValue s, AValue code) {
    AValue e = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    code = a_retain(code);
    { AValue __old = e; e = fn_cli_esc_code(); a_release(__old); }
    __ret = a_concat_n(7, a_to_str(e), a_string("["), a_to_str(code), a_string("m"), a_to_str(s), a_to_str(e), a_string("[0m")); goto __fn_cleanup;
__fn_cleanup:
    a_release(e);
    a_release(s);
    a_release(code);
    return __ret;
}

AValue fn_cli_red(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("31")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_green(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("32")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_yellow(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("33")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_blue(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("34")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_magenta(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("35")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_cyan(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("36")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_gray(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("90")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_bold(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("1")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_dim(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("2")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_cli_underline(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = fn_cli_wrap(s, a_string("4")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_toml__t_trim(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = a_str_trim(s); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_toml__t_is_int(AValue s) {
    AValue chars = {0}, n = {0}, start = {0}, i = {0}, c = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    if (a_truthy(a_eq(n, a_int(0)))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = start; start = a_int(0); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("-")))) {
        { AValue __old = start; start = a_int(1); a_release(__old); }
    }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("+")))) {
        { AValue __old = start; start = a_int(1); a_release(__old); }
    }
    if (a_truthy(a_gteq(start, n))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = i; i = a_retain(start); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = c; c = a_array_get(chars, i); a_release(__old); }
        if (a_truthy(a_eq(a_is_digit(c), a_bool(0)))) {
            if (a_truthy(a_neq(c, a_string("_")))) {
                __ret = a_bool(0); goto __fn_cleanup;
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_bool(1); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(n);
    a_release(start);
    a_release(i);
    a_release(c);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_is_float(AValue s) {
    AValue chars = {0}, n = {0}, start = {0}, has_dot = {0}, has_dig = {0}, i = {0}, c = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    if (a_truthy(a_eq(n, a_int(0)))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = start; start = a_int(0); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("-")))) {
        { AValue __old = start; start = a_int(1); a_release(__old); }
    }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("+")))) {
        { AValue __old = start; start = a_int(1); a_release(__old); }
    }
    if (a_truthy(a_gteq(start, n))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = has_dot; has_dot = a_bool(0); a_release(__old); }
    { AValue __old = has_dig; has_dig = a_bool(0); a_release(__old); }
    { AValue __old = i; i = a_retain(start); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = c; c = a_array_get(chars, i); a_release(__old); }
        if (a_truthy(a_eq(c, a_string(".")))) {
            if (a_truthy(has_dot)) {
                __ret = a_bool(0); goto __fn_cleanup;
            }
            { AValue __old = has_dot; has_dot = a_bool(1); a_release(__old); }
        } else {
            if (a_truthy(a_is_digit(c))) {
                { AValue __old = has_dig; has_dig = a_bool(1); a_release(__old); }
            } else {
                if (a_truthy(a_neq(c, a_string("_")))) {
                    __ret = a_bool(0); goto __fn_cleanup;
                }
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    if (a_truthy(a_eq(has_dot, a_bool(0)))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    __ret = a_retain(has_dig); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(n);
    a_release(start);
    a_release(has_dot);
    a_release(has_dig);
    a_release(i);
    a_release(c);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_strip_underscores(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    __ret = a_str_replace(s, a_string("_"), a_string("")); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_toml__t_parse_value(AValue s) {
    AValue t = {0}, chars = {0}, n = {0}, sq = {0}, last = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = t; t = fn_toml__t_trim(s); a_release(__old); }
    if (a_truthy(a_eq(t, a_string("true")))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(t, a_string("false")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = chars; chars = a_str_chars(t); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    if (a_truthy(a_eq(n, a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("\"")))) {
        if (a_truthy(a_gteq(n, a_int(2)))) {
            if (a_truthy(a_eq(a_array_get(chars, a_sub(n, a_int(1))), a_string("\"")))) {
                __ret = a_str_slice(t, a_int(1), a_sub(n, a_int(1))); goto __fn_cleanup;
            }
        }
        __ret = a_retain(t); goto __fn_cleanup;
    }
    { AValue __old = sq; sq = a_from_code(a_int(39)); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), sq))) {
        if (a_truthy(a_gteq(n, a_int(2)))) {
            { AValue __old = last; last = a_array_get(chars, a_sub(n, a_int(1))); a_release(__old); }
            if (a_truthy(a_eq(a_char_code(last), a_int(39)))) {
                __ret = a_str_slice(t, a_int(1), a_sub(n, a_int(1))); goto __fn_cleanup;
            }
        }
        __ret = a_retain(t); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("[")))) {
        __ret = fn_toml__t_parse_array(t); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("{")))) {
        __ret = fn_toml__t_parse_inline_table(t); goto __fn_cleanup;
    }
    if (a_truthy(fn_toml__t_is_float(t))) {
        __ret = a_to_float(fn_toml__t_strip_underscores(t)); goto __fn_cleanup;
    }
    if (a_truthy(fn_toml__t_is_int(t))) {
        __ret = a_to_int(fn_toml__t_strip_underscores(t)); goto __fn_cleanup;
    }
    __ret = a_retain(t); goto __fn_cleanup;
__fn_cleanup:
    a_release(t);
    a_release(chars);
    a_release(n);
    a_release(sq);
    a_release(last);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_split_array(AValue s) {
    AValue inner = {0}, chars = {0}, n = {0}, parts = {0}, cur = {0}, depth = {0}, in_dq = {0}, in_sq = {0}, i = {0}, c = {0}, trimmed = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = inner; inner = fn_toml__t_trim(a_str_slice(s, a_int(1), a_sub(a_len(s), a_int(1)))); a_release(__old); }
    if (a_truthy(a_eq(inner, a_string("")))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    { AValue __old = chars; chars = a_str_chars(inner); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
    { AValue __old = cur; cur = a_string(""); a_release(__old); }
    { AValue __old = depth; depth = a_int(0); a_release(__old); }
    { AValue __old = in_dq; in_dq = a_bool(0); a_release(__old); }
    { AValue __old = in_sq; in_sq = a_bool(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = c; c = a_array_get(chars, i); a_release(__old); }
        if (a_truthy(in_dq)) {
            { AValue __old = cur; cur = a_str_concat(cur, c); a_release(__old); }
            if (a_truthy(a_eq(c, a_string("\"")))) {
                { AValue __old = in_dq; in_dq = a_bool(0); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(in_sq)) {
            { AValue __old = cur; cur = a_str_concat(cur, c); a_release(__old); }
            if (a_truthy(a_eq(a_char_code(c), a_int(39)))) {
                { AValue __old = in_sq; in_sq = a_bool(0); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("\"")))) {
            { AValue __old = in_dq; in_dq = a_bool(1); a_release(__old); }
            { AValue __old = cur; cur = a_str_concat(cur, c); a_release(__old); }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(a_char_code(c), a_int(39)))) {
            { AValue __old = in_sq; in_sq = a_bool(1); a_release(__old); }
            { AValue __old = cur; cur = a_str_concat(cur, c); a_release(__old); }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("[")))) {
            { AValue __old = depth; depth = a_add(depth, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(c, a_string("]")))) {
            { AValue __old = depth; depth = a_sub(depth, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(c, a_string("{")))) {
            { AValue __old = depth; depth = a_add(depth, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(c, a_string("}")))) {
            { AValue __old = depth; depth = a_sub(depth, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_eq(c, a_string(",")))) {
            if (a_truthy(a_eq(depth, a_int(0)))) {
                { AValue __old = trimmed; trimmed = fn_toml__t_trim(cur); a_release(__old); }
                if (a_truthy(a_neq(trimmed, a_string("")))) {
                    { AValue __old = parts; parts = a_array_push(parts, trimmed); a_release(__old); }
                }
                { AValue __old = cur; cur = a_string(""); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
                continue;
            }
        }
        if (a_truthy(a_eq(c, a_string("#")))) {
            if (a_truthy(a_eq(depth, a_int(0)))) {
                { AValue __old = trimmed; trimmed = fn_toml__t_trim(cur); a_release(__old); }
                if (a_truthy(a_neq(trimmed, a_string("")))) {
                    { AValue __old = parts; parts = a_array_push(parts, trimmed); a_release(__old); }
                }
                __ret = a_retain(parts); goto __fn_cleanup;
            }
        }
        { AValue __old = cur; cur = a_str_concat(cur, c); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    { AValue __old = trimmed; trimmed = fn_toml__t_trim(cur); a_release(__old); }
    if (a_truthy(a_neq(trimmed, a_string("")))) {
        { AValue __old = parts; parts = a_array_push(parts, trimmed); a_release(__old); }
    }
    __ret = a_retain(parts); goto __fn_cleanup;
__fn_cleanup:
    a_release(inner);
    a_release(chars);
    a_release(n);
    a_release(parts);
    a_release(cur);
    a_release(depth);
    a_release(in_dq);
    a_release(in_sq);
    a_release(i);
    a_release(c);
    a_release(trimmed);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_parse_array(AValue s) {
    AValue items = {0}, result = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = items; items = fn_toml__t_split_array(s); a_release(__old); }
    { AValue __old = result; result = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = result; result = a_array_push(result, fn_toml__t_parse_value(item)); a_release(__old); }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(result);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_parse_inline_table(AValue s) {
    AValue inner = {0}, empty = {0}, parts = {0}, result = {0}, trimmed = {0}, eq_pos = {0}, key = {0}, val = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = inner; inner = fn_toml__t_trim(a_str_slice(s, a_int(1), a_sub(a_len(s), a_int(1)))); a_release(__old); }
    if (a_truthy(a_eq(inner, a_string("")))) {
        { AValue __old = empty; empty = a_map_new(0); a_release(__old); }
        __ret = a_retain(empty); goto __fn_cleanup;
    }
    { AValue __old = parts; parts = a_str_split(inner, a_string(",")); a_release(__old); }
    { AValue __old = result; result = a_map_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(parts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue part = {0}, trimmed = {0}, eq_pos = {0}, key = {0}, val = {0};
            part = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = trimmed; trimmed = fn_toml__t_trim(part); a_release(__old); }
            if (a_truthy(a_eq(trimmed, a_string("")))) {
                continue;
            }
            { AValue __old = eq_pos; eq_pos = fn_toml__t_find_eq(trimmed); a_release(__old); }
            if (a_truthy(a_gt(eq_pos, a_int(0)))) {
                { AValue __old = key; key = fn_toml__t_trim(a_str_slice(trimmed, a_int(0), eq_pos)); a_release(__old); }
                { AValue __old = val; val = fn_toml__t_trim(a_str_slice(trimmed, a_add(eq_pos, a_int(1)), a_len(trimmed))); a_release(__old); }
                { AValue __old = result; result = a_map_set(result, key, fn_toml__t_parse_value(val)); a_release(__old); }
            }
            a_release(part);
            a_release(trimmed);
            a_release(eq_pos);
            a_release(key);
            a_release(val);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(inner);
    a_release(empty);
    a_release(parts);
    a_release(result);
    a_release(trimmed);
    a_release(eq_pos);
    a_release(key);
    a_release(val);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_find_eq(AValue s) {
    AValue chars = {0}, n = {0}, in_dq = {0}, in_sq = {0}, i = {0}, c = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    { AValue __old = in_dq; in_dq = a_bool(0); a_release(__old); }
    { AValue __old = in_sq; in_sq = a_bool(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = c; c = a_array_get(chars, i); a_release(__old); }
        if (a_truthy(in_dq)) {
            if (a_truthy(a_eq(c, a_string("\"")))) {
                { AValue __old = in_dq; in_dq = a_bool(0); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(in_sq)) {
            if (a_truthy(a_eq(a_char_code(c), a_int(39)))) {
                { AValue __old = in_sq; in_sq = a_bool(0); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("\"")))) {
            { AValue __old = in_dq; in_dq = a_bool(1); a_release(__old); }
        }
        if (a_truthy(a_eq(a_char_code(c), a_int(39)))) {
            { AValue __old = in_sq; in_sq = a_bool(1); a_release(__old); }
        }
        if (a_truthy(a_eq(c, a_string("=")))) {
            __ret = a_retain(i); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_sub(a_int(0), a_int(1)); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(n);
    a_release(in_dq);
    a_release(in_sq);
    a_release(i);
    a_release(c);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_strip_comment(AValue s) {
    AValue chars = {0}, n = {0}, in_dq = {0}, in_sq = {0}, i = {0}, c = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    { AValue __old = n; n = a_len(chars); a_release(__old); }
    { AValue __old = in_dq; in_dq = a_bool(0); a_release(__old); }
    { AValue __old = in_sq; in_sq = a_bool(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = c; c = a_array_get(chars, i); a_release(__old); }
        if (a_truthy(in_dq)) {
            if (a_truthy(a_eq(c, a_string("\"")))) {
                { AValue __old = in_dq; in_dq = a_bool(0); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(in_sq)) {
            if (a_truthy(a_eq(a_char_code(c), a_int(39)))) {
                { AValue __old = in_sq; in_sq = a_bool(0); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_eq(c, a_string("\"")))) {
            { AValue __old = in_dq; in_dq = a_bool(1); a_release(__old); }
        }
        if (a_truthy(a_eq(a_char_code(c), a_int(39)))) {
            { AValue __old = in_sq; in_sq = a_bool(1); a_release(__old); }
        }
        if (a_truthy(a_eq(c, a_string("#")))) {
            __ret = fn_toml__t_trim(a_str_slice(s, a_int(0), i)); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(s); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(n);
    a_release(in_dq);
    a_release(in_sq);
    a_release(i);
    a_release(c);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_set_nested(AValue root, AValue path, AValue val) {
    AValue key = {0}, rest = {0}, existing = {0}, updated = {0};
    AValue __ret = a_void();
    root = a_retain(root);
    path = a_retain(path);
    val = a_retain(val);
    if (a_truthy(a_eq(a_len(path), a_int(0)))) {
        __ret = a_retain(root); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(a_len(path), a_int(1)))) {
        __ret = a_map_set(root, a_array_get(path, a_int(0)), val); goto __fn_cleanup;
    }
    { AValue __old = key; key = a_array_get(path, a_int(0)); a_release(__old); }
    { AValue __old = rest; rest = a_drop(path, a_int(1)); a_release(__old); }
    { AValue __old = existing; existing = a_map_new(0); a_release(__old); }
    if (a_truthy(a_map_has(root, key))) {
        { AValue __old = existing; existing = a_map_get(root, key); a_release(__old); }
    }
    { AValue __old = updated; updated = fn_toml__t_set_nested(existing, rest, val); a_release(__old); }
    __ret = a_map_set(root, key, updated); goto __fn_cleanup;
__fn_cleanup:
    a_release(key);
    a_release(rest);
    a_release(existing);
    a_release(updated);
    a_release(root);
    a_release(path);
    a_release(val);
    return __ret;
}

AValue fn_toml__t_get_nested(AValue root, AValue path) {
    AValue cur = {0}, empty = {0};
    AValue __ret = a_void();
    root = a_retain(root);
    path = a_retain(path);
    { AValue __old = cur; cur = a_retain(root); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(path);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue key = {0}, empty = {0};
            key = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_map_has(cur, key), a_bool(0)))) {
                { AValue __old = empty; empty = a_map_new(0); a_release(__old); }
                __ret = a_retain(empty); goto __fn_cleanup;
            }
            { AValue __old = cur; cur = a_map_get(cur, key); a_release(__old); }
            a_release(key);
            a_release(empty);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(cur); goto __fn_cleanup;
__fn_cleanup:
    a_release(cur);
    a_release(empty);
    a_release(root);
    a_release(path);
    return __ret;
}

AValue fn_toml__t_parse_key(AValue s) {
    AValue parts = {0}, result = {0}, t = {0}, chars = {0}, n = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = parts; parts = a_str_split(s, a_string(".")); a_release(__old); }
    { AValue __old = result; result = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(parts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue p = {0}, t = {0}, chars = {0}, n = {0};
            p = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = t; t = fn_toml__t_trim(p); a_release(__old); }
            { AValue __old = chars; chars = a_str_chars(t); a_release(__old); }
            { AValue __old = n; n = a_len(chars); a_release(__old); }
            if (a_truthy(a_gteq(n, a_int(2)))) {
                if (a_truthy(a_eq(a_array_get(chars, a_int(0)), a_string("\"")))) {
                    if (a_truthy(a_eq(a_array_get(chars, a_sub(n, a_int(1))), a_string("\"")))) {
                        { AValue __old = result; result = a_array_push(result, a_str_slice(t, a_int(1), a_sub(n, a_int(1)))); a_release(__old); }
                        continue;
                    }
                }
            }
            { AValue __old = result; result = a_array_push(result, t); a_release(__old); }
            a_release(p);
            a_release(t);
            a_release(chars);
            a_release(n);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(parts);
    a_release(result);
    a_release(t);
    a_release(chars);
    a_release(n);
    a_release(s);
    return __ret;
}

AValue fn_toml_parse(AValue text) {
    AValue lines = {0}, result = {0}, current_path = {0}, n = {0}, i = {0}, raw = {0}, line = {0}, table_name = {0}, path = {0}, existing = {0}, t = {0}, new_entry = {0}, updated = {0}, arr = {0}, empty = {0}, eq_pos = {0}, key_str = {0}, val_str = {0}, key_parts = {0}, val = {0}, arr_len = {0}, last = {0}, updated_last = {0}, new_arr = {0}, j = {0}, full_path = {0};
    AValue __ret = a_void();
    text = a_retain(text);
    { AValue __old = lines; lines = a_str_lines(a_str_replace(text, a_string("\r"), a_string(""))); a_release(__old); }
    { AValue __old = result; result = a_map_new(0); a_release(__old); }
    { AValue __old = current_path; current_path = a_array_new(0); a_release(__old); }
    { AValue __old = n; n = a_len(lines); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, n))) {
        { AValue __old = raw; raw = a_array_get(lines, i); a_release(__old); }
        { AValue __old = line; line = fn_toml__t_trim(fn_toml__t_strip_comment(raw)); a_release(__old); }
        if (a_truthy(a_eq(line, a_string("")))) {
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        if (a_truthy(a_str_starts_with(line, a_string("[[")))) {
            if (a_truthy(a_str_ends_with(line, a_string("]]")))) {
                { AValue __old = table_name; table_name = fn_toml__t_trim(a_str_slice(line, a_int(2), a_sub(a_len(line), a_int(2)))); a_release(__old); }
                { AValue __old = path; path = fn_toml__t_parse_key(table_name); a_release(__old); }
                { AValue __old = current_path; current_path = a_retain(path); a_release(__old); }
                { AValue __old = existing; existing = fn_toml__t_get_nested(result, path); a_release(__old); }
                { AValue __old = t; t = a_type_of(existing); a_release(__old); }
                if (a_truthy(a_eq(t, a_string("array")))) {
                    { AValue __old = new_entry; new_entry = a_map_new(0); a_release(__old); }
                    { AValue __old = updated; updated = a_array_push(existing, new_entry); a_release(__old); }
                    { AValue __old = result; result = fn_toml__t_set_nested(result, path, updated); a_release(__old); }
                } else {
                    { AValue __old = new_entry; new_entry = a_map_new(0); a_release(__old); }
                    { AValue __old = arr; arr = a_array_new(1, new_entry); a_release(__old); }
                    { AValue __old = result; result = fn_toml__t_set_nested(result, path, arr); a_release(__old); }
                }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
                continue;
            }
        }
        if (a_truthy(a_str_starts_with(line, a_string("[")))) {
            if (a_truthy(a_str_ends_with(line, a_string("]")))) {
                { AValue __old = table_name; table_name = fn_toml__t_trim(a_str_slice(line, a_int(1), a_sub(a_len(line), a_int(1)))); a_release(__old); }
                { AValue __old = current_path; current_path = fn_toml__t_parse_key(table_name); a_release(__old); }
                { AValue __old = existing; existing = fn_toml__t_get_nested(result, current_path); a_release(__old); }
                { AValue __old = t; t = a_type_of(existing); a_release(__old); }
                if (a_truthy(a_neq(t, a_string("map")))) {
                    { AValue __old = empty; empty = a_map_new(0); a_release(__old); }
                    { AValue __old = result; result = fn_toml__t_set_nested(result, current_path, empty); a_release(__old); }
                }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
                continue;
            }
        }
        { AValue __old = eq_pos; eq_pos = fn_toml__t_find_eq(line); a_release(__old); }
        if (a_truthy(a_gt(eq_pos, a_int(0)))) {
            { AValue __old = key_str; key_str = fn_toml__t_trim(a_str_slice(line, a_int(0), eq_pos)); a_release(__old); }
            { AValue __old = val_str; val_str = fn_toml__t_trim(a_str_slice(line, a_add(eq_pos, a_int(1)), a_len(line))); a_release(__old); }
            { AValue __old = key_parts; key_parts = fn_toml__t_parse_key(key_str); a_release(__old); }
            { AValue __old = val; val = fn_toml__t_parse_value(val_str); a_release(__old); }
            if (a_truthy(a_gt(a_len(current_path), a_int(0)))) {
                { AValue __old = existing; existing = fn_toml__t_get_nested(result, current_path); a_release(__old); }
                { AValue __old = t; t = a_type_of(existing); a_release(__old); }
                if (a_truthy(a_eq(t, a_string("array")))) {
                    { AValue __old = arr_len; arr_len = a_len(existing); a_release(__old); }
                    if (a_truthy(a_gt(arr_len, a_int(0)))) {
                        { AValue __old = last; last = a_array_get(existing, a_sub(arr_len, a_int(1))); a_release(__old); }
                        { AValue __old = updated_last; updated_last = fn_toml__t_set_nested(last, key_parts, val); a_release(__old); }
                        { AValue __old = new_arr; new_arr = a_array_new(0); a_release(__old); }
                        { AValue __old = j; j = a_int(0); a_release(__old); }
                        while (a_truthy(a_lt(j, a_sub(arr_len, a_int(1))))) {
                            { AValue __old = new_arr; new_arr = a_array_push(new_arr, a_array_get(existing, j)); a_release(__old); }
                            { AValue __old = j; j = a_add(j, a_int(1)); a_release(__old); }
                        }
                        { AValue __old = new_arr; new_arr = a_array_push(new_arr, updated_last); a_release(__old); }
                        { AValue __old = result; result = fn_toml__t_set_nested(result, current_path, new_arr); a_release(__old); }
                    }
                } else {
                    { AValue __old = full_path; full_path = a_array_new(0); a_release(__old); }
                    {
                        AValue __iter_arr = a_iterable(current_path);
                        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                            AValue p = {0};
                            p = a_array_get(__iter_arr, a_int(__fi));
                            { AValue __old = full_path; full_path = a_array_push(full_path, p); a_release(__old); }
                            a_release(p);
                        }
                        a_release(__iter_arr);
                    }
                    {
                        AValue __iter_arr = a_iterable(key_parts);
                        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                            AValue p = {0};
                            p = a_array_get(__iter_arr, a_int(__fi));
                            { AValue __old = full_path; full_path = a_array_push(full_path, p); a_release(__old); }
                            a_release(p);
                        }
                        a_release(__iter_arr);
                    }
                    { AValue __old = result; result = fn_toml__t_set_nested(result, full_path, val); a_release(__old); }
                }
            } else {
                { AValue __old = result; result = fn_toml__t_set_nested(result, key_parts, val); a_release(__old); }
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(lines);
    a_release(result);
    a_release(current_path);
    a_release(n);
    a_release(i);
    a_release(raw);
    a_release(line);
    a_release(table_name);
    a_release(path);
    a_release(existing);
    a_release(t);
    a_release(new_entry);
    a_release(updated);
    a_release(arr);
    a_release(empty);
    a_release(eq_pos);
    a_release(key_str);
    a_release(val_str);
    a_release(key_parts);
    a_release(val);
    a_release(arr_len);
    a_release(last);
    a_release(updated_last);
    a_release(new_arr);
    a_release(j);
    a_release(full_path);
    a_release(text);
    return __ret;
}

AValue fn_toml__t_needs_quote(AValue s) {
    AValue chars = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    if (a_truthy(a_eq(s, a_string("")))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    { AValue __old = chars; chars = a_str_chars(s); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(chars);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue c = {0};
            c = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(c, a_string(".")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string(" ")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string("=")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string("#")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string("{")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string("}")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string("[")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            if (a_truthy(a_eq(c, a_string("]")))) {
                __ret = a_bool(1); goto __fn_cleanup;
            }
            a_release(c);
        }
        a_release(__iter_arr);
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(chars);
    a_release(s);
    return __ret;
}

AValue fn_toml__t_quote_key(AValue s) {
    AValue __ret = a_void();
    s = a_retain(s);
    if (a_truthy(fn_toml__t_needs_quote(s))) {
        __ret = a_str_concat(a_string("\""), a_str_concat(s, a_string("\""))); goto __fn_cleanup;
    }
    __ret = a_retain(s); goto __fn_cleanup;
__fn_cleanup:
    a_release(s);
    return __ret;
}

AValue fn_toml__t_stringify_value(AValue val) {
    AValue t = {0}, parts = {0}, keys = {0}, v = {0}, ks = {0}, vs = {0};
    AValue __ret = a_void();
    val = a_retain(val);
    { AValue __old = t; t = a_type_of(val); a_release(__old); }
    if (a_truthy(a_eq(t, a_string("bool")))) {
        if (a_truthy(val)) {
            __ret = a_string("true"); goto __fn_cleanup;
        }
        __ret = a_string("false"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(t, a_string("int")))) {
        __ret = a_to_str(val); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(t, a_string("float")))) {
        __ret = a_to_str(val); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(t, a_string("str")))) {
        __ret = a_str_concat(a_string("\""), a_str_concat(a_str_replace(val, a_string("\""), a_string("\\\"")), a_string("\""))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(t, a_string("array")))) {
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(val);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue item = {0};
                item = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = parts; parts = a_array_push(parts, fn_toml__t_stringify_value(item)); a_release(__old); }
                a_release(item);
            }
            a_release(__iter_arr);
        }
        __ret = a_str_concat(a_string("["), a_str_concat(a_str_join(parts, a_string(", ")), a_string("]"))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(t, a_string("map")))) {
        { AValue __old = keys; keys = a_map_keys(val); a_release(__old); }
        { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(keys);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue k = {0}, v = {0}, ks = {0}, vs = {0};
                k = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = v; v = a_map_get(val, k); a_release(__old); }
                { AValue __old = ks; ks = fn_toml__t_quote_key(k); a_release(__old); }
                { AValue __old = vs; vs = fn_toml__t_stringify_value(v); a_release(__old); }
                { AValue __old = parts; parts = a_array_push(parts, a_str_concat(ks, a_str_concat(a_string(" = "), vs))); a_release(__old); }
                a_release(k);
                a_release(v);
                a_release(ks);
                a_release(vs);
            }
            a_release(__iter_arr);
        }
        __ret = a_str_concat(a_string("{"), a_str_concat(a_str_join(parts, a_string(", ")), a_string("}"))); goto __fn_cleanup;
    }
    __ret = a_to_str(val); goto __fn_cleanup;
__fn_cleanup:
    a_release(t);
    a_release(parts);
    a_release(keys);
    a_release(v);
    a_release(ks);
    a_release(vs);
    a_release(val);
    return __ret;
}

AValue fn_toml__t_stringify_section(AValue val, AValue prefix, AValue lines) {
    AValue keys = {0}, simple_keys = {0}, table_keys = {0}, array_keys = {0}, v = {0}, t = {0}, first_type = {0}, ks = {0}, vs = {0}, section = {0}, arr = {0};
    AValue __ret = a_void();
    val = a_retain(val);
    prefix = a_retain(prefix);
    lines = a_retain(lines);
    { AValue __old = keys; keys = a_map_keys(val); a_release(__old); }
    { AValue __old = simple_keys; simple_keys = a_array_new(0); a_release(__old); }
    { AValue __old = table_keys; table_keys = a_array_new(0); a_release(__old); }
    { AValue __old = array_keys; array_keys = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(keys);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue k = {0}, v = {0}, t = {0}, first_type = {0};
            k = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = v; v = a_map_get(val, k); a_release(__old); }
            { AValue __old = t; t = a_type_of(v); a_release(__old); }
            if (a_truthy(a_eq(t, a_string("map")))) {
                { AValue __old = table_keys; table_keys = a_array_push(table_keys, k); a_release(__old); }
            } else {
                if (a_truthy(a_eq(t, a_string("array")))) {
                    if (a_truthy(a_gt(a_len(v), a_int(0)))) {
                        { AValue __old = first_type; first_type = a_type_of(a_array_get(v, a_int(0))); a_release(__old); }
                        if (a_truthy(a_eq(first_type, a_string("map")))) {
                            { AValue __old = array_keys; array_keys = a_array_push(array_keys, k); a_release(__old); }
                        } else {
                            { AValue __old = simple_keys; simple_keys = a_array_push(simple_keys, k); a_release(__old); }
                        }
                    } else {
                        { AValue __old = simple_keys; simple_keys = a_array_push(simple_keys, k); a_release(__old); }
                    }
                } else {
                    { AValue __old = simple_keys; simple_keys = a_array_push(simple_keys, k); a_release(__old); }
                }
            }
            a_release(k);
            a_release(v);
            a_release(t);
            a_release(first_type);
        }
        a_release(__iter_arr);
    }
    {
        AValue __iter_arr = a_iterable(simple_keys);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue k = {0}, v = {0}, ks = {0}, vs = {0};
            k = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = v; v = a_map_get(val, k); a_release(__old); }
            { AValue __old = ks; ks = fn_toml__t_quote_key(k); a_release(__old); }
            { AValue __old = vs; vs = fn_toml__t_stringify_value(v); a_release(__old); }
            { AValue __old = lines; lines = a_array_push(lines, a_str_concat(ks, a_str_concat(a_string(" = "), vs))); a_release(__old); }
            a_release(k);
            a_release(v);
            a_release(ks);
            a_release(vs);
        }
        a_release(__iter_arr);
    }
    {
        AValue __iter_arr = a_iterable(table_keys);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue k = {0}, v = {0}, section = {0};
            k = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = v; v = a_map_get(val, k); a_release(__old); }
            { AValue __old = section; section = a_retain(k); a_release(__old); }
            if (a_truthy(a_neq(prefix, a_string("")))) {
                { AValue __old = section; section = a_str_concat(prefix, a_str_concat(a_string("."), k)); a_release(__old); }
            }
            { AValue __old = lines; lines = a_array_push(lines, a_string("")); a_release(__old); }
            { AValue __old = lines; lines = a_array_push(lines, a_str_concat(a_string("["), a_str_concat(section, a_string("]")))); a_release(__old); }
            { AValue __old = lines; lines = fn_toml__t_stringify_section(v, section, lines); a_release(__old); }
            a_release(k);
            a_release(v);
            a_release(section);
        }
        a_release(__iter_arr);
    }
    {
        AValue __iter_arr = a_iterable(array_keys);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue k = {0}, arr = {0}, section = {0};
            k = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = arr; arr = a_map_get(val, k); a_release(__old); }
            { AValue __old = section; section = a_retain(k); a_release(__old); }
            if (a_truthy(a_neq(prefix, a_string("")))) {
                { AValue __old = section; section = a_str_concat(prefix, a_str_concat(a_string("."), k)); a_release(__old); }
            }
            {
                AValue __iter_arr = a_iterable(arr);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue item = {0};
                    item = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = lines; lines = a_array_push(lines, a_string("")); a_release(__old); }
                    { AValue __old = lines; lines = a_array_push(lines, a_str_concat(a_string("[["), a_str_concat(section, a_string("]]")))); a_release(__old); }
                    { AValue __old = lines; lines = fn_toml__t_stringify_section(item, section, lines); a_release(__old); }
                    a_release(item);
                }
                a_release(__iter_arr);
            }
            a_release(k);
            a_release(arr);
            a_release(section);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(lines); goto __fn_cleanup;
__fn_cleanup:
    a_release(keys);
    a_release(simple_keys);
    a_release(table_keys);
    a_release(array_keys);
    a_release(v);
    a_release(t);
    a_release(first_type);
    a_release(ks);
    a_release(vs);
    a_release(section);
    a_release(arr);
    a_release(val);
    a_release(prefix);
    a_release(lines);
    return __ret;
}

AValue fn_toml_stringify(AValue val) {
    AValue lines = {0};
    AValue __ret = a_void();
    val = a_retain(val);
    { AValue __old = lines; lines = a_array_new(0); a_release(__old); }
    { AValue __old = lines; lines = fn_toml__t_stringify_section(val, a_string(""), lines); a_release(__old); }
    __ret = a_str_trim(a_str_join(lines, a_string("\n"))); goto __fn_cleanup;
__fn_cleanup:
    a_release(lines);
    a_release(val);
    return __ret;
}

AValue fn_semver_parse(AValue s) {
    AValue v = {0}, parts = {0}, major = {0}, minor = {0}, patch = {0};
    AValue __ret = a_void();
    s = a_retain(s);
    { AValue __old = v; v = a_retain(s); a_release(__old); }
    if (a_truthy(a_str_starts_with(v, a_string("v")))) {
        { AValue __old = v; v = a_str_slice(v, a_int(1), a_len(v)); a_release(__old); }
    }
    { AValue __old = parts; parts = a_str_split(v, a_string(".")); a_release(__old); }
    { AValue __old = major; major = a_to_int(a_array_get(parts, a_int(0))); a_release(__old); }
    { AValue __old = minor; minor = a_int(0); a_release(__old); }
    if (a_truthy(a_gt(a_len(parts), a_int(1)))) {
        { AValue __old = minor; minor = a_to_int(a_array_get(parts, a_int(1))); a_release(__old); }
    }
    { AValue __old = patch; patch = a_int(0); a_release(__old); }
    if (a_truthy(a_gt(a_len(parts), a_int(2)))) {
        { AValue __old = patch; patch = a_to_int(a_array_get(parts, a_int(2))); a_release(__old); }
    }
    __ret = a_map_new(3, "major", major, "minor", minor, "patch", patch); goto __fn_cleanup;
__fn_cleanup:
    a_release(v);
    a_release(parts);
    a_release(major);
    a_release(minor);
    a_release(patch);
    a_release(s);
    return __ret;
}

AValue fn_semver_format(AValue v) {
    AValue __ret = a_void();
    v = a_retain(v);
    __ret = a_add(a_add(a_add(a_add(a_to_str(a_map_get(v, a_string("major"))), a_string(".")), a_to_str(a_map_get(v, a_string("minor")))), a_string(".")), a_to_str(a_map_get(v, a_string("patch")))); goto __fn_cleanup;
__fn_cleanup:
    a_release(v);
    return __ret;
}

AValue fn_semver_compare(AValue a, AValue b) {
    AValue am = {0}, bm = {0}, ai = {0}, bi = {0}, ap = {0}, bp = {0};
    AValue __ret = a_void();
    a = a_retain(a);
    b = a_retain(b);
    { AValue __old = am; am = a_map_get(a, a_string("major")); a_release(__old); }
    { AValue __old = bm; bm = a_map_get(b, a_string("major")); a_release(__old); }
    if (a_truthy(a_lt(am, bm))) {
        __ret = a_neg(a_int(1)); goto __fn_cleanup;
    }
    if (a_truthy(a_gt(am, bm))) {
        __ret = a_int(1); goto __fn_cleanup;
    }
    { AValue __old = ai; ai = a_map_get(a, a_string("minor")); a_release(__old); }
    { AValue __old = bi; bi = a_map_get(b, a_string("minor")); a_release(__old); }
    if (a_truthy(a_lt(ai, bi))) {
        __ret = a_neg(a_int(1)); goto __fn_cleanup;
    }
    if (a_truthy(a_gt(ai, bi))) {
        __ret = a_int(1); goto __fn_cleanup;
    }
    { AValue __old = ap; ap = a_map_get(a, a_string("patch")); a_release(__old); }
    { AValue __old = bp; bp = a_map_get(b, a_string("patch")); a_release(__old); }
    if (a_truthy(a_lt(ap, bp))) {
        __ret = a_neg(a_int(1)); goto __fn_cleanup;
    }
    if (a_truthy(a_gt(ap, bp))) {
        __ret = a_int(1); goto __fn_cleanup;
    }
    __ret = a_int(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(am);
    a_release(bm);
    a_release(ai);
    a_release(bi);
    a_release(ap);
    a_release(bp);
    a_release(a);
    a_release(b);
    return __ret;
}

AValue fn_semver__gte(AValue v, AValue min) {
    AValue __ret = a_void();
    v = a_retain(v);
    min = a_retain(min);
    __ret = a_gteq(fn_semver_compare(v, min), a_int(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(v);
    a_release(min);
    return __ret;
}

AValue fn_semver__lt(AValue v, AValue max) {
    AValue __ret = a_void();
    v = a_retain(v);
    max = a_retain(max);
    __ret = a_lt(fn_semver_compare(v, max), a_int(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(v);
    a_release(max);
    return __ret;
}

AValue fn_semver_satisfies(AValue version, AValue constraint) {
    AValue v = {0}, min = {0}, max = {0}, target = {0};
    AValue __ret = a_void();
    version = a_retain(version);
    constraint = a_retain(constraint);
    { AValue __old = v; v = fn_semver_parse(version); a_release(__old); }
    if (a_truthy(a_str_starts_with(constraint, a_string("^")))) {
        { AValue __old = min; min = fn_semver_parse(a_str_slice(constraint, a_int(1), a_len(constraint))); a_release(__old); }
        { AValue __old = max; max = a_map_new(3, "major", a_add(a_map_get(min, a_string("major")), a_int(1)), "minor", a_int(0), "patch", a_int(0)); a_release(__old); }
        __ret = a_and(fn_semver__gte(v, min), fn_semver__lt(v, max)); goto __fn_cleanup;
    }
    if (a_truthy(a_str_starts_with(constraint, a_string("~")))) {
        { AValue __old = min; min = fn_semver_parse(a_str_slice(constraint, a_int(1), a_len(constraint))); a_release(__old); }
        { AValue __old = max; max = a_map_new(3, "major", a_map_get(min, a_string("major")), "minor", a_add(a_map_get(min, a_string("minor")), a_int(1)), "patch", a_int(0)); a_release(__old); }
        __ret = a_and(fn_semver__gte(v, min), fn_semver__lt(v, max)); goto __fn_cleanup;
    }
    if (a_truthy(a_str_starts_with(constraint, a_string(">=")))) {
        { AValue __old = min; min = fn_semver_parse(a_str_trim(a_str_slice(constraint, a_int(2), a_len(constraint)))); a_release(__old); }
        __ret = fn_semver__gte(v, min); goto __fn_cleanup;
    }
    if (a_truthy(a_str_starts_with(constraint, a_string(">")))) {
        { AValue __old = min; min = fn_semver_parse(a_str_trim(a_str_slice(constraint, a_int(1), a_len(constraint)))); a_release(__old); }
        __ret = a_gt(fn_semver_compare(v, min), a_int(0)); goto __fn_cleanup;
    }
    if (a_truthy(a_str_starts_with(constraint, a_string("<=")))) {
        { AValue __old = max; max = fn_semver_parse(a_str_trim(a_str_slice(constraint, a_int(2), a_len(constraint)))); a_release(__old); }
        __ret = a_lteq(fn_semver_compare(v, max), a_int(0)); goto __fn_cleanup;
    }
    if (a_truthy(a_str_starts_with(constraint, a_string("<")))) {
        { AValue __old = max; max = fn_semver_parse(a_str_trim(a_str_slice(constraint, a_int(1), a_len(constraint)))); a_release(__old); }
        __ret = fn_semver__lt(v, max); goto __fn_cleanup;
    }
    { AValue __old = target; target = fn_semver_parse(constraint); a_release(__old); }
    __ret = a_eq(fn_semver_compare(v, target), a_int(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(v);
    a_release(min);
    a_release(max);
    a_release(target);
    a_release(version);
    a_release(constraint);
    return __ret;
}

AValue fn_semver_best_match(AValue versions, AValue constraint) {
    AValue best = {0}, best_v = {0}, i = {0}, vs = {0}, v = {0};
    AValue __ret = a_void();
    versions = a_retain(versions);
    constraint = a_retain(constraint);
    { AValue __old = best; best = a_string(""); a_release(__old); }
    { AValue __old = best_v; best_v = a_map_new(3, "major", a_neg(a_int(1)), "minor", a_neg(a_int(1)), "patch", a_neg(a_int(1))); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(versions)))) {
        { AValue __old = vs; vs = a_array_get(versions, i); a_release(__old); }
        if (a_truthy(fn_semver_satisfies(vs, constraint))) {
            { AValue __old = v; v = fn_semver_parse(vs); a_release(__old); }
            if (a_truthy(a_gt(fn_semver_compare(v, best_v), a_int(0)))) {
                { AValue __old = best; best = a_retain(vs); a_release(__old); }
                { AValue __old = best_v; best_v = a_retain(v); a_release(__old); }
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(best); goto __fn_cleanup;
__fn_cleanup:
    a_release(best);
    a_release(best_v);
    a_release(i);
    a_release(vs);
    a_release(v);
    a_release(versions);
    a_release(constraint);
    return __ret;
}

AValue fn_pkg__manifest_path(AValue dir) {
    AValue __ret = a_void();
    dir = a_retain(dir);
    __ret = fn_path_join(dir, a_string("pkg.toml")); goto __fn_cleanup;
__fn_cleanup:
    a_release(dir);
    return __ret;
}

AValue fn_pkg__lock_path(AValue dir) {
    AValue __ret = a_void();
    dir = a_retain(dir);
    __ret = fn_path_join(dir, a_string("pkg.lock")); goto __fn_cleanup;
__fn_cleanup:
    a_release(dir);
    return __ret;
}

AValue fn_pkg__modules_dir(AValue dir) {
    AValue __ret = a_void();
    dir = a_retain(dir);
    __ret = fn_path_join(dir, a_string("a_modules")); goto __fn_cleanup;
__fn_cleanup:
    a_release(dir);
    return __ret;
}

AValue fn_pkg_parse_source(AValue source) {
    AValue at = {0}, repo_part = {0}, constraint = {0}, colon = {0}, host = {0}, repo = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    { AValue __old = at; at = a_str_find(source, a_string("@")); a_release(__old); }
    { AValue __old = repo_part; repo_part = a_retain(source); a_release(__old); }
    { AValue __old = constraint; constraint = a_string(""); a_release(__old); }
    if (a_truthy(a_gteq(at, a_int(0)))) {
        { AValue __old = repo_part; repo_part = a_str_slice(source, a_int(0), at); a_release(__old); }
        { AValue __old = constraint; constraint = a_str_slice(source, a_add(at, a_int(1)), a_len(source)); a_release(__old); }
    }
    { AValue __old = colon; colon = a_str_find(repo_part, a_string(":")); a_release(__old); }
    { AValue __old = host; host = a_string("github"); a_release(__old); }
    { AValue __old = repo; repo = a_retain(repo_part); a_release(__old); }
    if (a_truthy(a_gteq(colon, a_int(0)))) {
        { AValue __old = host; host = a_str_slice(repo_part, a_int(0), colon); a_release(__old); }
        { AValue __old = repo; repo = a_str_slice(repo_part, a_add(colon, a_int(1)), a_len(repo_part)); a_release(__old); }
    }
    __ret = a_map_new(3, "host", host, "repo", repo, "constraint", constraint); goto __fn_cleanup;
__fn_cleanup:
    a_release(at);
    a_release(repo_part);
    a_release(constraint);
    a_release(colon);
    a_release(host);
    a_release(repo);
    a_release(source);
    return __ret;
}

AValue fn_pkg__git_url(AValue source) {
    AValue host = {0}, repo = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    { AValue __old = host; host = a_map_get(source, a_string("host")); a_release(__old); }
    { AValue __old = repo; repo = a_map_get(source, a_string("repo")); a_release(__old); }
    if (a_truthy(a_eq(host, a_string("github")))) {
        __ret = a_add(a_add(a_string("https://github.com/"), repo), a_string(".git")); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(host, a_string("gitlab")))) {
        __ret = a_add(a_add(a_string("https://gitlab.com/"), repo), a_string(".git")); goto __fn_cleanup;
    }
    __ret = a_add(a_add(a_add(a_add(a_string("https://"), host), a_string("/")), repo), a_string(".git")); goto __fn_cleanup;
__fn_cleanup:
    a_release(host);
    a_release(repo);
    a_release(source);
    return __ret;
}

AValue fn_pkg__fetch_tags(AValue url) {
    AValue result = {0}, lines = {0}, tags = {0}, i = {0}, line = {0}, tab = {0}, tag = {0}, peeled = {0};
    AValue __ret = a_void();
    url = a_retain(url);
    { AValue __old = result; result = a_exec(a_add(a_add(a_string("git ls-remote --tags "), url), a_string(" 2>/dev/null"))); a_release(__old); }
    if (a_truthy(a_neq(a_map_get(result, a_string("code")), a_int(0)))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    { AValue __old = lines; lines = a_str_lines(a_map_get(result, a_string("stdout"))); a_release(__old); }
    { AValue __old = tags; tags = a_array_new(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(lines)))) {
        { AValue __old = line; line = a_str_trim(a_array_get(lines, i)); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(a_len(line), a_int(0)))) {
            continue;
        }
        { AValue __old = tab; tab = a_str_find(line, a_string("refs/tags/")); a_release(__old); }
        if (a_truthy(a_lt(tab, a_int(0)))) {
            continue;
        }
        { AValue __old = tag; tag = a_str_slice(line, a_add(tab, a_int(10)), a_len(line)); a_release(__old); }
        { AValue __old = peeled; peeled = a_add(a_add(a_string("^"), a_from_code(a_int(123))), a_from_code(a_int(125))); a_release(__old); }
        if (a_truthy(a_str_ends_with(tag, peeled))) {
            continue;
        }
        if (a_truthy(a_str_starts_with(tag, a_string("v")))) {
            { AValue __old = tags; tags = a_array_push(tags, a_str_slice(tag, a_int(1), a_len(tag))); a_release(__old); }
        } else {
            { AValue __old = tags; tags = a_array_push(tags, tag); a_release(__old); }
        }
    }
    __ret = a_retain(tags); goto __fn_cleanup;
__fn_cleanup:
    a_release(result);
    a_release(lines);
    a_release(tags);
    a_release(i);
    a_release(line);
    a_release(tab);
    a_release(tag);
    a_release(peeled);
    a_release(url);
    return __ret;
}

AValue fn_pkg__clone_at_tag(AValue url, AValue tag, AValue dest) {
    AValue tag_ref = {0}, r = {0}, r2 = {0};
    AValue __ret = a_void();
    url = a_retain(url);
    tag = a_retain(tag);
    dest = a_retain(dest);
    { AValue __old = tag_ref; tag_ref = a_add(a_string("v"), tag); a_release(__old); }
    { AValue __old = r; r = a_exec(a_add(a_add(a_add(a_add(a_add(a_add(a_string("git clone --depth 1 --branch "), tag_ref), a_string(" ")), url), a_string(" ")), dest), a_string(" 2>/dev/null"))); a_release(__old); }
    if (a_truthy(a_eq(a_map_get(r, a_string("code")), a_int(0)))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    { AValue __old = r2; r2 = a_exec(a_add(a_add(a_add(a_add(a_add(a_add(a_string("git clone --depth 1 --branch "), tag), a_string(" ")), url), a_string(" ")), dest), a_string(" 2>/dev/null"))); a_release(__old); }
    __ret = a_eq(a_map_get(r2, a_string("code")), a_int(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag_ref);
    a_release(r);
    a_release(r2);
    a_release(url);
    a_release(tag);
    a_release(dest);
    return __ret;
}

AValue fn_pkg__get_commit(AValue dest) {
    AValue r = {0};
    AValue __ret = a_void();
    dest = a_retain(dest);
    { AValue __old = r; r = a_exec(a_add(a_add(a_string("git -C "), dest), a_string(" rev-parse HEAD 2>/dev/null"))); a_release(__old); }
    if (a_truthy(a_eq(a_map_get(r, a_string("code")), a_int(0)))) {
        __ret = a_str_trim(a_map_get(r, a_string("stdout"))); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(dest);
    return __ret;
}

AValue fn_pkg__copy_a_files(AValue src_dir, AValue dst_dir) {
    AValue entries = {0}, i = {0}, name = {0}, is_dir = {0}, src_path = {0}, dst_path = {0}, content = {0};
    AValue __ret = a_void();
    src_dir = a_retain(src_dir);
    dst_dir = a_retain(dst_dir);
    if (a_truthy(a_not(a_fs_exists(dst_dir)))) {
        a_fs_mkdir(dst_dir);
    }
    { AValue __old = entries; entries = a_fs_ls(src_dir); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(entries)))) {
        { AValue __old = name; name = a_map_get(a_array_get(entries, i), a_string("name")); a_release(__old); }
        { AValue __old = is_dir; is_dir = a_map_get(a_array_get(entries, i), a_string("is_dir")); a_release(__old); }
        { AValue __old = src_path; src_path = fn_path_join(src_dir, name); a_release(__old); }
        { AValue __old = dst_path; dst_path = fn_path_join(dst_dir, name); a_release(__old); }
        if (a_truthy(is_dir)) {
            if (a_truthy(a_neq(name, a_string(".git")))) {
                fn_pkg__copy_a_files(src_path, dst_path);
            }
        } else {
            if (a_truthy(a_str_ends_with(name, a_string(".a")))) {
                { AValue __old = content; content = a_io_read_file(src_path); a_release(__old); }
                a_io_write_file(dst_path, content);
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
__fn_cleanup:
    a_release(entries);
    a_release(i);
    a_release(name);
    a_release(is_dir);
    a_release(src_path);
    a_release(dst_path);
    a_release(content);
    a_release(src_dir);
    a_release(dst_dir);
    return __ret;
}

AValue fn_pkg_init(AValue dir) {
    AValue name = {0}, text = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    { AValue __old = name; name = fn_path_basename(dir); a_release(__old); }
    { AValue __old = text; text = a_add(a_add(a_string("[package]\nname = \""), name), a_string("\"\nversion = \"0.1.0\"\n\n[deps]\n")); a_release(__old); }
    a_io_write_file(fn_pkg__manifest_path(dir), text);
__fn_cleanup:
    a_release(name);
    a_release(text);
    a_release(dir);
    return __ret;
}

AValue fn_pkg_read_manifest(AValue dir) {
    AValue text = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    { AValue __old = text; text = a_io_read_file(fn_pkg__manifest_path(dir)); a_release(__old); }
    __ret = fn_toml_parse(text); goto __fn_cleanup;
__fn_cleanup:
    a_release(text);
    a_release(dir);
    return __ret;
}

AValue fn_pkg_write_manifest(AValue dir, AValue manifest) {
    AValue pkg = {0}, text = {0}, deps = {0}, keys = {0}, i = {0}, k = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    manifest = a_retain(manifest);
    { AValue __old = pkg; pkg = a_map_get(manifest, a_string("package")); a_release(__old); }
    { AValue __old = text; text = a_string("[package]\n"); a_release(__old); }
    { AValue __old = text; text = a_add(a_add(a_add(text, a_string("name = \"")), a_map_get(pkg, a_string("name"))), a_string("\"\n")); a_release(__old); }
    { AValue __old = text; text = a_add(a_add(a_add(text, a_string("version = \"")), a_map_get(pkg, a_string("version"))), a_string("\"\n")); a_release(__old); }
    { AValue __old = text; text = a_add(text, a_string("\n[deps]\n")); a_release(__old); }
    { AValue __old = deps; deps = a_map_get(manifest, a_string("deps")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(deps), a_string("map")))) {
        { AValue __old = keys; keys = a_map_keys(deps); a_release(__old); }
        { AValue __old = i; i = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(keys)))) {
            { AValue __old = k; k = a_array_get(keys, i); a_release(__old); }
            { AValue __old = text; text = a_add(a_add(a_add(a_add(text, k), a_string(" = \"")), a_map_get(deps, k)), a_string("\"\n")); a_release(__old); }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
        }
    }
    a_io_write_file(fn_pkg__manifest_path(dir), text);
__fn_cleanup:
    a_release(pkg);
    a_release(text);
    a_release(deps);
    a_release(keys);
    a_release(i);
    a_release(k);
    a_release(dir);
    a_release(manifest);
    return __ret;
}

AValue fn_pkg_add_dep(AValue dir, AValue name, AValue source) {
    AValue manifest = {0}, deps = {0}, updated = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    name = a_retain(name);
    source = a_retain(source);
    { AValue __old = manifest; manifest = fn_pkg_read_manifest(dir); a_release(__old); }
    { AValue __old = deps; deps = a_map_get(manifest, a_string("deps")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(deps), a_string("map")))) {
        { AValue __old = deps; deps = a_map_from_entries(a_array_new(0)); a_release(__old); }
    }
    { AValue __old = deps; deps = a_map_set(deps, name, source); a_release(__old); }
    { AValue __old = updated; updated = a_map_set(manifest, a_string("deps"), deps); a_release(__old); }
    fn_pkg_write_manifest(dir, updated);
    fn_pkg_install(dir);
__fn_cleanup:
    a_release(manifest);
    a_release(deps);
    a_release(updated);
    a_release(dir);
    a_release(name);
    a_release(source);
    return __ret;
}

AValue fn_pkg__read_lock(AValue dir) {
    AValue lp = {0}, text = {0}, parsed = {0}, pkgs = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    { AValue __old = lp; lp = fn_pkg__lock_path(dir); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(lp)))) {
        __ret = a_array_new(0); goto __fn_cleanup;
    }
    { AValue __old = text; text = a_io_read_file(lp); a_release(__old); }
    { AValue __old = parsed; parsed = fn_toml_parse(text); a_release(__old); }
    { AValue __old = pkgs; pkgs = a_map_get(parsed, a_string("package")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(pkgs), a_string("array")))) {
        __ret = a_retain(pkgs); goto __fn_cleanup;
    }
    __ret = a_array_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(lp);
    a_release(text);
    a_release(parsed);
    a_release(pkgs);
    a_release(dir);
    return __ret;
}

AValue fn_pkg__write_lock(AValue dir, AValue packages) {
    AValue text = {0}, i = {0}, p = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    packages = a_retain(packages);
    { AValue __old = text; text = a_string(""); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(packages)))) {
        { AValue __old = p; p = a_array_get(packages, i); a_release(__old); }
        { AValue __old = text; text = a_add(text, a_string("[[package]]\n")); a_release(__old); }
        { AValue __old = text; text = a_add(a_add(a_add(text, a_string("name = \"")), a_map_get(p, a_string("name"))), a_string("\"\n")); a_release(__old); }
        { AValue __old = text; text = a_add(a_add(a_add(text, a_string("version = \"")), a_map_get(p, a_string("version"))), a_string("\"\n")); a_release(__old); }
        { AValue __old = text; text = a_add(a_add(a_add(text, a_string("source = \"")), a_map_get(p, a_string("source"))), a_string("\"\n")); a_release(__old); }
        { AValue __old = text; text = a_add(a_add(a_add(text, a_string("commit = \"")), a_map_get(p, a_string("commit"))), a_string("\"\n\n")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    a_io_write_file(fn_pkg__lock_path(dir), text);
__fn_cleanup:
    a_release(text);
    a_release(i);
    a_release(p);
    a_release(dir);
    a_release(packages);
    return __ret;
}

AValue fn_pkg__find_locked(AValue lock_entries, AValue name) {
    AValue i = {0};
    AValue __ret = a_void();
    lock_entries = a_retain(lock_entries);
    name = a_retain(name);
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(lock_entries)))) {
        if (a_truthy(a_eq(a_map_get(a_array_get(lock_entries, i), a_string("name")), name))) {
            __ret = a_array_get(lock_entries, i); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_map_from_entries(a_array_new(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(i);
    a_release(lock_entries);
    a_release(name);
    return __ret;
}

AValue fn_pkg_install(AValue dir) {
    AValue manifest = {0}, deps = {0}, dep_keys = {0}, modules = {0}, lock_entries = {0}, new_lock = {0}, installed = {0}, i = {0}, name = {0}, source_str = {0}, source = {0}, constraint = {0}, url = {0}, locked = {0}, dest = {0}, locked_ver = {0}, tags = {0}, version = {0}, tmp = {0}, ok = {0}, commit = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    { AValue __old = manifest; manifest = fn_pkg_read_manifest(dir); a_release(__old); }
    { AValue __old = deps; deps = a_map_get(manifest, a_string("deps")); a_release(__old); }
    if (a_truthy(a_neq(a_type_of(deps), a_string("map")))) {
        __ret = a_map_new(1, "installed", a_int(0)); goto __fn_cleanup;
    }
    { AValue __old = dep_keys; dep_keys = a_map_keys(deps); a_release(__old); }
    { AValue __old = modules; modules = fn_pkg__modules_dir(dir); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(modules)))) {
        a_fs_mkdir(modules);
    }
    { AValue __old = lock_entries; lock_entries = fn_pkg__read_lock(dir); a_release(__old); }
    { AValue __old = new_lock; new_lock = a_array_new(0); a_release(__old); }
    { AValue __old = installed; installed = a_int(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(dep_keys)))) {
        { AValue __old = name; name = a_array_get(dep_keys, i); a_release(__old); }
        { AValue __old = source_str; source_str = a_map_get(deps, name); a_release(__old); }
        { AValue __old = source; source = fn_pkg_parse_source(source_str); a_release(__old); }
        { AValue __old = constraint; constraint = a_map_get(source, a_string("constraint")); a_release(__old); }
        { AValue __old = url; url = fn_pkg__git_url(source); a_release(__old); }
        { AValue __old = locked; locked = fn_pkg__find_locked(lock_entries, name); a_release(__old); }
        { AValue __old = dest; dest = fn_path_join(modules, name); a_release(__old); }
        if (a_truthy(a_map_has(locked, a_string("version")))) {
            { AValue __old = locked_ver; locked_ver = a_map_get(locked, a_string("version")); a_release(__old); }
            if (a_truthy(a_or(a_eq(a_len(constraint), a_int(0)), fn_semver_satisfies(locked_ver, constraint)))) {
                if (a_truthy(a_fs_exists(dest))) {
                    { AValue __old = new_lock; new_lock = a_array_push(new_lock, locked); a_release(__old); }
                    { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
                    continue;
                }
            }
        }
        { AValue __old = tags; tags = fn_pkg__fetch_tags(url); a_release(__old); }
        { AValue __old = version; version = a_string(""); a_release(__old); }
        if (a_truthy(a_gt(a_len(constraint), a_int(0)))) {
            { AValue __old = version; version = fn_semver_best_match(tags, constraint); a_release(__old); }
        } else {
            if (a_truthy(a_gt(a_len(tags), a_int(0)))) {
                { AValue __old = version; version = a_array_get(tags, a_sub(a_len(tags), a_int(1))); a_release(__old); }
            }
        }
        if (a_truthy(a_eq(a_len(version), a_int(0)))) {
            a_println(a_add(a_add(a_add(a_add(a_string("  warning: no matching version for "), name), a_string(" (")), constraint), a_string(")")));
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        { AValue __old = tmp; tmp = a_add(a_string("/tmp/a_pkg_"), name); a_release(__old); }
        a_exec(a_add(a_string("rm -rf "), tmp));
        { AValue __old = ok; ok = fn_pkg__clone_at_tag(url, version, tmp); a_release(__old); }
        if (a_truthy(a_not(ok))) {
            a_println(a_add(a_add(a_add(a_string("  warning: failed to clone "), name), a_string("@")), version));
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            continue;
        }
        { AValue __old = commit; commit = fn_pkg__get_commit(tmp); a_release(__old); }
        if (a_truthy(a_fs_exists(dest))) {
            a_exec(a_add(a_string("rm -rf "), dest));
        }
        fn_pkg__copy_a_files(tmp, dest);
        a_exec(a_add(a_string("rm -rf "), tmp));
        { AValue __old = new_lock; new_lock = a_array_push(new_lock, a_map_new(4, "name", name, "version", version, "source", a_add(a_add(a_map_get(source, a_string("host")), a_string(":")), a_map_get(source, a_string("repo"))), "commit", commit)); a_release(__old); }
        { AValue __old = installed; installed = a_add(installed, a_int(1)); a_release(__old); }
        a_println(a_add(a_add(a_add(a_string("  installed "), name), a_string("@")), version));
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    fn_pkg__write_lock(dir, new_lock);
    __ret = a_map_new(1, "installed", installed); goto __fn_cleanup;
__fn_cleanup:
    a_release(manifest);
    a_release(deps);
    a_release(dep_keys);
    a_release(modules);
    a_release(lock_entries);
    a_release(new_lock);
    a_release(installed);
    a_release(i);
    a_release(name);
    a_release(source_str);
    a_release(source);
    a_release(constraint);
    a_release(url);
    a_release(locked);
    a_release(dest);
    a_release(locked_ver);
    a_release(tags);
    a_release(version);
    a_release(tmp);
    a_release(ok);
    a_release(commit);
    a_release(dir);
    return __ret;
}

AValue fn_plugin__plugins_dir(void) {
    AValue home = {0};
    AValue __ret = a_void();
    { AValue __old = home; home = a_env_get(a_string("HOME")); a_release(__old); }
    __ret = a_add(home, a_string("/.a/plugins")); goto __fn_cleanup;
__fn_cleanup:
    a_release(home);
    return __ret;
}

AValue fn_plugin__ensure_plugins_dir(void) {
    AValue d = {0};
    AValue __ret = a_void();
    { AValue __old = d; d = fn_plugin__plugins_dir(); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(d)))) {
        a_exec(a_add(a_string("mkdir -p "), d));
    }
__fn_cleanup:
    a_release(d);
    return __ret;
}

AValue fn_plugin__registry_path(void) {
    AValue __ret = a_void();
    __ret = a_add(fn_plugin__plugins_dir(), a_string("/registry.json")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_plugin__load_registry(void) {
    AValue p = {0};
    AValue __ret = a_void();
    { AValue __old = p; p = fn_plugin__registry_path(); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(p)))) {
        __ret = a_map_new(1, "plugins", a_map_new(0)); goto __fn_cleanup;
    }
    __ret = a_json_parse(a_io_read_file(p)); goto __fn_cleanup;
__fn_cleanup:
    a_release(p);
    return __ret;
}

AValue fn_plugin__save_registry(AValue reg) {
    AValue __ret = a_void();
    reg = a_retain(reg);
    a_io_write_file(fn_plugin__registry_path(), a_json_pretty(reg));
__fn_cleanup:
    a_release(reg);
    return __ret;
}

AValue fn_plugin__parse_manifest(AValue dir) {
    AValue toml_path = {0}, content = {0}, lines = {0}, m = {0}, i = {0}, line = {0}, eq = {0}, key = {0}, val = {0}, q = {0}, json_path = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    { AValue __old = toml_path; toml_path = a_add(dir, a_string("/plugin.toml")); a_release(__old); }
    if (a_truthy(a_fs_exists(toml_path))) {
        { AValue __old = content; content = a_io_read_file(toml_path); a_release(__old); }
        { AValue __old = lines; lines = a_str_lines(content); a_release(__old); }
        { AValue __old = m; m = a_map_new(0); a_release(__old); }
        { AValue __old = i; i = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(lines)))) {
            { AValue __old = line; line = a_str_trim(a_array_get(lines, i)); a_release(__old); }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            if (a_truthy(a_or(a_or(a_eq(a_len(line), a_int(0)), a_str_starts_with(line, a_string("#"))), a_str_starts_with(line, a_string(";"))))) {
                continue;
            }
            { AValue __old = eq; eq = a_str_find(line, a_string("=")); a_release(__old); }
            if (a_truthy(a_gt(eq, a_int(0)))) {
                { AValue __old = key; key = a_str_trim(a_str_slice(line, a_int(0), eq)); a_release(__old); }
                { AValue __old = val; val = a_str_trim(a_str_slice(line, a_add(eq, a_int(1)), a_len(line))); a_release(__old); }
                { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
                if (a_truthy(a_and(a_str_starts_with(val, q), a_str_ends_with(val, q)))) {
                    { AValue __old = val; val = a_str_slice(val, a_int(1), a_sub(a_len(val), a_int(1))); a_release(__old); }
                }
                { AValue __old = m; m = a_map_set(m, key, val); a_release(__old); }
            }
        }
        __ret = a_retain(m); goto __fn_cleanup;
    }
    { AValue __old = json_path; json_path = a_add(dir, a_string("/plugin.json")); a_release(__old); }
    if (a_truthy(a_fs_exists(json_path))) {
        __ret = a_json_parse(a_io_read_file(json_path)); goto __fn_cleanup;
    }
    __ret = a_map_new(2, "name", a_string("unknown"), "version", a_string("0.0.0")); goto __fn_cleanup;
__fn_cleanup:
    a_release(toml_path);
    a_release(content);
    a_release(lines);
    a_release(m);
    a_release(i);
    a_release(line);
    a_release(eq);
    a_release(key);
    a_release(val);
    a_release(q);
    a_release(json_path);
    a_release(dir);
    return __ret;
}

AValue fn_plugin_install(AValue source_dir) {
    AValue manifest = {0}, name = {0}, dest = {0}, reg = {0}, plugins = {0};
    AValue __ret = a_void();
    source_dir = a_retain(source_dir);
    fn_plugin__ensure_plugins_dir();
    { AValue __old = manifest; manifest = fn_plugin__parse_manifest(source_dir); a_release(__old); }
    { AValue __old = name; name = a_array_get(manifest, a_string("name")); a_release(__old); }
    if (a_truthy(a_eq(name, a_string("unknown")))) {
        __ret = a_err(a_add(a_string("no plugin.toml or plugin.json found in "), source_dir)); goto __fn_cleanup;
    }
    { AValue __old = dest; dest = a_add(a_add(fn_plugin__plugins_dir(), a_string("/")), name); a_release(__old); }
    if (a_truthy(a_fs_exists(dest))) {
        a_exec(a_add(a_string("rm -rf "), dest));
    }
    a_exec(a_add(a_add(a_add(a_string("cp -r "), source_dir), a_string(" ")), dest));
    { AValue __old = reg; reg = fn_plugin__load_registry(); a_release(__old); }
    { AValue __old = plugins; plugins = a_array_get(reg, a_string("plugins")); a_release(__old); }
    { AValue __old = plugins; plugins = a_map_set(plugins, name, a_map_new(6, "name", name, "version", a_array_get(manifest, a_string("version")), "dir", dest, "entry", fn_plugin__entry_point(manifest, dest), "capabilities", fn_plugin__plugin_caps(manifest), "installed_at", a_time_now())); a_release(__old); }
    { AValue __old = reg; reg = a_map_set(reg, a_string("plugins"), plugins); a_release(__old); }
    fn_plugin__save_registry(reg);
    __ret = a_ok(a_map_new(3, "name", name, "version", a_array_get(manifest, a_string("version")), "dir", dest)); goto __fn_cleanup;
__fn_cleanup:
    a_release(manifest);
    a_release(name);
    a_release(dest);
    a_release(reg);
    a_release(plugins);
    a_release(source_dir);
    return __ret;
}

AValue fn_plugin_install_git(AValue repo) {
    AValue tmp = {0}, url = {0}, result = {0}, r = {0};
    AValue __ret = a_void();
    repo = a_retain(repo);
    fn_plugin__ensure_plugins_dir();
    { AValue __old = tmp; tmp = a_add(a_string("/tmp/a_plugin_"), a_to_str(a_time_now())); a_release(__old); }
    { AValue __old = url; url = a_add(a_add(a_string("https://github.com/"), repo), a_string(".git")); a_release(__old); }
    { AValue __old = result; result = a_exec(a_add(a_add(a_add(a_add(a_string("git clone --depth 1 "), url), a_string(" ")), tmp), a_string(" 2>&1"))); a_release(__old); }
    if (a_truthy(a_neq(a_array_get(result, a_string("code")), a_int(0)))) {
        __ret = a_err(a_add(a_string("git clone failed: "), a_array_get(result, a_string("stderr")))); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_plugin_install(tmp); a_release(__old); }
    a_exec(a_add(a_string("rm -rf "), tmp));
    __ret = a_retain(r); goto __fn_cleanup;
__fn_cleanup:
    a_release(tmp);
    a_release(url);
    a_release(result);
    a_release(r);
    a_release(repo);
    return __ret;
}

AValue fn_plugin__entry_point(AValue manifest, AValue dir) {
    AValue __ret = a_void();
    manifest = a_retain(manifest);
    dir = a_retain(dir);
    if (a_truthy(a_map_has(manifest, a_string("entry")))) {
        __ret = a_add(a_add(dir, a_string("/")), a_array_get(manifest, a_string("entry"))); goto __fn_cleanup;
    }
    if (a_truthy(a_fs_exists(a_add(dir, a_string("/main.a"))))) {
        __ret = a_add(dir, a_string("/main.a")); goto __fn_cleanup;
    }
    if (a_truthy(a_fs_exists(a_add(dir, a_string("/init.a"))))) {
        __ret = a_add(dir, a_string("/init.a")); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(manifest);
    a_release(dir);
    return __ret;
}

AValue fn_plugin__plugin_caps(AValue manifest) {
    AValue caps_str = {0}, parts = {0}, result = {0}, i = {0}, cap = {0};
    AValue __ret = a_void();
    manifest = a_retain(manifest);
    if (a_truthy(a_map_has(manifest, a_string("capabilities")))) {
        { AValue __old = caps_str; caps_str = a_array_get(manifest, a_string("capabilities")); a_release(__old); }
        { AValue __old = parts; parts = a_str_split(caps_str, a_string(",")); a_release(__old); }
        { AValue __old = result; result = a_map_new(0); a_release(__old); }
        { AValue __old = i; i = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(parts)))) {
            { AValue __old = cap; cap = a_str_trim(a_array_get(parts, i)); a_release(__old); }
            if (a_truthy(a_gt(a_len(cap), a_int(0)))) {
                { AValue __old = result; result = a_map_set(result, cap, a_bool(1)); a_release(__old); }
            }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
        }
        __ret = a_retain(result); goto __fn_cleanup;
    }
    __ret = a_map_new(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(caps_str);
    a_release(parts);
    a_release(result);
    a_release(i);
    a_release(cap);
    a_release(manifest);
    return __ret;
}

AValue fn_plugin_remove(AValue name) {
    AValue reg = {0}, plugins = {0}, info = {0}, pdir = {0}, new_plugins = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    { AValue __old = reg; reg = fn_plugin__load_registry(); a_release(__old); }
    { AValue __old = plugins; plugins = a_array_get(reg, a_string("plugins")); a_release(__old); }
    if (a_truthy(a_not(a_map_has(plugins, name)))) {
        __ret = a_err(a_add(a_string("not installed: "), name)); goto __fn_cleanup;
    }
    { AValue __old = info; info = a_array_get(plugins, name); a_release(__old); }
    { AValue __old = pdir; pdir = a_array_get(info, a_string("dir")); a_release(__old); }
    if (a_truthy(a_fs_exists(pdir))) {
        a_exec(a_add(a_string("rm -rf "), pdir));
    }
    { AValue __old = new_plugins; new_plugins = a_map_delete(plugins, name); a_release(__old); }
    fn_plugin__save_registry(a_map_set(reg, a_string("plugins"), new_plugins));
    __ret = a_ok(name); goto __fn_cleanup;
__fn_cleanup:
    a_release(reg);
    a_release(plugins);
    a_release(info);
    a_release(pdir);
    a_release(new_plugins);
    a_release(name);
    return __ret;
}

AValue fn_plugin_list(void) {
    AValue reg = {0}, plugins = {0}, kk = {0}, result = {0}, i = {0};
    AValue __ret = a_void();
    { AValue __old = reg; reg = fn_plugin__load_registry(); a_release(__old); }
    { AValue __old = plugins; plugins = a_array_get(reg, a_string("plugins")); a_release(__old); }
    { AValue __old = kk; kk = a_map_keys(plugins); a_release(__old); }
    { AValue __old = result; result = a_array_new(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(kk)))) {
        { AValue __old = result; result = a_array_push(result, a_array_get(plugins, a_array_get(kk, i))); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(result); goto __fn_cleanup;
__fn_cleanup:
    a_release(reg);
    a_release(plugins);
    a_release(kk);
    a_release(result);
    a_release(i);
    return __ret;
}

AValue fn_plugin_get(AValue name) {
    AValue reg = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    { AValue __old = reg; reg = fn_plugin__load_registry(); a_release(__old); }
    if (a_truthy(a_map_has(a_array_get(reg, a_string("plugins")), name))) {
        __ret = a_ok(a_array_get(a_array_get(reg, a_string("plugins")), name)); goto __fn_cleanup;
    }
    __ret = a_err(a_add(a_string("not installed: "), name)); goto __fn_cleanup;
__fn_cleanup:
    a_release(reg);
    a_release(name);
    return __ret;
}

AValue fn_plugin_is_installed(AValue name) {
    AValue reg = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    { AValue __old = reg; reg = fn_plugin__load_registry(); a_release(__old); }
    __ret = a_map_has(a_array_get(reg, a_string("plugins")), name); goto __fn_cleanup;
__fn_cleanup:
    a_release(reg);
    a_release(name);
    return __ret;
}

AValue fn_plugin_load(AValue name) {
    AValue info_result = {0}, info = {0}, ep = {0}, source = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    { AValue __old = info_result; info_result = fn_plugin_get(name); a_release(__old); }
    if (a_truthy(a_is_err(info_result))) {
        __ret = a_retain(info_result); goto __fn_cleanup;
    }
    { AValue __old = info; info = a_unwrap(info_result); a_release(__old); }
    { AValue __old = ep; ep = a_array_get(info, a_string("entry")); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_len(ep), a_int(0)), a_not(a_fs_exists(ep))))) {
        __ret = a_err(a_add(a_string("no entry point for "), name)); goto __fn_cleanup;
    }
    { AValue __old = source; source = a_io_read_file(ep); a_release(__old); }
    __ret = a_ok(a_map_new(3, "name", name, "source", source, "info", info)); goto __fn_cleanup;
__fn_cleanup:
    a_release(info_result);
    a_release(info);
    a_release(ep);
    a_release(source);
    a_release(name);
    return __ret;
}

AValue fn_plugin_run(AValue name) {
    AValue loaded = {0}, pl = {0}, cli = {0}, r = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    { AValue __old = loaded; loaded = fn_plugin_load(name); a_release(__old); }
    if (a_truthy(a_is_err(loaded))) {
        __ret = a_retain(loaded); goto __fn_cleanup;
    }
    { AValue __old = pl; pl = a_unwrap(loaded); a_release(__old); }
    { AValue __old = cli; cli = fn_plugin__find_cli(); a_release(__old); }
    { AValue __old = r; r = a_exec(a_add(a_add(cli, a_string(" run ")), a_array_get(a_array_get(pl, a_string("info")), a_string("entry")))); a_release(__old); }
    __ret = a_ok(a_map_new(3, "stdout", a_array_get(r, a_string("stdout")), "stderr", a_array_get(r, a_string("stderr")), "code", a_array_get(r, a_string("code")))); goto __fn_cleanup;
__fn_cleanup:
    a_release(loaded);
    a_release(pl);
    a_release(cli);
    a_release(r);
    a_release(name);
    return __ret;
}

AValue fn_plugin__find_cli(void) {
    AValue which = {0};
    AValue __ret = a_void();
    if (a_truthy(a_fs_exists(a_string("./a")))) {
        __ret = a_string("./a"); goto __fn_cleanup;
    }
    { AValue __old = which; which = a_exec(a_string("which a")); a_release(__old); }
    if (a_truthy(a_eq(a_array_get(which, a_string("code")), a_int(0)))) {
        __ret = a_str_trim(a_array_get(which, a_string("stdout"))); goto __fn_cleanup;
    }
    __ret = a_string("a"); goto __fn_cleanup;
__fn_cleanup:
    a_release(which);
    return __ret;
}

AValue fn_plugin_create_manifest(AValue name, AValue version, AValue desc, AValue ep) {
    AValue q = {0}, out = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    version = a_retain(version);
    desc = a_retain(desc);
    ep = a_retain(ep);
    { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
    { AValue __old = out; out = a_array_new(0); a_release(__old); }
    { AValue __old = out; out = a_array_push(out, a_add(a_add(a_add(a_string("name = "), q), name), q)); a_release(__old); }
    { AValue __old = out; out = a_array_push(out, a_add(a_add(a_add(a_string("version = "), q), version), q)); a_release(__old); }
    { AValue __old = out; out = a_array_push(out, a_add(a_add(a_add(a_string("description = "), q), desc), q)); a_release(__old); }
    { AValue __old = out; out = a_array_push(out, a_add(a_add(a_add(a_string("entry = "), q), ep), q)); a_release(__old); }
    __ret = a_add(a_str_join(out, a_string("\n")), a_string("\n")); goto __fn_cleanup;
__fn_cleanup:
    a_release(q);
    a_release(out);
    a_release(name);
    a_release(version);
    a_release(desc);
    a_release(ep);
    return __ret;
}

AValue fn_plugin_init(AValue dir, AValue name) {
    AValue manifest = {0}, q = {0}, lb = {0}, rb = {0}, main_src = {0};
    AValue __ret = a_void();
    dir = a_retain(dir);
    name = a_retain(name);
    if (a_truthy(a_not(a_fs_exists(dir)))) {
        a_fs_mkdir(dir);
    }
    { AValue __old = manifest; manifest = fn_plugin_create_manifest(name, a_string("0.1.0"), a_string("An a-lang module"), a_string("main.a")); a_release(__old); }
    a_io_write_file(a_add(dir, a_string("/plugin.toml")), manifest);
    { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = main_src; main_src = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("fn main() "), lb), a_string("\n  println(")), q), a_string("hello from ")), name), q), a_string(")\n")), rb), a_string("\n")); a_release(__old); }
    a_io_write_file(a_add(dir, a_string("/main.a")), main_src);
    __ret = a_ok(dir); goto __fn_cleanup;
__fn_cleanup:
    a_release(manifest);
    a_release(q);
    a_release(lb);
    a_release(rb);
    a_release(main_src);
    a_release(dir);
    a_release(name);
    return __ret;
}

AValue fn__version(void) {
    AValue __ret = a_void();
    __ret = a_string("2.1.0"); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn__die(AValue msg) {
    AValue __ret = a_void();
    msg = a_retain(msg);
    a_eprintln(fn_cli_red(a_str_concat(a_string("error: "), msg)));
    (exit((int)a_int(1).ival), a_void());
__fn_cleanup:
    a_release(msg);
    return __ret;
}

AValue fn__find_runtime_dir(void) {
    AValue candidates = {0}, cache = {0}, probe = {0}, rt_files = {0}, content = {0}, stub = {0}, stub2 = {0};
    AValue __ret = a_void();
    { AValue __old = candidates; candidates = a_array_new(2, a_string("c_runtime"), a_string("../c_runtime")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(candidates);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue dir = {0};
            dir = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_fs_exists(a_str_concat(dir, a_string("/runtime.c"))))) {
                __ret = a_retain(dir); goto __fn_cleanup;
            }
            a_release(dir);
        }
        a_release(__iter_arr);
    }
    { AValue __old = cache; cache = a_string("/tmp/a_rt"); a_release(__old); }
    if (a_truthy(a_fs_exists(a_str_concat(cache, a_string("/runtime.c"))))) {
        __ret = a_retain(cache); goto __fn_cleanup;
    }
    { AValue __old = probe; probe = a_embedded_file(a_string("c_runtime/runtime.c")); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(probe), a_string("void")), a_eq(a_len(probe), a_int(0))))) {
        fn__die(a_string("cannot find c_runtime/ directory and no embedded runtime available"));
    }
    a_fs_mkdir(cache);
    { AValue __old = rt_files; rt_files = a_array_new(9, a_string("runtime.c"), a_string("runtime.h"), a_string("sqlite3.c"), a_string("sqlite3.h"), a_string("miniz.c"), a_string("miniz.h"), a_string("stb_image.h"), a_string("stb_image_write.h"), a_string("stb_impl.c")); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(rt_files);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue f = {0}, content = {0};
            f = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = content; content = a_embedded_file(a_add(a_string("c_runtime/"), f)); a_release(__old); }
            if (a_truthy(a_neq(a_type_of(content), a_string("void")))) {
                a_io_write_file(a_add(a_add(cache, a_string("/")), f), content);
            }
            a_release(f);
            a_release(content);
        }
        a_release(__iter_arr);
    }
    { AValue __old = stub; stub = a_add(a_add(a_add(a_add(a_string("#include "), a_from_code(a_int(34))), a_string("runtime.h")), a_from_code(a_int(34))), a_from_code(a_int(10))); a_release(__old); }
    { AValue __old = stub2; stub2 = a_add(a_add(a_add(a_add(a_add(stub, a_string("AValue a_embedded_file(AValue p)")), a_from_code(a_int(123))), a_string("(void)p;return a_void();")), a_from_code(a_int(125))), a_from_code(a_int(10))); a_release(__old); }
    a_io_write_file(a_add(cache, a_string("/embedded.c")), stub2);
    __ret = a_retain(cache); goto __fn_cleanup;
__fn_cleanup:
    a_release(candidates);
    a_release(cache);
    a_release(probe);
    a_release(rt_files);
    a_release(content);
    a_release(stub);
    a_release(stub2);
    return __ret;
}

AValue fn__generate_c(AValue source_path) {
    AValue source = {0}, lines = {0}, prog_ast = {0}, bm = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        fn__die(a_str_concat(a_string("cannot read file: "), source_path));
    }
    if (a_truthy(a_str_starts_with(source, a_string("#!")))) {
        { AValue __old = lines; lines = a_str_lines(source); a_release(__old); }
        { AValue __old = source; source = a_str_join(a_drop(lines, a_int(1)), a_string("\n")); a_release(__old); }
    }
    { AValue __old = prog_ast; prog_ast = fn_parser_parse(source); a_release(__old); }
    if (a_truthy(a_and(a_eq(a_type_of(prog_ast), a_string("map")), a_map_has(prog_ast, a_string("tag"))))) {
        if (a_truthy(a_eq(a_array_get(prog_ast, a_string("tag")), a_string("ParseError")))) {
            fn__die(a_str_concat(a_string("parse error: "), a_str_concat(a_array_get(prog_ast, a_string("msg")), a_str_concat(a_string(" at token "), a_to_str(a_array_get(prog_ast, a_string("pos")))))));
        }
    }
    { AValue __old = bm; bm = fn_cgen__builtin_map(); a_release(__old); }
    __ret = fn_cgen_emit_program(prog_ast, bm); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(lines);
    a_release(prog_ast);
    a_release(bm);
    a_release(source_path);
    return __ret;
}

AValue fn__gcc_flags(void) {
    AValue uname_result = {0}, os = {0};
    AValue __ret = a_void();
    { AValue __old = uname_result; uname_result = a_exec(a_string("uname -s")); a_release(__old); }
    { AValue __old = os; os = a_str_trim(a_array_get(uname_result, a_string("stdout"))); a_release(__old); }
    if (a_truthy(a_eq(os, a_string("Darwin")))) {
        __ret = a_string("-lm -O2 -Wl,-stack_size,0x10000000 -framework Security -framework CoreFoundation"); goto __fn_cleanup;
    }
    __ret = a_string("-lm -O2 -ldl"); goto __fn_cleanup;
__fn_cleanup:
    a_release(uname_result);
    a_release(os);
    return __ret;
}

AValue fn__sqlite_flags(void) {
    AValue __ret = a_void();
    __ret = a_string("-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION"); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn__ensure_runtime_objs(AValue runtime_dir) {
    AValue sentinel = {0}, h = {0}, obj_dir = {0}, inc = {0}, sf = {0}, sources = {0}, r = {0};
    AValue __ret = a_void();
    runtime_dir = a_retain(runtime_dir);
    { AValue __old = sentinel; sentinel = a_io_read_file(a_add(runtime_dir, a_string("/runtime.c"))); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(sentinel), a_string("void")), a_eq(a_len(sentinel), a_int(0))))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = h; h = a_hash_sha256(sentinel); a_release(__old); }
    { AValue __old = obj_dir; obj_dir = a_add(a_string("/tmp/a_obj/"), h); a_release(__old); }
    if (a_truthy(a_fs_exists(a_add(obj_dir, a_string("/runtime.o"))))) {
        __ret = a_retain(obj_dir); goto __fn_cleanup;
    }
    a_eprintln(a_string("precompiling runtime (one-time)..."));
    a_exec(a_add(a_string("mkdir -p "), obj_dir));
    { AValue __old = inc; inc = a_add(a_string("-I "), runtime_dir); a_release(__old); }
    { AValue __old = sf; sf = fn__sqlite_flags(); a_release(__old); }
    { AValue __old = sources; sources = a_array_new(6, a_array_new(2, a_string("runtime.c"), a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("gcc -c "), runtime_dir), a_string("/runtime.c ")), inc), a_string(" -O2 ")), sf), a_string(" -o ")), obj_dir), a_string("/runtime.o"))), a_array_new(2, a_string("gguf.c"), a_add(a_add(a_add(a_add(a_add(a_add(a_string("gcc -c "), runtime_dir), a_string("/gguf.c ")), inc), a_string(" -O2 -o ")), obj_dir), a_string("/gguf.o"))), a_array_new(2, a_string("sqlite3.c"), a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("gcc -c "), runtime_dir), a_string("/sqlite3.c ")), inc), a_string(" -O2 ")), sf), a_string(" -o ")), obj_dir), a_string("/sqlite3.o"))), a_array_new(2, a_string("miniz.c"), a_add(a_add(a_add(a_add(a_add(a_add(a_string("gcc -c "), runtime_dir), a_string("/miniz.c ")), inc), a_string(" -O2 -o ")), obj_dir), a_string("/miniz.o"))), a_array_new(2, a_string("stb_impl.c"), a_add(a_add(a_add(a_add(a_add(a_add(a_string("gcc -c "), runtime_dir), a_string("/stb_impl.c ")), inc), a_string(" -O2 -o ")), obj_dir), a_string("/stb_impl.o"))), a_array_new(2, a_string("embedded.c"), a_add(a_add(a_add(a_add(a_add(a_add(a_string("gcc -c "), runtime_dir), a_string("/embedded.c ")), inc), a_string(" -O2 -o ")), obj_dir), a_string("/embedded.o")))); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(sources);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue pair = {0}, r = {0};
            pair = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = r; r = a_exec(a_array_get(pair, a_int(1))); a_release(__old); }
            if (a_truthy(a_neq(a_array_get(r, a_string("code")), a_int(0)))) {
                a_eprintln(fn_cli_red(a_add(a_string("failed to precompile "), a_array_get(pair, a_int(0)))));
                if (a_truthy(a_gt(a_len(a_array_get(r, a_string("stderr"))), a_int(0)))) {
                    a_eprintln(a_array_get(r, a_string("stderr")));
                }
                a_exec(a_add(a_string("rm -rf "), obj_dir));
                __ret = a_string(""); goto __fn_cleanup;
            }
            a_release(pair);
            a_release(r);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(obj_dir); goto __fn_cleanup;
__fn_cleanup:
    a_release(sentinel);
    a_release(h);
    a_release(obj_dir);
    a_release(inc);
    a_release(sf);
    a_release(sources);
    a_release(r);
    a_release(runtime_dir);
    return __ret;
}

AValue fn__gcc_try(AValue c_path, AValue out_path, AValue runtime_dir) {
    AValue obj_dir = {0}, inc = {0}, flags = {0}, sf = {0}, user_o = {0}, compile = {0}, cr = {0}, link = {0}, lr = {0}, cmd = {0}, result = {0};
    AValue __ret = a_void();
    c_path = a_retain(c_path);
    out_path = a_retain(out_path);
    runtime_dir = a_retain(runtime_dir);
    { AValue __old = obj_dir; obj_dir = fn__ensure_runtime_objs(runtime_dir); a_release(__old); }
    { AValue __old = inc; inc = a_add(a_string("-I "), runtime_dir); a_release(__old); }
    { AValue __old = flags; flags = fn__gcc_flags(); a_release(__old); }
    { AValue __old = sf; sf = fn__sqlite_flags(); a_release(__old); }
    if (a_truthy(a_gt(a_len(obj_dir), a_int(0)))) {
        { AValue __old = user_o; user_o = fn__tmp_path(a_string(".o")); a_release(__old); }
        { AValue __old = compile; compile = a_str_join(a_array_new(7, a_string("gcc -c"), c_path, inc, a_string("-O2"), sf, a_string("-o"), user_o), a_string(" ")); a_release(__old); }
        { AValue __old = cr; cr = a_exec(compile); a_release(__old); }
        if (a_truthy(a_neq(a_array_get(cr, a_string("code")), a_int(0)))) {
            a_fs_rm(user_o);
            __ret = a_array_get(cr, a_string("code")); goto __fn_cleanup;
        }
        { AValue __old = link; link = a_str_join(a_array_new(11, a_string("gcc"), user_o, a_add(obj_dir, a_string("/runtime.o")), a_add(obj_dir, a_string("/gguf.o")), a_add(obj_dir, a_string("/sqlite3.o")), a_add(obj_dir, a_string("/miniz.o")), a_add(obj_dir, a_string("/stb_impl.o")), a_add(obj_dir, a_string("/embedded.o")), a_string("-o"), out_path, flags), a_string(" ")); a_release(__old); }
        { AValue __old = lr; lr = a_exec(link); a_release(__old); }
        a_fs_rm(user_o);
        __ret = a_array_get(lr, a_string("code")); goto __fn_cleanup;
    }
    { AValue __old = cmd; cmd = a_str_join(a_array_new(13, a_string("gcc"), c_path, a_add(runtime_dir, a_string("/runtime.c")), a_add(runtime_dir, a_string("/gguf.c")), a_add(runtime_dir, a_string("/sqlite3.c")), a_add(runtime_dir, a_string("/miniz.c")), a_add(runtime_dir, a_string("/stb_impl.c")), a_add(runtime_dir, a_string("/embedded.c")), a_string("-o"), out_path, inc, flags, sf), a_string(" ")); a_release(__old); }
    { AValue __old = result; result = a_exec(cmd); a_release(__old); }
    __ret = a_array_get(result, a_string("code")); goto __fn_cleanup;
__fn_cleanup:
    a_release(obj_dir);
    a_release(inc);
    a_release(flags);
    a_release(sf);
    a_release(user_o);
    a_release(compile);
    a_release(cr);
    a_release(link);
    a_release(lr);
    a_release(cmd);
    a_release(result);
    a_release(c_path);
    a_release(out_path);
    a_release(runtime_dir);
    return __ret;
}

AValue fn__gcc(AValue c_path, AValue out_path, AValue runtime_dir) {
    AValue code = {0};
    AValue __ret = a_void();
    c_path = a_retain(c_path);
    out_path = a_retain(out_path);
    runtime_dir = a_retain(runtime_dir);
    { AValue __old = code; code = fn__gcc_try(c_path, out_path, runtime_dir); a_release(__old); }
    if (a_truthy(a_neq(code, a_int(0)))) {
        a_eprintln(fn_cli_red(a_string("gcc compilation failed")));
        (exit((int)a_int(1).ival), a_void());
    }
__fn_cleanup:
    a_release(code);
    a_release(c_path);
    a_release(out_path);
    a_release(runtime_dir);
    return __ret;
}

AValue fn__tmp_path(AValue suffix) {
    AValue ts = {0};
    AValue __ret = a_void();
    suffix = a_retain(suffix);
    { AValue __old = ts; ts = a_to_str(a_time_now()); a_release(__old); }
    __ret = a_str_concat(a_string("/tmp/a_cli_"), a_str_concat(ts, suffix)); goto __fn_cleanup;
__fn_cleanup:
    a_release(ts);
    a_release(suffix);
    return __ret;
}

AValue fn_cmd_cc(AValue source_path, AValue out_file) {
    AValue c_code = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    out_file = a_retain(out_file);
    { AValue __old = c_code; c_code = fn__generate_c(source_path); a_release(__old); }
    if (a_truthy(a_gt(a_len(out_file), a_int(0)))) {
        a_io_write_file(out_file, c_code);
    } else {
        a_print(c_code);
    }
__fn_cleanup:
    a_release(c_code);
    a_release(source_path);
    a_release(out_file);
    return __ret;
}

AValue fn_cmd_wat(AValue source_path, AValue out_file) {
    AValue source = {0}, wat_code = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    out_file = a_retain(out_file);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    { AValue __old = wat_code; wat_code = fn_wasmgen_compile(source); a_release(__old); }
    if (a_truthy(a_gt(a_len(out_file), a_int(0)))) {
        a_io_write_file(out_file, wat_code);
        a_eprintln(a_add(a_add(fn_cli_green(a_string("generated")), a_string(" ")), out_file));
    } else {
        a_print(wat_code);
    }
__fn_cleanup:
    a_release(source);
    a_release(wat_code);
    a_release(source_path);
    a_release(out_file);
    return __ret;
}

AValue fn__codegen_subprocess(AValue source_path, AValue c_path) {
    AValue self = {0}, cmd = {0}, attempts = {0}, max_attempts = {0}, result = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    c_path = a_retain(c_path);
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = cmd; cmd = a_str_join(a_array_new(5, self, a_string("cc"), source_path, a_string("-o"), c_path), a_string(" ")); a_release(__old); }
    { AValue __old = attempts; attempts = a_int(0); a_release(__old); }
    { AValue __old = max_attempts; max_attempts = a_int(5); a_release(__old); }
    while (a_truthy(a_lt(attempts, max_attempts))) {
        { AValue __old = result; result = a_exec(cmd); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(result, a_string("code")), a_int(0)))) {
            __ret = a_void(); goto __fn_cleanup;
        }
        { AValue __old = attempts; attempts = a_add(attempts, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(attempts, max_attempts))) {
            a_eprintln(fn_cli_red(a_str_concat(a_string("codegen failed after "), a_str_concat(a_to_str(max_attempts), a_string(" attempts")))));
            if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stderr"))), a_int(0)))) {
                a_eprintln(a_array_get(result, a_string("stderr")));
            }
            (exit((int)a_int(1).ival), a_void());
        }
    }
__fn_cleanup:
    a_release(self);
    a_release(cmd);
    a_release(attempts);
    a_release(max_attempts);
    a_release(result);
    a_release(source_path);
    a_release(c_path);
    return __ret;
}

AValue fn_cmd_build(AValue source_path, AValue out_path) {
    AValue runtime_dir = {0}, c_path = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    out_path = a_retain(out_path);
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = c_path; c_path = fn__tmp_path(a_string(".c")); a_release(__old); }
    fn__codegen_subprocess(source_path, c_path);
    fn__gcc(c_path, out_path, runtime_dir);
    a_fs_rm(c_path);
    a_eprintln(a_str_concat(fn_cli_green(a_string("built")), a_str_concat(a_string(" "), out_path)));
__fn_cleanup:
    a_release(runtime_dir);
    a_release(c_path);
    a_release(source_path);
    a_release(out_path);
    return __ret;
}

AValue fn__known_targets(void) {
    AValue __ret = a_void();
    __ret = a_array_new(6, a_string("wasm32-wasi"), a_string("linux-x86_64"), a_string("linux-aarch64"), a_string("windows-x86_64"), a_string("macos-x86_64"), a_string("macos-aarch64")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn__target_compiler(AValue tgt) {
    AValue has_emcc = {0}, has_clang = {0}, ccs = {0}, r = {0}, uname = {0}, arch = {0};
    AValue __ret = a_void();
    tgt = a_retain(tgt);
    if (a_truthy(a_eq(tgt, a_string("wasm32-wasi")))) {
        { AValue __old = has_emcc; has_emcc = a_exec(a_string("which emcc 2>/dev/null")); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(has_emcc, a_string("code")), a_int(0)))) {
            __ret = a_string("emcc"); goto __fn_cleanup;
        }
        { AValue __old = has_clang; has_clang = a_exec(a_string("which clang 2>/dev/null")); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(has_clang, a_string("code")), a_int(0)))) {
            __ret = a_string("clang"); goto __fn_cleanup;
        }
        __ret = a_string(""); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("linux-x86_64")))) {
        { AValue __old = ccs; ccs = a_array_new(3, a_string("x86_64-linux-musl-gcc"), a_string("x86_64-linux-gnu-gcc"), a_string("x86_64-unknown-linux-gnu-gcc")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(ccs);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue cc = {0}, r = {0};
                cc = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = r; r = a_exec(a_add(a_add(a_string("which "), cc), a_string(" 2>/dev/null"))); a_release(__old); }
                if (a_truthy(a_eq(a_array_get(r, a_string("code")), a_int(0)))) {
                    __ret = a_retain(cc); goto __fn_cleanup;
                }
                a_release(cc);
                a_release(r);
            }
            a_release(__iter_arr);
        }
        { AValue __old = uname; uname = a_str_trim(a_array_get(a_exec(a_string("uname -s")), a_string("stdout"))); a_release(__old); }
        if (a_truthy(a_eq(uname, a_string("Linux")))) {
            { AValue __old = arch; arch = a_str_trim(a_array_get(a_exec(a_string("uname -m")), a_string("stdout"))); a_release(__old); }
            if (a_truthy(a_eq(arch, a_string("x86_64")))) {
                __ret = a_string("gcc"); goto __fn_cleanup;
            }
        }
        __ret = a_string(""); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("linux-aarch64")))) {
        { AValue __old = ccs; ccs = a_array_new(2, a_string("aarch64-linux-musl-gcc"), a_string("aarch64-linux-gnu-gcc")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(ccs);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue cc = {0}, r = {0};
                cc = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = r; r = a_exec(a_add(a_add(a_string("which "), cc), a_string(" 2>/dev/null"))); a_release(__old); }
                if (a_truthy(a_eq(a_array_get(r, a_string("code")), a_int(0)))) {
                    __ret = a_retain(cc); goto __fn_cleanup;
                }
                a_release(cc);
                a_release(r);
            }
            a_release(__iter_arr);
        }
        { AValue __old = uname; uname = a_str_trim(a_array_get(a_exec(a_string("uname -s")), a_string("stdout"))); a_release(__old); }
        if (a_truthy(a_eq(uname, a_string("Linux")))) {
            { AValue __old = arch; arch = a_str_trim(a_array_get(a_exec(a_string("uname -m")), a_string("stdout"))); a_release(__old); }
            if (a_truthy(a_eq(arch, a_string("aarch64")))) {
                __ret = a_string("gcc"); goto __fn_cleanup;
            }
        }
        __ret = a_string(""); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("windows-x86_64")))) {
        { AValue __old = ccs; ccs = a_array_new(2, a_string("x86_64-w64-mingw32-gcc"), a_string("i686-w64-mingw32-gcc")); a_release(__old); }
        {
            AValue __iter_arr = a_iterable(ccs);
            for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                AValue cc = {0}, r = {0};
                cc = a_array_get(__iter_arr, a_int(__fi));
                { AValue __old = r; r = a_exec(a_add(a_add(a_string("which "), cc), a_string(" 2>/dev/null"))); a_release(__old); }
                if (a_truthy(a_eq(a_array_get(r, a_string("code")), a_int(0)))) {
                    __ret = a_retain(cc); goto __fn_cleanup;
                }
                a_release(cc);
                a_release(r);
            }
            a_release(__iter_arr);
        }
        __ret = a_string(""); goto __fn_cleanup;
    }
    if (a_truthy(a_or(a_eq(tgt, a_string("macos-x86_64")), a_eq(tgt, a_string("macos-aarch64"))))) {
        { AValue __old = uname; uname = a_str_trim(a_array_get(a_exec(a_string("uname -s")), a_string("stdout"))); a_release(__old); }
        if (a_truthy(a_eq(uname, a_string("Darwin")))) {
            __ret = a_string("gcc"); goto __fn_cleanup;
        }
        __ret = a_string(""); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(has_emcc);
    a_release(has_clang);
    a_release(ccs);
    a_release(r);
    a_release(uname);
    a_release(arch);
    a_release(tgt);
    return __ret;
}

AValue fn__target_flags(AValue tgt) {
    AValue __ret = a_void();
    tgt = a_retain(tgt);
    if (a_truthy(a_eq(tgt, a_string("wasm32-wasi")))) {
        __ret = a_string("-O2 -DWASM_BUILD -DNO_FORK -DNO_SIGNAL -DNO_IMAGE"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("linux-x86_64")))) {
        __ret = a_string("-lm -O2 -ldl -DLINUX_BUILD"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("linux-aarch64")))) {
        __ret = a_string("-lm -O2 -ldl -DLINUX_BUILD"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("windows-x86_64")))) {
        __ret = a_string("-lm -O2 -lws2_32 -DWINDOWS_BUILD"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("macos-x86_64")))) {
        __ret = a_string("-lm -O2 -target x86_64-apple-macos11 -framework Security -framework CoreFoundation"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("macos-aarch64")))) {
        __ret = a_string("-lm -O2 -target arm64-apple-macos11 -framework Security -framework CoreFoundation"); goto __fn_cleanup;
    }
    __ret = a_string("-lm -O2"); goto __fn_cleanup;
__fn_cleanup:
    a_release(tgt);
    return __ret;
}

AValue fn__target_ext(AValue tgt) {
    AValue __ret = a_void();
    tgt = a_retain(tgt);
    if (a_truthy(a_eq(tgt, a_string("wasm32-wasi")))) {
        __ret = a_string(".wasm"); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(tgt, a_string("windows-x86_64")))) {
        __ret = a_string(".exe"); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(tgt);
    return __ret;
}

AValue fn__target_emcc_flags(void) {
    AValue __ret = a_void();
    __ret = a_string("-s WASI=1 -s STANDALONE_WASM=1 -s TOTAL_MEMORY=67108864 --no-entry"); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cmd_build_target(AValue source_path, AValue out_path, AValue tgt) {
    AValue runtime_dir = {0}, c_path = {0}, cc = {0}, flags = {0}, sf = {0}, inc = {0}, cmd = {0}, emcc_extra = {0}, r = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    out_path = a_retain(out_path);
    tgt = a_retain(tgt);
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = c_path; c_path = fn__tmp_path(a_string(".c")); a_release(__old); }
    fn__codegen_subprocess(source_path, c_path);
    { AValue __old = cc; cc = fn__target_compiler(tgt); a_release(__old); }
    if (a_truthy(a_eq(a_len(cc), a_int(0)))) {
        a_fs_rm(c_path);
        fn__die(a_add(a_add(a_string("no compiler found for target: "), tgt), a_string(". Install the appropriate cross-compilation toolchain.")));
    }
    { AValue __old = flags; flags = fn__target_flags(tgt); a_release(__old); }
    { AValue __old = sf; sf = fn__sqlite_flags(); a_release(__old); }
    { AValue __old = inc; inc = a_add(a_string("-I "), runtime_dir); a_release(__old); }
    { AValue __old = cmd; cmd = a_string(""); a_release(__old); }
    if (a_truthy(a_and(a_eq(tgt, a_string("wasm32-wasi")), a_eq(cc, a_string("emcc"))))) {
        { AValue __old = emcc_extra; emcc_extra = fn__target_emcc_flags(); a_release(__old); }
        { AValue __old = cmd; cmd = a_str_join(a_array_new(13, cc, c_path, a_add(runtime_dir, a_string("/runtime.c")), a_add(runtime_dir, a_string("/gguf.c")), a_add(runtime_dir, a_string("/sqlite3.c")), a_add(runtime_dir, a_string("/miniz.c")), a_add(runtime_dir, a_string("/embedded.c")), a_string("-o"), out_path, inc, flags, sf, emcc_extra), a_string(" ")); a_release(__old); }
    } else {
        if (a_truthy(a_and(a_eq(tgt, a_string("wasm32-wasi")), a_eq(cc, a_string("clang"))))) {
            { AValue __old = cmd; cmd = a_str_join(a_array_new(14, cc, a_string("--target=wasm32-wasi"), a_string("--sysroot=/opt/wasi-sdk/share/wasi-sysroot"), c_path, a_add(runtime_dir, a_string("/runtime.c")), a_add(runtime_dir, a_string("/gguf.c")), a_add(runtime_dir, a_string("/sqlite3.c")), a_add(runtime_dir, a_string("/miniz.c")), a_add(runtime_dir, a_string("/embedded.c")), a_string("-o"), out_path, inc, flags, sf), a_string(" ")); a_release(__old); }
        } else {
            { AValue __old = cmd; cmd = a_str_join(a_array_new(13, cc, c_path, a_add(runtime_dir, a_string("/runtime.c")), a_add(runtime_dir, a_string("/gguf.c")), a_add(runtime_dir, a_string("/sqlite3.c")), a_add(runtime_dir, a_string("/miniz.c")), a_add(runtime_dir, a_string("/stb_impl.c")), a_add(runtime_dir, a_string("/embedded.c")), a_string("-o"), out_path, inc, flags, sf), a_string(" ")); a_release(__old); }
        }
    }
    { AValue __old = r; r = a_exec(cmd); a_release(__old); }
    a_fs_rm(c_path);
    if (a_truthy(a_neq(a_array_get(r, a_string("code")), a_int(0)))) {
        if (a_truthy(a_gt(a_len(a_array_get(r, a_string("stderr"))), a_int(0)))) {
            a_eprintln(a_array_get(r, a_string("stderr")));
        }
        fn__die(a_add(a_string("cross-compilation failed for target: "), tgt));
    }
    a_eprintln(a_add(a_add(a_add(a_add(a_add(fn_cli_green(a_string("built")), a_string(" ")), out_path), a_string(" (")), tgt), a_string(")")));
__fn_cleanup:
    a_release(runtime_dir);
    a_release(c_path);
    a_release(cc);
    a_release(flags);
    a_release(sf);
    a_release(inc);
    a_release(cmd);
    a_release(emcc_extra);
    a_release(r);
    a_release(source_path);
    a_release(out_path);
    a_release(tgt);
    return __ret;
}

AValue fn_cmd_targets(void) {
    AValue targets = {0}, cc = {0}, status = {0};
    AValue __ret = a_void();
    { AValue __old = targets; targets = fn__known_targets(); a_release(__old); }
    a_eprintln(a_string("supported cross-compilation targets:"));
    {
        AValue __iter_arr = a_iterable(targets);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue tgt = {0}, cc = {0}, status = {0};
            tgt = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = cc; cc = fn__target_compiler(tgt); a_release(__old); }
            { AValue __old = status; status = fn_cli_dim(a_string("(not available)")); a_release(__old); }
            if (a_truthy(a_gt(a_len(cc), a_int(0)))) {
                { AValue __old = status; status = a_add(a_add(fn_cli_green(a_string("available")), a_string(" via ")), cc); a_release(__old); }
            }
            a_eprintln(a_add(a_add(a_add(a_string("  "), fn_cli_bold(tgt)), a_string("  ")), status));
            a_release(tgt);
            a_release(cc);
            a_release(status);
        }
        a_release(__iter_arr);
    }
__fn_cleanup:
    a_release(targets);
    a_release(cc);
    a_release(status);
    return __ret;
}

AValue fn__cache_dir(void) {
    AValue __ret = a_void();
    __ret = a_string(".a_cache"); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn__ensure_cache_dir(void) {
    AValue dir = {0};
    AValue __ret = a_void();
    { AValue __old = dir; dir = fn__cache_dir(); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(dir)))) {
        a_fs_mkdir(dir);
    }
__fn_cleanup:
    a_release(dir);
    return __ret;
}

AValue fn__cached_bin(AValue source_path) {
    AValue source = {0}, h = {0}, cache_path = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = h; h = a_hash_sha256(source); a_release(__old); }
    { AValue __old = cache_path; cache_path = a_str_concat(fn__cache_dir(), a_str_concat(a_string("/"), h)); a_release(__old); }
    if (a_truthy(a_fs_exists(cache_path))) {
        __ret = a_retain(cache_path); goto __fn_cleanup;
    }
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(h);
    a_release(cache_path);
    a_release(source_path);
    return __ret;
}

AValue fn__store_cache(AValue source_path, AValue bin_path) {
    AValue source = {0}, h = {0}, cache_path = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    bin_path = a_retain(bin_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    fn__ensure_cache_dir();
    { AValue __old = h; h = a_hash_sha256(source); a_release(__old); }
    { AValue __old = cache_path; cache_path = a_str_concat(fn__cache_dir(), a_str_concat(a_string("/"), h)); a_release(__old); }
    a_fs_cp(bin_path, cache_path);
    a_exec(a_str_concat(a_string("chmod +x "), cache_path));
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(h);
    a_release(cache_path);
    a_release(source_path);
    a_release(bin_path);
    return __ret;
}

AValue fn_cmd_run(AValue source_path, AValue extra_args) {
    AValue cached = {0}, run_cmd = {0}, result = {0}, runtime_dir = {0}, c_path = {0}, bin_path = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    extra_args = a_retain(extra_args);
    { AValue __old = cached; cached = fn__cached_bin(source_path); a_release(__old); }
    if (a_truthy(a_gt(a_len(cached), a_int(0)))) {
        { AValue __old = run_cmd; run_cmd = a_retain(cached); a_release(__old); }
        if (a_truthy(a_gt(a_len(extra_args), a_int(0)))) {
            { AValue __old = run_cmd; run_cmd = a_str_concat(cached, a_str_concat(a_string(" "), a_str_join(extra_args, a_string(" ")))); a_release(__old); }
        }
        { AValue __old = result; result = a_exec(run_cmd); a_release(__old); }
        if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stdout"))), a_int(0)))) {
            a_print(a_array_get(result, a_string("stdout")));
        }
        if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stderr"))), a_int(0)))) {
            a_eprintln(a_array_get(result, a_string("stderr")));
        }
        if (a_truthy(a_neq(a_array_get(result, a_string("code")), a_int(0)))) {
            (exit((int)a_array_get(result, a_string("code")).ival), a_void());
        }
        __ret = a_string(""); goto __fn_cleanup;
    }
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = c_path; c_path = fn__tmp_path(a_string(".c")); a_release(__old); }
    { AValue __old = bin_path; bin_path = fn__tmp_path(a_string("")); a_release(__old); }
    fn__codegen_subprocess(source_path, c_path);
    fn__gcc(c_path, bin_path, runtime_dir);
    a_fs_rm(c_path);
    fn__store_cache(source_path, bin_path);
    { AValue __old = run_cmd; run_cmd = a_retain(bin_path); a_release(__old); }
    if (a_truthy(a_gt(a_len(extra_args), a_int(0)))) {
        { AValue __old = run_cmd; run_cmd = a_str_concat(bin_path, a_str_concat(a_string(" "), a_str_join(extra_args, a_string(" ")))); a_release(__old); }
    }
    { AValue __old = result; result = a_exec(run_cmd); a_release(__old); }
    if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stdout"))), a_int(0)))) {
        a_print(a_array_get(result, a_string("stdout")));
    }
    if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stderr"))), a_int(0)))) {
        a_eprintln(a_array_get(result, a_string("stderr")));
    }
    a_fs_rm(bin_path);
    if (a_truthy(a_neq(a_array_get(result, a_string("code")), a_int(0)))) {
        (exit((int)a_array_get(result, a_string("code")).ival), a_void());
    }
__fn_cleanup:
    a_release(cached);
    a_release(run_cmd);
    a_release(result);
    a_release(runtime_dir);
    a_release(c_path);
    a_release(bin_path);
    a_release(source_path);
    a_release(extra_args);
    return __ret;
}

AValue fn_cmd_test(AValue test_dir) {
    AValue runtime_dir = {0}, self = {0}, entries = {0}, test_files = {0}, name = {0}, passed = {0}, failed = {0}, c_path = {0}, bin_path = {0}, c_code = {0}, gcc_code = {0}, run_result = {0};
    AValue __ret = a_void();
    test_dir = a_retain(test_dir);
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = entries; entries = a_fs_ls(test_dir); a_release(__old); }
    { AValue __old = test_files; test_files = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(entries);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue entry = {0}, name = {0};
            entry = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = name; name = a_array_get(entry, a_string("name")); a_release(__old); }
            if (a_truthy(a_and(a_str_starts_with(name, a_string("test_")), a_str_ends_with(name, a_string(".a"))))) {
                { AValue __old = test_files; test_files = a_array_push(test_files, fn_path_join(test_dir, name)); a_release(__old); }
            }
            a_release(entry);
            a_release(name);
        }
        a_release(__iter_arr);
    }
    if (a_truthy(a_eq(a_len(test_files), a_int(0)))) {
        a_eprintln(a_str_concat(a_string("no test_*.a files found in "), test_dir));
        (exit((int)a_int(1).ival), a_void());
    }
    { AValue __old = test_files; test_files = a_sort(test_files); a_release(__old); }
    { AValue __old = passed; passed = a_int(0); a_release(__old); }
    { AValue __old = failed; failed = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(test_files);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue src = {0}, name = {0}, c_path = {0}, bin_path = {0}, c_code = {0}, gcc_code = {0}, run_result = {0};
            src = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = name; name = fn_path_basename(src); a_release(__old); }
            { AValue __old = c_path; c_path = fn__tmp_path(a_string(".c")); a_release(__old); }
            { AValue __old = bin_path; bin_path = fn__tmp_path(a_string("")); a_release(__old); }
            { AValue __old = c_code; c_code = fn__generate_c(src); a_release(__old); }
            a_io_write_file(c_path, c_code);
            { AValue __old = gcc_code; gcc_code = fn__gcc_try(c_path, bin_path, runtime_dir); a_release(__old); }
            a_fs_rm(c_path);
            if (a_truthy(a_neq(gcc_code, a_int(0)))) {
                a_println(a_str_concat(a_string("  "), a_str_concat(fn_cli_red(a_string("FAIL")), a_str_concat(a_string(" "), a_str_concat(name, a_string(" (compile error)"))))));
                { AValue __old = failed; failed = a_add(failed, a_int(1)); a_release(__old); }
            } else {
                { AValue __old = run_result; run_result = a_exec(bin_path); a_release(__old); }
                a_fs_rm(bin_path);
                if (a_truthy(a_eq(a_array_get(run_result, a_string("code")), a_int(0)))) {
                    a_println(a_str_concat(a_string("  "), a_str_concat(fn_cli_green(a_string("PASS")), a_str_concat(a_string(" "), name))));
                    { AValue __old = passed; passed = a_add(passed, a_int(1)); a_release(__old); }
                } else {
                    a_println(a_str_concat(a_string("  "), a_str_concat(fn_cli_red(a_string("FAIL")), a_str_concat(a_string(" "), a_str_concat(name, a_str_concat(a_string(" (exit "), a_str_concat(a_to_str(a_array_get(run_result, a_string("code"))), a_string(")"))))))));
                    if (a_truthy(a_gt(a_len(a_array_get(run_result, a_string("stdout"))), a_int(0)))) {
                        a_println(a_array_get(run_result, a_string("stdout")));
                    }
                    { AValue __old = failed; failed = a_add(failed, a_int(1)); a_release(__old); }
                }
            }
            a_release(src);
            a_release(name);
            a_release(c_path);
            a_release(bin_path);
            a_release(c_code);
            a_release(gcc_code);
            a_release(run_result);
        }
        a_release(__iter_arr);
    }
    a_println(a_string(""));
    a_println(a_str_concat(a_to_str(passed), a_str_concat(a_string(" passed, "), a_str_concat(a_to_str(failed), a_string(" failed")))));
    if (a_truthy(a_gt(failed, a_int(0)))) {
        (exit((int)a_int(1).ival), a_void());
    }
__fn_cleanup:
    a_release(runtime_dir);
    a_release(self);
    a_release(entries);
    a_release(test_files);
    a_release(name);
    a_release(passed);
    a_release(failed);
    a_release(c_path);
    a_release(bin_path);
    a_release(c_code);
    a_release(gcc_code);
    a_release(run_result);
    a_release(test_dir);
    return __ret;
}

AValue fn_cmd_cache_clean(void) {
    AValue dir = {0};
    AValue __ret = a_void();
    { AValue __old = dir; dir = fn__cache_dir(); a_release(__old); }
    if (a_truthy(a_fs_exists(dir))) {
        a_exec(a_str_concat(a_string("rm -rf "), dir));
        a_eprintln(fn_cli_green(a_string("cache cleared")));
    } else {
        a_eprintln(a_string("no cache to clean"));
    }
__fn_cleanup:
    a_release(dir);
    return __ret;
}

AValue fn__profile_insert_fn_hits(AValue c_code, AValue fn_names, AValue fn_hit_map) {
    AValue nl = {0}, lbr = {0}, lines = {0}, new_lines = {0}, trimmed = {0}, mangled = {0}, sig_start = {0}, hit_id = {0};
    AValue __ret = a_void();
    c_code = a_retain(c_code);
    fn_names = a_retain(fn_names);
    fn_hit_map = a_retain(fn_hit_map);
    { AValue __old = nl; nl = a_from_code(a_int(10)); a_release(__old); }
    { AValue __old = lbr; lbr = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = lines; lines = a_str_split(c_code, nl); a_release(__old); }
    { AValue __old = new_lines; new_lines = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(lines);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue ln = {0}, trimmed = {0}, mangled = {0}, sig_start = {0}, hit_id = {0};
            ln = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = new_lines; new_lines = a_array_push(new_lines, ln); a_release(__old); }
            { AValue __old = trimmed; trimmed = a_str_trim(ln); a_release(__old); }
            {
                AValue __iter_arr = a_iterable(fn_names);
                for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                    AValue fn2 = {0}, mangled = {0}, sig_start = {0}, hit_id = {0};
                    fn2 = a_array_get(__iter_arr, a_int(__fi));
                    { AValue __old = mangled; mangled = a_str_replace(a_str_replace(a_str_replace(fn2, a_string("."), a_string("_")), a_string("-"), a_string("_")), a_string("!"), a_string("_bang")); a_release(__old); }
                    { AValue __old = sig_start; sig_start = a_add(a_add(a_string("AValue fn_"), mangled), a_string("(")); a_release(__old); }
                    if (a_truthy(a_str_starts_with(trimmed, sig_start))) {
                        if (a_truthy(a_str_ends_with(trimmed, a_add(a_string(") "), lbr)))) {
                            { AValue __old = hit_id; hit_id = a_array_get(fn_hit_map, fn2); a_release(__old); }
                            { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("    a_profile_hit("), a_to_str(hit_id)), a_string(");"))); a_release(__old); }
                        }
                    }
                    a_release(fn2);
                    a_release(mangled);
                    a_release(sig_start);
                    a_release(hit_id);
                }
                a_release(__iter_arr);
            }
            a_release(ln);
            a_release(trimmed);
            a_release(mangled);
            a_release(sig_start);
            a_release(hit_id);
        }
        a_release(__iter_arr);
    }
    __ret = a_str_join(new_lines, nl); goto __fn_cleanup;
__fn_cleanup:
    a_release(nl);
    a_release(lbr);
    a_release(lines);
    a_release(new_lines);
    a_release(trimmed);
    a_release(mangled);
    a_release(sig_start);
    a_release(hit_id);
    a_release(c_code);
    a_release(fn_names);
    a_release(fn_hit_map);
    return __ret;
}

AValue fn__profile_insert_branch_hits(AValue c_code, AValue n_ifs, AValue n_whiles, AValue n_fors, AValue hit_base) {
    AValue nl = {0}, lbr = {0}, src_lines = {0}, new_lines = {0}, idx_if = {0}, idx_wh = {0}, idx_fo = {0}, trimmed = {0}, cid = {0};
    AValue __ret = a_void();
    c_code = a_retain(c_code);
    n_ifs = a_retain(n_ifs);
    n_whiles = a_retain(n_whiles);
    n_fors = a_retain(n_fors);
    hit_base = a_retain(hit_base);
    { AValue __old = nl; nl = a_from_code(a_int(10)); a_release(__old); }
    { AValue __old = lbr; lbr = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = src_lines; src_lines = a_str_split(c_code, nl); a_release(__old); }
    { AValue __old = new_lines; new_lines = a_array_new(0); a_release(__old); }
    { AValue __old = idx_if; idx_if = a_int(0); a_release(__old); }
    { AValue __old = idx_wh; idx_wh = a_int(0); a_release(__old); }
    { AValue __old = idx_fo; idx_fo = a_int(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(src_lines);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue ln = {0}, trimmed = {0}, cid = {0};
            ln = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = new_lines; new_lines = a_array_push(new_lines, ln); a_release(__old); }
            { AValue __old = trimmed; trimmed = a_str_trim(ln); a_release(__old); }
            if (a_truthy(a_str_starts_with(trimmed, a_string("if (a_truthy(")))) {
                if (a_truthy(a_str_ends_with(trimmed, a_add(a_string(") "), lbr)))) {
                    if (a_truthy(a_lt(idx_if, n_ifs))) {
                        { AValue __old = cid; cid = a_add(hit_base, idx_if); a_release(__old); }
                        { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("        a_profile_hit("), a_to_str(cid)), a_string(");"))); a_release(__old); }
                        { AValue __old = idx_if; idx_if = a_add(idx_if, a_int(1)); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_str_starts_with(trimmed, a_string("while (a_truthy(")))) {
                if (a_truthy(a_str_ends_with(trimmed, a_add(a_string(") "), lbr)))) {
                    { AValue __old = cid; cid = a_add(a_add(hit_base, n_ifs), idx_wh); a_release(__old); }
                    { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("        a_profile_hit("), a_to_str(cid)), a_string(");"))); a_release(__old); }
                    { AValue __old = idx_wh; idx_wh = a_add(idx_wh, a_int(1)); a_release(__old); }
                }
            }
            if (a_truthy(a_str_starts_with(trimmed, a_string("for (int __fi")))) {
                { AValue __old = cid; cid = a_add(a_add(a_add(hit_base, n_ifs), n_whiles), idx_fo); a_release(__old); }
                { AValue __old = new_lines; new_lines = a_array_push(new_lines, a_add(a_add(a_string("            a_profile_hit("), a_to_str(cid)), a_string(");"))); a_release(__old); }
                { AValue __old = idx_fo; idx_fo = a_add(idx_fo, a_int(1)); a_release(__old); }
            }
            a_release(ln);
            a_release(trimmed);
            a_release(cid);
        }
        a_release(__iter_arr);
    }
    __ret = a_str_join(new_lines, nl); goto __fn_cleanup;
__fn_cleanup:
    a_release(nl);
    a_release(lbr);
    a_release(src_lines);
    a_release(new_lines);
    a_release(idx_if);
    a_release(idx_wh);
    a_release(idx_fo);
    a_release(trimmed);
    a_release(cid);
    a_release(c_code);
    a_release(n_ifs);
    a_release(n_whiles);
    a_release(n_fors);
    a_release(hit_base);
    return __ret;
}

AValue fn__profile_count_branches(AValue stmts, AValue counts) {
    AValue tag = {0}, then_stmts = {0}, else_br = {0};
    AValue __ret = a_void();
    stmts = a_retain(stmts);
    counts = a_retain(counts);
    {
        AValue __iter_arr = a_iterable(stmts);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue st = {0}, tag = {0}, then_stmts = {0}, else_br = {0};
            st = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = tag; tag = a_array_get(st, a_string("tag")); a_release(__old); }
            if (a_truthy(a_eq(tag, a_string("If")))) {
                { AValue __old = counts; counts = a_map_set(counts, a_string("ifs"), a_add(a_array_get(counts, a_string("ifs")), a_int(1))); a_release(__old); }
                { AValue __old = then_stmts; then_stmts = a_array_get(a_array_get(st, a_string("then")), a_string("stmts")); a_release(__old); }
                { AValue __old = counts; counts = fn__profile_count_branches(then_stmts, counts); a_release(__old); }
                { AValue __old = else_br; else_br = a_array_get(st, a_string("else")); a_release(__old); }
                if (a_truthy(a_neq(a_type_of(else_br), a_string("void")))) {
                    if (a_truthy(a_eq(a_array_get(else_br, a_string("tag")), a_string("ElseBlock")))) {
                        { AValue __old = counts; counts = a_map_set(counts, a_string("elses"), a_add(a_array_get(counts, a_string("elses")), a_int(1))); a_release(__old); }
                        { AValue __old = counts; counts = fn__profile_count_branches(a_array_get(a_array_get(else_br, a_string("block")), a_string("stmts")), counts); a_release(__old); }
                    }
                    if (a_truthy(a_eq(a_array_get(else_br, a_string("tag")), a_string("ElseIf")))) {
                        { AValue __old = counts; counts = fn__profile_count_branches(a_array_new(1, a_array_get(else_br, a_string("stmt"))), counts); a_release(__old); }
                    }
                }
            }
            if (a_truthy(a_eq(tag, a_string("While")))) {
                { AValue __old = counts; counts = a_map_set(counts, a_string("whiles"), a_add(a_array_get(counts, a_string("whiles")), a_int(1))); a_release(__old); }
                { AValue __old = counts; counts = fn__profile_count_branches(a_array_get(a_array_get(st, a_string("body")), a_string("stmts")), counts); a_release(__old); }
            }
            if (a_truthy(a_or(a_eq(tag, a_string("For")), a_eq(tag, a_string("ForDestructure"))))) {
                { AValue __old = counts; counts = a_map_set(counts, a_string("fors"), a_add(a_array_get(counts, a_string("fors")), a_int(1))); a_release(__old); }
                { AValue __old = counts; counts = fn__profile_count_branches(a_array_get(a_array_get(st, a_string("body")), a_string("stmts")), counts); a_release(__old); }
            }
            a_release(st);
            a_release(tag);
            a_release(then_stmts);
            a_release(else_br);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(counts); goto __fn_cleanup;
__fn_cleanup:
    a_release(tag);
    a_release(then_stmts);
    a_release(else_br);
    a_release(stmts);
    a_release(counts);
    return __ret;
}

AValue fn_cmd_profile(AValue source_path, AValue profile_out) {
    AValue runtime_dir = {0}, prof_out = {0}, source = {0}, prog = {0}, items = {0}, fn_names = {0}, branch_counts = {0}, body = {0}, base_c = {0}, nl = {0}, q = {0}, counter_id = {0}, reg_lines = {0}, fn_hit_map = {0}, c_code = {0}, n_ifs = {0}, n_whiles = {0}, n_fors = {0}, bi = {0}, hit_base = {0}, old_main = {0}, new_main = {0}, old_end = {0}, new_end = {0}, c_path = {0}, bin_path = {0}, gcc_code = {0}, run_result = {0}, report = {0}, hf = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    profile_out = a_retain(profile_out);
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = prof_out; prof_out = a_retain(profile_out); a_release(__old); }
    if (a_truthy(a_eq(a_len(prof_out), a_int(0)))) {
        { AValue __old = prof_out; prof_out = a_string("profile.json"); a_release(__old); }
    }
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        fn__die(a_string("profiler: could not read source file"));
    }
    { AValue __old = prog; prog = fn__parse_source(source_path); a_release(__old); }
    { AValue __old = items; items = a_array_get(prog, a_string("items")); a_release(__old); }
    { AValue __old = fn_names; fn_names = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item = {0};
            item = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = fn_names; fn_names = a_array_push(fn_names, a_array_get(item, a_string("name"))); a_release(__old); }
            }
            a_release(item);
        }
        a_release(__iter_arr);
    }
    { AValue __old = branch_counts; branch_counts = a_map_new(4, "ifs", a_int(0), "elses", a_int(0), "whiles", a_int(0), "fors", a_int(0)); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(items);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue item2 = {0}, body = {0};
            item2 = a_array_get(__iter_arr, a_int(__fi));
            if (a_truthy(a_eq(a_array_get(item2, a_string("tag")), a_string("FnDecl")))) {
                { AValue __old = body; body = a_array_get(item2, a_string("body")); a_release(__old); }
                if (a_truthy(a_eq(a_type_of(body), a_string("map")))) {
                    if (a_truthy(a_map_has(body, a_string("stmts")))) {
                        { AValue __old = branch_counts; branch_counts = fn__profile_count_branches(a_array_get(body, a_string("stmts")), branch_counts); a_release(__old); }
                    }
                }
            }
            a_release(item2);
            a_release(body);
        }
        a_release(__iter_arr);
    }
    { AValue __old = base_c; base_c = fn__generate_c(source_path); a_release(__old); }
    { AValue __old = nl; nl = a_from_code(a_int(10)); a_release(__old); }
    { AValue __old = q; q = a_from_code(a_int(34)); a_release(__old); }
    { AValue __old = counter_id; counter_id = a_int(0); a_release(__old); }
    { AValue __old = reg_lines; reg_lines = a_string(""); a_release(__old); }
    { AValue __old = fn_hit_map; fn_hit_map = a_map_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(fn_names);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue fname = {0};
            fname = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("fn:")), fname), q), a_string(");")), nl); a_release(__old); }
            { AValue __old = fn_hit_map; fn_hit_map = a_map_set(fn_hit_map, fname, counter_id); a_release(__old); }
            { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
            a_release(fname);
        }
        a_release(__iter_arr);
    }
    { AValue __old = c_code; c_code = fn__profile_insert_fn_hits(base_c, fn_names, fn_hit_map); a_release(__old); }
    { AValue __old = n_ifs; n_ifs = a_array_get(branch_counts, a_string("ifs")); a_release(__old); }
    { AValue __old = n_whiles; n_whiles = a_array_get(branch_counts, a_string("whiles")); a_release(__old); }
    { AValue __old = n_fors; n_fors = a_array_get(branch_counts, a_string("fors")); a_release(__old); }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, n_ifs))) {
        { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("if:")), a_to_str(bi)), q), a_string(");")), nl); a_release(__old); }
        { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, n_whiles))) {
        { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("while:")), a_to_str(bi)), q), a_string(");")), nl); a_release(__old); }
        { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = bi; bi = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(bi, n_fors))) {
        { AValue __old = reg_lines; reg_lines = a_add(a_add(a_add(a_add(a_add(a_add(a_add(reg_lines, a_string("    a_profile_register(")), q), a_string("for:")), a_to_str(bi)), q), a_string(");")), nl); a_release(__old); }
        { AValue __old = counter_id; counter_id = a_add(counter_id, a_int(1)); a_release(__old); }
        { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
    }
    { AValue __old = hit_base; hit_base = a_len(fn_names); a_release(__old); }
    { AValue __old = c_code; c_code = fn__profile_insert_branch_hits(c_code, n_ifs, n_whiles, n_fors, hit_base); a_release(__old); }
    { AValue __old = old_main; old_main = a_string("g_argc = argc; g_argv = argv;"); a_release(__old); }
    { AValue __old = new_main; new_main = a_add(a_add(a_add(a_add(a_string("g_argc = argc; g_argv = argv;"), nl), a_string("    a_profile_init();")), nl), reg_lines); a_release(__old); }
    { AValue __old = c_code; c_code = a_str_replace(c_code, old_main, new_main); a_release(__old); }
    { AValue __old = old_end; old_end = a_add(a_add(a_string("fn_main();"), nl), a_string("    return 0;")); a_release(__old); }
    { AValue __old = new_end; new_end = a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_add(a_string("fn_main();"), nl), a_string("    a_profile_dump_json(a_string(")), q), prof_out), q), a_string("));")), nl), a_string("    return 0;")); a_release(__old); }
    { AValue __old = c_code; c_code = a_str_replace(c_code, old_end, new_end); a_release(__old); }
    { AValue __old = c_path; c_path = fn__tmp_path(a_string("_prof.c")); a_release(__old); }
    a_io_write_file(c_path, c_code);
    { AValue __old = bin_path; bin_path = fn__tmp_path(a_string("_prof_bin")); a_release(__old); }
    fn__ensure_runtime_objs(runtime_dir);
    { AValue __old = gcc_code; gcc_code = fn__gcc_try(c_path, bin_path, runtime_dir); a_release(__old); }
    a_fs_rm(c_path);
    if (a_truthy(a_neq(gcc_code, a_int(0)))) {
        fn__die(a_string("profiler: gcc compilation failed"));
    }
    a_eprintln(a_string("running instrumented binary..."));
    { AValue __old = run_result; run_result = a_exec(bin_path); a_release(__old); }
    a_fs_rm(bin_path);
    if (a_truthy(a_fs_exists(prof_out))) {
        a_eprintln(fn_cli_green(a_add(a_string("profile written to "), prof_out)));
        { AValue __old = report; report = fn_optimizer_analyze_profile(prof_out); a_release(__old); }
        if (a_truthy(a_not(a_map_has(report, a_string("error"))))) {
            a_eprintln(a_add(a_string("  total instrumentation points: "), a_to_str(a_array_get(report, a_string("total_points")))));
            a_eprintln(a_add(a_string("  total hits: "), a_to_str(a_array_get(report, a_string("total_hits")))));
            { AValue __old = hf; hf = a_array_get(report, a_string("hot_functions")); a_release(__old); }
            if (a_truthy(a_gt(a_len(hf), a_int(0)))) {
                a_eprintln(a_string("  hot functions:"));
                {
                    AValue __iter_arr = a_iterable(hf);
                    for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
                        AValue hfn = {0};
                        hfn = a_array_get(__iter_arr, a_int(__fi));
                        a_eprintln(a_add(a_add(a_add(a_add(a_add(a_add(a_string("    "), a_array_get(hfn, a_string("name"))), a_string(": ")), a_to_str(a_array_get(hfn, a_string("count")))), a_string(" calls (")), a_to_str(a_array_get(hfn, a_string("hot_ratio")))), a_string("% of max)")));
                        a_release(hfn);
                    }
                    a_release(__iter_arr);
                }
            }
        }
    } else {
        a_eprintln(fn_cli_red(a_string("profile file not generated")));
    }
__fn_cleanup:
    a_release(runtime_dir);
    a_release(prof_out);
    a_release(source);
    a_release(prog);
    a_release(items);
    a_release(fn_names);
    a_release(branch_counts);
    a_release(body);
    a_release(base_c);
    a_release(nl);
    a_release(q);
    a_release(counter_id);
    a_release(reg_lines);
    a_release(fn_hit_map);
    a_release(c_code);
    a_release(n_ifs);
    a_release(n_whiles);
    a_release(n_fors);
    a_release(bi);
    a_release(hit_base);
    a_release(old_main);
    a_release(new_main);
    a_release(old_end);
    a_release(new_end);
    a_release(c_path);
    a_release(bin_path);
    a_release(gcc_code);
    a_release(run_result);
    a_release(report);
    a_release(hf);
    a_release(source_path);
    a_release(profile_out);
    return __ret;
}

AValue fn_cmd_gentests(AValue source_path, AValue out_path) {
    AValue tests_str = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    out_path = a_retain(out_path);
    { AValue __old = tests_str; tests_str = fn_testgen_gen_tests_for_file(source_path); a_release(__old); }
    if (a_truthy(a_gt(a_len(out_path), a_int(0)))) {
        a_io_write_file(out_path, tests_str);
        a_eprintln(fn_cli_green(a_add(a_string("tests written to "), out_path)));
    } else {
        a_print(tests_str);
    }
__fn_cleanup:
    a_release(tests_str);
    a_release(source_path);
    a_release(out_path);
    return __ret;
}

AValue fn_cmd_optimize_report(AValue source_path, AValue profile_path) {
    AValue source = {0}, report = {0}, suggs = {0}, hf = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    profile_path = a_retain(profile_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    { AValue __old = report; report = fn_optimizer_analyze_profile(profile_path); a_release(__old); }
    if (a_truthy(a_map_has(report, a_string("error")))) {
        fn__die(a_add(a_string("optimizer: "), a_array_get(report, a_string("error"))));
    }
    { AValue __old = suggs; suggs = fn_optimizer_suggest(source, report); a_release(__old); }
    a_eprintln(a_string("=== Optimization Report ==="));
    a_eprintln(a_string(""));
    a_eprintln(a_add(a_string("Total instrumentation points: "), a_to_str(a_array_get(report, a_string("total_points")))));
    a_eprintln(a_add(a_string("Total hits: "), a_to_str(a_array_get(report, a_string("total_hits")))));
    a_eprintln(a_string(""));
    { AValue __old = hf; hf = a_array_get(report, a_string("hot_functions")); a_release(__old); }
    a_eprintln(a_add(a_add(a_string("Hot Functions ("), a_to_str(a_len(hf))), a_string("):")));
    {
        AValue __iter_arr = a_iterable(hf);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue f = {0};
            f = a_array_get(__iter_arr, a_int(__fi));
            a_eprintln(a_add(a_add(a_add(a_add(a_add(a_add(a_string("  "), a_array_get(f, a_string("name"))), a_string(": ")), a_to_str(a_array_get(f, a_string("count")))), a_string(" calls (")), a_to_str(a_array_get(f, a_string("hot_ratio")))), a_string("% of max)")));
            a_release(f);
        }
        a_release(__iter_arr);
    }
    a_eprintln(a_string(""));
    a_eprintln(a_add(a_add(a_string("Suggestions ("), a_to_str(a_len(suggs))), a_string("):")));
    {
        AValue __iter_arr = a_iterable(suggs);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue sg = {0};
            sg = a_array_get(__iter_arr, a_int(__fi));
            a_eprintln(a_add(a_add(a_add(a_add(a_add(a_string("  ["), a_array_get(sg, a_string("priority"))), a_string("] ")), a_array_get(sg, a_string("type"))), a_string(" -- ")), a_array_get(sg, a_string("reason"))));
            a_release(sg);
        }
        a_release(__iter_arr);
    }
__fn_cleanup:
    a_release(source);
    a_release(report);
    a_release(suggs);
    a_release(hf);
    a_release(source_path);
    a_release(profile_path);
    return __ret;
}

AValue fn_cmd_eval(AValue expr, AValue extra_args) {
    AValue wrapped = {0}, tmp = {0};
    AValue __ret = a_void();
    expr = a_retain(expr);
    extra_args = a_retain(extra_args);
    { AValue __old = wrapped; wrapped = a_str_concat(a_string("fn main() {\n  println("), a_str_concat(expr, a_string(")\n}"))); a_release(__old); }
    { AValue __old = tmp; tmp = fn__tmp_path(a_string("_eval.a")); a_release(__old); }
    a_io_write_file(tmp, wrapped);
    fn_cmd_run(tmp, extra_args);
    a_fs_rm(tmp);
__fn_cleanup:
    a_release(wrapped);
    a_release(tmp);
    a_release(expr);
    a_release(extra_args);
    return __ret;
}

AValue fn_cmd_lsp(void) {
    AValue runtime_dir = {0}, lsp_source = {0};
    AValue __ret = a_void();
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = lsp_source; lsp_source = a_string("src/lsp.a"); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(lsp_source)))) {
        fn__die(a_string("cannot find src/lsp.a -- run from the project root"));
    }
    fn_cmd_build(lsp_source, a_string("a-lsp"));
    a_eprintln(a_string("configure your editor to run ./a-lsp as the language server for *.a files"));
__fn_cleanup:
    a_release(runtime_dir);
    a_release(lsp_source);
    return __ret;
}

AValue fn__parse_source(AValue source_path) {
    AValue source = {0}, ast = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_or(a_eq(a_type_of(source), a_string("void")), a_eq(a_len(source), a_int(0))))) {
        fn__die(a_add(a_string("cannot read file: "), source_path));
    }
    { AValue __old = ast; ast = fn_parser_parse(source); a_release(__old); }
    if (a_truthy(a_and(a_eq(a_type_of(ast), a_string("map")), a_map_has(ast, a_string("tag"))))) {
        if (a_truthy(a_eq(a_array_get(ast, a_string("tag")), a_string("ParseError")))) {
            fn__die(a_add(a_add(a_add(a_string("parse error: "), a_array_get(ast, a_string("msg"))), a_string(" at token ")), a_to_str(a_array_get(ast, a_string("pos")))));
        }
    }
    __ret = a_retain(ast); goto __fn_cleanup;
__fn_cleanup:
    a_release(source);
    a_release(ast);
    a_release(source_path);
    return __ret;
}

AValue fn__fmt_file(AValue file_path) {
    AValue ast = {0}, formatted = {0};
    AValue __ret = a_void();
    file_path = a_retain(file_path);
    { AValue __old = ast; ast = fn__parse_source(file_path); a_release(__old); }
    { AValue __old = formatted; formatted = fn_emitter_emit(ast); a_release(__old); }
    a_io_write_file(file_path, formatted);
__fn_cleanup:
    a_release(ast);
    a_release(formatted);
    a_release(file_path);
    return __ret;
}

AValue fn__is_dir(AValue p) {
    AValue result = {0};
    AValue __ret = a_void();
    p = a_retain(p);
    if (a_truthy(a_not(a_fs_exists(p)))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    if (a_truthy(a_str_ends_with(p, a_string(".a")))) {
        __ret = a_bool(0); goto __fn_cleanup;
    }
    { AValue __old = result; result = a_exec(a_add(a_add(a_string("test -d "), p), a_string(" && echo yes || echo no"))); a_release(__old); }
    __ret = a_eq(a_str_trim(a_array_get(result, a_string("stdout"))), a_string("yes")); goto __fn_cleanup;
__fn_cleanup:
    a_release(result);
    a_release(p);
    return __ret;
}

AValue fn__fmt_dir(AValue target) {
    AValue entries = {0}, i = {0}, name = {0}, full = {0};
    AValue __ret = a_void();
    target = a_retain(target);
    { AValue __old = entries; entries = a_fs_ls(target); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(entries)))) {
        { AValue __old = name; name = a_map_get(a_array_get(entries, i), a_string("name")); a_release(__old); }
        { AValue __old = full; full = fn_path_join(target, name); a_release(__old); }
        if (a_truthy(a_map_get(a_array_get(entries, i), a_string("is_dir")))) {
            fn__fmt_dir(full);
        } else {
            if (a_truthy(a_str_ends_with(name, a_string(".a")))) {
                fn__fmt_file(full);
            }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
__fn_cleanup:
    a_release(entries);
    a_release(i);
    a_release(name);
    a_release(full);
    a_release(target);
    return __ret;
}

AValue fn_cmd_fmt(AValue target) {
    AValue __ret = a_void();
    target = a_retain(target);
    if (a_truthy(fn__is_dir(target))) {
        fn__fmt_dir(target);
        a_println(a_add(a_add(fn_cli_green(a_string("formatted")), a_string(" ")), target));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_and(a_str_ends_with(target, a_string(".a")), a_fs_exists(target)))) {
        fn__fmt_file(target);
        a_println(a_add(a_add(fn_cli_green(a_string("formatted")), a_string(" ")), target));
    } else {
        fn__die(a_add(a_string("expected a .a file or directory: "), target));
    }
__fn_cleanup:
    a_release(target);
    return __ret;
}

AValue fn_cmd_ast(AValue source_path) {
    AValue ast = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = ast; ast = fn__parse_source(source_path); a_release(__old); }
    a_println(a_json_pretty(ast));
__fn_cleanup:
    a_release(ast);
    a_release(source_path);
    return __ret;
}

AValue fn_cmd_check(AValue source_path) {
    AValue ast = {0}, diagnostics = {0}, errors = {0}, warnings = {0}, i = {0}, d = {0}, sev = {0}, line = {0}, msg = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = ast; ast = fn__parse_source(source_path); a_release(__old); }
    { AValue __old = diagnostics; diagnostics = fn_checker_check(ast); a_release(__old); }
    if (a_truthy(a_eq(a_len(diagnostics), a_int(0)))) {
        a_println(fn_cli_green(a_string("no issues found")));
        __ret = a_void(); goto __fn_cleanup;
    }
    { AValue __old = errors; errors = a_int(0); a_release(__old); }
    { AValue __old = warnings; warnings = a_int(0); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(diagnostics)))) {
        { AValue __old = d; d = a_array_get(diagnostics, i); a_release(__old); }
        { AValue __old = sev; sev = a_map_get(d, a_string("severity")); a_release(__old); }
        { AValue __old = line; line = a_map_get(d, a_string("line")); a_release(__old); }
        { AValue __old = msg; msg = a_map_get(d, a_string("msg")); a_release(__old); }
        if (a_truthy(a_eq(sev, a_string("error")))) {
            a_eprintln(a_add(a_add(a_add(a_add(fn_cli_red(a_string("error")), a_string(" (line ")), a_to_str(line)), a_string("): ")), msg));
            { AValue __old = errors; errors = a_add(errors, a_int(1)); a_release(__old); }
        } else {
            a_eprintln(a_add(a_add(a_add(a_add(fn_cli_yellow(a_string("warning")), a_string(" (line ")), a_to_str(line)), a_string("): ")), msg));
            { AValue __old = warnings; warnings = a_add(warnings, a_int(1)); a_release(__old); }
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    a_println(a_string(""));
    a_println(a_add(a_add(a_add(a_to_str(errors), a_string(" error(s), ")), a_to_str(warnings)), a_string(" warning(s)")));
    if (a_truthy(a_gt(errors, a_int(0)))) {
        (exit((int)a_int(1).ival), a_void());
    }
__fn_cleanup:
    a_release(ast);
    a_release(diagnostics);
    a_release(errors);
    a_release(warnings);
    a_release(i);
    a_release(d);
    a_release(sev);
    a_release(line);
    a_release(msg);
    a_release(source_path);
    return __ret;
}

AValue fn__repl_build_source(AValue fns, AValue bindings, AValue expr) {
    AValue lb = {0}, rb = {0}, src = {0}, i = {0};
    AValue __ret = a_void();
    fns = a_retain(fns);
    bindings = a_retain(bindings);
    expr = a_retain(expr);
    { AValue __old = lb; lb = a_from_code(a_int(123)); a_release(__old); }
    { AValue __old = rb; rb = a_from_code(a_int(125)); a_release(__old); }
    { AValue __old = src; src = a_string(""); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(fns)))) {
        { AValue __old = src; src = a_add(a_add(src, a_array_get(fns, i)), a_string("\n")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    { AValue __old = src; src = a_add(a_add(a_add(src, a_string("fn main() ")), lb), a_string("\n")); a_release(__old); }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(bindings)))) {
        { AValue __old = src; src = a_add(a_add(a_add(src, a_string("  ")), a_array_get(bindings, i)), a_string("\n")); a_release(__old); }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    if (a_truthy(a_gt(a_len(expr), a_int(0)))) {
        { AValue __old = src; src = a_add(a_add(a_add(src, a_string("  ")), expr), a_string("\n")); a_release(__old); }
    }
    { AValue __old = src; src = a_add(a_add(src, rb), a_string("\n")); a_release(__old); }
    __ret = a_retain(src); goto __fn_cleanup;
__fn_cleanup:
    a_release(lb);
    a_release(rb);
    a_release(src);
    a_release(i);
    a_release(fns);
    a_release(bindings);
    a_release(expr);
    return __ret;
}

AValue fn_cmd_repl(void) {
    AValue runtime_dir = {0}, bindings = {0}, fns = {0}, line_buf = {0}, brace_depth = {0}, line = {0}, trimmed = {0}, bi = {0}, chars = {0}, ci = {0}, ch = {0}, input = {0}, src = {0}, tmp = {0}, c_path = {0}, bin_path = {0}, gcc_code = {0}, expr_line = {0}, result = {0};
    AValue __ret = a_void();
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = bindings; bindings = a_array_new(0); a_release(__old); }
    { AValue __old = fns; fns = a_array_new(0); a_release(__old); }
    { AValue __old = line_buf; line_buf = a_string(""); a_release(__old); }
    { AValue __old = brace_depth; brace_depth = a_int(0); a_release(__old); }
    a_println(a_string("a repl -- type :quit to exit, :clear to reset"));
    while (a_truthy(a_bool(1))) {
        if (a_truthy(a_gt(brace_depth, a_int(0)))) {
            a_print(a_string(".. "));
        } else {
            a_print(a_string("a> "));
        }
        a_io_flush();
        { AValue __old = line; line = a_io_read_line(); a_release(__old); }
        if (a_truthy(a_eq(a_type_of(line), a_string("void")))) {
            break;
        }
        { AValue __old = trimmed; trimmed = a_str_trim(line); a_release(__old); }
        if (a_truthy(a_eq(brace_depth, a_int(0)))) {
            if (a_truthy(a_or(a_eq(trimmed, a_string(":quit")), a_eq(trimmed, a_string(":q"))))) {
                break;
            }
            if (a_truthy(a_eq(trimmed, a_string(":clear")))) {
                { AValue __old = bindings; bindings = a_array_new(0); a_release(__old); }
                { AValue __old = fns; fns = a_array_new(0); a_release(__old); }
                a_println(a_string("state cleared"));
                continue;
            }
            if (a_truthy(a_eq(trimmed, a_string(":bindings")))) {
                { AValue __old = bi; bi = a_int(0); a_release(__old); }
                while (a_truthy(a_lt(bi, a_len(bindings)))) {
                    a_println(a_add(a_string("  "), a_array_get(bindings, bi)));
                    { AValue __old = bi; bi = a_add(bi, a_int(1)); a_release(__old); }
                }
                continue;
            }
            if (a_truthy(a_eq(a_len(trimmed), a_int(0)))) {
                continue;
            }
        }
        { AValue __old = line_buf; line_buf = a_add(a_add(line_buf, line), a_string("\n")); a_release(__old); }
        { AValue __old = chars; chars = a_str_chars(trimmed); a_release(__old); }
        { AValue __old = ci; ci = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(ci, a_len(chars)))) {
            { AValue __old = ch; ch = a_array_get(chars, ci); a_release(__old); }
            if (a_truthy(a_eq(ch, a_from_code(a_int(123))))) {
                { AValue __old = brace_depth; brace_depth = a_add(brace_depth, a_int(1)); a_release(__old); }
            }
            if (a_truthy(a_eq(ch, a_from_code(a_int(125))))) {
                { AValue __old = brace_depth; brace_depth = a_sub(brace_depth, a_int(1)); a_release(__old); }
            }
            { AValue __old = ci; ci = a_add(ci, a_int(1)); a_release(__old); }
        }
        if (a_truthy(a_gt(brace_depth, a_int(0)))) {
            continue;
        }
        { AValue __old = input; input = a_str_trim(line_buf); a_release(__old); }
        { AValue __old = line_buf; line_buf = a_string(""); a_release(__old); }
        if (a_truthy(a_str_starts_with(input, a_string("fn ")))) {
            { AValue __old = fns; fns = a_array_push(fns, input); a_release(__old); }
            continue;
        }
        if (a_truthy(a_str_starts_with(input, a_string("let ")))) {
            { AValue __old = src; src = fn__repl_build_source(fns, a_array_push(bindings, input), a_string("")); a_release(__old); }
            { AValue __old = tmp; tmp = fn__tmp_path(a_string("_repl.a")); a_release(__old); }
            a_io_write_file(tmp, src);
            { AValue __old = c_path; c_path = fn__tmp_path(a_string("_repl.c")); a_release(__old); }
            { AValue __old = bin_path; bin_path = fn__tmp_path(a_string("_repl")); a_release(__old); }
            fn__codegen_subprocess(tmp, c_path);
            { AValue __old = gcc_code; gcc_code = fn__gcc_try(c_path, bin_path, runtime_dir); a_release(__old); }
            a_fs_rm(tmp);
            a_fs_rm(c_path);
            if (a_truthy(a_neq(gcc_code, a_int(0)))) {
                a_eprintln(a_add(fn_cli_red(a_string("error: ")), a_string("invalid binding")));
            } else {
                a_fs_rm(bin_path);
                { AValue __old = bindings; bindings = a_array_push(bindings, input); a_release(__old); }
            }
            continue;
        }
        { AValue __old = expr_line; expr_line = a_add(a_add(a_string("println(to_str("), input), a_string("))")); a_release(__old); }
        { AValue __old = src; src = fn__repl_build_source(fns, bindings, expr_line); a_release(__old); }
        { AValue __old = tmp; tmp = fn__tmp_path(a_string("_repl.a")); a_release(__old); }
        a_io_write_file(tmp, src);
        { AValue __old = c_path; c_path = fn__tmp_path(a_string("_repl.c")); a_release(__old); }
        { AValue __old = bin_path; bin_path = fn__tmp_path(a_string("_repl")); a_release(__old); }
        fn__codegen_subprocess(tmp, c_path);
        { AValue __old = gcc_code; gcc_code = fn__gcc_try(c_path, bin_path, runtime_dir); a_release(__old); }
        a_fs_rm(tmp);
        a_fs_rm(c_path);
        if (a_truthy(a_neq(gcc_code, a_int(0)))) {
            a_eprintln(a_add(fn_cli_red(a_string("error: ")), a_string("cannot evaluate expression")));
        } else {
            { AValue __old = result; result = a_exec(bin_path); a_release(__old); }
            a_fs_rm(bin_path);
            if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stdout"))), a_int(0)))) {
                a_print(a_array_get(result, a_string("stdout")));
            }
            if (a_truthy(a_neq(a_array_get(result, a_string("code")), a_int(0)))) {
                a_eprintln(a_add(a_add(a_add(fn_cli_red(a_string("runtime error")), a_string(" (exit ")), a_to_str(a_array_get(result, a_string("code")))), a_string(")")));
            }
        }
    }
__fn_cleanup:
    a_release(runtime_dir);
    a_release(bindings);
    a_release(fns);
    a_release(line_buf);
    a_release(brace_depth);
    a_release(line);
    a_release(trimmed);
    a_release(bi);
    a_release(chars);
    a_release(ci);
    a_release(ch);
    a_release(input);
    a_release(src);
    a_release(tmp);
    a_release(c_path);
    a_release(bin_path);
    a_release(gcc_code);
    a_release(expr_line);
    a_release(result);
    return __ret;
}

AValue fn_cmd_pkg_init(void) {
    AValue dir = {0};
    AValue __ret = a_void();
    { AValue __old = dir; dir = a_fs_cwd(); a_release(__old); }
    fn_pkg_init(dir);
    a_println(a_add(a_string("created pkg.toml in "), dir));
__fn_cleanup:
    a_release(dir);
    return __ret;
}

AValue fn_cmd_pkg_add(AValue name, AValue source) {
    AValue dir = {0};
    AValue __ret = a_void();
    name = a_retain(name);
    source = a_retain(source);
    { AValue __old = dir; dir = a_fs_cwd(); a_release(__old); }
    fn_pkg_add_dep(dir, name, source);
__fn_cleanup:
    a_release(dir);
    a_release(name);
    a_release(source);
    return __ret;
}

AValue fn_cmd_pkg_install(void) {
    AValue dir = {0}, result = {0};
    AValue __ret = a_void();
    { AValue __old = dir; dir = a_fs_cwd(); a_release(__old); }
    { AValue __old = result; result = fn_pkg_install(dir); a_release(__old); }
    a_println(a_add(a_add(a_string("done ("), a_to_str(a_map_get(result, a_string("installed")))), a_string(" packages installed)")));
__fn_cleanup:
    a_release(dir);
    a_release(result);
    return __ret;
}

AValue fn__collect_watch_files(AValue source_path) {
    AValue files = {0}, source = {0}, lines = {0}, trimmed = {0}, mod_path = {0}, parts = {0}, file = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    { AValue __old = files; files = a_array_new(1, source_path); a_release(__old); }
    { AValue __old = source; source = a_io_read_file(source_path); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(source), a_string("void")))) {
        __ret = a_retain(files); goto __fn_cleanup;
    }
    { AValue __old = lines; lines = a_str_lines(source); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(lines);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue line = {0}, trimmed = {0}, mod_path = {0}, parts = {0}, file = {0};
            line = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = trimmed; trimmed = a_str_trim(line); a_release(__old); }
            if (a_truthy(a_str_starts_with(trimmed, a_string("use ")))) {
                { AValue __old = mod_path; mod_path = a_str_trim(a_str_slice(trimmed, a_int(4), a_len(trimmed))); a_release(__old); }
                { AValue __old = parts; parts = a_str_split(mod_path, a_string(".")); a_release(__old); }
                { AValue __old = file; file = a_add(a_str_join(parts, a_string("/")), a_string(".a")); a_release(__old); }
                if (a_truthy(a_fs_exists(file))) {
                    { AValue __old = files; files = a_array_push(files, file); a_release(__old); }
                }
            }
            a_release(line);
            a_release(trimmed);
            a_release(mod_path);
            a_release(parts);
            a_release(file);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(files); goto __fn_cleanup;
__fn_cleanup:
    a_release(files);
    a_release(source);
    a_release(lines);
    a_release(trimmed);
    a_release(mod_path);
    a_release(parts);
    a_release(file);
    a_release(source_path);
    return __ret;
}

AValue fn__get_mtimes(AValue files) {
    AValue mtimes = {0}, s = {0};
    AValue __ret = a_void();
    files = a_retain(files);
    { AValue __old = mtimes; mtimes = a_array_new(0); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(files);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue f = {0}, s = {0};
            f = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = s; s = a_fs_stat(f); a_release(__old); }
            if (a_truthy(a_eq(a_type_of(s), a_string("map")))) {
                { AValue __old = mtimes; mtimes = a_array_push(mtimes, a_array_get(s, a_string("mtime"))); a_release(__old); }
            } else {
                { AValue __old = mtimes; mtimes = a_array_push(mtimes, a_int(0)); a_release(__old); }
            }
            a_release(f);
            a_release(s);
        }
        a_release(__iter_arr);
    }
    __ret = a_retain(mtimes); goto __fn_cleanup;
__fn_cleanup:
    a_release(mtimes);
    a_release(s);
    a_release(files);
    return __ret;
}

AValue fn__mtimes_changed(AValue old_mt, AValue new_mt) {
    AValue i = {0};
    AValue __ret = a_void();
    old_mt = a_retain(old_mt);
    new_mt = a_retain(new_mt);
    if (a_truthy(a_neq(a_len(old_mt), a_len(new_mt)))) {
        __ret = a_bool(1); goto __fn_cleanup;
    }
    { AValue __old = i; i = a_int(0); a_release(__old); }
    while (a_truthy(a_lt(i, a_len(old_mt)))) {
        if (a_truthy(a_neq(a_array_get(old_mt, i), a_array_get(new_mt, i)))) {
            __ret = a_bool(1); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    a_release(i);
    a_release(old_mt);
    a_release(new_mt);
    return __ret;
}

AValue fn_cmd_watch(AValue source_path) {
    AValue self = {0}, files = {0}, mtimes = {0}, r = {0}, new_files = {0}, new_mtimes = {0}, result = {0};
    AValue __ret = a_void();
    source_path = a_retain(source_path);
    if (a_truthy(a_not(a_fs_exists(source_path)))) {
        fn__die(a_add(a_string("watch: file not found: "), source_path));
    }
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = files; files = fn__collect_watch_files(source_path); a_release(__old); }
    { AValue __old = mtimes; mtimes = fn__get_mtimes(files); a_release(__old); }
    a_eprintln(fn_cli_dim(a_add(a_add(a_string("[watch] watching "), a_to_str(a_len(files))), a_string(" file(s), press Ctrl+C to stop"))));
    { AValue __old = r; r = a_exec(a_add(a_add(self, a_string(" run ")), source_path)); a_release(__old); }
    if (a_truthy(a_gt(a_len(a_array_get(r, a_string("stdout"))), a_int(0)))) {
        a_print(a_array_get(r, a_string("stdout")));
    }
    if (a_truthy(a_gt(a_len(a_array_get(r, a_string("stderr"))), a_int(0)))) {
        a_eprintln(a_array_get(r, a_string("stderr")));
    }
    a_io_flush();
    while (a_truthy(a_bool(1))) {
        a_time_sleep(a_int(500));
        { AValue __old = new_files; new_files = fn__collect_watch_files(source_path); a_release(__old); }
        { AValue __old = new_mtimes; new_mtimes = fn__get_mtimes(new_files); a_release(__old); }
        if (a_truthy(a_or(fn__mtimes_changed(mtimes, new_mtimes), a_neq(a_len(new_files), a_len(files))))) {
            a_eprintln(fn_cli_dim(a_string("[watch] change detected, restarting...")));
            { AValue __old = files; files = a_retain(new_files); a_release(__old); }
            { AValue __old = mtimes; mtimes = fn__get_mtimes(files); a_release(__old); }
            { AValue __old = result; result = a_exec(a_add(a_add(self, a_string(" run ")), source_path)); a_release(__old); }
            if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stdout"))), a_int(0)))) {
                a_print(a_array_get(result, a_string("stdout")));
            }
            if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stderr"))), a_int(0)))) {
                a_eprintln(a_array_get(result, a_string("stderr")));
            }
            a_io_flush();
        }
    }
__fn_cleanup:
    a_release(self);
    a_release(files);
    a_release(mtimes);
    a_release(r);
    a_release(new_files);
    a_release(new_mtimes);
    a_release(result);
    a_release(source_path);
    return __ret;
}

AValue fn_cmd_spawn(AValue source, AValue agent_name) {
    AValue self = {0}, bin_name = {0}, build_result = {0}, run_cmd = {0}, r = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    agent_name = a_retain(agent_name);
    if (a_truthy(a_not(a_fs_exists(source)))) {
        fn__die(a_add(a_string("spawn: file not found: "), source));
    }
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = bin_name; bin_name = a_add(a_string(".a_cache/agent_"), agent_name); a_release(__old); }
    { AValue __old = build_result; build_result = a_exec(a_add(a_add(a_add(a_add(self, a_string(" build ")), source), a_string(" -o ")), bin_name)); a_release(__old); }
    if (a_truthy(a_neq(a_array_get(build_result, a_string("code")), a_int(0)))) {
        a_eprintln(a_array_get(build_result, a_string("stderr")));
        fn__die(a_string("spawn: build failed"));
    }
    if (a_truthy(a_not(a_fs_exists(a_string("/tmp/a_agents"))))) {
        a_fs_mkdir(a_string("/tmp/a_agents"));
    }
    { AValue __old = run_cmd; run_cmd = a_add(a_add(a_add(a_add(a_string("A_AGENT_NAME="), agent_name), a_string(" ")), bin_name), a_string(" &")); a_release(__old); }
    { AValue __old = r; r = a_exec(run_cmd); a_release(__old); }
    a_eprintln(a_add(a_add(a_add(a_add(fn_cli_green(a_string("[spawn]")), a_string(" launched agent ")), fn_cli_bold(agent_name)), a_string(" from ")), source));
__fn_cleanup:
    a_release(self);
    a_release(bin_name);
    a_release(build_result);
    a_release(run_cmd);
    a_release(r);
    a_release(source);
    a_release(agent_name);
    return __ret;
}

AValue fn__agent_log(AValue name, AValue msg) {
    AValue __ret = a_void();
    name = a_retain(name);
    msg = a_retain(msg);
    a_eprintln(a_add(a_add(a_add(a_add(fn_cli_dim(a_string("[agent]")), a_string(" ")), fn_cli_bold(name)), a_string(" ")), msg));
__fn_cleanup:
    a_release(name);
    a_release(msg);
    return __ret;
}

AValue fn_cmd_agent(AValue source, AValue agent_name, AValue health_port, AValue max_restarts, AValue no_restart) {
    AValue self = {0}, bin = {0}, build_r = {0}, pid_file = {0}, restarts = {0}, running = {0}, start_time = {0}, env_prefix = {0}, result = {0}, code = {0}, uptime = {0}, rebuild = {0}, backoff = {0}, e = {0};
    AValue __ret = a_void();
    source = a_retain(source);
    agent_name = a_retain(agent_name);
    health_port = a_retain(health_port);
    max_restarts = a_retain(max_restarts);
    no_restart = a_retain(no_restart);
    if (a_truthy(a_not(a_fs_exists(source)))) {
        fn__die(a_add(a_string("agent: file not found: "), source));
    }
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    if (a_truthy(a_not(a_fs_exists(a_string(".a_cache"))))) {
        a_fs_mkdir(a_string(".a_cache"));
    }
    if (a_truthy(a_not(a_fs_exists(a_string("/tmp/a_agents"))))) {
        a_fs_mkdir(a_string("/tmp/a_agents"));
    }
    { AValue __old = bin; bin = a_add(a_string(".a_cache/agent_"), agent_name); a_release(__old); }
    { AValue __old = build_r; build_r = a_exec(a_add(a_add(a_add(a_add(self, a_string(" build ")), source), a_string(" -o ")), bin)); a_release(__old); }
    if (a_truthy(a_neq(a_array_get(build_r, a_string("code")), a_int(0)))) {
        if (a_truthy(a_gt(a_len(a_array_get(build_r, a_string("stderr"))), a_int(0)))) {
            a_eprintln(a_array_get(build_r, a_string("stderr")));
        }
        fn__die(a_string("agent: build failed"));
    }
    { AValue __old = pid_file; pid_file = a_add(a_add(a_string("/tmp/a_agents/"), agent_name), a_string(".pid")); a_release(__old); }
    a_io_write_file(pid_file, a_to_str(a_reflect_pid()));
    fn__agent_log(agent_name, a_add(a_add(a_string("supervisor started (pid "), a_to_str(a_reflect_pid())), a_string(")")));
    { AValue __old = restarts; restarts = a_int(0); a_release(__old); }
    { AValue __old = running; running = a_bool(1); a_release(__old); }
    while (a_truthy(running)) {
        { AValue __old = start_time; start_time = a_time_now(); a_release(__old); }
        fn__agent_log(agent_name, a_string("starting agent process"));
        { AValue __old = env_prefix; env_prefix = a_add(a_add(a_add(a_string("A_AGENT_NAME="), agent_name), a_string(" A_COMPILER=")), self); a_release(__old); }
        if (a_truthy(a_gt(health_port, a_int(0)))) {
            { AValue __old = env_prefix; env_prefix = a_add(a_add(env_prefix, a_string(" A_AGENT_PORT=")), a_to_str(health_port)); a_release(__old); }
        }
        { AValue __old = result; result = a_exec(a_add(a_add(env_prefix, a_string(" ")), bin)); a_release(__old); }
        if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stdout"))), a_int(0)))) {
            a_print(a_array_get(result, a_string("stdout")));
        }
        if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stderr"))), a_int(0)))) {
            a_eprintln(a_array_get(result, a_string("stderr")));
        }
        { AValue __old = code; code = a_array_get(result, a_string("code")); a_release(__old); }
        { AValue __old = uptime; uptime = a_sub(a_time_now(), start_time); a_release(__old); }
        if (a_truthy(a_eq(code, a_int(42)))) {
            fn__agent_log(agent_name, a_string("update requested, rebuilding..."));
            { AValue __old = rebuild; rebuild = a_exec(a_add(a_add(a_add(a_add(self, a_string(" build ")), source), a_string(" -o ")), bin)); a_release(__old); }
            if (a_truthy(a_neq(a_array_get(rebuild, a_string("code")), a_int(0)))) {
                fn__agent_log(agent_name, a_string("rebuild failed, restarting with old binary"));
            } else {
                fn__agent_log(agent_name, a_string("rebuild succeeded"));
            }
            { AValue __old = restarts; restarts = a_int(0); a_release(__old); }
        } else         if (a_truthy(a_or(a_eq(code, a_int(0)), no_restart))) {
            fn__agent_log(agent_name, a_add(a_add(a_string("exited (code "), a_to_str(code)), a_string("), stopping")));
            { AValue __old = running; running = a_bool(0); a_release(__old); }
        } else {
            fn__agent_log(agent_name, a_add(a_add(a_add(a_add(a_string("crashed (exit "), a_to_str(code)), a_string(", uptime ")), a_to_str(a_div(uptime, a_int(1000)))), a_string("s)")));
            if (a_truthy(a_gt(uptime, a_int(60000)))) {
                { AValue __old = restarts; restarts = a_int(0); a_release(__old); }
            }
            { AValue __old = restarts; restarts = a_add(restarts, a_int(1)); a_release(__old); }
            if (a_truthy(a_and(a_gt(max_restarts, a_int(0)), a_gteq(restarts, max_restarts)))) {
                fn__agent_log(agent_name, a_add(a_add(a_string("max restarts ("), a_to_str(max_restarts)), a_string(") reached, giving up")));
                { AValue __old = running; running = a_bool(0); a_release(__old); }
            } else {
                { AValue __old = backoff; backoff = a_int(1000); a_release(__old); }
                { AValue __old = e; e = a_int(0); a_release(__old); }
                while (a_truthy(a_lt(e, a_sub(restarts, a_int(1))))) {
                    { AValue __old = backoff; backoff = a_mul(backoff, a_int(2)); a_release(__old); }
                    { AValue __old = e; e = a_add(e, a_int(1)); a_release(__old); }
                }
                if (a_truthy(a_gt(backoff, a_int(30000)))) {
                    { AValue __old = backoff; backoff = a_int(30000); a_release(__old); }
                }
                fn__agent_log(agent_name, a_add(a_add(a_add(a_add(a_string("restarting in "), a_to_str(a_div(backoff, a_int(1000)))), a_string("s (attempt ")), a_to_str(restarts)), a_string(")")));
                a_time_sleep(backoff);
            }
        }
    }
    if (a_truthy(a_fs_exists(pid_file))) {
        a_fs_rm(pid_file);
    }
    fn__agent_log(agent_name, a_string("supervisor stopped"));
__fn_cleanup:
    a_release(self);
    a_release(bin);
    a_release(build_r);
    a_release(pid_file);
    a_release(restarts);
    a_release(running);
    a_release(start_time);
    a_release(env_prefix);
    a_release(result);
    a_release(code);
    a_release(uptime);
    a_release(rebuild);
    a_release(backoff);
    a_release(e);
    a_release(source);
    a_release(agent_name);
    a_release(health_port);
    a_release(max_restarts);
    a_release(no_restart);
    return __ret;
}

AValue fn_cmd_plugin(AValue argv) {
    AValue sub = {0}, source = {0}, result = {0}, repo = {0}, info = {0}, plugins = {0}, i = {0}, p = {0}, r = {0};
    AValue __ret = a_void();
    argv = a_retain(argv);
    if (a_truthy(a_lt(a_len(argv), a_int(1)))) {
        fn__die(a_string("usage: a plugin <install|list|remove|init|run>"));
    }
    { AValue __old = sub; sub = a_array_get(argv, a_int(0)); a_release(__old); }
    if (a_truthy(a_eq(sub, a_string("install")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("usage: a plugin install <dir|git:user/repo>"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = result; result = a_err(a_string("unknown")); a_release(__old); }
        if (a_truthy(a_or(a_str_starts_with(source, a_string("git:")), a_str_starts_with(source, a_string("github:"))))) {
            { AValue __old = repo; repo = a_str_replace(a_str_replace(source, a_string("git:"), a_string("")), a_string("github:"), a_string("")); a_release(__old); }
            { AValue __old = result; result = fn_plugin_install_git(repo); a_release(__old); }
        } else {
            { AValue __old = result; result = fn_plugin_install(source); a_release(__old); }
        }
        if (a_truthy(a_is_ok(result))) {
            { AValue __old = info; info = a_unwrap(result); a_release(__old); }
            a_eprintln(a_add(a_add(a_add(a_add(fn_cli_green(a_string("[plugin]")), a_string(" installed ")), fn_cli_bold(a_array_get(info, a_string("name")))), a_string(" v")), a_array_get(info, a_string("version"))));
        } else {
            fn__die(a_unwrap_or(result, a_string("install failed")));
        }
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(sub, a_string("list")))) {
        { AValue __old = plugins; plugins = fn_plugin_list(); a_release(__old); }
        if (a_truthy(a_eq(a_len(plugins), a_int(0)))) {
            a_eprintln(a_string("no plugins installed"));
            __ret = a_void(); goto __fn_cleanup;
        }
        { AValue __old = i; i = a_int(0); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(plugins)))) {
            { AValue __old = p; p = a_array_get(plugins, i); a_release(__old); }
            a_eprintln(a_add(a_add(a_add(a_add(a_add(a_string("  "), fn_cli_bold(a_array_get(p, a_string("name")))), a_string(" v")), a_array_get(p, a_string("version"))), a_string("  ")), fn_cli_dim(a_array_get(p, a_string("dir")))));
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
        }
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(sub, a_string("remove")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("usage: a plugin remove <name>"));
        }
        { AValue __old = result; result = fn_plugin_remove(a_array_get(argv, a_int(1))); a_release(__old); }
        if (a_truthy(a_is_ok(result))) {
            a_eprintln(a_add(a_add(fn_cli_green(a_string("[plugin]")), a_string(" removed ")), fn_cli_bold(a_array_get(argv, a_int(1)))));
        } else {
            fn__die(a_unwrap_or(result, a_string("remove failed")));
        }
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(sub, a_string("init")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(3)))) {
            fn__die(a_string("usage: a plugin init <dir> <name>"));
        }
        { AValue __old = result; result = fn_plugin_init(a_array_get(argv, a_int(1)), a_array_get(argv, a_int(2))); a_release(__old); }
        if (a_truthy(a_is_ok(result))) {
            a_eprintln(a_add(a_add(fn_cli_green(a_string("[plugin]")), a_string(" created plugin scaffold at ")), a_array_get(argv, a_int(1))));
        } else {
            fn__die(a_string("plugin init failed"));
        }
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(sub, a_string("run")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("usage: a plugin run <name>"));
        }
        { AValue __old = result; result = fn_plugin_run(a_array_get(argv, a_int(1))); a_release(__old); }
        if (a_truthy(a_is_ok(result))) {
            { AValue __old = r; r = a_unwrap(result); a_release(__old); }
            if (a_truthy(a_gt(a_len(a_array_get(r, a_string("stdout"))), a_int(0)))) {
                a_print(a_array_get(r, a_string("stdout")));
            }
            if (a_truthy(a_neq(a_array_get(r, a_string("code")), a_int(0)))) {
                (exit((int)a_array_get(r, a_string("code")).ival), a_void());
            }
        } else {
            fn__die(a_unwrap_or(result, a_string("plugin run failed")));
        }
        __ret = a_void(); goto __fn_cleanup;
    }
    fn__die(a_string("usage: a plugin <install|list|remove|init|run>"));
__fn_cleanup:
    a_release(sub);
    a_release(source);
    a_release(result);
    a_release(repo);
    a_release(info);
    a_release(plugins);
    a_release(i);
    a_release(p);
    a_release(r);
    a_release(argv);
    return __ret;
}

AValue fn__usage(void) {
    AValue __ret = a_void();
    a_eprintln(a_add(a_add(a_string("a "), fn__version()), a_string(" -- the a language native toolchain")));
    a_eprintln(a_string(""));
    a_eprintln(a_string("usage:"));
    a_eprintln(a_string("  a run <file.a> [args...]    compile and run (cached)"));
    a_eprintln(a_string("  a build <file.a> [-o out]   compile to native binary"));
    a_eprintln(a_string("  a build <file.a> --target T cross-compile (wasm32-wasi, linux-x86_64, ...)"));
    a_eprintln(a_string("  a targets                  list available cross-compilation targets"));
    a_eprintln(a_string("  a eval <expr>              evaluate an expression"));
    a_eprintln(a_string("  a repl                     interactive read-eval-print loop"));
    a_eprintln(a_string("  a cc <file.a>              emit C to stdout"));
    a_eprintln(a_string("  a wat <file.a>             emit WebAssembly Text Format to stdout"));
    a_eprintln(a_string("  a fmt <file.a|dir/>        format source code"));
    a_eprintln(a_string("  a ast <file.a>             dump parsed AST as JSON"));
    a_eprintln(a_string("  a check <file.a>           type-check source file"));
    a_eprintln(a_string("  a test <dir/>              run test_*.a files in directory"));
    a_eprintln(a_string("  a lsp                      start language server (JSON-RPC over stdio)"));
    a_eprintln(a_string("  a watch <file.a>           recompile and run on file change"));
    a_eprintln(a_string("  a agent <file.a> [opts]     deploy supervised long-running agent"));
    a_eprintln(a_string("     --name N                  agent name (default: filename)"));
    a_eprintln(a_string("     --max-restarts N          max restart attempts (default: 10, 0=unlimited)"));
    a_eprintln(a_string("     --no-restart              run once, exit on child exit"));
    a_eprintln(a_string("  a spawn <file.a> --name N  launch named agent process (background)"));
    a_eprintln(a_string("  a plugin install <dir>     install a plugin from directory"));
    a_eprintln(a_string("  a plugin list              list installed plugins"));
    a_eprintln(a_string("  a plugin remove <name>     uninstall a plugin"));
    a_eprintln(a_string("  a plugin init <dir> <name> scaffold a new plugin"));
    a_eprintln(a_string("  a plugin run <name>        run an installed plugin"));
    a_eprintln(a_string("  a profile <file.a> [-o p]   profile-guided: instrument, run, dump JSON"));
    a_eprintln(a_string("  a gentests <file.a> [-o f]  auto-generate tests from source analysis"));
    a_eprintln(a_string("  a optimize <file.a> <prof>  optimization report from profile data"));
    a_eprintln(a_string("  a cache clean              clear the compilation cache"));
    a_eprintln(a_string("  a pkg init                 create pkg.toml in current directory"));
    a_eprintln(a_string("  a pkg add <name> <source>  add a dependency"));
    a_eprintln(a_string("  a pkg install              install all dependencies"));
    a_eprintln(a_string(""));
    a_eprintln(a_string("  a run and a build expect c_runtime/ in the current directory."));
    a_eprintln(a_string("  shebang: #!/usr/bin/env a run"));
__fn_cleanup:
    return __ret;
}

AValue fn_main(void) {
    AValue argv = {0}, subcmd = {0}, cc_out = {0}, wat_out = {0}, source = {0}, out = {0}, tgt = {0}, i = {0}, ext = {0}, extra = {0}, plugin_argv = {0}, agent_name = {0}, health_port = {0}, max_restarts = {0}, no_restart = {0}, prof_out = {0}, test_out = {0};
    AValue __ret = a_void();
    { AValue __old = argv; argv = a_args(); a_release(__old); }
    if (a_truthy(a_eq(a_len(argv), a_int(0)))) {
        fn__usage();
        (exit((int)a_int(1).ival), a_void());
    }
    { AValue __old = subcmd; subcmd = a_array_get(argv, a_int(0)); a_release(__old); }
    if (a_truthy(a_or(a_eq(subcmd, a_string("--version")), a_eq(subcmd, a_string("version"))))) {
        a_println(a_add(a_string("a "), fn__version()));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("cc")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("cc requires a source file"));
        }
        { AValue __old = cc_out; cc_out = a_string(""); a_release(__old); }
        if (a_truthy(a_and(a_gteq(a_len(argv), a_int(4)), a_eq(a_array_get(argv, a_int(2)), a_string("-o"))))) {
            { AValue __old = cc_out; cc_out = a_array_get(argv, a_int(3)); a_release(__old); }
        }
        fn_cmd_cc(a_array_get(argv, a_int(1)), cc_out);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("wat")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("wat requires a source file"));
        }
        { AValue __old = wat_out; wat_out = a_string(""); a_release(__old); }
        if (a_truthy(a_and(a_gteq(a_len(argv), a_int(4)), a_eq(a_array_get(argv, a_int(2)), a_string("-o"))))) {
            { AValue __old = wat_out; wat_out = a_array_get(argv, a_int(3)); a_release(__old); }
        }
        fn_cmd_wat(a_array_get(argv, a_int(1)), wat_out);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("build")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("build requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = out; out = fn_path_stem(source); a_release(__old); }
        { AValue __old = tgt; tgt = a_string(""); a_release(__old); }
        { AValue __old = i; i = a_int(2); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("-o")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = out; out = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else {
                if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("--target")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                    { AValue __old = tgt; tgt = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                    { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
                } else {
                    { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
                }
            }
        }
        if (a_truthy(a_gt(a_len(tgt), a_int(0)))) {
            { AValue __old = ext; ext = fn__target_ext(tgt); a_release(__old); }
            if (a_truthy(a_and(a_gt(a_len(ext), a_int(0)), a_not(a_str_ends_with(out, ext))))) {
                { AValue __old = out; out = a_add(out, ext); a_release(__old); }
            }
            fn_cmd_build_target(source, out, tgt);
        } else {
            fn_cmd_build(source, out);
        }
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("targets")))) {
        fn_cmd_targets();
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("run")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("run requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = extra; extra = a_array_new(0); a_release(__old); }
        if (a_truthy(a_gt(a_len(argv), a_int(2)))) {
            { AValue __old = i; i = a_int(2); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(argv)))) {
                { AValue __old = extra; extra = a_array_push(extra, a_array_get(argv, i)); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_run(source, extra);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("test")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("test requires a directory"));
        }
        fn_cmd_test(a_array_get(argv, a_int(1)));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("eval")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("eval requires an expression"));
        }
        { AValue __old = extra; extra = a_array_new(0); a_release(__old); }
        if (a_truthy(a_gt(a_len(argv), a_int(2)))) {
            { AValue __old = i; i = a_int(2); a_release(__old); }
            while (a_truthy(a_lt(i, a_len(argv)))) {
                { AValue __old = extra; extra = a_array_push(extra, a_array_get(argv, i)); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_eval(a_array_get(argv, a_int(1)), extra);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("fmt")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("fmt requires a file or directory"));
        }
        fn_cmd_fmt(a_array_get(argv, a_int(1)));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("ast")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("ast requires a source file"));
        }
        fn_cmd_ast(a_array_get(argv, a_int(1)));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("check")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("check requires a source file"));
        }
        fn_cmd_check(a_array_get(argv, a_int(1)));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("repl")))) {
        fn_cmd_repl();
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("plugin")))) {
        { AValue __old = plugin_argv; plugin_argv = a_array_new(0); a_release(__old); }
        { AValue __old = i; i = a_int(1); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            { AValue __old = plugin_argv; plugin_argv = a_array_push(plugin_argv, a_array_get(argv, i)); a_release(__old); }
            { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
        }
        fn_cmd_plugin(plugin_argv);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("pkg")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("usage: a pkg <init|add|install>"));
        }
        if (a_truthy(a_eq(a_array_get(argv, a_int(1)), a_string("init")))) {
            fn_cmd_pkg_init();
            __ret = a_void(); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(a_array_get(argv, a_int(1)), a_string("add")))) {
            if (a_truthy(a_lt(a_len(argv), a_int(4)))) {
                fn__die(a_string("usage: a pkg add <name> <source>"));
            }
            fn_cmd_pkg_add(a_array_get(argv, a_int(2)), a_array_get(argv, a_int(3)));
            __ret = a_void(); goto __fn_cleanup;
        }
        if (a_truthy(a_eq(a_array_get(argv, a_int(1)), a_string("install")))) {
            fn_cmd_pkg_install();
            __ret = a_void(); goto __fn_cleanup;
        }
        fn__die(a_string("usage: a pkg <init|add|install>"));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("cache")))) {
        if (a_truthy(a_and(a_gteq(a_len(argv), a_int(2)), a_eq(a_array_get(argv, a_int(1)), a_string("clean"))))) {
            fn_cmd_cache_clean();
            __ret = a_void(); goto __fn_cleanup;
        }
        fn__die(a_string("usage: a cache clean"));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("watch")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("watch requires a source file"));
        }
        fn_cmd_watch(a_array_get(argv, a_int(1)));
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("agent")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("agent requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = agent_name; agent_name = fn_path_stem(source); a_release(__old); }
        { AValue __old = health_port; health_port = a_int(0); a_release(__old); }
        { AValue __old = max_restarts; max_restarts = a_int(10); a_release(__old); }
        { AValue __old = no_restart; no_restart = a_bool(0); a_release(__old); }
        { AValue __old = i; i = a_int(2); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("--name")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = agent_name; agent_name = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else             if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("--health-port")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = health_port; health_port = a_to_int(a_array_get(argv, a_add(i, a_int(1)))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else             if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("--max-restarts")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = max_restarts; max_restarts = a_to_int(a_array_get(argv, a_add(i, a_int(1)))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else             if (a_truthy(a_eq(a_array_get(argv, i), a_string("--no-restart")))) {
                { AValue __old = no_restart; no_restart = a_bool(1); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            } else {
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_agent(source, agent_name, health_port, max_restarts, no_restart);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("spawn")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("spawn requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = agent_name; agent_name = fn_path_stem(source); a_release(__old); }
        { AValue __old = i; i = a_int(2); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("--name")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = agent_name; agent_name = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else {
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_spawn(source, agent_name);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("lsp")))) {
        fn_cmd_lsp();
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("profile")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("profile requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = prof_out; prof_out = a_string(""); a_release(__old); }
        { AValue __old = i; i = a_int(2); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("-o")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = prof_out; prof_out = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else {
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_profile(source, prof_out);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("gentests")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("gentests requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = test_out; test_out = a_string(""); a_release(__old); }
        { AValue __old = i; i = a_int(2); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("-o")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = test_out; test_out = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else {
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_gentests(source, test_out);
        __ret = a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("optimize")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(3)))) {
            fn__die(a_string("optimize requires a source file and profile path"));
        }
        fn_cmd_optimize_report(a_array_get(argv, a_int(1)), a_array_get(argv, a_int(2)));
        __ret = a_void(); goto __fn_cleanup;
    }
    fn__die(a_str_concat(a_string("unknown command: "), subcmd));
__fn_cleanup:
    a_release(argv);
    a_release(subcmd);
    a_release(cc_out);
    a_release(wat_out);
    a_release(source);
    a_release(out);
    a_release(tgt);
    a_release(i);
    a_release(ext);
    a_release(extra);
    a_release(plugin_argv);
    a_release(agent_name);
    a_release(health_port);
    a_release(max_restarts);
    a_release(no_restart);
    a_release(prof_out);
    a_release(test_out);
    return __ret;
}

int main(int argc, char** argv) {
    g_argc = argc; g_argv = argv;
    fn_main();
    return 0;
}
