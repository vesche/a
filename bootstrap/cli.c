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
AValue fn_parser_is_err(AValue r);
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
AValue fn__die(AValue msg);
AValue fn__find_runtime_dir(void);
AValue fn__generate_c(AValue source_path);
AValue fn__gcc_flags(void);
AValue fn__sqlite_flags(void);
AValue fn__gcc(AValue c_path, AValue out_path, AValue runtime_dir);
AValue fn__tmp_path(AValue suffix);
AValue fn_cmd_cc(AValue source_path, AValue out_file);
AValue fn__codegen_subprocess(AValue source_path, AValue c_path);
AValue fn_cmd_build(AValue source_path, AValue out_path);
AValue fn__cache_dir(void);
AValue fn__ensure_cache_dir(void);
AValue fn__cached_bin(AValue source_path);
AValue fn__store_cache(AValue source_path, AValue bin_path);
AValue fn_cmd_run(AValue source_path, AValue extra_args);
AValue fn_cmd_test(AValue test_dir);
AValue fn_cmd_cache_clean(void);
AValue fn_cmd_eval(AValue expr, AValue extra_args);
AValue fn_cmd_lsp(void);
AValue fn__usage(void);
AValue fn_main(void);

AValue fn_lexer_is_ws(AValue c) {
    AValue __ret = a_void();
    __ret = a_or(a_or(a_eq(c, a_string(" ")), a_eq(c, a_string("\t"))), a_eq(c, a_string("\r"))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_lexer_is_id_start(AValue c) {
    AValue __ret = a_void();
    __ret = a_or(a_is_alpha(c), a_eq(c, a_string("_"))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_lexer_is_id_char(AValue c) {
    AValue __ret = a_void();
    __ret = a_or(a_is_alnum(c), a_eq(c, a_string("_"))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_lexer_keyword_or_ident(AValue word) {
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_lexer_process_escape(AValue c) {
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_lexer_lex(AValue src) {
    AValue chars = {0}, n = {0}, pos = {0}, toks = {0}, interp_depth = {0}, c = {0}, content = {0}, hash_start = {0}, hashes = {0}, found = {0}, hi = {0}, ok = {0}, start = {0}, word = {0}, kind = {0}, is_float = {0}, text = {0}, done = {0}, sc = {0}, tlen = {0}, ended = {0};
    AValue __ret = a_void();
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
            if (a_truthy(a_lt(pos, n))) {
                if (a_truthy(a_eq(a_array_get(chars, pos), a_string("\"")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
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
                if (a_truthy(a_eq(a_array_get(chars, pos), a_string("#")))) {
                    { AValue __old = hash_start; hash_start = a_retain(pos); a_release(__old); }
                    { AValue __old = hashes; hashes = a_int(0); a_release(__old); }
                    while (a_truthy(a_lt(pos, n))) {
                        if (a_truthy(a_eq(a_array_get(chars, pos), a_string("#")))) {
                            { AValue __old = hashes; hashes = a_add(hashes, a_int(1)); a_release(__old); }
                            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                        } else {
                            break;
                        }
                    }
                    if (a_truthy(a_lt(pos, n))) {
                        if (a_truthy(a_eq(a_array_get(chars, pos), a_string("\"")))) {
                            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
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
    return __ret;
}

AValue fn_lexer_tk(AValue toks, AValue i) {
    AValue __ret = a_void();
    __ret = a_array_get(toks, a_mul(i, a_int(2))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_lexer_tv(AValue toks, AValue i) {
    AValue __ret = a_void();
    __ret = a_array_get(toks, a_add(a_mul(i, a_int(2)), a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_lexer_tcount(AValue toks) {
    AValue __ret = a_void();
    __ret = a_div(a_len(toks), a_int(2)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_lexer_skip_nl(AValue toks, AValue p) {
    AValue i = {0};
    AValue __ret = a_void();
    { AValue __old = i; i = a_retain(p); a_release(__old); }
    while (a_truthy(a_eq(fn_lexer_tk(toks, i), a_string("Newline")))) {
        { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(i); goto __fn_cleanup;
__fn_cleanup:
    a_release(i);
    return __ret;
}

AValue fn_ast_mk_program(AValue items) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Program"), "items", items); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_fn_decl(AValue name, AValue params, AValue ret_type, AValue effs, AValue precond, AValue postcond, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(8, "tag", a_string("FnDecl"), "name", name, "params", params, "ret_type", ret_type, "effects", effs, "pre", precond, "post", postcond, "body", body); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_type_decl(AValue name, AValue type_params, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("TypeDecl"), "name", name, "params", type_params, "body", body); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_mod_decl(AValue name, AValue items) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("ModDecl"), "name", name, "items", items); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_use_decl(AValue path) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("UseDecl"), "path", path); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_extern_fn(AValue name, AValue params, AValue ret_type) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("ExternFn"), "name", name, "params", params, "ret_type", ret_type); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_type_record(AValue fields) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TypeRecord"), "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_type_sum(AValue variants) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TypeSum"), "variants", variants); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_type_alias(AValue typ, AValue where_expr) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("TypeAlias"), "type", typ, "where", where_expr); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_variant(AValue name, AValue fields) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Variant"), "name", name, "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_field(AValue name, AValue typ) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Field"), "name", name, "type", typ); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_named(AValue name, AValue args) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("TyNamed"), "name", name, "args", args); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_array(AValue elem) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TyArray"), "elem", elem); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_tuple(AValue elems) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TyTuple"), "elems", elems); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_record(AValue fields) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TyRecord"), "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_fn(AValue params, AValue ret_ty) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("TyFn"), "params", params, "ret", ret_ty); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_map(AValue key, AValue val) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("TyMap"), "key", key, "val", val); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ty_prim(AValue name) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TyPrim"), "name", name); goto __fn_cleanup;
__fn_cleanup:
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
    __ret = a_map_new(3, "tag", a_string("Param"), "name", name, "type", typ); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_block(AValue stmts) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Block"), "stmts", stmts); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_let(AValue mutable, AValue name, AValue typ, AValue value) {
    AValue __ret = a_void();
    __ret = a_map_new(5, "tag", a_string("Let"), "mutable", mutable, "name", name, "type", typ, "value", value); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_assign(AValue target, AValue value) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Assign"), "target", target, "value", value); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_return(AValue expr) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Return"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_expr_stmt(AValue expr) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("ExprStmt"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_if_stmt(AValue cond, AValue then_block, AValue else_branch) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("If"), "cond", cond, "then", then_block, "else", else_branch); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_match_stmt(AValue expr, AValue arms) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Match"), "expr", expr, "arms", arms); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_for_stmt(AValue var, AValue typ, AValue iter, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(5, "tag", a_string("For"), "var", var, "type", typ, "iter", iter, "body", body); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_while_stmt(AValue cond, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("While"), "cond", cond, "body", body); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_let_destructure(AValue bindings, AValue rest, AValue value) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("LetDestructure"), "bindings", bindings, "rest", rest, "value", value); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_for_destructure(AValue bindings, AValue iter, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("ForDestructure"), "bindings", bindings, "iter", iter, "body", body); goto __fn_cleanup;
__fn_cleanup:
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
    __ret = a_map_new(2, "tag", a_string("ElseBlock"), "block", block); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_else_if(AValue if_stmt) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("ElseIf"), "stmt", if_stmt); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_match_arm(AValue pattern, AValue guard, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("MatchArm"), "pattern", pattern, "guard", guard, "body", body); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_ident(AValue name) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("PatIdent"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_constructor(AValue name, AValue args) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("PatConstructor"), "name", name, "args", args); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_literal(AValue lit) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("PatLiteral"), "value", lit); goto __fn_cleanup;
__fn_cleanup:
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
    __ret = a_map_new(2, "tag", a_string("PatArray"), "elems", elems); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_array_elem(AValue pat) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("PatElem"), "pattern", pat); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_rest(AValue name) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("PatRest"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_map(AValue entries) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("PatMap"), "entries", entries); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pat_map_entry(AValue key, AValue pat) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("PatMapEntry"), "key", key, "pattern", pat); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_int(AValue val) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Int"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_float(AValue val) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Float"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_string(AValue val) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("String"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_bool_lit(AValue val) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Bool"), "value", val); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_ident(AValue name) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Ident"), "name", name); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_binop(AValue op, AValue left, AValue right) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("BinOp"), "op", op, "left", left, "right", right); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_unary(AValue op, AValue expr) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("UnaryOp"), "op", op, "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_call(AValue func, AValue args) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Call"), "func", func, "args", args); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_field_access(AValue expr, AValue field) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("FieldAccess"), "expr", expr, "field", field); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_index(AValue expr, AValue index) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Index"), "expr", expr, "index", index); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_try(AValue expr) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Try"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_try_block(AValue block) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("TryBlock"), "block", block); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_if_expr(AValue cond, AValue then_block, AValue else_block) {
    AValue __ret = a_void();
    __ret = a_map_new(4, "tag", a_string("IfExpr"), "cond", cond, "then", then_block, "else", else_block); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_match_expr(AValue expr, AValue arms) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("MatchExpr"), "expr", expr, "arms", arms); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_block_expr(AValue block) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("BlockExpr"), "block", block); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_array(AValue elems) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Array"), "elems", elems); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_record(AValue fields) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Record"), "fields", fields); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_record_field(AValue name, AValue value) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("RecordField"), "name", name, "value", value); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_lambda(AValue params, AValue body) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Lambda"), "params", params, "body", body); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_pipe(AValue left, AValue right) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("Pipe"), "left", left, "right", right); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_interpolation(AValue parts) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Interpolation"), "parts", parts); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_interp_lit(AValue text) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("InterpLit"), "value", text); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_interp_expr(AValue expr) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("InterpExpr"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_map_literal(AValue entries) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("MapLiteral"), "entries", entries); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_map_entry(AValue key, AValue value) {
    AValue __ret = a_void();
    __ret = a_map_new(3, "tag", a_string("MapEntry"), "key", key, "value", value); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_ast_mk_spread(AValue expr) {
    AValue __ret = a_void();
    __ret = a_map_new(2, "tag", a_string("Spread"), "expr", expr); goto __fn_cleanup;
__fn_cleanup:
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
    __ret = a_array_get(toks, a_mul(i, a_int(2))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_tv(AValue toks, AValue i) {
    AValue __ret = a_void();
    __ret = a_array_get(toks, a_add(a_mul(i, a_int(2)), a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_err(AValue msg, AValue pos) {
    AValue __ret = a_void();
    __ret = a_array_new(2, a_map_new(3, "tag", a_string("ParseError"), "msg", msg, "pos", pos), pos); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_is_err(AValue r) {
    AValue __ret = a_void();
    if (a_truthy(a_eq(a_type_of(a_array_get(r, a_int(0))), a_string("map")))) {
        __ret = a_and(a_map_has(a_array_get(r, a_int(0)), a_string("tag")), a_eq(a_array_get(a_array_get(r, a_int(0)), a_string("tag")), a_string("ParseError"))); goto __fn_cleanup;
    }
    __ret = a_bool(0); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_skip_nl(AValue toks, AValue pos) {
    AValue __ret = a_void();
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Newline")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
    }
    __ret = a_retain(pos); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_expect(AValue toks, AValue pos, AValue kind) {
    AValue __ret = a_void();
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), kind))) {
        __ret = a_array_new(2, fn_parser_tv(toks, pos), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_add(a_add(a_string("expected "), kind), a_string(", found ")), fn_parser_tk(toks, pos)), pos); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_expect_ident(AValue toks, AValue pos) {
    AValue __ret = a_void();
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Ident")))) {
        __ret = a_array_new(2, fn_parser_tv(toks, pos), a_add(pos, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_err(a_add(a_string("expected identifier, found "), fn_parser_tk(toks, pos)), pos); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_expect_nl_or_eof(AValue toks, AValue pos) {
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_parser_parse(AValue src) {
    AValue toks = {0}, r = {0};
    AValue __ret = a_void();
    { AValue __old = toks; toks = fn_lexer_lex(src); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_program(toks, a_int(0)); a_release(__old); }
    __ret = a_array_get(r, a_int(0)); goto __fn_cleanup;
__fn_cleanup:
    a_release(toks);
    a_release(r);
    return __ret;
}

AValue fn_parser_parse_program(AValue toks, AValue pos) {
    AValue items = {0}, r = {0};
    AValue __ret = a_void();
    { AValue __old = items; items = a_array_new(0); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("Eof")))) {
        { AValue __old = r; r = fn_parser_parse_top_level(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = items; items = a_array_push(items, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_program(items), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(items);
    a_release(r);
    return __ret;
}

AValue fn_parser_parse_top_level(AValue toks, AValue pos) {
    AValue k = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_parser_parse_fn_decl(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, params = {0}, ret_type = {0}, effs = {0}, precond = {0}, postcond = {0}, body = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwFn")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
        { AValue __old = r; r = fn_parser_parse_param_list(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = ret_type; ret_type = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Arrow")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = ret_type; ret_type = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = effs; effs = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwEffects")))) {
        { AValue __old = r; r = fn_parser_parse_effects(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = precond; precond = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = postcond; postcond = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwPost")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = postcond; postcond = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_extern_fn(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, params = {0}, ret_type = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwExtern")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwFn")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
        { AValue __old = r; r = fn_parser_parse_param_list(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = ret_type; ret_type = fn_ast_mk_ty_prim(a_string("void")); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Arrow")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = ret_type; ret_type = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_extern_fn(name, params, ret_type), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(params);
    a_release(ret_type);
    return __ret;
}

AValue fn_parser_parse_param_list(AValue toks, AValue pos) {
    AValue params = {0}, r = {0};
    AValue __ret = a_void();
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_param(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_param(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, params, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(params);
    a_release(r);
    return __ret;
}

AValue fn_parser_parse_param(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, typ = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = typ; typ = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Colon")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_effects(AValue toks, AValue pos) {
    AValue r = {0}, effs = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwEffects")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBracket")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = effs; effs = a_array_new(0); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = effs; effs = a_array_push(effs, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = effs; effs = a_array_push(effs, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, effs, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(effs);
    return __ret;
}

AValue fn_parser_parse_mod_decl(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, items = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwMod")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = items; items = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        { AValue __old = r; r = fn_parser_parse_top_level(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = items; items = a_array_push(items, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_mod_decl(name, items), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(items);
    return __ret;
}

AValue fn_parser_parse_use_decl(AValue toks, AValue pos) {
    AValue r = {0}, path = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwUse")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = path; path = a_array_new(1, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Dot")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = path; path = a_array_push(path, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_use_decl(path), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(path);
    return __ret;
}

AValue fn_parser_parse_type_decl(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, type_params = {0}, body = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwTy")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = type_params; type_params = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Lt")))) {
        { AValue __old = r; r = fn_parser_parse_type_params(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = type_params; type_params = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Eq")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_type_body(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_type_decl(name, type_params, body), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(type_params);
    a_release(body);
    return __ret;
}

AValue fn_parser_parse_type_params(AValue toks, AValue pos) {
    AValue r = {0}, params = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Lt")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = params; params = a_array_new(0); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Gt")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, params, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(params);
    return __ret;
}

AValue fn_parser_looks_like_sum(AValue toks, AValue pos) {
    AValue next = {0}, j = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_parser_parse_type_body(AValue toks, AValue pos) {
    AValue r = {0}, variants = {0}, alias_ty = {0}, where_expr = {0};
    AValue __ret = a_void();
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBrace")))) {
        { AValue __old = r; r = fn_parser_parse_record_fields(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_type_record(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(fn_parser_looks_like_sum(toks, pos))) {
        { AValue __old = variants; variants = a_array_new(0); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_variant(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = variants; variants = a_array_push(variants, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Pipe")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_variant(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = variants; variants = a_array_push(variants, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        __ret = a_array_new(2, fn_ast_mk_type_sum(variants), pos); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = alias_ty; alias_ty = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = where_expr; where_expr = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwWhere")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = where_expr; where_expr = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_variant(AValue toks, AValue pos) {
    AValue r = {0}, name = {0}, fields = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = fields; fields = a_array_new(0); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LParen")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
            { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, fn_ast_mk_variant(name, fields), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    a_release(fields);
    return __ret;
}

AValue fn_parser_parse_record_fields(AValue toks, AValue pos) {
    AValue r = {0}, fields = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = fields; fields = a_array_new(0); a_release(__old); }
    if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        { AValue __old = r; r = fn_parser_parse_field(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = fields; fields = a_array_push(fields, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fields, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(fields);
    return __ret;
}

AValue fn_parser_parse_field(AValue toks, AValue pos) {
    AValue r = {0}, name = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_field(name, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(name);
    return __ret;
}

AValue fn_parser_parse_type_expr(AValue toks, AValue pos) {
    AValue k = {0}, r = {0}, inner = {0}, types = {0}, params = {0}, key_ty = {0}, val_ty = {0}, name = {0}, type_args = {0};
    AValue __ret = a_void();
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
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = inner; inner = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_ty_array(inner), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LParen")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = types; types = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = types; types = a_array_push(types, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = types; types = a_array_push(types, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        __ret = a_array_new(2, fn_ast_mk_ty_tuple(types), pos); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwFn")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LParen")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = params; params = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
            { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = params; params = a_array_push(params, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Arrow")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_ty_fn(params, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Hash")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = key_ty; key_ty = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = val_ty; val_ty = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = type_args; type_args = a_array_push(type_args, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = type_args; type_args = a_array_push(type_args, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Gt")); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_block(AValue toks, AValue pos) {
    AValue r = {0}, stmts = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = stmts; stmts = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Eof")))) {
            __ret = fn_parser_err(a_string("unexpected end of file in block"), pos); goto __fn_cleanup;
        }
        { AValue __old = r; r = fn_parser_parse_stmt(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = stmts; stmts = a_array_push(stmts, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_block(stmts), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(stmts);
    return __ret;
}

AValue fn_parser_parse_stmt(AValue toks, AValue pos) {
    AValue k = {0}, r = {0}, expr = {0}, value = {0};
    AValue __ret = a_void();
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
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_break(), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwContinue")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_continue(), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Eq")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = value; value = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_assign(expr, value), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_expr_stmt(expr), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(r);
    a_release(expr);
    a_release(value);
    return __ret;
}

AValue fn_parser_parse_let_stmt(AValue toks, AValue pos) {
    AValue r = {0}, mutable = {0}, name = {0}, typ = {0}, value = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwLet")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = name; name = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = typ; typ = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Colon")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = typ; typ = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Eq")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = value; value = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_let(mutable, name, typ, value), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(mutable);
    a_release(name);
    a_release(typ);
    a_release(value);
    return __ret;
}

AValue fn_parser_parse_let_destructure(AValue toks, AValue pos) {
    AValue r = {0}, bindings = {0}, rest = {0}, has_rest = {0}, value = {0}, rest_val = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBracket")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
                    if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
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
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Eq")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = value; value = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_ret_stmt(AValue toks, AValue pos) {
    AValue r = {0}, expr = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwRet")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect_nl_or_eof(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_return(expr), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(expr);
    return __ret;
}

AValue fn_parser_parse_if_stmt(AValue toks, AValue pos) {
    AValue r = {0}, cond = {0}, then_block = {0}, else_branch = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwIf")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = cond; cond = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = then_block; then_block = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = else_branch; else_branch = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwElse")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwIf")))) {
            { AValue __old = r; r = fn_parser_parse_if_stmt(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = else_branch; else_branch = fn_ast_mk_else_if(a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_match_stmt(AValue toks, AValue pos) {
    AValue r = {0}, expr = {0}, arms = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwMatch")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = expr; expr = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = arms; arms = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
        { AValue __old = r; r = fn_parser_parse_match_arm(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = arms; arms = a_array_push(arms, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    __ret = a_array_new(2, fn_ast_mk_match_stmt(expr, arms), pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(expr);
    a_release(arms);
    return __ret;
}

AValue fn_parser_parse_match_arm(AValue toks, AValue pos) {
    AValue r = {0}, pattern = {0}, guard = {0}, body = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pattern; pattern = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = guard; guard = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("KwIf")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = guard; guard = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("FatArrow")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = body; body = fn_ast_mk_void_lit(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBrace")))) {
        { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    } else {
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_for_stmt(AValue toks, AValue pos) {
    AValue r = {0}, var = {0}, typ = {0}, iter = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwFor")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBracket")))) {
        __ret = fn_parser_parse_for_destructure(toks, pos); goto __fn_cleanup;
    }
    { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = var; var = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = typ; typ = fn_ast_mk_ty_infer(); a_release(__old); }
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Colon")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_type_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = typ; typ = a_array_get(r, a_int(0)); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwIn")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = iter; iter = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_for_stmt(var, typ, iter, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(var);
    a_release(typ);
    a_release(iter);
    return __ret;
}

AValue fn_parser_parse_for_destructure(AValue toks, AValue pos) {
    AValue r = {0}, bindings = {0}, iter = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBracket")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = bindings; bindings = a_array_new(0); a_release(__old); }
    while (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
        { AValue __old = r; r = fn_parser_expect_ident(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwIn")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = iter; iter = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_for_destructure(bindings, iter, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(bindings);
    a_release(iter);
    return __ret;
}

AValue fn_parser_parse_while_stmt(AValue toks, AValue pos) {
    AValue r = {0}, cond = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("KwWhile")); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = cond; cond = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
    { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    __ret = a_array_new(2, fn_ast_mk_while_stmt(cond, a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(cond);
    return __ret;
}

AValue fn_parser_parse_pattern(AValue toks, AValue pos) {
    AValue k = {0}, elems = {0}, r = {0}, entries = {0}, key = {0}, name = {0}, pats = {0};
    AValue __ret = a_void();
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
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = elems; elems = a_array_push(elems, fn_ast_mk_pat_rest(a_array_get(r, a_int(0)))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                break;
            }
            { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_pat_array(elems), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Hash")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("LBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
        if (a_truthy(a_is_err(r))) {
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
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = pats; pats = a_array_push(pats, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_pattern(toks, pos); a_release(__old); }
                    if (a_truthy(a_is_err(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = pats; pats = a_array_push(pats, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_expr(AValue toks, AValue pos) {
    AValue __ret = a_void();
    __ret = fn_parser_parse_pipe_expr(toks, pos); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_parser_parse_pipe_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, saved = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_or_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("PipeArrow")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_or_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
                    if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_or_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_and_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("PipePipe")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_and_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = left; left = fn_ast_mk_binop(a_string("||"), left, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    return __ret;
}

AValue fn_parser_parse_and_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_eq_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
        __ret = a_retain(r); goto __fn_cleanup;
    }
    { AValue __old = left; left = a_array_get(r, a_int(0)); a_release(__old); }
    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("AmpAmp")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_eq_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = left; left = fn_ast_mk_binop(a_string("&&"), left, a_array_get(r, a_int(0))); a_release(__old); }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
    }
    __ret = a_array_new(2, left, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    a_release(left);
    return __ret;
}

AValue fn_parser_parse_eq_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_cmp_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("=="), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("NotEq")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_cmp_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_cmp_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("<"), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Gt")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = left; left = fn_ast_mk_binop(a_string(">"), left, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            } else {
                if (a_truthy(a_eq(k, a_string("LtEq")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
                    if (a_truthy(a_is_err(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = left; left = fn_ast_mk_binop(a_string("<="), left, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                } else {
                    if (a_truthy(a_eq(k, a_string("GtEq")))) {
                        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                        { AValue __old = r; r = fn_parser_parse_add_expr(toks, pos); a_release(__old); }
                        if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_add_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_mul_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("+"), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Minus")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_mul_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_mul_expr(AValue toks, AValue pos) {
    AValue r = {0}, left = {0}, cont = {0}, k = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = left; left = fn_ast_mk_binop(a_string("*"), left, a_array_get(r, a_int(0))); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            if (a_truthy(a_eq(k, a_string("Slash")))) {
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = left; left = fn_ast_mk_binop(a_string("/"), left, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            } else {
                if (a_truthy(a_eq(k, a_string("Percent")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
                    if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_unary_expr(AValue toks, AValue pos) {
    AValue k = {0}, r = {0};
    AValue __ret = a_void();
    { AValue __old = k; k = fn_parser_tk(toks, pos); a_release(__old); }
    if (a_truthy(a_eq(k, a_string("Minus")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_unary(a_string("-"), a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("Bang")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_unary(a_string("!"), a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("KwTry")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("LBrace")))) {
            { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            __ret = a_array_new(2, fn_ast_mk_try_block(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
        }
        { AValue __old = r; r = fn_parser_parse_unary_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_try(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_parse_postfix_expr(toks, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(k);
    a_release(r);
    return __ret;
}

AValue fn_parser_parse_postfix_expr(AValue toks, AValue pos) {
    AValue r = {0}, expr = {0}, cont = {0}, k = {0}, args = {0}, idx = {0};
    AValue __ret = a_void();
    { AValue __old = r; r = fn_parser_parse_primary_expr(toks, pos); a_release(__old); }
    if (a_truthy(a_is_err(r))) {
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
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = args; args = a_array_push(args, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                while (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("Comma")))) {
                    { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                    if (a_truthy(a_is_err(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = args; args = a_array_push(args, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            }
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
                    if (a_truthy(a_is_err(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = expr; expr = fn_ast_mk_field_access(expr, a_array_get(r, a_int(0))); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                }
            } else {
                if (a_truthy(a_eq(k, a_string("LBracket")))) {
                    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
                    { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                    if (a_truthy(a_is_err(r))) {
                        __ret = a_retain(r); goto __fn_cleanup;
                    }
                    { AValue __old = idx; idx = a_array_get(r, a_int(0)); a_release(__old); }
                    { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                    { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
                    if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_primary_expr(AValue toks, AValue pos) {
    AValue k = {0}, name = {0}, r = {0}, params = {0}, body = {0}, entries = {0}, key = {0}, r2 = {0}, elems = {0};
    AValue __ret = a_void();
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
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        { AValue __old = params; params = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RParen")))) {
            { AValue __old = r; r = fn_parser_parse_param_list(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = params; params = a_array_get(r, a_int(0)); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = body; body = fn_ast_mk_void_lit(); a_release(__old); }
        if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("FatArrow")))) {
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = body; body = a_array_get(r, a_int(0)); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
        } else {
            { AValue __old = r; r = fn_parser_parse_block(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = entries; entries = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBrace")))) {
            { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = key; key = a_array_get(r, a_int(0)); a_release(__old); }
            { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
                __ret = a_retain(r); goto __fn_cleanup;
            }
            { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
            { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = key; key = a_array_get(r, a_int(0)); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
                { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("Colon")); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
                { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = entries; entries = a_array_push(entries, fn_ast_mk_map_entry(key, a_array_get(r, a_int(0)))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBrace")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_map_literal(entries), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LParen")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_array_get(r, a_int(1))); a_release(__old); }
        { AValue __old = r2; r2 = fn_parser_expect(toks, pos, a_string("RParen")); a_release(__old); }
        if (a_truthy(a_is_err(r2))) {
            __ret = a_retain(r2); goto __fn_cleanup;
        }
        __ret = a_array_new(2, a_array_get(r, a_int(0)), a_array_get(r2, a_int(1))); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(k, a_string("LBracket")))) {
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, a_add(pos, a_int(1))); a_release(__old); }
        { AValue __old = elems; elems = a_array_new(0); a_release(__old); }
        if (a_truthy(a_neq(fn_parser_tk(toks, pos), a_string("RBracket")))) {
            { AValue __old = r; r = fn_parser_parse_array_element(toks, pos); a_release(__old); }
            if (a_truthy(a_is_err(r))) {
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
                if (a_truthy(a_is_err(r))) {
                    __ret = a_retain(r); goto __fn_cleanup;
                }
                { AValue __old = elems; elems = a_array_push(elems, a_array_get(r, a_int(0))); a_release(__old); }
                { AValue __old = pos; pos = a_array_get(r, a_int(1)); a_release(__old); }
            }
        }
        { AValue __old = pos; pos = fn_parser_skip_nl(toks, pos); a_release(__old); }
        { AValue __old = r; r = fn_parser_expect(toks, pos, a_string("RBracket")); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_parser_parse_array_element(AValue toks, AValue pos) {
    AValue r = {0};
    AValue __ret = a_void();
    if (a_truthy(a_eq(fn_parser_tk(toks, pos), a_string("DotDotDot")))) {
        { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
            __ret = a_retain(r); goto __fn_cleanup;
        }
        __ret = a_array_new(2, fn_ast_mk_spread(a_array_get(r, a_int(0))), a_array_get(r, a_int(1))); goto __fn_cleanup;
    }
    __ret = fn_parser_parse_expr(toks, pos); goto __fn_cleanup;
__fn_cleanup:
    a_release(r);
    return __ret;
}

AValue fn_parser_parse_interp_string(AValue toks, AValue pos) {
    AValue parts = {0}, text = {0}, cont = {0}, r = {0}, k = {0};
    AValue __ret = a_void();
    { AValue __old = parts; parts = a_array_new(0); a_release(__old); }
    { AValue __old = text; text = fn_parser_tv(toks, pos); a_release(__old); }
    if (a_truthy(a_gt(a_len(a_str_chars(text)), a_int(0)))) {
        { AValue __old = parts; parts = a_array_push(parts, fn_ast_mk_interp_lit(text)); a_release(__old); }
    }
    { AValue __old = pos; pos = a_add(pos, a_int(1)); a_release(__old); }
    { AValue __old = cont; cont = a_bool(1); a_release(__old); }
    while (a_truthy(cont)) {
        { AValue __old = r; r = fn_parser_parse_expr(toks, pos); a_release(__old); }
        if (a_truthy(a_is_err(r))) {
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
    return __ret;
}

AValue fn_cgen__builtin_map(void) {
    AValue m = {0}, m2 = {0}, m3 = {0}, m4 = {0};
    AValue __ret = a_void();
    { AValue __old = m; m = a_map_new(27, "println", a_string("a_println"), "print", a_string("a_print"), "eprintln", a_string("a_eprintln"), "len", a_string("a_len"), "push", a_string("a_array_push"), "to_str", a_string("a_to_str"), "fail", a_string("a_fail"), "type_of", a_string("a_type_of"), "int", a_string("a_to_int"), "float", a_string("a_to_float"), "sort", a_string("a_sort"), "contains", a_string("a_contains"), "reverse_arr", a_string("a_reverse_arr"), "concat_arr", a_string("a_concat_arr"), "args", a_string("a_args"), "slice", a_string("a_array_slice"), "char_code", a_string("a_char_code"), "from_code", a_string("a_from_code"), "is_alpha", a_string("a_is_alpha"), "is_digit", a_string("a_is_digit"), "is_alnum", a_string("a_is_alnum"), "Ok", a_string("a_ok"), "Err", a_string("a_err"), "unwrap", a_string("a_unwrap"), "is_ok", a_string("a_is_ok"), "is_err", a_string("a_is_err"), "unwrap_or", a_string("a_unwrap_or")); a_release(__old); }
    { AValue __old = m2; m2 = a_map_new(24, "str.concat", a_string("a_str_concat"), "str.split", a_string("a_str_split"), "str.contains", a_string("a_str_contains"), "str.replace", a_string("a_str_replace"), "str.trim", a_string("a_str_trim"), "str.upper", a_string("a_str_upper"), "str.lower", a_string("a_str_lower"), "str.join", a_string("a_str_join"), "str.chars", a_string("a_str_chars"), "str.slice", a_string("a_str_slice"), "str.starts_with", a_string("a_str_starts_with"), "str.ends_with", a_string("a_str_ends_with"), "str.find", a_string("a_str_find"), "str.count", a_string("a_str_count"), "str.lines", a_string("a_str_lines"), "map.get", a_string("a_map_get"), "map.set", a_string("a_map_set"), "map.has", a_string("a_map_has"), "map.keys", a_string("a_map_keys"), "map.values", a_string("a_map_values"), "map.merge", a_string("a_map_merge"), "map.delete", a_string("a_map_delete"), "map.entries", a_string("a_map_entries"), "map.from_entries", a_string("a_map_from_entries")); a_release(__old); }
    { AValue __old = m3; m3 = a_map_new(29, "io.read_file", a_string("a_io_read_file"), "io.write_file", a_string("a_io_write_file"), "io.read_stdin", a_string("a_io_read_stdin"), "io.read_line", a_string("a_io_read_line"), "io.read_bytes", a_string("a_io_read_bytes"), "io.flush", a_string("a_io_flush"), "fs.ls", a_string("a_fs_ls"), "fs.mkdir", a_string("a_fs_mkdir"), "fs.cwd", a_string("a_fs_cwd"), "fs.exists", a_string("a_fs_exists"), "fs.is_dir", a_string("a_fs_is_dir"), "fs.rm", a_string("a_fs_rm"), "fs.mv", a_string("a_fs_mv"), "fs.cp", a_string("a_fs_cp"), "fs.abs", a_string("a_fs_abs"), "fs.is_file", a_string("a_fs_is_file"), "exec", a_string("a_exec"), "proc.spawn", a_string("a_proc_spawn"), "proc.write", a_string("a_proc_write"), "proc.read_line", a_string("a_proc_read_line"), "proc.kill", a_string("a_proc_kill"), "proc.wait", a_string("a_proc_wait"), "proc.is_running", a_string("a_proc_is_running"), "env.get", a_string("a_env_get"), "env.set", a_string("a_env_set"), "env.all", a_string("a_env_all"), "json.parse", a_string("a_json_parse"), "json.stringify", a_string("a_json_stringify"), "json.pretty", a_string("a_json_pretty")); a_release(__old); }
    { AValue __old = m4; m4 = a_map_new(64, "math.sqrt", a_string("a_math_sqrt"), "math.abs", a_string("a_math_abs"), "math.floor", a_string("a_math_floor"), "math.ceil", a_string("a_math_ceil"), "math.round", a_string("a_math_round"), "math.pow", a_string("a_math_pow"), "math.min", a_string("a_math_min"), "math.max", a_string("a_math_max"), "time.now", a_string("a_time_now"), "time.sleep", a_string("a_time_sleep"), "hash.sha256", a_string("a_hash_sha256"), "hash.md5", a_string("a_hash_md5"), "uuid.v4", a_string("a_uuid_v4"), "signal.on", a_string("a_signal_on"), "image.load", a_string("a_image_load"), "image.decode", a_string("a_image_decode"), "image.save", a_string("a_image_save"), "image.encode", a_string("a_image_encode"), "image.width", a_string("a_image_width"), "image.height", a_string("a_image_height"), "image.resize", a_string("a_image_resize"), "image.pixels", a_string("a_image_pixels"), "http.get", a_string("a_http_get"), "http.post", a_string("a_http_post"), "http.put", a_string("a_http_put"), "http.patch", a_string("a_http_patch"), "http.delete", a_string("a_http_delete"), "http.stream", a_string("a_http_stream"), "http.stream_read", a_string("a_http_stream_read"), "http.stream_close", a_string("a_http_stream_close"), "ws.connect", a_string("a_ws_connect"), "ws.send", a_string("a_ws_send"), "ws.recv", a_string("a_ws_recv"), "ws.close", a_string("a_ws_close"), "http.serve", a_string("a_http_serve"), "http.serve_static", a_string("a_http_serve_static"), "db.open", a_string("a_db_open"), "db.close", a_string("a_db_close"), "db.exec", a_string("a_db_exec"), "db.query", a_string("a_db_query"), "map", a_string("a_hof_map"), "filter", a_string("a_hof_filter"), "reduce", a_string("a_hof_reduce"), "each", a_string("a_hof_each"), "sort_by", a_string("a_hof_sort_by"), "find", a_string("a_hof_find"), "any", a_string("a_hof_any"), "all", a_string("a_hof_all"), "flat_map", a_string("a_hof_flat_map"), "min_by", a_string("a_hof_min_by"), "max_by", a_string("a_hof_max_by"), "enumerate", a_string("a_enumerate"), "zip", a_string("a_zip"), "take", a_string("a_take"), "drop", a_string("a_drop"), "unique", a_string("a_unique"), "chunk", a_string("a_chunk"), "ptr.null", a_string("a_ptr_null"), "ptr.is_null", a_string("a_is_null"), "argv0", a_string("a_argv0"), "compress.deflate", a_string("a_compress_deflate"), "compress.inflate", a_string("a_compress_inflate"), "compress.gzip", a_string("a_compress_gzip"), "compress.gunzip", a_string("a_compress_gunzip")); a_release(__old); }
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
    __ret = a_array_new(11, a_string("println"), a_string("print"), a_string("eprintln"), a_string("fail"), a_string("each"), a_string("env.set"), a_string("time.sleep"), a_string("io.flush"), a_string("http.serve"), a_string("http.serve_static"), a_string("db.close")); goto __fn_cleanup;
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
    { AValue __old = out; out = a_str_replace(a_str_replace(name, a_string("."), a_string("_")), a_string("-"), a_string("_")); a_release(__old); }
    if (a_truthy(a_contains(fn_cgen__c_keywords(), out))) {
        __ret = a_str_concat(a_string("_"), out); goto __fn_cleanup;
    }
    __ret = a_retain(out); goto __fn_cleanup;
__fn_cleanup:
    a_release(out);
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
    return __ret;
}

AValue fn_cgen__indent(AValue depth) {
    AValue out = {0}, i = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__prefixed_name(AValue name, AValue ctx) {
    AValue prefix = {0};
    AValue __ret = a_void();
    { AValue __old = prefix; prefix = a_array_get(ctx, a_string("prefix")); a_release(__old); }
    if (a_truthy(a_eq(a_type_of(prefix), a_string("str")))) {
        if (a_truthy(a_gt(a_len(prefix), a_int(0)))) {
            __ret = a_str_concat(prefix, a_str_concat(a_string("_"), name)); goto __fn_cleanup;
        }
    }
    __ret = a_retain(name); goto __fn_cleanup;
__fn_cleanup:
    a_release(prefix);
    return __ret;
}

AValue fn_cgen__is_intra_module_call(AValue fname, AValue ctx) {
    AValue prefix = {0}, fns = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__R(AValue code, AValue li) {
    AValue __ret = a_void();
    __ret = a_array_new(3, code, li, a_array_new(0)); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__RL(AValue code, AValue li, AValue lifted) {
    AValue __ret = a_void();
    __ret = a_array_new(3, code, li, lifted); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__is_ident(AValue node) {
    AValue n = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_cleanup(AValue vars, AValue depth) {
    AValue ind = {0}, out = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__ea_collect_idents(AValue expr) {
    AValue tag = {0}, n = {0}, ids = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__escape_analysis(AValue stmts, AValue captures) {
    AValue escaping = {0}, tag = {0}, ids = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__collect_idents_in_expr(AValue node) {
    AValue tag = {0}, ids = {0}, func = {0}, body = {0}, bound = {0}, body_ids = {0}, free = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__collect_idents_in_block(AValue block) {
    AValue ids = {0}, stmts = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__collect_idents_in_stmt(AValue s) {
    AValue tag = {0}, ids = {0}, eb = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__compute_captures(AValue lambda_node, AValue enclosing_vars, AValue bm) {
    AValue bound = {0}, body = {0}, body_lets = {0}, body_ids = {0}, captures = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__collect_lets_in_block(AValue block) {
    AValue names = {0}, stmts = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen_emit_expr(AValue node, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, name = {0}, op = {0}, lr = {0}, rr = {0}, l = {0}, r = {0}, lifted = {0}, code = {0}, ir = {0}, func = {0}, a = {0}, fname = {0}, ar = {0}, arg_codes = {0}, cfn = {0}, all_fns = {0}, is_known_fn = {0}, call_args = {0}, mangled = {0}, aliases = {0}, er = {0}, field = {0}, elems = {0}, has_spread = {0}, parts = {0}, entries = {0}, key_node = {0}, key_str = {0}, vr = {0}, cparts = {0}, cr = {0}, tr = {0}, stmts = {0}, last = {0}, right = {0}, new_args = {0}, new_call = {0}, lambda_name = {0}, params = {0}, body = {0}, enclosing_vars = {0}, captures = {0}, fn_code = {0}, ci = {0}, pi = {0}, next_li = {0}, body_lifted = {0}, lambda_vars = {0}, body_tag = {0}, body_vars = {0}, bound = {0}, decl_vars = {0}, body_lets = {0}, lambda_ctx = {0}, init_parts = {0}, lambda_cleanup = {0}, has_return = {0}, si = {0}, s = {0}, is_last = {0}, cleanup = {0}, sr = {0}, env_code = {0}, cap_parts = {0}, closure_code = {0}, all_lifted = {0}, block = {0}, nl = {0}, try_vars = {0}, nstmts = {0}, has_tail = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__resolve_call_name(AValue func) {
    AValue obj = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_pat_cond(AValue pat, AValue target, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, val = {0}, vr = {0}, name = {0}, elems = {0}, has_rest = {0}, fixed_count = {0}, cond = {0}, idx = {0}, lifted = {0}, sub = {0}, elem_var = {0}, sc = {0}, entries = {0}, key = {0}, val_expr = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_pat_bind(AValue pat, AValue target, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, ind = {0}, name = {0}, args = {0}, code = {0}, inner = {0}, inner_expr = {0}, br = {0}, elems = {0}, idx = {0}, lifted = {0}, elem_var = {0}, rest_expr = {0}, entries = {0}, key = {0}, val_expr = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_arm_body(AValue body, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue code = {0}, lifted = {0}, sr = {0}, er = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_match(AValue node, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue ind = {0}, d1 = {0}, d2 = {0}, er = {0}, lifted = {0}, arms = {0}, out = {0}, pat = {0}, guard = {0}, body = {0}, cr = {0}, has_guard = {0}, br = {0}, gr = {0}, br2 = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_match_expr(AValue node, AValue bm, AValue ctx, AValue li) {
    AValue nl = {0}, er = {0}, lifted = {0}, arms = {0}, code = {0}, arm_vars = {0}, pv = {0}, pat = {0}, guard = {0}, body = {0}, cr = {0}, has_guard = {0}, br = {0}, gr = {0}, body_r = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_match_expr_body(AValue body, AValue bm, AValue ctx, AValue li) {
    AValue lifted = {0}, stmts = {0}, last = {0}, nl = {0}, preamble = {0}, i = {0}, sr = {0}, er = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen_emit_stmt(AValue node, AValue depth, AValue bm, AValue ctx, AValue li) {
    AValue tag = {0}, ind = {0}, vr = {0}, vname = {0}, assign_expr = {0}, code = {0}, bindings = {0}, i = {0}, rest_name = {0}, target = {0}, tname = {0}, tr = {0}, ir = {0}, use_goto = {0}, cleanup_vars = {0}, has_cleanup = {0}, cleanup = {0}, val_expr = {0}, ret_expr = {0}, er = {0}, cr = {0}, lifted = {0}, out = {0}, then_stmts = {0}, sr = {0}, else_branch = {0}, else_stmts = {0}, body_stmts = {0}, var_name = {0}, body_vars = {0}, for_decls = {0}, init_parts = {0}, for_cleanup = {0}, decls = {0}, bi = {0}, fd_cleanup = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__collect_pattern_vars(AValue pat) {
    AValue tag = {0}, vs = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__collect_vars_in_stmts(AValue stmts) {
    AValue vars = {0}, tag = {0}, name = {0}, tv = {0}, eb = {0}, ev = {0}, eiv = {0}, wv = {0}, fv = {0}, rn = {0}, pv = {0}, body = {0}, bv = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen_emit_fn(AValue node, AValue bm, AValue ctx, AValue li) {
    AValue name = {0}, full_name = {0}, params = {0}, body = {0}, param_parts = {0}, param_str = {0}, out = {0}, all_vars = {0}, param_names = {0}, decl_vars = {0}, init_parts = {0}, all_cleanup_vars = {0}, enc_vars = {0}, fn_ctx = {0}, lifted = {0}, stmts = {0}, sr = {0}, cleanup = {0};
    AValue __ret = a_void();
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
    { AValue __old = all_cleanup_vars; all_cleanup_vars = a_retain(decl_vars); a_release(__old); }
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
    a_release(all_cleanup_vars);
    a_release(enc_vars);
    a_release(fn_ctx);
    a_release(lifted);
    a_release(stmts);
    a_release(sr);
    a_release(cleanup);
    return __ret;
}

AValue fn_cgen__emit_fwd_decl(AValue name, AValue params) {
    AValue param_parts = {0}, param_str = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__use_path_to_file(AValue path_arr) {
    AValue __ret = a_void();
    __ret = a_str_concat(a_str_join(path_arr, a_string("/")), a_string(".a")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__use_path_short_name(AValue path_arr) {
    AValue __ret = a_void();
    __ret = a_array_get(path_arr, a_sub(a_len(path_arr), a_int(1))); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cgen__collect_fn_names(AValue items) {
    AValue names = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__load_module(AValue path_arr, AValue bm, AValue loaded, AValue li) {
    AValue use_key = {0}, file_path = {0}, source = {0}, mod_ast = {0}, items = {0}, _short = {0}, all_fwd = {0}, all_fns = {0}, sub = {0}, fn_names = {0}, ctx = {0}, aliases = {0}, full_name = {0}, fr = {0};
    AValue __ret = a_void();
    { AValue __old = use_key; use_key = a_str_join(path_arr, a_string(".")); a_release(__old); }
    if (a_truthy(a_contains(loaded, use_key))) {
        __ret = a_map_new(4, "fwd", a_array_new(0), "fns", a_array_new(0), "loaded", loaded, "li", li); goto __fn_cleanup;
    }
    { AValue __old = loaded; loaded = a_array_push(loaded, use_key); a_release(__old); }
    { AValue __old = file_path; file_path = fn_cgen__use_path_to_file(path_arr); a_release(__old); }
    { AValue __old = source; source = a_io_read_file(file_path); a_release(__old); }
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
    return __ret;
}

AValue fn_cgen__ffi_c_type(AValue ty_node) {
    AValue tag = {0}, name = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__ffi_extract(AValue ty_node, AValue var_name) {
    AValue tag = {0}, name = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__ffi_wrap(AValue ty_node, AValue expr) {
    AValue tag = {0}, name = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen__emit_extern_fn(AValue node) {
    AValue name = {0}, params = {0}, ret_type = {0}, c_ret = {0}, is_void = {0}, c_param_parts = {0}, c_param_str = {0}, raw_proto = {0}, shim_params = {0}, pi = {0}, shim_param_str = {0}, shim = {0}, call_args = {0}, p = {0}, extracted = {0}, call_str = {0}, fwd = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cgen_emit_program(AValue prog_ast, AValue bm) {
    AValue items = {0}, all_fwd = {0}, all_fns = {0}, loaded = {0}, li = {0}, import_aliases = {0}, result = {0}, al = {0}, main_fn_names = {0}, extern_protos = {0}, extern_shims = {0}, parts = {0}, main_ctx = {0}, fr = {0}, out = {0}, lambda_fwd = {0}, i = {0};
    AValue __ret = a_void();
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

AValue fn_path_join(AValue a, AValue b) {
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path_join3(AValue a, AValue b, AValue c) {
    AValue __ret = a_void();
    __ret = fn_path_join(fn_path_join(a, b), c); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_path_dirname(AValue p) {
    AValue clean = {0}, idx = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path_basename(AValue p) {
    AValue clean = {0}, idx = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path_extension(AValue p) {
    AValue base = {0}, idx = {0};
    AValue __ret = a_void();
    { AValue __old = base; base = fn_path_basename(p); a_release(__old); }
    { AValue __old = idx; idx = fn_path__last_dot(base); a_release(__old); }
    if (a_truthy(a_lteq(idx, a_int(0)))) {
        __ret = a_string(""); goto __fn_cleanup;
    }
    __ret = a_str_slice(base, a_add(idx, a_int(1)), a_len(base)); goto __fn_cleanup;
__fn_cleanup:
    a_release(base);
    a_release(idx);
    return __ret;
}

AValue fn_path_stem(AValue p) {
    AValue base = {0}, idx = {0};
    AValue __ret = a_void();
    { AValue __old = base; base = fn_path_basename(p); a_release(__old); }
    { AValue __old = idx; idx = fn_path__last_dot(base); a_release(__old); }
    if (a_truthy(a_lteq(idx, a_int(0)))) {
        __ret = a_retain(base); goto __fn_cleanup;
    }
    __ret = a_str_slice(base, a_int(0), idx); goto __fn_cleanup;
__fn_cleanup:
    a_release(base);
    a_release(idx);
    return __ret;
}

AValue fn_path_with_extension(AValue p, AValue ext) {
    AValue dir = {0}, s = {0}, new_base = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path_is_absolute(AValue p) {
    AValue __ret = a_void();
    __ret = a_str_starts_with(p, a_string("/")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_path_segments(AValue p) {
    AValue parts = {0}, out = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path_normalize(AValue p) {
    AValue is_abs = {0}, parts = {0}, stack = {0}, top = {0}, result = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path__last_slash(AValue s) {
    AValue chars = {0}, last = {0}, i = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_path__last_dot(AValue s) {
    AValue chars = {0}, last = {0}, i = {0};
    AValue __ret = a_void();
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
    { AValue __old = e; e = fn_cli_esc_code(); a_release(__old); }
    __ret = a_concat_n(7, a_to_str(e), a_string("["), a_to_str(code), a_string("m"), a_to_str(s), a_to_str(e), a_string("[0m")); goto __fn_cleanup;
__fn_cleanup:
    a_release(e);
    return __ret;
}

AValue fn_cli_red(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("31")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_green(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("32")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_yellow(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("33")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_blue(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("34")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_magenta(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("35")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_cyan(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("36")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_gray(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("90")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_bold(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("1")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_dim(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("2")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn_cli_underline(AValue s) {
    AValue __ret = a_void();
    __ret = fn_cli_wrap(s, a_string("4")); goto __fn_cleanup;
__fn_cleanup:
    return __ret;
}

AValue fn__die(AValue msg) {
    AValue __ret = a_void();
    a_eprintln(fn_cli_red(a_str_concat(a_string("error: "), msg)));
    (exit((int)a_int(1).ival), a_void());
__fn_cleanup:
    return __ret;
}

AValue fn__find_runtime_dir(void) {
    AValue candidates = {0};
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
    fn__die(a_string("cannot find c_runtime/ directory (runtime.c + runtime.h). Run from the project root."));
    __ret = a_string(""); goto __fn_cleanup;
__fn_cleanup:
    a_release(candidates);
    return __ret;
}

AValue fn__generate_c(AValue source_path) {
    AValue source = {0}, lines = {0}, prog_ast = {0}, bm = {0};
    AValue __ret = a_void();
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

AValue fn__gcc(AValue c_path, AValue out_path, AValue runtime_dir) {
    AValue runtime_c = {0}, sqlite_c = {0}, miniz_c = {0}, stb_impl_c = {0}, include_flag = {0}, cmd = {0}, result = {0};
    AValue __ret = a_void();
    { AValue __old = runtime_c; runtime_c = a_str_concat(runtime_dir, a_string("/runtime.c")); a_release(__old); }
    { AValue __old = sqlite_c; sqlite_c = a_str_concat(runtime_dir, a_string("/sqlite3.c")); a_release(__old); }
    { AValue __old = miniz_c; miniz_c = a_str_concat(runtime_dir, a_string("/miniz.c")); a_release(__old); }
    { AValue __old = stb_impl_c; stb_impl_c = a_str_concat(runtime_dir, a_string("/stb_impl.c")); a_release(__old); }
    { AValue __old = include_flag; include_flag = a_str_concat(a_string("-I "), runtime_dir); a_release(__old); }
    { AValue __old = cmd; cmd = a_str_join(a_array_new(11, a_string("gcc"), c_path, runtime_c, sqlite_c, miniz_c, stb_impl_c, a_string("-o"), out_path, include_flag, fn__gcc_flags(), fn__sqlite_flags()), a_string(" ")); a_release(__old); }
    { AValue __old = result; result = a_exec(cmd); a_release(__old); }
    if (a_truthy(a_neq(a_array_get(result, a_string("code")), a_int(0)))) {
        a_eprintln(fn_cli_red(a_string("gcc compilation failed:")));
        if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stdout"))), a_int(0)))) {
            a_eprintln(a_array_get(result, a_string("stdout")));
        }
        if (a_truthy(a_gt(a_len(a_array_get(result, a_string("stderr"))), a_int(0)))) {
            a_eprintln(a_array_get(result, a_string("stderr")));
        }
        (exit((int)a_int(1).ival), a_void());
    }
__fn_cleanup:
    a_release(runtime_c);
    a_release(sqlite_c);
    a_release(miniz_c);
    a_release(stb_impl_c);
    a_release(include_flag);
    a_release(cmd);
    a_release(result);
    return __ret;
}

AValue fn__tmp_path(AValue suffix) {
    AValue ts = {0};
    AValue __ret = a_void();
    { AValue __old = ts; ts = a_to_str(a_time_now()); a_release(__old); }
    __ret = a_str_concat(a_string("/tmp/a_cli_"), a_str_concat(ts, suffix)); goto __fn_cleanup;
__fn_cleanup:
    a_release(ts);
    return __ret;
}

AValue fn_cmd_cc(AValue source_path, AValue out_file) {
    AValue c_code = {0};
    AValue __ret = a_void();
    { AValue __old = c_code; c_code = fn__generate_c(source_path); a_release(__old); }
    if (a_truthy(a_gt(a_len(out_file), a_int(0)))) {
        a_io_write_file(out_file, c_code);
    } else {
        a_print(c_code);
    }
__fn_cleanup:
    a_release(c_code);
    return __ret;
}

AValue fn__codegen_subprocess(AValue source_path, AValue c_path) {
    AValue self = {0}, cmd = {0}, attempts = {0}, max_attempts = {0}, result = {0};
    AValue __ret = a_void();
    { AValue __old = self; self = a_argv0(); a_release(__old); }
    { AValue __old = cmd; cmd = a_str_join(a_array_new(5, self, a_string("cc"), source_path, a_string("-o"), c_path), a_string(" ")); a_release(__old); }
    { AValue __old = attempts; attempts = a_int(0); a_release(__old); }
    { AValue __old = max_attempts; max_attempts = a_int(5); a_release(__old); }
    while (a_truthy(a_lt(attempts, max_attempts))) {
        { AValue __old = result; result = a_exec(cmd); a_release(__old); }
        if (a_truthy(a_eq(a_array_get(result, a_string("code")), a_int(0)))) {
            __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
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
    return __ret;
}

AValue fn_cmd_build(AValue source_path, AValue out_path) {
    AValue runtime_dir = {0}, c_path = {0};
    AValue __ret = a_void();
    { AValue __old = runtime_dir; runtime_dir = fn__find_runtime_dir(); a_release(__old); }
    { AValue __old = c_path; c_path = fn__tmp_path(a_string(".c")); a_release(__old); }
    fn__codegen_subprocess(source_path, c_path);
    fn__gcc(c_path, out_path, runtime_dir);
    a_fs_rm(c_path);
    a_eprintln(a_str_concat(fn_cli_green(a_string("built")), a_str_concat(a_string(" "), out_path)));
__fn_cleanup:
    a_release(runtime_dir);
    a_release(c_path);
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
    return __ret;
}

AValue fn__store_cache(AValue source_path, AValue bin_path) {
    AValue source = {0}, h = {0}, cache_path = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cmd_run(AValue source_path, AValue extra_args) {
    AValue cached = {0}, run_cmd = {0}, result = {0}, runtime_dir = {0}, c_path = {0}, bin_path = {0};
    AValue __ret = a_void();
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
    return __ret;
}

AValue fn_cmd_test(AValue test_dir) {
    AValue runtime_dir = {0}, self = {0}, entries = {0}, test_files = {0}, name = {0}, passed = {0}, failed = {0}, flags = {0}, sflags = {0}, c_path = {0}, bin_path = {0}, c_code = {0}, gcc_result = {0}, run_result = {0};
    AValue __ret = a_void();
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
    { AValue __old = flags; flags = fn__gcc_flags(); a_release(__old); }
    { AValue __old = sflags; sflags = fn__sqlite_flags(); a_release(__old); }
    {
        AValue __iter_arr = a_iterable(test_files);
        for (int __fi = 0; __fi < a_ilen(__iter_arr); __fi++) {
            AValue src = {0}, name = {0}, c_path = {0}, bin_path = {0}, c_code = {0}, gcc_result = {0}, run_result = {0};
            src = a_array_get(__iter_arr, a_int(__fi));
            { AValue __old = name; name = fn_path_basename(src); a_release(__old); }
            { AValue __old = c_path; c_path = fn__tmp_path(a_string(".c")); a_release(__old); }
            { AValue __old = bin_path; bin_path = fn__tmp_path(a_string("")); a_release(__old); }
            { AValue __old = c_code; c_code = fn__generate_c(src); a_release(__old); }
            a_io_write_file(c_path, c_code);
            { AValue __old = gcc_result; gcc_result = a_exec(a_str_join(a_array_new(11, a_string("gcc"), c_path, a_str_concat(runtime_dir, a_string("/runtime.c")), a_str_concat(runtime_dir, a_string("/sqlite3.c")), a_str_concat(runtime_dir, a_string("/miniz.c")), a_str_concat(runtime_dir, a_string("/stb_impl.c")), a_string("-o"), bin_path, a_str_concat(a_string("-I "), runtime_dir), flags, sflags), a_string(" "))); a_release(__old); }
            a_fs_rm(c_path);
            if (a_truthy(a_neq(a_array_get(gcc_result, a_string("code")), a_int(0)))) {
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
            a_release(gcc_result);
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
    a_release(flags);
    a_release(sflags);
    a_release(c_path);
    a_release(bin_path);
    a_release(c_code);
    a_release(gcc_result);
    a_release(run_result);
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

AValue fn_cmd_eval(AValue expr, AValue extra_args) {
    AValue wrapped = {0}, tmp = {0};
    AValue __ret = a_void();
    { AValue __old = wrapped; wrapped = a_str_concat(a_string("fn main() {\n  println("), a_str_concat(expr, a_string(")\n}"))); a_release(__old); }
    { AValue __old = tmp; tmp = fn__tmp_path(a_string("_eval.a")); a_release(__old); }
    a_io_write_file(tmp, wrapped);
    fn_cmd_run(tmp, extra_args);
    a_fs_rm(tmp);
__fn_cleanup:
    a_release(wrapped);
    a_release(tmp);
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

AValue fn__usage(void) {
    AValue __ret = a_void();
    a_eprintln(a_string("a -- the a language native toolchain"));
    a_eprintln(a_string(""));
    a_eprintln(a_string("usage:"));
    a_eprintln(a_string("  a run <file.a> [args...]    compile and run (cached)"));
    a_eprintln(a_string("  a build <file.a> [-o out]   compile to native binary"));
    a_eprintln(a_string("  a eval <expr>              evaluate an expression"));
    a_eprintln(a_string("  a cc <file.a>              emit C to stdout"));
    a_eprintln(a_string("  a test <dir/>              run test_*.a files in directory"));
    a_eprintln(a_string("  a lsp                      start language server (JSON-RPC over stdio)"));
    a_eprintln(a_string("  a cache clean              clear the compilation cache"));
    a_eprintln(a_string(""));
    a_eprintln(a_string("  a run and a build expect c_runtime/ in the current directory."));
    a_eprintln(a_string("  shebang: #!/usr/bin/env a run"));
__fn_cleanup:
    return __ret;
}

AValue fn_main(void) {
    AValue argv = {0}, subcmd = {0}, cc_out = {0}, source = {0}, out = {0}, i = {0}, extra = {0};
    AValue __ret = a_void();
    { AValue __old = argv; argv = a_args(); a_release(__old); }
    if (a_truthy(a_eq(a_len(argv), a_int(0)))) {
        fn__usage();
        (exit((int)a_int(1).ival), a_void());
    }
    { AValue __old = subcmd; subcmd = a_array_get(argv, a_int(0)); a_release(__old); }
    if (a_truthy(a_eq(subcmd, a_string("cc")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("cc requires a source file"));
        }
        { AValue __old = cc_out; cc_out = a_string(""); a_release(__old); }
        if (a_truthy(a_and(a_gteq(a_len(argv), a_int(4)), a_eq(a_array_get(argv, a_int(2)), a_string("-o"))))) {
            { AValue __old = cc_out; cc_out = a_array_get(argv, a_int(3)); a_release(__old); }
        }
        fn_cmd_cc(a_array_get(argv, a_int(1)), cc_out);
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("build")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("build requires a source file"));
        }
        { AValue __old = source; source = a_array_get(argv, a_int(1)); a_release(__old); }
        { AValue __old = out; out = fn_path_stem(source); a_release(__old); }
        { AValue __old = i; i = a_int(2); a_release(__old); }
        while (a_truthy(a_lt(i, a_len(argv)))) {
            if (a_truthy(a_and(a_eq(a_array_get(argv, i), a_string("-o")), a_lt(a_add(i, a_int(1)), a_len(argv))))) {
                { AValue __old = out; out = a_array_get(argv, a_add(i, a_int(1))); a_release(__old); }
                { AValue __old = i; i = a_add(i, a_int(2)); a_release(__old); }
            } else {
                { AValue __old = i; i = a_add(i, a_int(1)); a_release(__old); }
            }
        }
        fn_cmd_build(source, out);
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
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
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("test")))) {
        if (a_truthy(a_lt(a_len(argv), a_int(2)))) {
            fn__die(a_string("test requires a directory"));
        }
        fn_cmd_test(a_array_get(argv, a_int(1)));
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
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
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("cache")))) {
        if (a_truthy(a_and(a_gteq(a_len(argv), a_int(2)), a_eq(a_array_get(argv, a_int(1)), a_string("clean"))))) {
            fn_cmd_cache_clean();
            __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
        }
        fn__die(a_string("usage: a cache clean"));
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
    }
    if (a_truthy(a_eq(subcmd, a_string("lsp")))) {
        fn_cmd_lsp();
        __ret = /* unhandled expr: ParseError */a_void(); goto __fn_cleanup;
    }
    fn__die(a_str_concat(a_string("unknown command: "), subcmd));
__fn_cleanup:
    a_release(argv);
    a_release(subcmd);
    a_release(cc_out);
    a_release(source);
    a_release(out);
    a_release(i);
    a_release(extra);
    return __ret;
}

int main(int argc, char** argv) {
    g_argc = argc; g_argv = argv;
    fn_main();
    return 0;
}
