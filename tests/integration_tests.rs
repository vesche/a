use a_lang::lexer::Lexer;
use a_lang::parser::Parser;
use a_lang::checker::Checker;
use a_lang::compiler::Compiler;
use a_lang::errors::AError;
use a_lang::interpreter::Interpreter;
use a_lang::vm::VM;
use a_lang::formatter;

fn parse(src: &str) -> a_lang::ast::Program {
    let mut lexer = Lexer::new(src);
    let tokens = lexer.tokenize().expect("lex failed");
    let mut parser = Parser::new(tokens);
    parser.parse_program().expect("parse failed")
}

fn run(src: &str) -> a_lang::interpreter::Value {
    let program = parse(src);
    let mut interp = Interpreter::new();
    interp.run(&program).expect("runtime error")
}

fn run_vm(src: &str) -> a_lang::interpreter::Value {
    let program = parse(src);
    let mut compiler = Compiler::new();
    let compiled = compiler.compile_program(&program);
    let mut vm = VM::new(compiled);
    vm.run().expect("vm runtime error")
}

fn run_vm_err(src: &str) -> AError {
    let program = parse(src);
    let mut compiler = Compiler::new();
    let compiled = compiler.compile_program(&program);
    let mut vm = VM::new(compiled);
    vm.run().expect_err("expected runtime error")
}

// --- Lexer tests ---

#[test]
fn lex_basic_tokens() {
    let mut lexer = Lexer::new("fn main() -> void { }");
    let tokens = lexer.tokenize().unwrap();
    let kinds: Vec<_> = tokens.iter().map(|t| format!("{}", t.kind)).collect();
    assert_eq!(kinds, vec!["fn", "main", "(", ")", "->", "void", "{", "}", "EOF"]);
}

#[test]
fn lex_numbers() {
    let mut lexer = Lexer::new("42 3.14");
    let tokens = lexer.tokenize().unwrap();
    assert!(matches!(tokens[0].kind, a_lang::tokens::TokenKind::IntLit(42)));
    assert!(matches!(tokens[1].kind, a_lang::tokens::TokenKind::FloatLit(f) if (f - 3.14).abs() < 0.001));
}

#[test]
fn lex_string() {
    let mut lexer = Lexer::new("\"hello\\nworld\"");
    let tokens = lexer.tokenize().unwrap();
    if let a_lang::tokens::TokenKind::StringLit(s) = &tokens[0].kind {
        assert_eq!(s, "hello\nworld");
    } else {
        panic!("expected string literal");
    }
}

#[test]
fn lex_operators() {
    let mut lexer = Lexer::new("== != <= >= && || -> =>");
    let tokens = lexer.tokenize().unwrap();
    let kinds: Vec<_> = tokens.iter().map(|t| format!("{}", t.kind)).collect();
    assert_eq!(kinds, vec!["==", "!=", "<=", ">=", "&&", "||", "->", "=>", "EOF"]);
}

#[test]
fn lex_comments_skipped() {
    let mut lexer = Lexer::new("; this is a comment\nfn");
    let tokens = lexer.tokenize().unwrap();
    let kinds: Vec<_> = tokens.iter()
        .filter(|t| !matches!(t.kind, a_lang::tokens::TokenKind::Newline))
        .map(|t| format!("{}", t.kind))
        .collect();
    assert_eq!(kinds, vec!["fn", "EOF"]);
}

#[test]
fn lex_error_on_invalid_char() {
    let mut lexer = Lexer::new("@");
    assert!(lexer.tokenize().is_err());
}

// --- Parser tests ---

#[test]
fn parse_empty_fn() {
    let program = parse("fn main() -> void { }");
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_fn_with_params() {
    let program = parse("fn add(a: i64, b: i64) -> i64 { ret a + b }");
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_type_decl_record() {
    let program = parse("ty Point = { x: f64, y: f64 }");
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_type_decl_sum() {
    let program = parse("ty Option<T> = Some(T) | None");
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_effects_clause() {
    let program = parse("fn f() -> void effects [io, net] { }");
    if let a_lang::ast::TopLevelKind::FnDecl(f) = &program.items[0].kind {
        assert_eq!(f.effects, vec!["io", "net"]);
    } else {
        panic!("expected fn decl");
    }
}

#[test]
fn parse_pre_post() {
    let program = parse("fn f(x: i64) -> i64\n  pre { x >= 0 }\n  post { ret >= 0 }\n{ ret x }");
    if let a_lang::ast::TopLevelKind::FnDecl(f) = &program.items[0].kind {
        assert!(f.pre.is_some());
        assert!(f.post.is_some());
    } else {
        panic!("expected fn decl");
    }
}

#[test]
fn parse_let_stmt() {
    let program = parse("fn f() -> void { let x: i64 = 42 }");
    if let a_lang::ast::TopLevelKind::FnDecl(f) = &program.items[0].kind {
        assert_eq!(f.body.stmts.len(), 1);
    } else {
        panic!("expected fn decl");
    }
}

#[test]
fn parse_if_else() {
    let program = parse("fn f(x: bool) -> i64 {\n  if x { ret 1 } else { ret 0 }\n}");
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_match() {
    let src = "fn f(x: i64) -> str {\n  match x {\n    0 => \"zero\"\n    _ => \"other\"\n  }\n}";
    let program = parse(src);
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_for_loop() {
    let src = "fn f() -> void { for i: i64 in [1, 2, 3] { println(to_str(i)) } }";
    let program = parse(src);
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_module() {
    let src = "mod math {\n  fn add(a: i64, b: i64) -> i64 { ret a + b }\n}";
    let program = parse(src);
    assert_eq!(program.items.len(), 1);
}

#[test]
fn parse_array_literal() {
    let program = parse("fn f() -> [i64] { ret [1, 2, 3] }");
    assert_eq!(program.items.len(), 1);
}

// --- Type checker tests ---

#[test]
fn check_valid_program() {
    let program = parse("fn main() -> void effects [io] { println(\"ok\") }");
    let mut checker = Checker::new();
    assert!(checker.check_program(&program).is_ok());
}

#[test]
fn check_effect_violation() {
    let program = parse("fn f() -> void effects [pure] { println(\"oops\") }");
    let mut checker = Checker::new();
    let result = checker.check_program(&program);
    assert!(result.is_err());
}

#[test]
fn check_type_mismatch() {
    let program = parse("fn f() -> void { let x: i64 = \"not a number\" }");
    let mut checker = Checker::new();
    let result = checker.check_program(&program);
    assert!(result.is_err());
}

#[test]
fn check_undefined_variable() {
    let program = parse("fn f() -> void { println(to_str(undefined_var)) }");
    let mut checker = Checker::new();
    let result = checker.check_program(&program);
    assert!(result.is_err());
}

// --- Interpreter tests ---

#[test]
fn interp_arithmetic() {
    let val = run("fn main() -> i64 { ret 2 + 3 * 4 }");
    if let a_lang::interpreter::Value::Int(n) = val {
        assert_eq!(n, 14);
    } else {
        panic!("expected int");
    }
}

#[test]
fn interp_function_call() {
    let val = run("fn double(x: i64) -> i64 { ret x * 2 }\nfn main() -> i64 { ret double(21) }");
    if let a_lang::interpreter::Value::Int(n) = val {
        assert_eq!(n, 42);
    } else {
        panic!("expected int");
    }
}

#[test]
fn interp_recursion() {
    let val = run("fn fib(n: i64) -> i64 {\n  if n <= 1 { ret n }\n  ret fib(n - 1) + fib(n - 2)\n}\nfn main() -> i64 { ret fib(10) }");
    if let a_lang::interpreter::Value::Int(n) = val {
        assert_eq!(n, 55);
    } else {
        panic!("expected int");
    }
}

#[test]
fn interp_string_concat() {
    let val = run("fn main() -> str { ret \"hello\" + \" world\" }");
    if let a_lang::interpreter::Value::String(s) = val {
        assert_eq!(s.as_str(), "hello world");
    } else {
        panic!("expected string");
    }
}

#[test]
fn interp_boolean_logic() {
    let val = run("fn main() -> bool { ret true && !false }");
    if let a_lang::interpreter::Value::Bool(b) = val {
        assert!(b);
    } else {
        panic!("expected bool");
    }
}

#[test]
fn interp_if_expression() {
    let val = run("fn main() -> i64 {\n  if true { ret 1 } else { ret 0 }\n}");
    if let a_lang::interpreter::Value::Int(n) = val {
        assert_eq!(n, 1);
    } else {
        panic!("expected int");
    }
}

#[test]
fn interp_for_loop() {
    let val = run("fn main() -> i64 {\n  let mut sum: i64 = 0\n  for i: i64 in [1, 2, 3, 4, 5] {\n    sum = sum + i\n  }\n  ret sum\n}");
    if let a_lang::interpreter::Value::Int(n) = val {
        assert_eq!(n, 15);
    } else {
        panic!("expected int");
    }
}

#[test]
fn interp_match() {
    let val = run("fn f(x: i64) -> str {\n  match x {\n    1 => \"one\"\n    2 => \"two\"\n    _ => \"other\"\n  }\n}\nfn main() -> str { ret f(2) }");
    if let a_lang::interpreter::Value::String(s) = val {
        assert_eq!(s.as_str(), "two");
    } else {
        panic!("expected string");
    }
}

#[test]
fn interp_nested_calls() {
    let src = "fn add(a: i64, b: i64) -> i64 { ret a + b }\nfn mul(a: i64, b: i64) -> i64 { ret a * b }\nfn main() -> i64 { ret add(mul(2, 3), mul(4, 5)) }";
    let val = run(src);
    if let a_lang::interpreter::Value::Int(n) = val {
        assert_eq!(n, 26);
    } else {
        panic!("expected int");
    }
}

// --- Formatter tests ---

#[test]
fn fmt_round_trip() {
    let src = "fn add(a: i64, b: i64) -> i64\n  effects [pure]\n{\n  ret (a + b)\n}\n";
    let program = parse(src);
    let formatted = formatter::format_program(&program);
    let re_parsed = parse(&formatted);
    let re_formatted = formatter::format_program(&re_parsed);
    assert_eq!(formatted, re_formatted);
}

// --- AST JSON serialization test ---

#[test]
fn ast_serializes_to_json() {
    let program = parse("fn main() -> void { }");
    let json = serde_json::to_string(&program);
    assert!(json.is_ok());
    let json_str = json.unwrap();
    assert!(json_str.contains("\"FnDecl\""));
    assert!(json_str.contains("\"main\""));
}

// --- Closure tests ---

#[test]
fn interp_lambda_basic() {
    let val = run("fn main() -> i64 {\n  let f: fn(i64) -> i64 = fn(x: i64) => x * 2\n  ret f(21)\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 42); }
    else { panic!("expected int"); }
}

#[test]
fn interp_lambda_closure_capture() {
    let val = run("fn main() -> i64 {\n  let factor: i64 = 10\n  let f: fn(i64) -> i64 = fn(x: i64) => x * factor\n  ret f(5)\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 50); }
    else { panic!("expected int"); }
}

#[test]
fn interp_map_builtin() {
    let val = run("fn main() -> [i64] { ret map([1, 2, 3], fn(x: i64) => x * 10) }");
    if let a_lang::interpreter::Value::Array(arr) = val {
        assert_eq!(arr.len(), 3);
        if let a_lang::interpreter::Value::Int(n) = &arr[0] { assert_eq!(*n, 10); }
        if let a_lang::interpreter::Value::Int(n) = &arr[2] { assert_eq!(*n, 30); }
    } else { panic!("expected array"); }
}

#[test]
fn interp_filter_builtin() {
    let val = run("fn main() -> [i64] { ret filter([1, 2, 3, 4, 5], fn(x: i64) => x > 3) }");
    if let a_lang::interpreter::Value::Array(arr) = val {
        assert_eq!(arr.len(), 2);
    } else { panic!("expected array"); }
}

#[test]
fn interp_reduce_builtin() {
    let val = run("fn main() -> i64 { ret reduce([1, 2, 3, 4, 5], 0, fn(acc: i64, x: i64) => acc + x) }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 15); }
    else { panic!("expected int"); }
}

// --- Pipe operator tests ---

#[test]
fn interp_pipe_basic() {
    let val = run("fn double(x: i64) -> i64 { ret x * 2 }\nfn main() -> i64 { ret 5 |> double }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 10); }
    else { panic!("expected int"); }
}

#[test]
fn interp_pipe_chain() {
    let val = run("fn main() -> [i64] {\n  ret [1, 2, 3, 4]\n    |> filter(fn(x: i64) => x > 2)\n    |> map(fn(x: i64) => x * 10)\n}");
    if let a_lang::interpreter::Value::Array(arr) = val {
        assert_eq!(arr.len(), 2);
        if let a_lang::interpreter::Value::Int(n) = &arr[0] { assert_eq!(*n, 30); }
        if let a_lang::interpreter::Value::Int(n) = &arr[1] { assert_eq!(*n, 40); }
    } else { panic!("expected array"); }
}

#[test]
fn interp_pipe_to_lambda() {
    let val = run("fn main() -> i64 { ret 5 |> fn(x: i64) => x + 1 }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 6); }
    else { panic!("expected int"); }
}

// --- String interpolation tests ---

#[test]
fn interp_string_interpolation_basic() {
    let val = run("fn main() -> str {\n  let x: i64 = 42\n  ret \"{to_str(x)}\"\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "42"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_string_interpolation_multi() {
    let val = run("fn main() -> str {\n  let a: str = \"hello\"\n  let b: str = \"world\"\n  ret \"{a} {b}\"\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "hello world"); }
    else { panic!("expected string"); }
}

// --- Map data structure tests ---

#[test]
fn interp_map_literal() {
    let val = run("fn main() -> i64 {\n  let m: #{str: i64} = #{\"x\": 10, \"y\": 20}\n  ret map.get(m, \"y\")\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 20); }
    else { panic!("expected int"); }
}

#[test]
fn interp_map_set() {
    let val = run("fn main() -> i64 {\n  let m: #{str: i64} = #{\"a\": 1}\n  let m2: #{str: i64} = map.set(m, \"b\", 2)\n  ret map.get(m2, \"b\")\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 2); }
    else { panic!("expected int"); }
}

#[test]
fn interp_map_keys() {
    let val = run("fn main() -> i64 {\n  let m: #{str: i64} = #{\"a\": 1, \"b\": 2, \"c\": 3}\n  ret len(map.keys(m))\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

#[test]
fn interp_map_has() {
    let val = run("fn main() -> bool {\n  let m: #{str: i64} = #{\"key\": 42}\n  ret map.has(m, \"key\")\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

// --- String builtins ---

#[test]
fn interp_str_contains() {
    let val = run("fn main() -> bool { ret str.contains(\"hello world\", \"world\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_str_starts_with() {
    let val = run("fn main() -> bool { ret str.starts_with(\"hello\", \"hel\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_str_ends_with() {
    let val = run("fn main() -> bool { ret str.ends_with(\"hello\", \"llo\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_str_replace() {
    let val = run("fn main() -> str { ret str.replace(\"foo bar foo\", \"foo\", \"baz\") }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "baz bar baz"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_str_trim() {
    let val = run("fn main() -> str { ret str.trim(\"  hello  \") }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "hello"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_str_upper_lower() {
    let v1 = run("fn main() -> str { ret str.upper(\"hello\") }");
    if let a_lang::interpreter::Value::String(s) = v1 { assert_eq!(s.as_str(), "HELLO"); }
    else { panic!("expected string"); }

    let v2 = run("fn main() -> str { ret str.lower(\"WORLD\") }");
    if let a_lang::interpreter::Value::String(s) = v2 { assert_eq!(s.as_str(), "world"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_str_join() {
    let val = run("fn main() -> str { ret str.join([\"a\", \"b\", \"c\"], \"-\") }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "a-b-c"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_str_chars() {
    let val = run("fn main() -> i64 { ret len(str.chars(\"abc\")) }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

#[test]
fn interp_str_slice() {
    let val = run("fn main() -> str { ret str.slice(\"hello world\", 6, 11) }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "world"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_str_lines() {
    let val = run("fn main() -> i64 { ret len(str.lines(\"a\\nb\\nc\")) }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

// --- exec ---

#[test]
fn interp_exec_echo() {
    let program = parse("ty CmdResult = {stdout: str, stderr: str, code: i64}\nfn main() -> str effects [io] {\n  let r: CmdResult = exec(\"echo hi\")\n  ret r.stdout\n}");
    let mut interp = Interpreter::new();
    let val = interp.run(&program).expect("runtime error");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.trim(), "hi"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_exec_exit_code() {
    let program = parse("ty CmdResult = {stdout: str, stderr: str, code: i64}\nfn main() -> i64 effects [io] {\n  let r: CmdResult = exec(\"exit 42\")\n  ret r.code\n}");
    let mut interp = Interpreter::new();
    let val = interp.run(&program).expect("runtime error");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 42); }
    else { panic!("expected int"); }
}

// --- JSON ---

#[test]
fn interp_json_parse_array() {
    let val = run("fn main() -> i64 {\n  let arr: [i64] = json.parse(\"[1, 2, 3]\")\n  ret len(arr)\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

#[test]
fn interp_json_stringify() {
    let val = run("fn main() -> str { ret json.stringify(42) }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "42"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_json_roundtrip() {
    let src = r#"fn main() -> str {
  let m: #{str: i64} = #{"x": 1, "y": 2}
  let s: str = json.stringify(m)
  let m2: #{str: i64} = json.parse(s)
  ret to_str(map.get(m2, "x"))
}"#;
    let val = run(src);
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "1"); }
    else { panic!("expected string"); }
}

// --- env ---

#[test]
fn interp_env_get_set() {
    let val = run("fn main() -> str effects [io] {\n  env.set(\"A_LANG_TEST_VAR\", \"hello\")\n  ret env.get(\"A_LANG_TEST_VAR\")\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "hello"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_env_get_missing() {
    let val = run("fn main() -> str effects [io] { ret type_of(env.get(\"A_LANG_NONEXISTENT_VAR_XYZ\")) }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "void"); }
    else { panic!("expected string"); }
}

// --- type_of / int / float ---

#[test]
fn interp_type_of() {
    let val = run("fn main() -> str { ret type_of(42) }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "int"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_int_cast() {
    let val = run("fn main() -> i64 { ret int(\"123\") }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 123); }
    else { panic!("expected int"); }
}

#[test]
fn interp_float_cast() {
    let val = run("fn main() -> f64 { ret float(\"3.14\") }");
    if let a_lang::interpreter::Value::Float(f) = val { assert!((f - 3.14).abs() < 0.001); }
    else { panic!("expected float"); }
}

// --- Filesystem tests ---

#[test]
fn interp_fs_exists() {
    let val = run("fn main() -> bool effects [io] { ret fs.exists(\"Cargo.toml\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_fs_exists_missing() {
    let val = run("fn main() -> bool effects [io] { ret fs.exists(\"nonexistent_file_xyz.txt\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(!b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_fs_is_dir() {
    let val = run("fn main() -> bool effects [io] { ret fs.is_dir(\"src\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_fs_is_file() {
    let val = run("fn main() -> bool effects [io] { ret fs.is_file(\"Cargo.toml\") }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_fs_cwd() {
    let val = run("fn main() -> str effects [io] { ret type_of(fs.cwd()) }");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "str"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_fs_mkdir_and_rm() {
    let val = run("fn main() -> bool effects [io] {\n  fs.mkdir(\"_test_tmp_dir\")\n  let exists: bool = fs.exists(\"_test_tmp_dir\")\n  fs.rm(\"_test_tmp_dir\")\n  ret exists\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_fs_write_read_rm() {
    let val = run("fn main() -> str effects [io] {\n  io.write_file(\"_test_tmp.txt\", \"hello a\")\n  let content: str = io.read_file(\"_test_tmp.txt\")\n  fs.rm(\"_test_tmp.txt\")\n  ret content\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "hello a"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_fs_ls() {
    let val = run("fn main() -> bool effects [io] {\n  let entries: [str] = fs.ls(\"src\")\n  ret len(entries) > 0\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_fs_cp_and_mv() {
    let val = run("fn main() -> str effects [io] {\n  io.write_file(\"_test_cp_src.txt\", \"copy me\")\n  fs.cp(\"_test_cp_src.txt\", \"_test_cp_dst.txt\")\n  let content: str = io.read_file(\"_test_cp_dst.txt\")\n  fs.rm(\"_test_cp_src.txt\")\n  fs.rm(\"_test_cp_dst.txt\")\n  ret content\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "copy me"); }
    else { panic!("expected string"); }
}

// --- HTTP tests (use httpbin-style safe endpoint) ---

#[test]
fn interp_http_get_returns_record() {
    let val = run("fn main() -> bool effects [io] {\n  let r: str = type_of(http.get(\"https://httpbin.org/get\"))\n  ret r == \"record\"\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_http_get_status() {
    let program = parse("ty HttpResp = {status: i64, body: str, headers: #{str: str}}\nfn main() -> i64 effects [io] {\n  let r: HttpResp = http.get(\"https://httpbin.org/status/200\")\n  ret r.status\n}");
    let mut interp = Interpreter::new();
    let val = interp.run(&program).expect("runtime error");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 200); }
    else { panic!("expected int, got {:?}", val); }
}

// --- Error handling: Ok / Err constructors ---

#[test]
fn interp_ok_constructor() {
    let val = run("fn main() -> bool { ret is_ok(Ok(42)) }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_err_constructor() {
    let val = run("fn main() -> bool { ret is_err(Err(\"oops\")) }");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_unwrap_ok() {
    let val = run("fn main() -> i64 { ret unwrap(Ok(99)) }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 99); }
    else { panic!("expected int"); }
}

#[test]
fn interp_unwrap_or_on_err() {
    let val = run("fn main() -> i64 { ret unwrap_or(Err(\"fail\"), 0) }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 0); }
    else { panic!("expected int"); }
}

// --- Error handling: ? operator ---

#[test]
fn interp_question_on_ok() {
    let val = run("fn main() -> i64 { ret Ok(42)? }");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 42); }
    else { panic!("expected int"); }
}

#[test]
fn interp_question_on_err_crashes() {
    let program = parse("fn main() -> i64 { ret Err(\"boom\")? }");
    let mut interp = Interpreter::new();
    let result = interp.run(&program);
    assert!(result.is_err());
}

// --- Error handling: try { } block ---

#[test]
fn interp_try_block_catches_error() {
    let val = run("fn main() -> bool {\n  let r: str = try {\n    let x: i64 = Err(\"bad\")?\n    ret x\n  }\n  ret is_err(r)\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_try_block_ok_path() {
    let val = run("fn main() -> i64 {\n  let r: str = try {\n    Ok(7)?\n  }\n  ret unwrap(r)\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 7); }
    else { panic!("expected int, got {:?}", val); }
}

#[test]
fn interp_try_block_catches_runtime_error() {
    let val = run("fn main() -> bool {\n  let r: str = try {\n    let x: [i64] = [1, 2]\n    let y: i64 = x[99]\n  }\n  ret is_err(r)\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

// --- Error handling: match on Result ---

#[test]
fn interp_match_ok() {
    let val = run("fn main() -> i64 {\n  let r: i64 = Ok(10)\n  let out: i64 = 0\n  match r {\n    Ok(v) => {\n      out = v + 1\n    }\n    Err(e) => {\n      out = 0\n    }\n  }\n  ret out\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 11); }
    else { panic!("expected int"); }
}

#[test]
fn interp_match_err() {
    let val = run("fn main() -> str {\n  let r: str = Err(\"bad\")\n  let out: str = \"\"\n  match r {\n    Ok(v) => {\n      out = \"ok\"\n    }\n    Err(e) => {\n      out = e\n    }\n  }\n  ret out\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "bad"); }
    else { panic!("expected string"); }
}

// --- Error handling: try with file operations ---

#[test]
fn interp_try_file_not_found() {
    let val = run("fn main() -> bool effects [io] {\n  let r: str = try {\n    let content: str = io.read_file(\"_nonexistent_file.txt\")?\n  }\n  ret is_err(r)\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

// --- v0.7: Self-hosting primitives ---

#[test]
fn interp_while_basic() {
    let val = run("fn main() -> i64 {\n  let mut i: i64 = 0\n  while i < 5 {\n    i = i + 1\n  }\n  ret i\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 5); }
    else { panic!("expected int"); }
}

#[test]
fn interp_while_sum() {
    let val = run("fn main() -> i64 {\n  let mut sum: i64 = 0\n  let mut i: i64 = 1\n  while i <= 10 {\n    sum = sum + i\n    i = i + 1\n  }\n  ret sum\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 55); }
    else { panic!("expected int"); }
}

#[test]
fn interp_while_false() {
    let val = run("fn main() -> i64 {\n  let mut x: i64 = 42\n  while false {\n    x = 0\n  }\n  ret x\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 42); }
    else { panic!("expected int"); }
}

#[test]
fn interp_break_in_while() {
    let val = run("fn main() -> i64 {\n  let mut i: i64 = 0\n  while true {\n    if i == 3 {\n      break\n    }\n    i = i + 1\n  }\n  ret i\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

#[test]
fn interp_continue_in_while() {
    let val = run("fn main() -> i64 {\n  let mut sum: i64 = 0\n  let mut i: i64 = 0\n  while i < 10 {\n    i = i + 1\n    if i % 2 == 0 {\n      continue\n    }\n    sum = sum + i\n  }\n  ret sum\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 25); }
    else { panic!("expected int"); }
}

#[test]
fn interp_break_in_for() {
    let val = run("fn main() -> i64 {\n  let mut last: i64 = 0\n  for x: i64 in [1, 2, 3, 4, 5] {\n    if x == 4 {\n      break\n    }\n    last = x\n  }\n  ret last\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

#[test]
fn interp_continue_in_for() {
    let val = run("fn main() -> i64 {\n  let mut sum: i64 = 0\n  for x: i64 in [1, 2, 3, 4, 5] {\n    if x == 3 {\n      continue\n    }\n    sum = sum + x\n  }\n  ret sum\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 12); }
    else { panic!("expected int"); }
}

#[test]
fn interp_variant_constructor() {
    let val = run("ty Token = Num(i64) | Word(str)\nfn main() -> str {\n  let t: str = Word(\"hello\")\n  match t {\n    Word(s) => {\n      ret s\n    }\n    Num(n) => {\n      ret \"num\"\n    }\n  }\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "hello"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_variant_match_num() {
    let val = run("ty Tok = Num(i64) | Id(str)\nfn main() -> i64 {\n  let t: str = Num(42)\n  match t {\n    Num(n) => {\n      ret n\n    }\n    Id(s) => {\n      ret 0\n    }\n  }\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 42); }
    else { panic!("expected int"); }
}

#[test]
fn interp_char_code() {
    let val = run("fn main() -> i64 {\n  ret char_code(\"A\")\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 65); }
    else { panic!("expected int"); }
}

#[test]
fn interp_from_code() {
    let val = run("fn main() -> str {\n  ret from_code(65)\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "A"); }
    else { panic!("expected string"); }
}

#[test]
fn interp_is_alpha() {
    let val = run("fn main() -> bool {\n  ret is_alpha(\"a\")\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_is_digit() {
    let val = run("fn main() -> bool {\n  ret is_digit(\"5\")\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_is_alnum() {
    let val = run("fn main() -> bool {\n  ret is_alnum(\"a3_b\")\n}");
    if let a_lang::interpreter::Value::Bool(b) = val { assert!(b); }
    else { panic!("expected bool"); }
}

#[test]
fn interp_push_builtin() {
    let val = run("fn main() -> i64 {\n  let a: [i64] = [1, 2]\n  let b: [i64] = push(a, 3)\n  ret len(b)\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 3); }
    else { panic!("expected int"); }
}

#[test]
fn interp_slice_builtin() {
    let val = run("fn main() -> i64 {\n  let a: [i64] = [10, 20, 30, 40, 50]\n  let b: [i64] = slice(a, 1, 3)\n  ret len(b)\n}");
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 2); }
    else { panic!("expected int"); }
}

#[test]
fn interp_while_with_string_building() {
    let val = run("fn main() -> str {\n  let chars: [str] = str.chars(\"hello\")\n  let mut result: str = \"\"\n  let mut i: i64 = 0\n  while i < len(chars) {\n    let c: str = chars[i]\n    if is_alpha(c) {\n      result = str.concat(result, str.upper(c))\n    }\n    i = i + 1\n  }\n  ret result\n}");
    if let a_lang::interpreter::Value::String(s) = val { assert_eq!(s.as_str(), "HELLO"); }
    else { panic!("expected string"); }
}

#[test]
fn parse_while_stmt() {
    let program = parse("fn main() -> void {\n  while true {\n    break\n  }\n}");
    assert_eq!(program.items.len(), 1);
}

// --- v0.8: Import system ---

fn run_with_dir(src: &str, dir: std::path::PathBuf) -> a_lang::interpreter::Value {
    let program = parse(src);
    let mut interp = Interpreter::new();
    interp.set_source_dir(dir);
    interp.run(&program).expect("runtime error")
}

#[test]
fn interp_import_basic() {
    let dir = std::env::temp_dir().join("a_test_import");
    std::fs::create_dir_all(&dir).unwrap();
    std::fs::write(dir.join("mylib.a"), "fn double(x: i64) -> i64 {\n  ret x * 2\n}\n").unwrap();
    let val = run_with_dir("use mylib\nfn main() -> i64 {\n  ret mylib.double(21)\n}", dir.clone());
    std::fs::remove_dir_all(&dir).ok();
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 42); }
    else { panic!("expected int"); }
}

#[test]
fn interp_import_nested_dir() {
    let dir = std::env::temp_dir().join("a_test_nested");
    let sub = dir.join("pkg");
    std::fs::create_dir_all(&sub).unwrap();
    std::fs::write(sub.join("util.a"), "fn triple(x: i64) -> i64 {\n  ret x * 3\n}\n").unwrap();
    let val = run_with_dir("use pkg.util\nfn main() -> i64 {\n  ret util.triple(10)\n}", dir.clone());
    std::fs::remove_dir_all(&dir).ok();
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 30); }
    else { panic!("expected int"); }
}

#[test]
fn interp_import_internal_calls() {
    let dir = std::env::temp_dir().join("a_test_internal");
    std::fs::create_dir_all(&dir).unwrap();
    std::fs::write(dir.join("helper.a"), "fn add1(x: i64) -> i64 {\n  ret x + 1\n}\nfn add2(x: i64) -> i64 {\n  ret add1(add1(x))\n}\n").unwrap();
    let val = run_with_dir("use helper\nfn main() -> i64 {\n  ret helper.add2(10)\n}", dir.clone());
    std::fs::remove_dir_all(&dir).ok();
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 12); }
    else { panic!("expected int"); }
}

#[test]
fn interp_import_multiple() {
    let dir = std::env::temp_dir().join("a_test_multi");
    std::fs::create_dir_all(&dir).unwrap();
    std::fs::write(dir.join("a.a"), "fn val() -> i64 {\n  ret 10\n}\n").unwrap();
    std::fs::write(dir.join("b.a"), "fn val() -> i64 {\n  ret 20\n}\n").unwrap();
    let val = run_with_dir("use a\nuse b\nfn main() -> i64 {\n  ret a.val() + b.val()\n}", dir.clone());
    std::fs::remove_dir_all(&dir).ok();
    if let a_lang::interpreter::Value::Int(n) = val { assert_eq!(n, 30); }
    else { panic!("expected int"); }
}

// --- sort / reverse_arr / contains tests ---

#[test]
fn interp_sort_integers() {
    let val = run("fn main() -> [i64] {\n  let a: [i64] = [3, 1, 4, 1, 5]\n  ret sort(a)\n}");
    if let a_lang::interpreter::Value::Array(arr) = val {
        let ints: Vec<i64> = arr.iter().map(|v| if let a_lang::interpreter::Value::Int(n) = v { *n } else { panic!("expected int") }).collect();
        assert_eq!(ints, vec![1, 1, 3, 4, 5]);
    } else { panic!("expected array"); }
}

#[test]
fn interp_sort_strings() {
    let val = run("fn main() -> [str] {\n  let a: [str] = [\"c\", \"a\", \"b\"]\n  ret sort(a)\n}");
    if let a_lang::interpreter::Value::Array(arr) = val {
        let strs: Vec<String> = arr.iter().map(|v| if let a_lang::interpreter::Value::String(s) = v { s.as_ref().clone() } else { panic!("expected str") }).collect();
        assert_eq!(strs, vec!["a", "b", "c"]);
    } else { panic!("expected array"); }
}

#[test]
fn interp_reverse_arr() {
    let val = run("fn main() -> [i64] {\n  let a: [i64] = [1, 2, 3]\n  ret reverse_arr(a)\n}");
    if let a_lang::interpreter::Value::Array(arr) = val {
        let ints: Vec<i64> = arr.iter().map(|v| if let a_lang::interpreter::Value::Int(n) = v { *n } else { panic!("expected int") }).collect();
        assert_eq!(ints, vec![3, 2, 1]);
    } else { panic!("expected array"); }
}

#[test]
fn interp_contains_true() {
    let val = run("fn main() -> bool {\n  let a: [i64] = [1, 2, 3]\n  ret contains(a, 2)\n}");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn interp_contains_false() {
    let val = run("fn main() -> bool {\n  let a: [str] = [\"x\", \"y\"]\n  ret contains(a, \"z\")\n}");
    assert_eq!(val, a_lang::interpreter::Value::Bool(false));
}

// --- Type inference tests ---

#[test]
fn infer_let_int() {
    let val = run("fn main() -> i64 {\n  let x = 42\n  ret x\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}

#[test]
fn infer_let_str() {
    let val = run("fn main() -> str {\n  let s = \"hello\"\n  ret s\n}");
    assert_eq!(val, a_lang::interpreter::Value::string("hello".into()));
}

#[test]
fn infer_let_array() {
    let val = run("fn main() -> i64 {\n  let a = [10, 20, 30]\n  ret len(a)\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn infer_for_loop() {
    let val = run("fn main() -> i64 {\n  let mut sum: i64 = 0\n  let nums = [1, 2, 3]\n  for n in nums {\n    sum = sum + n\n  }\n  ret sum\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(6));
}

#[test]
fn infer_let_mut() {
    let val = run("fn main() -> i64 {\n  let mut x = 0\n  x = 10\n  ret x\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(10));
}

// --- Multi-line expression tests ---

#[test]
fn multiline_array() {
    let val = run("fn main() -> i64 {\n  let a: [i64] = [\n    1,\n    2,\n    3\n  ]\n  ret len(a)\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn multiline_fn_call() {
    let val = run("fn add(a: i64, b: i64) -> i64 {\n  ret a + b\n}\nfn main() -> i64 {\n  ret add(\n    10,\n    20\n  )\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

// --- str.find / str.count / args tests ---

#[test]
fn interp_str_find_found() {
    let val = run("fn main() -> i64 { ret str.find(\"hello world\", \"world\") }");
    assert_eq!(val, a_lang::interpreter::Value::Int(6));
}

#[test]
fn interp_str_find_not_found() {
    let val = run("fn main() -> i64 { ret str.find(\"hello\", \"xyz\") }");
    assert_eq!(val, a_lang::interpreter::Value::Int(-1));
}

#[test]
fn interp_str_count() {
    let val = run("fn main() -> i64 { ret str.count(\"abcabc\", \"abc\") }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn interp_args_empty() {
    let val = run("fn main() -> i64 { ret len(args()) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

// --- io.read_stdin / eprintln ---

#[test]
fn interp_eprintln() {
    let val = run("fn main() -> i64 { eprintln(\"test\") \n ret 42 }");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}

#[test]
fn interp_io_read_stdin_is_builtin() {
    assert!(a_lang::builtins::is_builtin("io.read_stdin"));
}

#[test]
fn interp_io_read_line_is_builtin() {
    assert!(a_lang::builtins::is_builtin("io.read_line"));
}

// --- Structured error output test ---

#[test]
fn error_is_json_serializable() {
    let err = a_lang::errors::AError::parse("test error", None);
    let json = err.to_json();
    assert!(json.contains("ParseError"));
    assert!(json.contains("test error"));
}

// --- VM Closure Capture tests ---

#[test]
fn vm_closure_basic_capture() {
    let val = run_vm("fn main() -> i64 { let x = 10 \n let f = fn(y: i64) => y + x \n ret f(5) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(15));
}

#[test]
fn vm_closure_capture_string() {
    let val = run_vm("fn main() -> str { let prefix = \"hello_\" \n let f = fn(s: str) => str.concat(prefix, s) \n ret f(\"world\") }");
    assert_eq!(val, a_lang::interpreter::Value::string("hello_world".into()));
}

#[test]
fn vm_closure_nested_capture() {
    let val = run_vm("fn main() -> i64 { let a = 1 \n let f = fn(x: i64) { let g = fn(y: i64) => y + a + x \n ret g(2) } \n ret f(3) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(6));
}

#[test]
fn vm_closure_hof_filter() {
    let val = run_vm("fn main() -> [i64] { let threshold = 3 \n let data = [1, 2, 3, 4, 5] \n ret filter(data, fn(x: i64) => x > threshold) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(4),
        a_lang::interpreter::Value::Int(5),
    ]));
}

#[test]
fn vm_closure_hof_map() {
    let val = run_vm("fn main() -> [i64] { let offset = 100 \n ret map([1, 2, 3], fn(x: i64) => x + offset) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(101),
        a_lang::interpreter::Value::Int(102),
        a_lang::interpreter::Value::Int(103),
    ]));
}

#[test]
fn vm_closure_hof_reduce() {
    let val = run_vm("fn main() -> i64 { let bonus = 10 \n ret reduce([1, 2, 3], 0, fn(acc: i64, x: i64) => acc + x + bonus) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(36));
}

#[test]
fn vm_closure_pipe_chain_with_captures() {
    let val = run_vm("fn main() -> [i64] { let t = 2 \n ret [1,2,3,4,5] |> filter(fn(x: i64) => x > t) |> map(fn(x: i64) => x * 2) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(6),
        a_lang::interpreter::Value::Int(8),
        a_lang::interpreter::Value::Int(10),
    ]));
}

#[test]
fn vm_closure_value_semantics() {
    let val = run_vm("fn main() -> [i64] { let items = [1, 2] \n each([10, 20], fn(x: i64) => push(items, x)) \n ret items }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(2),
    ]));
}

#[test]
fn vm_closure_no_capture_still_works() {
    let val = run_vm("fn main() -> i64 { let f = fn(x: i64) => x * 3 \n ret f(7) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(21));
}

#[test]
fn vm_closure_multiple_captures() {
    let val = run_vm("fn main() -> i64 { let a = 10 \n let b = 20 \n let c = 30 \n let f = fn(x: i64) => x + a + b + c \n ret f(1) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(61));
}

#[test]
fn vm_closure_pipe_to_local() {
    let val = run_vm("fn main() -> i64 { let double = fn(x: i64) => x * 2 \n ret 5 |> double }");
    assert_eq!(val, a_lang::interpreter::Value::Int(10));
}

#[test]
fn vm_closure_capture_in_for_loop() {
    let val = run_vm("fn main() -> [i64] {
        let multiplier = 10
        let data = [1, 2, 3]
        ret map(data, fn(x: i64) => x * multiplier)
    }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(10),
        a_lang::interpreter::Value::Int(20),
        a_lang::interpreter::Value::Int(30),
    ]));
}

// --- Functional Toolkit tests (VM) ---

#[test]
fn vm_sort_by() {
    let val = run_vm("fn main() -> [str] { ret sort_by([\"banana\", \"fig\", \"apple\"], fn(a: str, b: str) => len(a) - len(b)) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::string("fig".into()),
        a_lang::interpreter::Value::string("apple".into()),
        a_lang::interpreter::Value::string("banana".into()),
    ]));
}

#[test]
fn vm_find_present() {
    let val = run_vm("fn main() -> i64 { ret unwrap(find([10, 20, 30], fn(x: i64) => x > 15)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(20));
}

#[test]
fn vm_find_absent() {
    let val = run_vm("fn main() -> bool { ret is_err(find([1, 2, 3], fn(x: i64) => x > 10)) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn vm_any_true() {
    let val = run_vm("fn main() -> bool { ret any([1, 2, 3], fn(x: i64) => x > 2) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn vm_any_false() {
    let val = run_vm("fn main() -> bool { ret any([1, 2, 3], fn(x: i64) => x > 5) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(false));
}

#[test]
fn vm_all_true() {
    let val = run_vm("fn main() -> bool { ret all([2, 4, 6], fn(x: i64) => x > 0) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn vm_all_false() {
    let val = run_vm("fn main() -> bool { ret all([2, 4, 6], fn(x: i64) => x > 3) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(false));
}

#[test]
fn vm_flat_map() {
    let val = run_vm("fn main() -> [i64] { ret flat_map([1, 2, 3], fn(x: i64) => [x, x * 10]) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(10),
        a_lang::interpreter::Value::Int(2),
        a_lang::interpreter::Value::Int(20),
        a_lang::interpreter::Value::Int(3),
        a_lang::interpreter::Value::Int(30),
    ]));
}

#[test]
fn vm_zip() {
    let val = run_vm("fn main() -> [[i64]] { ret zip([1, 2], [10, 20]) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::array(vec![a_lang::interpreter::Value::Int(1), a_lang::interpreter::Value::Int(10)]),
        a_lang::interpreter::Value::array(vec![a_lang::interpreter::Value::Int(2), a_lang::interpreter::Value::Int(20)]),
    ]));
}

#[test]
fn vm_enumerate() {
    let val = run_vm("fn main() -> i64 { let pairs = enumerate([10, 20, 30]) \n ret len(pairs) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn vm_take() {
    let val = run_vm("fn main() -> [i64] { ret take([1, 2, 3, 4, 5], 3) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(2),
        a_lang::interpreter::Value::Int(3),
    ]));
}

#[test]
fn vm_drop() {
    let val = run_vm("fn main() -> [i64] { ret drop([1, 2, 3, 4, 5], 2) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(3),
        a_lang::interpreter::Value::Int(4),
        a_lang::interpreter::Value::Int(5),
    ]));
}

#[test]
fn vm_chunk() {
    let val = run_vm("fn main() -> i64 { ret len(chunk([1, 2, 3, 4, 5], 2)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn vm_unique() {
    let val = run_vm("fn main() -> [i64] { ret unique([1, 2, 3, 2, 1, 4]) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(2),
        a_lang::interpreter::Value::Int(3),
        a_lang::interpreter::Value::Int(4),
    ]));
}

#[test]
fn vm_min_by() {
    let val = run_vm("fn main() -> str { ret unwrap(min_by([\"banana\", \"fig\", \"cherry\"], fn(s: str) => len(s))) }");
    assert_eq!(val, a_lang::interpreter::Value::string("fig".into()));
}

#[test]
fn vm_max_by() {
    let val = run_vm("fn main() -> str { ret unwrap(max_by([\"banana\", \"fig\", \"cherry\"], fn(s: str) => len(s))) }");
    assert_eq!(val, a_lang::interpreter::Value::string("banana".into()));
}

#[test]
fn vm_functional_pipeline() {
    let val = run_vm("fn main() -> [i64] { ret [1,2,3,4,5,6,7,8] |> filter(fn(x: i64) => x > 3) |> map(fn(x: i64) => x * x) |> take(3) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(16),
        a_lang::interpreter::Value::Int(25),
        a_lang::interpreter::Value::Int(36),
    ]));
}

// --- Functional Toolkit tests (interpreter) ---

#[test]
fn interp_sort_by() {
    let val = run("fn main() -> [i64] { ret sort_by([3, 1, 2], fn(a: i64, b: i64) => a - b) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(2),
        a_lang::interpreter::Value::Int(3),
    ]));
}

#[test]
fn interp_find() {
    let val = run("fn main() -> i64 { ret unwrap(find([5, 10, 15], fn(x: i64) => x > 7)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(10));
}

#[test]
fn interp_any_all() {
    let val = run("fn main() -> bool { ret any([1, 2, 3], fn(x: i64) => x == 2) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn interp_zip() {
    let val = run("fn main() -> i64 { ret len(zip([1, 2, 3], [4, 5])) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn interp_enumerate() {
    let val = run("fn main() -> i64 { ret len(enumerate([10, 20, 30])) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn interp_take_drop() {
    let val = run("fn main() -> [i64] { ret take([1, 2, 3, 4], 2) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(2),
    ]));
}

#[test]
fn interp_chunk() {
    let val = run("fn main() -> i64 { ret len(chunk([1, 2, 3, 4, 5], 2)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn interp_unique() {
    let val = run("fn main() -> [i64] { ret unique([5, 3, 5, 1, 3]) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(5),
        a_lang::interpreter::Value::Int(3),
        a_lang::interpreter::Value::Int(1),
    ]));
}

#[test]
fn interp_flat_map() {
    let val = run("fn main() -> [i64] { ret flat_map([[1, 2], [3]], fn(x: [i64]) => x) }");
    assert_eq!(val, a_lang::interpreter::Value::array(vec![
        a_lang::interpreter::Value::Int(1),
        a_lang::interpreter::Value::Int(2),
        a_lang::interpreter::Value::Int(3),
    ]));
}

#[test]
fn interp_min_by_max_by() {
    let val = run("fn main() -> i64 { ret unwrap(min_by([5, 2, 8, 1], fn(x: i64) => x)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

// --- Tail Call Optimization tests (VM) ---

#[test]
fn vm_tail_recursion_deep() {
    let val = run_vm("fn countdown(n: i64) -> i64 { if n == 0 { ret 0 } \n ret countdown(n - 1) }\nfn main() -> i64 { ret countdown(100000) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

#[test]
fn vm_tail_accumulator() {
    let val = run_vm("fn sum_acc(n: i64, acc: i64) -> i64 { if n == 0 { ret acc } \n ret sum_acc(n - 1, acc + n) }\nfn main() -> i64 { ret sum_acc(100000, 0) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(5000050000));
}

#[test]
fn vm_tail_mutual_recursion() {
    let val = run_vm("fn is_even(n: i64) -> bool { if n == 0 { ret true } \n ret is_odd(n - 1) }\nfn is_odd(n: i64) -> bool { if n == 0 { ret false } \n ret is_even(n - 1) }\nfn main() -> bool { ret is_even(100000) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn vm_tail_mutual_recursion_odd() {
    let val = run_vm("fn is_even(n: i64) -> bool { if n == 0 { ret true } \n ret is_odd(n - 1) }\nfn is_odd(n: i64) -> bool { if n == 0 { ret false } \n ret is_even(n - 1) }\nfn main() -> bool { ret is_odd(99999) }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn vm_tail_call_builtin_in_tail_position() {
    let val = run_vm("fn double(x: i64) -> i64 { ret x * 2 }\nfn apply(n: i64) -> i64 { ret double(n) }\nfn main() -> i64 { ret apply(21) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}

#[test]
fn vm_tail_call_preserves_return_value() {
    let val = run_vm("fn last(n: i64) -> i64 { if n <= 1 { ret 42 } \n ret last(n - 1) }\nfn main() -> i64 { ret last(10000) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}

#[test]
fn vm_tail_call_with_capture() {
    let val = run_vm("fn main() -> i64 { let offset = 100 \n let f = fn(n: i64) => n + offset \n ret f(42) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(142));
}

// --- Math builtins ---

#[test]
fn vm_floor() {
    let val = run_vm("fn main() -> i64 { ret floor(3.7) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn vm_ceil() {
    let val = run_vm("fn main() -> i64 { ret ceil(3.2) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

#[test]
fn vm_round() {
    let val = run_vm("fn main() -> i64 { ret round(3.5) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

#[test]
fn vm_floor_int_passthrough() {
    let val = run_vm("fn main() -> i64 { ret floor(5) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(5));
}

// --- Destructuring & Spread tests (VM) ---

#[test]
fn vm_let_destructure_basic() {
    let val = run_vm("fn main() -> i64 { let [a, b, c] = [10, 20, 30] \n ret b }");
    assert_eq!(val, a_lang::interpreter::Value::Int(20));
}

#[test]
fn vm_let_destructure_rest() {
    let val = run_vm("fn main() -> i64 { let [first, ...rest] = [1, 2, 3, 4, 5] \n ret len(rest) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

#[test]
fn vm_let_destructure_rest_value() {
    let val = run_vm("fn main() -> i64 { let [_, ...rest] = [10, 20, 30] \n ret rest[0] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(20));
}

#[test]
fn vm_let_destructure_wildcard() {
    let val = run_vm("fn main() -> i64 { let [_, second, _] = [100, 200, 300] \n ret second }");
    assert_eq!(val, a_lang::interpreter::Value::Int(200));
}

#[test]
fn vm_for_destructure_enumerate() {
    let val = run_vm("fn main() -> i64 { let mut sum = 0 \n for [i, v] in enumerate([10, 20, 30]) { sum = sum + i + v } \n ret sum }");
    assert_eq!(val, a_lang::interpreter::Value::Int(63));
}

#[test]
fn vm_for_destructure_zip() {
    let val = run_vm("fn main() -> i64 { let mut sum = 0 \n for [a, b] in zip([1, 2, 3], [10, 20, 30]) { sum = sum + a + b } \n ret sum }");
    assert_eq!(val, a_lang::interpreter::Value::Int(66));
}

#[test]
fn vm_spread_concat() {
    let val = run_vm("fn main() -> i64 { let a = [1, 2] \n let b = [3, 4] \n let c = [...a, ...b] \n ret len(c) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

#[test]
fn vm_spread_mixed() {
    let val = run_vm("fn main() -> i64 { let a = [2, 3] \n let c = [1, ...a, 4] \n ret c[2] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn vm_spread_empty() {
    let val = run_vm("fn main() -> i64 { let a = [] \n let c = [...a, 1, 2] \n ret c[0] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn vm_destructure_from_split() {
    let val = run_vm("fn main() -> str { let [name, age] = str.split(\"alice,30\", \",\") \n ret name }");
    assert_eq!(val, a_lang::interpreter::Value::string("alice".into()));
}

// --- Destructuring & Spread tests (Interpreter) ---

#[test]
fn interp_let_destructure() {
    let val = run("fn main() -> i64 { let [a, b] = [10, 20] \n ret a + b }");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

#[test]
fn interp_let_destructure_rest() {
    let val = run("fn main() -> i64 { let [first, ...rest] = [1, 2, 3] \n ret first + len(rest) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn interp_for_destructure() {
    let val = run("fn main() -> i64 { let mut sum = 0 \n for [i, v] in enumerate([5, 10]) { sum = sum + i + v } \n ret sum }");
    assert_eq!(val, a_lang::interpreter::Value::Int(16));
}

#[test]
fn interp_spread() {
    let val = run("fn main() -> i64 { let a = [1, 2] \n let b = [3, 4] \n ret len([...a, ...b]) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

// --- Source-Mapped Stack Trace tests ---

#[test]
fn vm_stack_trace_single_function() {
    let e = run_vm_err("fn main() -> i64 {\n  ret unwrap(Err(\"oops\"))\n}");
    assert!(!e.stack.is_empty(), "stack trace should not be empty");
    assert_eq!(e.stack.last().unwrap().function, "main");
    assert!(e.stack[0].line > 0, "line should be > 0");
}

#[test]
fn vm_stack_trace_nested_calls() {
    let src = "\
fn inner() -> i64 {\n  ret unwrap(Err(\"deep\"))\n}\n\
fn middle() -> i64 {\n  let x = inner()\n  ret x\n}\n\
fn outer() -> i64 {\n  let y = middle()\n  ret y\n}\n\
fn main() -> i64 {\n  let z = outer()\n  ret z\n}";
    let e = run_vm_err(src);
    let names: Vec<&str> = e.stack.iter().map(|f| f.function.as_str()).collect();
    assert_eq!(names, vec!["inner", "middle", "outer", "main"]);
}

#[test]
fn vm_stack_trace_correct_lines() {
    let src = "\
fn helper() -> i64 {\n  ret unwrap(Err(\"bad\"))\n}\n\
fn main() -> i64 {\n  let x = helper()\n  ret x\n}";
    let e = run_vm_err(src);
    assert_eq!(e.stack[0].function, "helper");
    assert_eq!(e.stack[0].line, 2);
    assert_eq!(e.stack[1].function, "main");
    assert_eq!(e.stack[1].line, 5);
}

#[test]
fn vm_stack_trace_tail_call_elides_frame() {
    let src = "\
fn a() -> i64 {\n  ret unwrap(Err(\"fail\"))\n}\n\
fn b() -> i64 {\n  ret a()\n}\n\
fn c() -> i64 {\n  ret b()\n}\n\
fn main() -> i64 {\n  let x = c()\n  ret x\n}";
    let e = run_vm_err(src);
    let names: Vec<&str> = e.stack.iter().map(|f| f.function.as_str()).collect();
    assert!(names.contains(&"a"), "innermost function should be in trace");
    assert!(names.contains(&"main"), "main should be in trace");
    assert!(!names.contains(&"b") || !names.contains(&"c"),
        "tail call should elide at least one intermediate frame");
}

#[test]
fn vm_stack_trace_fail_builtin() {
    let src = "\
fn bad() -> void {\n  fail(\"boom\")\n}\n\
fn main() -> void {\n  bad()\n}";
    let e = run_vm_err(src);
    assert!(e.message.contains("boom"));
    let names: Vec<&str> = e.stack.iter().map(|f| f.function.as_str()).collect();
    assert!(names.contains(&"bad"));
}

#[test]
fn vm_stack_trace_span_set() {
    let e = run_vm_err("fn main() -> i64 {\n  ret unwrap(Err(\"x\"))\n}");
    assert!(e.span.is_some(), "span should be set from stack trace");
    assert!(e.span.unwrap().line > 0);
}

#[test]
fn vm_stack_trace_json_contains_stack() {
    let e = run_vm_err("fn main() -> i64 {\n  ret unwrap(Err(\"x\"))\n}");
    let json = e.to_json();
    assert!(json.contains("\"stack\""), "JSON should contain stack field");
    assert!(json.contains("\"function\""), "JSON should contain function names");
    assert!(json.contains("\"line\""), "JSON should contain line numbers");
}

#[test]
fn vm_stack_trace_closure_error() {
    let src = "\
fn main() -> i64 {\n  let f = fn() => unwrap(Err(\"closure err\"))\n  let x = f()\n  ret x\n}";
    let e = run_vm_err(src);
    assert!(e.message.contains("closure err"));
    assert!(!e.stack.is_empty());
    assert!(e.stack.iter().any(|f| f.function == "main"));
}

// --- HashMap Map tests (VM) ---

#[test]
fn vm_map_literal_and_get() {
    let val = run_vm("fn main() -> str { let m = #{\"name\": \"Alice\"} \n ret m[\"name\"] }");
    assert_eq!(val, a_lang::interpreter::Value::string("Alice".into()));
}

#[test]
fn vm_map_field_access() {
    let val = run_vm("fn main() -> str { let m = #{\"greeting\": \"hi\"} \n ret m.greeting }");
    assert_eq!(val, a_lang::interpreter::Value::string("hi".into()));
}

#[test]
fn vm_map_bracket_write() {
    let val = run_vm("fn main() -> str { let mut m = #{\"x\": \"old\"} \n m[\"x\"] = \"new\" \n ret m[\"x\"] }");
    assert_eq!(val, a_lang::interpreter::Value::string("new".into()));
}

#[test]
fn vm_map_bracket_add_key() {
    let val = run_vm("fn main() -> i64 { let mut m = #{\"a\": 1} \n m[\"b\"] = 2 \n ret len(m) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_iteration() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n let mut sum = 0 \n for [k, v] in m { sum = sum + v } \n ret sum }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn vm_map_get_builtin() {
    let val = run_vm("fn main() -> i64 { let m = #{\"x\": 42} \n ret map.get(m, \"x\") }");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}

#[test]
fn vm_map_set_builtin() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 1} \n let m2 = map.set(m, \"b\", 2) \n ret len(m2) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_has_builtin() {
    let val = run_vm("fn main() -> bool { let m = #{\"key\": 1} \n ret map.has(m, \"key\") }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(true));
}

#[test]
fn vm_map_has_missing() {
    let val = run_vm("fn main() -> bool { let m = #{\"key\": 1} \n ret map.has(m, \"nope\") }");
    assert_eq!(val, a_lang::interpreter::Value::Bool(false));
}

#[test]
fn vm_map_keys() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n ret len(map.keys(m)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_values() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 10, \"b\": 20} \n let vals = map.values(m) \n ret len(vals) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_delete() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n let m2 = map.delete(m, \"a\") \n ret len(m2) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn vm_map_merge() {
    let val = run_vm("fn main() -> i64 { let a = #{\"x\": 1} \n let b = #{\"y\": 2} \n let c = map.merge(a, b) \n ret len(c) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_merge_overwrite() {
    let val = run_vm("fn main() -> i64 { let a = #{\"x\": 1} \n let b = #{\"x\": 99} \n let c = map.merge(a, b) \n ret c[\"x\"] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(99));
}

#[test]
fn vm_map_entries() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n ret len(map.entries(m)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_from_entries() {
    let val = run_vm("fn main() -> i64 { let m = map.from_entries([[\"x\", 10], [\"y\", 20]]) \n ret m[\"x\"] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(10));
}

#[test]
fn vm_map_roundtrip_entries() {
    let val = run_vm("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n let m2 = map.from_entries(map.entries(m)) \n ret m2[\"a\"] + m2[\"b\"] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn vm_map_empty() {
    let val = run_vm("fn main() -> i64 { let m = #{} \n ret len(m) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

#[test]
fn vm_map_missing_key_returns_void() {
    let val = run_vm("fn main() -> str { let m = #{\"a\": 1} \n ret type_of(m[\"missing\"]) }");
    assert_eq!(val, a_lang::interpreter::Value::string("void".into()));
}

// --- HashMap Map tests (Interpreter) ---

#[test]
fn interp_map_bracket_read() {
    let val = run("fn main() -> str { let m = #{\"name\": \"Bob\"} \n ret m[\"name\"] }");
    assert_eq!(val, a_lang::interpreter::Value::string("Bob".into()));
}

#[test]
fn interp_map_bracket_write() {
    let val = run("fn main() -> str { let mut m = #{\"x\": \"old\"} \n m[\"x\"] = \"new\" \n ret m[\"x\"] }");
    assert_eq!(val, a_lang::interpreter::Value::string("new".into()));
}

#[test]
fn interp_map_field_access() {
    let val = run("fn main() -> str { let m = #{\"greeting\": \"hello\"} \n ret m.greeting }");
    assert_eq!(val, a_lang::interpreter::Value::string("hello".into()));
}

#[test]
fn interp_map_iteration() {
    let val = run("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n let mut sum = 0 \n for [k, v] in m { sum = sum + v } \n ret sum }");
    assert_eq!(val, a_lang::interpreter::Value::Int(3));
}

#[test]
fn interp_map_delete() {
    let val = run("fn main() -> i64 { let m = #{\"a\": 1, \"b\": 2} \n let m2 = map.delete(m, \"a\") \n ret len(m2) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn interp_map_merge() {
    let val = run("fn main() -> i64 { let a = #{\"x\": 1} \n let b = #{\"y\": 2} \n ret len(map.merge(a, b)) }");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn interp_map_from_entries() {
    let val = run("fn main() -> i64 { let m = map.from_entries([[\"a\", 10], [\"b\", 20]]) \n ret m[\"a\"] }");
    assert_eq!(val, a_lang::interpreter::Value::Int(10));
}

// --- Pattern Matching Overhaul (v0.27) ---

#[test]
fn vm_match_literal_fix() {
    let val = run_vm("fn main() -> i64 {\n  let x = 2\n  match x {\n    1 => { ret 10 }\n    2 => { ret 20 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(20));
}

#[test]
fn vm_match_literal_fallthrough() {
    let val = run_vm("fn main() -> i64 {\n  let x = 99\n  match x {\n    1 => { ret 10 }\n    2 => { ret 20 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

#[test]
fn vm_array_pattern_empty() {
    let val = run_vm("fn main() -> i64 {\n  let a = []\n  match a {\n    [] => { ret 1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn vm_array_pattern_fixed() {
    let val = run_vm("fn main() -> i64 {\n  let a = [10, 20]\n  match a {\n    [x, y] => { ret x + y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

#[test]
fn vm_array_pattern_rest() {
    let val = run_vm("fn main() -> i64 {\n  let a = [1, 2, 3, 4]\n  match a {\n    [h, ...t] => { ret h + len(t) }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

#[test]
fn vm_array_pattern_mismatch_type() {
    let val = run_vm("fn main() -> i64 {\n  let a = 42\n  match a {\n    [x] => { ret x }\n    _ => { ret 99 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(99));
}

#[test]
fn vm_array_pattern_mismatch_len() {
    let val = run_vm("fn main() -> i64 {\n  let a = [1, 2, 3]\n  match a {\n    [x, y] => { ret x }\n    _ => { ret 99 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(99));
}

#[test]
fn vm_array_pattern_single() {
    let val = run_vm("fn main() -> i64 {\n  let a = [42]\n  match a {\n    [x] => { ret x }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}

#[test]
fn vm_array_pattern_nested_literal() {
    let val = run_vm("fn main() -> i64 {\n  let a = [1, 2]\n  match a {\n    [1, y] => { ret y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(2));
}

#[test]
fn vm_map_pattern_single_key() {
    let val = run_vm("fn main() -> i64 {\n  let m = #{\"a\": 10}\n  match m {\n    #{\"a\": v} => { ret v }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(10));
}

#[test]
fn vm_map_pattern_multi_key() {
    let val = run_vm("fn main() -> i64 {\n  let m = #{\"x\": 3, \"y\": 4}\n  match m {\n    #{\"x\": a, \"y\": b} => { ret a + b }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(7));
}

#[test]
fn vm_map_pattern_literal_value() {
    let val = run_vm("fn main() -> i64 {\n  let m = #{\"status\": 200}\n  match m {\n    #{\"status\": 200} => { ret 1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn vm_map_pattern_literal_mismatch() {
    let val = run_vm("fn main() -> i64 {\n  let m = #{\"status\": 404}\n  match m {\n    #{\"status\": 200} => { ret 1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

#[test]
fn vm_map_pattern_missing_key() {
    let val = run_vm("fn main() -> i64 {\n  let m = #{\"a\": 1}\n  match m {\n    #{\"b\": v} => { ret v }\n    _ => { ret 99 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(99));
}

#[test]
fn vm_map_pattern_not_map() {
    let val = run_vm("fn main() -> i64 {\n  let m = 42\n  match m {\n    #{\"a\": v} => { ret v }\n    _ => { ret 99 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(99));
}

#[test]
fn vm_guard_basic() {
    let val = run_vm("fn main() -> i64 {\n  let n = 5\n  match n {\n    x if x > 0 => { ret 1 }\n    x if x < 0 => { ret -1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn vm_guard_negative() {
    let val = run_vm("fn main() -> i64 {\n  let n = -3\n  match n {\n    x if x > 0 => { ret 1 }\n    x if x < 0 => { ret -1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(-1));
}

#[test]
fn vm_guard_fallthrough_to_wildcard() {
    let val = run_vm("fn main() -> i64 {\n  let n = 0\n  match n {\n    x if x > 0 => { ret 1 }\n    x if x < 0 => { ret -1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

#[test]
fn vm_guard_with_array_pattern() {
    let val = run_vm("fn main() -> i64 {\n  let a = [3, 5]\n  match a {\n    [x, y] if x > y => { ret x }\n    [x, y] if y > x => { ret y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(5));
}

#[test]
fn vm_nested_map_with_string_match() {
    let val = run_vm("fn main() -> i64 {\n  let e = #{\"type\": \"click\", \"x\": 10, \"y\": 20}\n  match e {\n    #{\"type\": \"click\", \"x\": x, \"y\": y} => { ret x + y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

#[test]
fn vm_multiple_array_arms() {
    let val = run_vm("fn main() -> i64 {\n  let a = [1, 2, 3]\n  match a {\n    [] => { ret 0 }\n    [x] => { ret x }\n    [x, y] => { ret x + y }\n    [x, y, z] => { ret x + y + z }\n    _ => { ret -1 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(6));
}

#[test]
fn interp_array_pattern_empty() {
    let val = run("fn main() -> i64 {\n  let a = []\n  match a {\n    [] => { ret 1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn interp_array_pattern_fixed() {
    let val = run("fn main() -> i64 {\n  let a = [10, 20]\n  match a {\n    [x, y] => { ret x + y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

#[test]
fn interp_array_pattern_rest() {
    let val = run("fn main() -> i64 {\n  let a = [1, 2, 3, 4]\n  match a {\n    [h, ...t] => { ret h + len(t) }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(4));
}

#[test]
fn interp_map_pattern() {
    let val = run("fn main() -> i64 {\n  let m = #{\"a\": 10, \"b\": 20}\n  match m {\n    #{\"a\": x, \"b\": y} => { ret x + y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

#[test]
fn interp_map_pattern_literal() {
    let val = run("fn main() -> i64 {\n  let m = #{\"status\": 200}\n  match m {\n    #{\"status\": 200} => { ret 1 }\n    #{\"status\": 404} => { ret 2 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn interp_guard_basic() {
    let val = run("fn main() -> i64 {\n  let n = 5\n  match n {\n    x if x > 0 => { ret 1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(1));
}

#[test]
fn interp_guard_fallthrough() {
    let val = run("fn main() -> i64 {\n  let n = -1\n  match n {\n    x if x > 0 => { ret 1 }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(0));
}

#[test]
fn interp_nested_map_array() {
    let val = run("fn main() -> i64 {\n  let e = #{\"pos\": [10, 20]}\n  match e {\n    #{\"pos\": [x, y]} => { ret x + y }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(30));
}

#[test]
fn interp_array_pattern_literal_elem() {
    let val = run("fn main() -> i64 {\n  let a = [1, 42]\n  match a {\n    [1, x] => { ret x }\n    _ => { ret 0 }\n  }\n}");
    assert_eq!(val, a_lang::interpreter::Value::Int(42));
}
