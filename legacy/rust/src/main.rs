use clap::{Parser, Subcommand};
use std::process;
use std::time::Instant;

use a_lang::ast::TopLevelKind;
use a_lang::bridge;
use a_lang::builtins::{json_to_value, value_to_json};
use a_lang::checker::Checker;
use a_lang::compiler::Compiler;
use a_lang::formatter;
use a_lang::interpreter::Interpreter;
use a_lang::lexer::Lexer;
use a_lang::parser::Parser as AParser;
use a_lang::vm::VM;

#[derive(Parser)]
#[command(name = "a", about = "The 'a' programming language -- by AI, for AI")]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

fn get_source_dir(file: &str) -> std::path::PathBuf {
    let src_path = std::path::Path::new(file);
    if let Some(parent) = src_path.parent() {
        if parent.as_os_str().is_empty() {
            std::env::current_dir().unwrap_or_default()
        } else {
            parent.to_path_buf()
        }
    } else {
        std::env::current_dir().unwrap_or_default()
    }
}

#[derive(Subcommand)]
enum Commands {
    /// Run an .a program (bytecode VM — fast)
    Run {
        /// Path to the .a source file
        file: String,
        /// Arguments passed to the program (available via args())
        #[arg(trailing_var_arg = true, allow_hyphen_values = true)]
        program_args: Vec<String>,
    },
    /// Run via tree-walking interpreter (legacy)
    Interp {
        /// Path to the .a source file
        file: String,
        /// Arguments passed to the program
        #[arg(trailing_var_arg = true, allow_hyphen_values = true)]
        program_args: Vec<String>,
    },
    /// Type-check an .a program without running it
    Check {
        /// Path to the .a source file
        file: String,
    },
    /// Format an .a program to canonical form
    Fmt {
        /// Path to the .a source file
        file: String,
    },
    /// Run test_* functions in an .a file
    Test {
        /// Path to the .a test file
        file: String,
        /// Optional filter: only run tests whose names contain this substring
        #[arg(short, long)]
        filter: Option<String>,
    },
    /// Dump the AST as JSON
    Ast {
        /// Path to the .a source file
        file: String,
    },
    /// Compile an .a file to a precompiled .ac JSON file
    Compile {
        /// Path to the .a source file
        file: String,
        /// Output path (default: <file>.ac)
        #[arg(short, long)]
        output: Option<String>,
    },
}

fn read_source(path: &str) -> String {
    std::fs::read_to_string(path).unwrap_or_else(|e| {
        let err = a_lang::errors::AError::runtime(
            format!("cannot read file '{path}': {e}"),
            None,
        );
        eprintln!("{}", err.to_json());
        process::exit(1);
    })
}

fn lex_and_parse(source: &str) -> a_lang::ast::Program {
    let mut lexer = Lexer::new(source);
    let tokens = match lexer.tokenize() {
        Ok(t) => t,
        Err(e) => {
            eprintln!("{}", e.to_json());
            process::exit(1);
        }
    };

    let mut parser = AParser::new(tokens);
    match parser.parse_program() {
        Ok(p) => p,
        Err(e) => {
            eprintln!("{}", e.to_json());
            process::exit(1);
        }
    }
}

fn format_error(e: &a_lang::errors::AError) {
    if !e.stack.is_empty() {
        eprintln!("\x1b[31m\x1b[1m{:?}\x1b[0m: {}", e.kind, e.message);
        for frame in &e.stack {
            eprintln!("  at \x1b[36m{}\x1b[0m \x1b[2m(line {})\x1b[0m", frame.function, frame.line);
        }
    }
    eprintln!("{}", e.to_json());
}

fn main() {
    let cli = Cli::parse();

    match cli.command {
        Commands::Run { file, program_args } => {
            if file.ends_with(".ac") {
                let json_str = read_source(&file);
                let compiled = load_ac(&json_str);
                let mut vm = VM::new(compiled);
                vm.set_args(program_args);
                match vm.run() {
                    Ok(_) => {}
                    Err(e) => {
                        format_error(&e);
                        process::exit(1);
                    }
                }
            } else {
                let source = read_source(&file);
                let program = lex_and_parse(&source);

                let mut compiler = Compiler::new();
                compiler.set_source_dir(get_source_dir(&file));
                let compiled = compiler.compile_program(&program);

                let mut vm = VM::new(compiled);
                vm.set_args(program_args);
                match vm.run() {
                    Ok(_) => {}
                    Err(e) => {
                        format_error(&e);
                        process::exit(1);
                    }
                }
            }
        }
        Commands::Interp { file, program_args } => {
            let source = read_source(&file);
            let program = lex_and_parse(&source);

            let mut checker = Checker::new();
            if let Err(errors) = checker.check_program(&program) {
                for e in &errors {
                    eprintln!("{}", e.to_json());
                }
            }

            let mut interp = Interpreter::new();
            interp.set_args(program_args);
            interp.set_source_dir(get_source_dir(&file));
            match interp.run(&program) {
                Ok(_) => {}
                Err(e) => {
                    eprintln!("{}", e.to_json());
                    process::exit(1);
                }
            }
        }
        Commands::Check { file } => {
            let source = read_source(&file);
            let program = lex_and_parse(&source);

            let mut checker = Checker::new();
            match checker.check_program(&program) {
                Ok(()) => {
                    println!("{{\"status\":\"ok\",\"file\":\"{file}\"}}");
                }
                Err(errors) => {
                    for e in &errors {
                        eprintln!("{}", e.to_json());
                    }
                    process::exit(1);
                }
            }
        }
        Commands::Fmt { file } => {
            let source = read_source(&file);
            let program = lex_and_parse(&source);
            print!("{}", formatter::format_program(&program));
        }
        Commands::Test { file, filter } => {
            let source = read_source(&file);
            let program = lex_and_parse(&source);

            let mut test_names: Vec<String> = Vec::new();
            for item in &program.items {
                if let TopLevelKind::FnDecl(f) = &item.kind {
                    if f.name.starts_with("test_") {
                        if let Some(ref filt) = filter {
                            if !f.name.contains(filt.as_str()) {
                                continue;
                            }
                        }
                        test_names.push(f.name.clone());
                    }
                }
            }

            if test_names.is_empty() {
                eprintln!("\x1b[33mno test_* functions found in {file}\x1b[0m");
                process::exit(1);
            }

            let mut compiler = Compiler::new();
            compiler.set_source_dir(get_source_dir(&file));
            let compiled = compiler.compile_program(&program);

            let total = test_names.len();
            let mut passed = 0usize;
            let mut failed = 0usize;
            let mut failures: Vec<(String, String)> = Vec::new();

            let suite_start = Instant::now();

            for name in &test_names {
                let mut vm = VM::new(compiled.clone());
                vm.set_test_mode(true);

                let t0 = Instant::now();
                let result = vm.run_function(name);
                let elapsed = t0.elapsed();
                let ms = elapsed.as_secs_f64() * 1000.0;

                match result {
                    Ok(_) => {
                        passed += 1;
                        println!(
                            "  \x1b[32m\u{2713}\x1b[0m {} \x1b[2m({:.1}ms)\x1b[0m",
                            name, ms
                        );
                    }
                    Err(e) => {
                        failed += 1;
                        let msg = e.message.clone();
                        failures.push((name.clone(), msg.clone()));
                        println!(
                            "  \x1b[31m\u{2717}\x1b[0m {} \x1b[2m({:.1}ms)\x1b[0m",
                            name, ms
                        );
                        println!("    \x1b[31m{}\x1b[0m", msg);
                        for frame in &e.stack {
                            println!("    \x1b[2mat {} (line {})\x1b[0m", frame.function, frame.line);
                        }
                    }
                }
            }

            let suite_ms = suite_start.elapsed().as_secs_f64() * 1000.0;
            println!();

            if failed == 0 {
                println!(
                    "\x1b[32m\x1b[1m{passed} passed\x1b[0m \x1b[2m({total} tests in {suite_ms:.0}ms)\x1b[0m"
                );
            } else {
                println!(
                    "\x1b[31m\x1b[1m{failed} failed\x1b[0m, \x1b[32m{passed} passed\x1b[0m \x1b[2m({total} tests in {suite_ms:.0}ms)\x1b[0m"
                );
                process::exit(1);
            }
        }
        Commands::Ast { file } => {
            let source = read_source(&file);
            let program = lex_and_parse(&source);
            match serde_json::to_string_pretty(&program) {
                Ok(json) => println!("{json}"),
                Err(e) => {
                    eprintln!("{{\"error\":\"serialization failed: {e}\"}}");
                    process::exit(1);
                }
            }
        }
        Commands::Compile { file, output } => {
            let source = read_source(&file);
            let program = lex_and_parse(&source);

            let mut compiler = Compiler::new();
            compiler.set_source_dir(get_source_dir(&file));
            let compiled = compiler.compile_program(&program);

            let value = bridge::program_to_value(&compiled);
            let json = value_to_json(&value);
            let json_str = match serde_json::to_string_pretty(&json) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("{{\"error\":\"serialization failed: {e}\"}}");
                    process::exit(1);
                }
            };

            let out_path = output.unwrap_or_else(|| {
                file.strip_suffix(".a").unwrap_or(&file).to_string() + ".ac"
            });
            if let Err(e) = std::fs::write(&out_path, &json_str) {
                eprintln!("{{\"error\":\"cannot write '{out_path}': {e}\"}}");
                process::exit(1);
            }
            eprintln!("compiled {} -> {}", file, out_path);
        }
    }
}

fn load_ac(json_str: &str) -> a_lang::bytecode::CompiledProgram {
    let json_val: serde_json::Value = match serde_json::from_str(json_str) {
        Ok(v) => v,
        Err(e) => {
            eprintln!("{{\"error\":\"invalid .ac JSON: {e}\"}}");
            process::exit(1);
        }
    };
    let value = json_to_value(&json_val);
    match bridge::value_to_program(&value) {
        Ok(prog) => prog,
        Err(e) => {
            eprintln!("{{\"error\":\"invalid .ac format: {e}\"}}");
            process::exit(1);
        }
    }
}
