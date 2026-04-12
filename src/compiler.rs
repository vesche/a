use std::collections::HashSet;
use std::path::PathBuf;

use crate::ast::*;
use crate::bridge;
use crate::builtins::{json_to_value, value_to_json};
use crate::bytecode::*;
use crate::interpreter::Value;
use crate::lexer::Lexer;
use crate::parser::Parser;

struct LoopCtx {
    break_patches: Vec<usize>,
    continue_patches: Vec<usize>,
}

struct PendingCache {
    ac_path: PathBuf,
    namespace: String,
    fn_names: Vec<String>,
    use_paths: Vec<Vec<String>>,
}

pub struct Compiler {
    locals: Vec<String>,
    loops: Vec<LoopCtx>,
    source_dir: PathBuf,
    loaded_modules: HashSet<PathBuf>,
    pending_fns: Vec<FnDecl>,
    lambda_counter: usize,
    precompiled_fns: Vec<CompiledFn>,
    pending_caches: Vec<PendingCache>,
}

impl Compiler {
    pub fn new() -> Self {
        Compiler {
            locals: Vec::new(),
            loops: Vec::new(),
            source_dir: std::env::current_dir().unwrap_or_default(),
            loaded_modules: HashSet::new(),
            pending_fns: Vec::new(),
            lambda_counter: 0,
            precompiled_fns: Vec::new(),
            pending_caches: Vec::new(),
        }
    }

    pub fn set_source_dir(&mut self, dir: PathBuf) {
        self.source_dir = dir;
    }

    pub fn compile_program(&mut self, program: &Program) -> CompiledProgram {
        let mut all_fn_decls: Vec<FnDecl> = Vec::new();

        for item in &program.items {
            self.collect_top_level(item, "", &mut all_fn_decls);
        }

        let mut functions = Vec::new();
        let mut main_idx = None;

        let mut i = 0;
        while i < all_fn_decls.len() {
            let f = all_fn_decls[i].clone();
            let compiled = self.compile_fn(&f);

            while !self.pending_fns.is_empty() {
                let extra: Vec<_> = self.pending_fns.drain(..).collect();
                for ef in extra {
                    all_fn_decls.push(ef);
                }
            }

            if f.name == "main" {
                main_idx = Some(functions.len());
            }
            functions.push(compiled);
            i += 1;
        }

        functions.extend(self.precompiled_fns.drain(..));

        self.flush_caches(&functions);

        CompiledProgram { functions, main_idx }
    }

    fn collect_top_level(&mut self, item: &TopLevel, namespace: &str, fns: &mut Vec<FnDecl>) {
        match &item.kind {
            TopLevelKind::FnDecl(f) => {
                if !namespace.is_empty() {
                    let mut namespaced = f.clone();
                    namespaced.name = format!("{namespace}.{}", f.name);
                    fns.push(namespaced);
                }
                fns.push(f.clone());
            }
            TopLevelKind::ModDecl(m) => {
                let ns = if namespace.is_empty() {
                    m.name.clone()
                } else {
                    format!("{namespace}.{}", m.name)
                };
                for sub in &m.items {
                    self.collect_top_level(sub, &ns, fns);
                }
            }
            TopLevelKind::UseDecl(u) => {
                self.load_module(&u.path, fns);
            }
            TopLevelKind::TypeDecl(_) => {}
            TopLevelKind::ExternFn(_) => {}
        }
    }

    fn load_module(&mut self, path: &[String], fns: &mut Vec<FnDecl>) {
        if path.is_empty() { return; }
        let namespace = path.last().unwrap().clone();
        let rel_path = path.join("/");

        let mut file_path = self.source_dir.join(&rel_path);
        file_path.set_extension("a");

        if !file_path.exists() {
            let mut cwd_path = std::env::current_dir().unwrap_or_default().join(&rel_path);
            cwd_path.set_extension("a");
            if cwd_path.exists() {
                file_path = cwd_path;
            }
        }

        let canonical = file_path.canonicalize().unwrap_or_else(|_| file_path.clone());
        if self.loaded_modules.contains(&canonical) { return; }
        self.loaded_modules.insert(canonical);

        let mut ac_path = file_path.clone();
        ac_path.set_extension("ac");
        if let Some(cached) = self.try_load_cached(&file_path, &ac_path, &namespace, fns) {
            if cached { return; }
        }

        let source = match std::fs::read_to_string(&file_path) {
            Ok(s) => s,
            Err(_) => return,
        };
        let mut lexer = Lexer::new(&source);
        let tokens = match lexer.tokenize() {
            Ok(t) => t,
            Err(e) => { eprintln!("[module-load] lex error in {}: {e}", file_path.display()); return; },
        };
        let mut parser = Parser::new(tokens);
        let program = match parser.parse_program() {
            Ok(p) => p,
            Err(e) => { eprintln!("[module-load] parse error in {}: {e}", file_path.display()); return; },
        };

        let mut use_paths: Vec<Vec<String>> = Vec::new();
        let mut module_fn_decls: Vec<FnDecl> = Vec::new();

        for item in &program.items {
            match &item.kind {
                TopLevelKind::FnDecl(f) => {
                    module_fn_decls.push(f.clone());
                    let mut namespaced = f.clone();
                    namespaced.name = format!("{namespace}.{}", f.name);
                    fns.push(namespaced);
                    fns.push(f.clone());
                }
                TopLevelKind::UseDecl(u) => {
                    use_paths.push(u.path.clone());
                    self.load_module(&u.path, fns);
                }
                _ => {}
            }
        }

        let fn_names: Vec<String> = module_fn_decls.iter().map(|f| f.name.clone()).collect();
        self.pending_caches.push(PendingCache { ac_path, namespace: namespace.clone(), fn_names, use_paths });
    }

    fn try_load_cached(&mut self, source_path: &PathBuf, ac_path: &PathBuf, namespace: &str, fns: &mut Vec<FnDecl>) -> Option<bool> {
        if !ac_path.exists() { return Some(false); }
        let src_modified = source_path.metadata().ok()?.modified().ok()?;
        let ac_modified = ac_path.metadata().ok()?.modified().ok()?;
        if ac_modified < src_modified { return Some(false); }

        let json_str = std::fs::read_to_string(ac_path).ok()?;
        let json_val: serde_json::Value = serde_json::from_str(&json_str).ok()?;
        let envelope = json_val.as_object()?;

        if let Some(uses_val) = envelope.get("uses") {
            if let Some(uses_arr) = uses_val.as_array() {
                for u in uses_arr {
                    if let Some(path_arr) = u.as_array() {
                        let use_path: Vec<String> = path_arr.iter()
                            .filter_map(|s| s.as_str().map(String::from))
                            .collect();
                        self.load_module(&use_path, fns);
                    }
                }
            }
        }

        let program_val = envelope.get("program")?;
        let value = json_to_value(program_val);
        let compiled = bridge::value_to_program(&value).ok()?;

        for cf in compiled.functions {
            let mut namespaced = cf.clone();
            namespaced.name = format!("{namespace}.{}", cf.name);
            self.precompiled_fns.push(namespaced);
            self.precompiled_fns.push(cf);
        }

        Some(true)
    }

    fn flush_caches(&self, all_functions: &[CompiledFn]) {
        let fn_map: std::collections::HashMap<&str, &CompiledFn> = all_functions
            .iter()
            .map(|f| (f.name.as_str(), f))
            .collect();

        for cache in &self.pending_caches {
            let mut functions: Vec<CompiledFn> = Vec::new();
            for name in &cache.fn_names {
                let namespaced = format!("{}.{}", cache.namespace, name);
                let cf = fn_map.get(namespaced.as_str()).or_else(|| fn_map.get(name.as_str()));
                if let Some(cf) = cf {
                    let mut bare = (*cf).clone();
                    bare.name = name.clone();
                    functions.push(bare);
                }
            }
            let prog = CompiledProgram { functions, main_idx: None };
            let value = bridge::program_to_value(&prog);
            let prog_json = value_to_json(&value);
            let uses_json: Vec<serde_json::Value> = cache.use_paths.iter()
                .map(|p| serde_json::Value::Array(
                    p.iter().map(|s| serde_json::Value::String(s.clone())).collect()
                ))
                .collect();
            let envelope = serde_json::json!({
                "program": prog_json,
                "uses": uses_json,
            });
            if let Ok(json_str) = serde_json::to_string(&envelope) {
                let _ = std::fs::write(&cache.ac_path, json_str);
            }
        }
    }

    fn compile_fn(&mut self, f: &FnDecl) -> CompiledFn {
        self.locals.clear();
        self.loops.clear();
        let mut chunk = Chunk::new();

        for p in &f.params {
            self.locals.push(p.name.clone());
        }

        self.compile_block(&f.body, &mut chunk);

        let idx = chunk.add_constant(Value::Void);
        chunk.emit(Op::Const(idx));
        chunk.emit(Op::Return);

        Self::optimize_tail_calls(&mut chunk);

        CompiledFn {
            name: f.name.clone(),
            params: f.params.iter().map(|p| p.name.clone()).collect(),
            chunk,
            locals: self.locals.clone(),
        }
    }

    fn compile_block(&mut self, block: &Block, chunk: &mut Chunk) {
        for stmt in &block.stmts {
            self.compile_stmt(stmt, chunk);
        }
    }

    fn compile_stmt(&mut self, stmt: &Stmt, chunk: &mut Chunk) {
        if let Some(span) = &stmt.span {
            chunk.set_line(span.line as u32);
        }
        match &stmt.kind {
            StmtKind::Let { name, value, .. } => {
                self.compile_expr(value, chunk);
                let idx = self.locals.len();
                self.locals.push(name.clone());
                chunk.emit(Op::SetLocal(idx));
            }
            StmtKind::Assign { target, value } => {
                match &target.kind {
                    ExprKind::Ident(name) => {
                        self.compile_expr(value, chunk);
                        if let Some(idx) = self.resolve_local(name) {
                            chunk.emit(Op::SetLocal(idx));
                        } else {
                            let si = chunk.intern_string(name.clone());
                            chunk.emit(Op::SetGlobal(si));
                        }
                    }
                    ExprKind::Index { expr: arr, index } => {
                        self.compile_expr(arr, chunk);
                        self.compile_expr(index, chunk);
                        self.compile_expr(value, chunk);
                        chunk.emit(Op::IndexSet);
                        if let ExprKind::Ident(name) = &arr.kind {
                            if let Some(idx) = self.resolve_local(name) {
                                chunk.emit(Op::SetLocal(idx));
                            }
                        }
                    }
                    _ => {
                        self.compile_expr(value, chunk);
                        chunk.emit(Op::Pop);
                    }
                }
            }
            StmtKind::Return(expr) => {
                self.compile_expr(expr, chunk);
                chunk.emit(Op::Return);
            }
            StmtKind::Expr(expr) => {
                self.compile_expr(expr, chunk);
                chunk.emit(Op::Pop);
            }
            StmtKind::If { cond, then_block, else_block } => {
                self.compile_expr(cond, chunk);
                let jump_else = chunk.emit(Op::JumpIfFalse(0));
                self.compile_block(then_block, chunk);
                let jump_end = chunk.emit(Op::Jump(0));
                chunk.patch_jump(jump_else);
                if let Some(eb) = else_block {
                    match eb {
                        ElseBranch::Block(b) => self.compile_block(b, chunk),
                        ElseBranch::If(s) => self.compile_stmt(s, chunk),
                    }
                }
                chunk.patch_jump(jump_end);
            }
            StmtKind::While { cond, body } => {
                let loop_start = chunk.code.len();
                self.loops.push(LoopCtx { break_patches: Vec::new(), continue_patches: Vec::new() });

                self.compile_expr(cond, chunk);
                let exit_jump = chunk.emit(Op::JumpIfFalse(0));
                self.compile_block(body, chunk);

                let continue_target = chunk.code.len();
                chunk.emit(Op::Loop(loop_start));
                chunk.patch_jump(exit_jump);

                let ctx = self.loops.pop().unwrap();
                for bp in ctx.break_patches { chunk.patch_jump(bp); }
                for cp in ctx.continue_patches {
                    match &mut chunk.code[cp] {
                        Op::Jump(ref mut t) => *t = continue_target,
                        _ => {}
                    }
                }
            }
            StmtKind::For { var, iter, body, .. } => {
                self.compile_expr(iter, chunk);
                let arr_local = self.locals.len();
                self.locals.push("__iter__".into());
                chunk.emit(Op::SetLocal(arr_local));

                let idx_const = chunk.add_constant(Value::Int(0));
                chunk.emit(Op::Const(idx_const));
                let idx_local = self.locals.len();
                self.locals.push("__idx__".into());
                chunk.emit(Op::SetLocal(idx_local));

                let var_local = self.locals.len();
                self.locals.push(var.clone());

                let loop_start = chunk.code.len();
                self.loops.push(LoopCtx { break_patches: Vec::new(), continue_patches: Vec::new() });

                chunk.emit(Op::GetLocal(idx_local));
                chunk.emit(Op::GetLocal(arr_local));
                let len_si = chunk.intern_string("len".into());
                chunk.emit(Op::Call(len_si, 1));
                chunk.emit(Op::Lt);
                let exit_jump = chunk.emit(Op::JumpIfFalse(0));

                chunk.emit(Op::GetLocal(arr_local));
                chunk.emit(Op::GetLocal(idx_local));
                chunk.emit(Op::Index);
                chunk.emit(Op::SetLocal(var_local));

                self.compile_block(body, chunk);

                let continue_target = chunk.code.len();
                chunk.emit(Op::GetLocal(idx_local));
                let one = chunk.add_constant(Value::Int(1));
                chunk.emit(Op::Const(one));
                chunk.emit(Op::Add);
                chunk.emit(Op::SetLocal(idx_local));
                chunk.emit(Op::Loop(loop_start));
                chunk.patch_jump(exit_jump);

                let ctx = self.loops.pop().unwrap();
                for bp in ctx.break_patches { chunk.patch_jump(bp); }
                for cp in ctx.continue_patches {
                    match &mut chunk.code[cp] {
                        Op::Jump(ref mut t) => *t = continue_target,
                        _ => {}
                    }
                }
            }
            StmtKind::LetDestructure { bindings, rest, value } => {
                self.compile_expr(value, chunk);
                let arr_tmp = self.locals.len();
                self.locals.push("__destructure__".into());
                chunk.emit(Op::SetLocal(arr_tmp));

                for (i, binding) in bindings.iter().enumerate() {
                    if let Some(name) = binding {
                        chunk.emit(Op::GetLocal(arr_tmp));
                        let idx = chunk.add_constant(Value::Int(i as i64));
                        chunk.emit(Op::Const(idx));
                        chunk.emit(Op::Index);
                        let local = self.locals.len();
                        self.locals.push(name.clone());
                        chunk.emit(Op::SetLocal(local));
                    }
                }

                if let Some(rest_name) = rest {
                    chunk.emit(Op::GetLocal(arr_tmp));
                    let n_idx = chunk.add_constant(Value::Int(bindings.len() as i64));
                    chunk.emit(Op::Const(n_idx));
                    let drop_si = chunk.intern_string("drop".into());
                    chunk.emit(Op::Call(drop_si, 2));
                    let local = self.locals.len();
                    self.locals.push(rest_name.clone());
                    chunk.emit(Op::SetLocal(local));
                }
            }
            StmtKind::ForDestructure { bindings, iter, body } => {
                self.compile_expr(iter, chunk);
                let arr_local = self.locals.len();
                self.locals.push("__iter__".into());
                chunk.emit(Op::SetLocal(arr_local));

                let idx_const = chunk.add_constant(Value::Int(0));
                chunk.emit(Op::Const(idx_const));
                let idx_local = self.locals.len();
                self.locals.push("__idx__".into());
                chunk.emit(Op::SetLocal(idx_local));

                let elem_local = self.locals.len();
                self.locals.push("__elem__".into());

                let bind_locals: Vec<usize> = bindings.iter().map(|name| {
                    let local = self.locals.len();
                    self.locals.push(name.clone());
                    local
                }).collect();

                let loop_start = chunk.code.len();
                self.loops.push(LoopCtx { break_patches: Vec::new(), continue_patches: Vec::new() });

                chunk.emit(Op::GetLocal(idx_local));
                chunk.emit(Op::GetLocal(arr_local));
                let len_si = chunk.intern_string("len".into());
                chunk.emit(Op::Call(len_si, 1));
                chunk.emit(Op::Lt);
                let exit_jump = chunk.emit(Op::JumpIfFalse(0));

                chunk.emit(Op::GetLocal(arr_local));
                chunk.emit(Op::GetLocal(idx_local));
                chunk.emit(Op::Index);
                chunk.emit(Op::SetLocal(elem_local));

                for (i, &bl) in bind_locals.iter().enumerate() {
                    chunk.emit(Op::GetLocal(elem_local));
                    let ci = chunk.add_constant(Value::Int(i as i64));
                    chunk.emit(Op::Const(ci));
                    chunk.emit(Op::Index);
                    chunk.emit(Op::SetLocal(bl));
                }

                self.compile_block(body, chunk);

                let continue_target = chunk.code.len();
                chunk.emit(Op::GetLocal(idx_local));
                let one = chunk.add_constant(Value::Int(1));
                chunk.emit(Op::Const(one));
                chunk.emit(Op::Add);
                chunk.emit(Op::SetLocal(idx_local));
                chunk.emit(Op::Loop(loop_start));
                chunk.patch_jump(exit_jump);

                let ctx = self.loops.pop().unwrap();
                for bp in ctx.break_patches { chunk.patch_jump(bp); }
                for cp in ctx.continue_patches {
                    match &mut chunk.code[cp] {
                        Op::Jump(ref mut t) => *t = continue_target,
                        _ => {}
                    }
                }
            }
            StmtKind::Break => {
                let bp = chunk.emit(Op::Jump(0));
                if let Some(ctx) = self.loops.last_mut() {
                    ctx.break_patches.push(bp);
                }
            }
            StmtKind::Continue => {
                let cp = chunk.emit(Op::Jump(0));
                if let Some(ctx) = self.loops.last_mut() {
                    ctx.continue_patches.push(cp);
                }
            }
            StmtKind::Match { expr, arms } => {
                self.compile_match_stmt(expr, arms, chunk);
            }
        }
    }

    fn compile_match_stmt(&mut self, expr: &Expr, arms: &[MatchArm], chunk: &mut Chunk) {
        self.compile_expr(expr, chunk);
        let mut end_jumps = Vec::new();

        for arm in arms {
            chunk.emit(Op::Dup);
            match &arm.pattern {
                Pattern::Wildcard | Pattern::Ident(_) if arm.guard.is_some() => {
                    if let Pattern::Ident(name) = &arm.pattern {
                        let idx = self.locals.len();
                        self.locals.push(name.clone());
                        chunk.emit(Op::SetLocal(idx));
                    } else {
                        chunk.emit(Op::Pop);
                    }
                    let guard_skip = self.compile_match_guard(&arm.guard, chunk).unwrap();
                    chunk.emit(Op::Pop);
                    self.compile_match_body(&arm.body, chunk);
                    end_jumps.push(chunk.emit(Op::Jump(0)));
                    chunk.patch_jump(guard_skip);
                }
                Pattern::Wildcard | Pattern::Ident(_) => {
                    if let Pattern::Ident(name) = &arm.pattern {
                        let idx = self.locals.len();
                        self.locals.push(name.clone());
                        chunk.emit(Op::SetLocal(idx));
                    } else {
                        chunk.emit(Op::Pop);
                    }
                    chunk.emit(Op::Pop);
                    self.compile_match_body(&arm.body, chunk);
                    end_jumps.push(chunk.emit(Op::Jump(0)));
                    break;
                }
                Pattern::Literal(lit) => {
                    let val = match lit {
                        Literal::Int(v) => Value::Int(*v),
                        Literal::Float(v) => Value::Float(*v),
                        Literal::String(v) => Value::string(v.clone()),
                        Literal::Bool(v) => Value::Bool(*v),
                    };
                    let cidx = chunk.add_constant(val);
                    let skip = chunk.emit(Op::MatchLiteral(cidx, 0));
                    let guard_skip = self.compile_match_guard(&arm.guard, chunk);
                    chunk.emit(Op::Pop);
                    self.compile_match_body(&arm.body, chunk);
                    end_jumps.push(chunk.emit(Op::Jump(0)));
                    chunk.patch_jump(skip);
                    if let Some(gs) = guard_skip { chunk.patch_jump(gs); }
                    chunk.emit(Op::Pop);
                }
                Pattern::Constructor(tag, sub_patterns) => {
                    let tag_si = chunk.intern_string(tag.clone());
                    let skip = chunk.emit(Op::MatchVariant(tag_si, 0));
                    for (i, sub_pat) in sub_patterns.iter().enumerate() {
                        if let Pattern::Ident(name) = sub_pat {
                            chunk.emit(Op::Dup);
                            let cidx = chunk.add_constant(Value::Int(i as i64));
                            chunk.emit(Op::Const(cidx));
                            let vf_si = chunk.intern_string("__variant_field__".into());
                            chunk.emit(Op::Call(vf_si, 2));
                            let local_idx = self.locals.len();
                            self.locals.push(name.clone());
                            chunk.emit(Op::SetLocal(local_idx));
                        }
                    }
                    chunk.emit(Op::Pop);
                    self.compile_match_body(&arm.body, chunk);
                    end_jumps.push(chunk.emit(Op::Jump(0)));
                    chunk.patch_jump(skip);
                    chunk.emit(Op::Pop);
                }
                Pattern::Array(elems) => {
                    let fail_jumps = self.compile_array_pattern(elems, chunk);
                    let guard_skip = self.compile_match_guard(&arm.guard, chunk);
                    chunk.emit(Op::Pop);
                    self.compile_match_body(&arm.body, chunk);
                    end_jumps.push(chunk.emit(Op::Jump(0)));
                    for fj in fail_jumps { chunk.patch_jump(fj); }
                    if let Some(gs) = guard_skip { chunk.patch_jump(gs); }
                    chunk.emit(Op::Pop);
                }
                Pattern::Map(entries) => {
                    let fail_jumps = self.compile_map_pattern(entries, chunk);
                    let guard_skip = self.compile_match_guard(&arm.guard, chunk);
                    chunk.emit(Op::Pop);
                    self.compile_match_body(&arm.body, chunk);
                    end_jumps.push(chunk.emit(Op::Jump(0)));
                    for fj in fail_jumps { chunk.patch_jump(fj); }
                    if let Some(gs) = guard_skip { chunk.patch_jump(gs); }
                    chunk.emit(Op::Pop);
                }
            }
        }
        chunk.emit(Op::Pop);
        for ej in end_jumps {
            chunk.patch_jump(ej);
        }
    }

    fn compile_match_body(&mut self, body: &MatchBody, chunk: &mut Chunk) {
        match body {
            MatchBody::Expr(e) => {
                self.compile_expr(e, chunk);
                chunk.emit(Op::Pop);
            }
            MatchBody::Block(b) => {
                self.compile_block(b, chunk);
            }
        }
    }

    fn compile_expr(&mut self, expr: &Expr, chunk: &mut Chunk) {
        if let Some(span) = &expr.span {
            chunk.set_line(span.line as u32);
        }
        match &expr.kind {
            ExprKind::Literal(lit) => {
                let val = match lit {
                    Literal::Int(v) => Value::Int(*v),
                    Literal::Float(v) => Value::Float(*v),
                    Literal::String(v) => Value::string(v.clone()),
                    Literal::Bool(v) => Value::Bool(*v),
                };
                let idx = chunk.add_constant(val);
                chunk.emit(Op::Const(idx));
            }
            ExprKind::Ident(name) => {
                if let Some(idx) = self.resolve_local(name) {
                    chunk.emit(Op::GetLocal(idx));
                } else {
                    let si = chunk.intern_string(name.clone());
                    chunk.emit(Op::GetGlobal(si));
                }
            }
            ExprKind::BinOp { op, left, right } => {
                match op {
                    BinOp::And => {
                        self.compile_expr(left, chunk);
                        let short = chunk.emit(Op::JumpIfFalseKeep(0));
                        chunk.emit(Op::Pop);
                        self.compile_expr(right, chunk);
                        chunk.patch_jump(short);
                    }
                    BinOp::Or => {
                        self.compile_expr(left, chunk);
                        let jump_true = chunk.emit(Op::JumpIfFalseKeep(0));
                        let skip = chunk.emit(Op::Jump(0));
                        chunk.patch_jump(jump_true);
                        chunk.emit(Op::Pop);
                        self.compile_expr(right, chunk);
                        chunk.patch_jump(skip);
                    }
                    _ => {
                        self.compile_expr(left, chunk);
                        self.compile_expr(right, chunk);
                        match op {
                            BinOp::Add => chunk.emit(Op::Add),
                            BinOp::Sub => chunk.emit(Op::Sub),
                            BinOp::Mul => chunk.emit(Op::Mul),
                            BinOp::Div => chunk.emit(Op::Div),
                            BinOp::Mod => chunk.emit(Op::Mod),
                            BinOp::Eq => chunk.emit(Op::Eq),
                            BinOp::NotEq => chunk.emit(Op::NotEq),
                            BinOp::Lt => chunk.emit(Op::Lt),
                            BinOp::Gt => chunk.emit(Op::Gt),
                            BinOp::LtEq => chunk.emit(Op::LtEq),
                            BinOp::GtEq => chunk.emit(Op::GtEq),
                            BinOp::And | BinOp::Or => unreachable!(),
                        };
                    }
                }
            }
            ExprKind::UnaryOp { op, expr: inner } => {
                self.compile_expr(inner, chunk);
                match op {
                    UnaryOp::Neg => chunk.emit(Op::Neg),
                    UnaryOp::Not => chunk.emit(Op::Not),
                };
            }
            ExprKind::Call { func, args } => {
                if let ExprKind::FieldAccess { expr: obj, field } = &func.kind {
                    if let ExprKind::Ident(module) = &obj.kind {
                        for a in args {
                            self.compile_expr(a, chunk);
                        }
                        let mi = chunk.intern_string(module.clone());
                        let fi = chunk.intern_string(field.clone());
                        chunk.emit(Op::CallMethod(mi, fi, args.len()));
                        return;
                    }
                }
                if let ExprKind::Ident(name) = &func.kind {
                    if self.resolve_local(name).is_some() {
                        for a in args {
                            self.compile_expr(a, chunk);
                        }
                        self.compile_expr(func, chunk);
                        chunk.emit(Op::CallClosure(args.len()));
                        return;
                    }
                    for a in args {
                        self.compile_expr(a, chunk);
                    }
                    let si = chunk.intern_string(name.clone());
                    chunk.emit(Op::Call(si, args.len()));
                    return;
                }
                // Indirect call (closure)
                for a in args {
                    self.compile_expr(a, chunk);
                }
                self.compile_expr(func, chunk);
                chunk.emit(Op::CallClosure(args.len()));
            }
            ExprKind::FieldAccess { expr: inner, field } => {
                self.compile_expr(inner, chunk);
                let fi = chunk.intern_string(field.clone());
                chunk.emit(Op::Field(fi));
            }
            ExprKind::Index { expr: arr, index } => {
                self.compile_expr(arr, chunk);
                self.compile_expr(index, chunk);
                chunk.emit(Op::Index);
            }
            ExprKind::Array(elems) => {
                let has_spread = elems.iter().any(|e| matches!(e.kind, ExprKind::Spread(_)));
                if !has_spread {
                    for e in elems {
                        self.compile_expr(e, chunk);
                    }
                    chunk.emit(Op::Array(elems.len()));
                } else {
                    chunk.emit(Op::Array(0));
                    let push_si = chunk.intern_string("push".into());
                    let concat_si = chunk.intern_string("concat_arr".into());
                    for e in elems {
                        if let ExprKind::Spread(inner) = &e.kind {
                            self.compile_expr(inner, chunk);
                            chunk.emit(Op::Call(concat_si, 2));
                        } else {
                            self.compile_expr(e, chunk);
                            chunk.emit(Op::Call(push_si, 2));
                        }
                    }
                }
            }
            ExprKind::Spread(_) => {
                let idx = chunk.add_constant(Value::Void);
                chunk.emit(Op::Const(idx));
            }
            ExprKind::Interpolation { parts } => {
                let to_str_si = chunk.intern_string("to_str".into());
                for part in parts {
                    match part {
                        StringPart::Lit(s) => {
                            let idx = chunk.add_constant(Value::string(s.clone()));
                            chunk.emit(Op::Const(idx));
                        }
                        StringPart::Expr(e) => {
                            self.compile_expr(e, chunk);
                            chunk.emit(Op::Call(to_str_si, 1));
                        }
                    }
                }
                chunk.emit(Op::Concat(parts.len()));
            }
            ExprKind::Record(fields) => {
                for (name, e) in fields {
                    let idx = chunk.add_constant(Value::string(name.clone()));
                    chunk.emit(Op::Const(idx));
                    self.compile_expr(e, chunk);
                }
                chunk.emit(Op::Record(fields.len()));
            }
            ExprKind::MapLiteral(entries) => {
                for (k, v) in entries {
                    self.compile_expr(k, chunk);
                    self.compile_expr(v, chunk);
                }
                chunk.emit(Op::MapNew(entries.len()));
            }
            ExprKind::TryBlock(block) => {
                let try_start = chunk.emit(Op::TryStart(0));
                self.compile_block(block, chunk);
                chunk.emit(Op::TryEnd);
                chunk.emit(Op::MakeOk);
                let skip_err = chunk.emit(Op::Jump(0));
                chunk.patch_jump(try_start);
                // error value is on stack from the ? unwrap failure
                chunk.emit(Op::MakeErr);
                chunk.patch_jump(skip_err);
            }
            ExprKind::Try(inner) => {
                self.compile_expr(inner, chunk);
                chunk.emit(Op::Unwrap);
            }
            ExprKind::Pipe { left, right } => {
                self.compile_expr(left, chunk);
                match &right.kind {
                    ExprKind::Call { func, args } => {
                        for a in args {
                            self.compile_expr(a, chunk);
                        }
                        if let ExprKind::Ident(name) = &func.kind {
                            if self.resolve_local(name).is_some() {
                                self.compile_expr(func, chunk);
                                chunk.emit(Op::CallClosure(args.len() + 1));
                            } else {
                                let si = chunk.intern_string(name.clone());
                                chunk.emit(Op::Call(si, args.len() + 1));
                            }
                        } else if let ExprKind::FieldAccess { expr: obj, field } = &func.kind {
                            if let ExprKind::Ident(module) = &obj.kind {
                                let mi = chunk.intern_string(module.clone());
                                let fi = chunk.intern_string(field.clone());
                                chunk.emit(Op::CallMethod(mi, fi, args.len() + 1));
                            }
                        } else {
                            self.compile_expr(func, chunk);
                            chunk.emit(Op::CallClosure(args.len() + 1));
                        }
                    }
                    ExprKind::Ident(name) => {
                        if self.resolve_local(name).is_some() {
                            self.compile_expr(right, chunk);
                            chunk.emit(Op::CallClosure(1));
                        } else {
                            let si = chunk.intern_string(name.clone());
                            chunk.emit(Op::Call(si, 1));
                        }
                    }
                    _ => {
                        self.compile_expr(right, chunk);
                        chunk.emit(Op::CallClosure(1));
                    }
                }
            }
            ExprKind::Lambda { params, body } => {
                let captures = self.collect_free_vars(body, params);

                self.lambda_counter += 1;
                let name = format!("__lambda_{}", self.lambda_counter);

                let mut all_params = Vec::new();
                for cap_name in &captures {
                    all_params.push(Param { name: cap_name.clone(), ty: TypeExpr::Infer });
                }
                all_params.extend(params.clone());

                let fn_decl = FnDecl {
                    name: name.clone(),
                    params: all_params,
                    ret_type: TypeExpr::Infer,
                    effects: Vec::new(),
                    pre: None,
                    post: None,
                    body: Block {
                        stmts: vec![Stmt {
                            kind: StmtKind::Return((**body).clone()),
                            span: body.span.clone(),
                        }],
                    },
                };
                self.pending_fns.push(fn_decl);

                for cap_name in &captures {
                    if let Some(idx) = self.resolve_local(cap_name) {
                        chunk.emit(Op::GetLocal(idx));
                    } else {
                        let ci = chunk.add_constant(Value::Void);
                        chunk.emit(Op::Const(ci));
                    }
                }

                let name_idx = chunk.add_constant(Value::string(name));
                chunk.emit(Op::MakeClosure(name_idx, captures.len()));
            }
            ExprKind::If { cond, then_block, else_block } => {
                self.compile_expr(cond, chunk);
                let jump_else = chunk.emit(Op::JumpIfFalse(0));
                self.compile_block_as_expr(then_block, chunk);
                let jump_end = chunk.emit(Op::Jump(0));
                chunk.patch_jump(jump_else);
                self.compile_block_as_expr(else_block, chunk);
                chunk.patch_jump(jump_end);
            }
            ExprKind::Match { expr: inner, arms } => {
                self.compile_expr(inner, chunk);
                let mut end_jumps = Vec::new();
                for arm in arms {
                    chunk.emit(Op::Dup);
                    match &arm.pattern {
                        Pattern::Wildcard | Pattern::Ident(_) if arm.guard.is_some() => {
                            if let Pattern::Ident(name) = &arm.pattern {
                                let idx = self.locals.len();
                                self.locals.push(name.clone());
                                chunk.emit(Op::SetLocal(idx));
                            } else {
                                chunk.emit(Op::Pop);
                            }
                            let guard_skip = self.compile_match_guard(&arm.guard, chunk).unwrap();
                            chunk.emit(Op::Pop);
                            self.compile_match_body_expr(&arm.body, chunk);
                            end_jumps.push(chunk.emit(Op::Jump(0)));
                            chunk.patch_jump(guard_skip);
                        }
                        Pattern::Wildcard | Pattern::Ident(_) => {
                            if let Pattern::Ident(name) = &arm.pattern {
                                let idx = self.locals.len();
                                self.locals.push(name.clone());
                                chunk.emit(Op::SetLocal(idx));
                            } else {
                                chunk.emit(Op::Pop);
                            }
                            chunk.emit(Op::Pop);
                            self.compile_match_body_expr(&arm.body, chunk);
                            end_jumps.push(chunk.emit(Op::Jump(0)));
                            break;
                        }
                        Pattern::Literal(lit) => {
                            let val = match lit {
                                Literal::Int(v) => Value::Int(*v),
                                Literal::Float(v) => Value::Float(*v),
                                Literal::String(v) => Value::string(v.clone()),
                                Literal::Bool(v) => Value::Bool(*v),
                            };
                            let cidx = chunk.add_constant(val);
                            let skip = chunk.emit(Op::MatchLiteral(cidx, 0));
                            let guard_skip = self.compile_match_guard(&arm.guard, chunk);
                            chunk.emit(Op::Pop);
                            self.compile_match_body_expr(&arm.body, chunk);
                            end_jumps.push(chunk.emit(Op::Jump(0)));
                            chunk.patch_jump(skip);
                            if let Some(gs) = guard_skip { chunk.patch_jump(gs); }
                            chunk.emit(Op::Pop);
                        }
                        Pattern::Constructor(tag, sub_patterns) => {
                            let tag_si = chunk.intern_string(tag.clone());
                            let skip = chunk.emit(Op::MatchVariant(tag_si, 0));
                            for (i, sub_pat) in sub_patterns.iter().enumerate() {
                                if let Pattern::Ident(name) = sub_pat {
                                    chunk.emit(Op::Dup);
                                    let cidx = chunk.add_constant(Value::Int(i as i64));
                                    chunk.emit(Op::Const(cidx));
                                    let vf_si = chunk.intern_string("__variant_field__".into());
                                    chunk.emit(Op::Call(vf_si, 2));
                                    let local_idx = self.locals.len();
                                    self.locals.push(name.clone());
                                    chunk.emit(Op::SetLocal(local_idx));
                                }
                            }
                            chunk.emit(Op::Pop);
                            self.compile_match_body_expr(&arm.body, chunk);
                            end_jumps.push(chunk.emit(Op::Jump(0)));
                            chunk.patch_jump(skip);
                            chunk.emit(Op::Pop);
                        }
                        Pattern::Array(elems) => {
                            let fail_jumps = self.compile_array_pattern(elems, chunk);
                            let guard_skip = self.compile_match_guard(&arm.guard, chunk);
                            chunk.emit(Op::Pop);
                            self.compile_match_body_expr(&arm.body, chunk);
                            end_jumps.push(chunk.emit(Op::Jump(0)));
                            for fj in fail_jumps { chunk.patch_jump(fj); }
                            if let Some(gs) = guard_skip { chunk.patch_jump(gs); }
                            chunk.emit(Op::Pop);
                        }
                        Pattern::Map(entries) => {
                            let fail_jumps = self.compile_map_pattern(entries, chunk);
                            let guard_skip = self.compile_match_guard(&arm.guard, chunk);
                            chunk.emit(Op::Pop);
                            self.compile_match_body_expr(&arm.body, chunk);
                            end_jumps.push(chunk.emit(Op::Jump(0)));
                            for fj in fail_jumps { chunk.patch_jump(fj); }
                            if let Some(gs) = guard_skip { chunk.patch_jump(gs); }
                            chunk.emit(Op::Pop);
                        }
                    }
                }
                let void_idx = chunk.add_constant(Value::Void);
                chunk.emit(Op::Pop);
                chunk.emit(Op::Const(void_idx));
                for ej in end_jumps {
                    chunk.patch_jump(ej);
                }
            }
            ExprKind::Block(block) => {
                self.compile_block_as_expr(block, chunk);
            }
        }
    }

    fn compile_block_as_expr(&mut self, block: &Block, chunk: &mut Chunk) {
        if block.stmts.is_empty() {
            let idx = chunk.add_constant(Value::Void);
            chunk.emit(Op::Const(idx));
            return;
        }
        let last = block.stmts.len() - 1;
        for (i, stmt) in block.stmts.iter().enumerate() {
            if i == last {
                if let StmtKind::Expr(e) = &stmt.kind {
                    self.compile_expr(e, chunk);
                    return;
                }
                if let StmtKind::Return(e) = &stmt.kind {
                    self.compile_expr(e, chunk);
                    return;
                }
            }
            self.compile_stmt(stmt, chunk);
        }
        let idx = chunk.add_constant(Value::Void);
        chunk.emit(Op::Const(idx));
    }

    fn compile_match_body_expr(&mut self, body: &MatchBody, chunk: &mut Chunk) {
        match body {
            MatchBody::Expr(e) => self.compile_expr(e, chunk),
            MatchBody::Block(b) => self.compile_block_as_expr(b, chunk),
        }
    }

    fn compile_match_guard(&mut self, guard: &Option<Expr>, chunk: &mut Chunk) -> Option<usize> {
        if let Some(g) = guard {
            self.compile_expr(g, chunk);
            Some(chunk.emit(Op::JumpIfFalse(0)))
        } else {
            None
        }
    }

    fn compile_array_pattern(&mut self, elems: &[ArrayPatElem], chunk: &mut Chunk) -> Vec<usize> {
        let mut fail_jumps = Vec::new();
        let mut has_rest = false;
        let mut fixed_count = 0usize;
        for elem in elems {
            match elem {
                ArrayPatElem::Pat(_) => fixed_count += 1,
                ArrayPatElem::Rest(_) => { has_rest = true; }
            }
        }

        let typeof_si = chunk.intern_string("type_of".into());
        chunk.emit(Op::Dup);
        chunk.emit(Op::Call(typeof_si, 1));
        let arr_str = chunk.add_constant(Value::string("array".into()));
        chunk.emit(Op::Const(arr_str));
        chunk.emit(Op::Eq);
        fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

        let len_si = chunk.intern_string("len".into());
        chunk.emit(Op::Dup);
        chunk.emit(Op::Call(len_si, 1));
        let expected_len = chunk.add_constant(Value::Int(fixed_count as i64));
        chunk.emit(Op::Const(expected_len));
        if has_rest {
            chunk.emit(Op::GtEq);
        } else {
            chunk.emit(Op::Eq);
        }
        fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

        let mut idx = 0usize;
        for elem in elems {
            match elem {
                ArrayPatElem::Pat(pat) => {
                    self.compile_elem_pattern(pat, idx, &mut fail_jumps, chunk);
                    idx += 1;
                }
                ArrayPatElem::Rest(name) => {
                    let drop_si = chunk.intern_string("drop".into());
                    chunk.emit(Op::Dup);
                    let n_cidx = chunk.add_constant(Value::Int(idx as i64));
                    chunk.emit(Op::Const(n_cidx));
                    chunk.emit(Op::Call(drop_si, 2));
                    let local_idx = self.locals.len();
                    self.locals.push(name.clone());
                    chunk.emit(Op::SetLocal(local_idx));
                }
            }
        }
        fail_jumps
    }

    fn compile_map_pattern(&mut self, entries: &[(String, Pattern)], chunk: &mut Chunk) -> Vec<usize> {
        let mut fail_jumps = Vec::new();

        let typeof_si = chunk.intern_string("type_of".into());
        chunk.emit(Op::Dup);
        chunk.emit(Op::Call(typeof_si, 1));
        let map_str = chunk.add_constant(Value::string("map".into()));
        chunk.emit(Op::Const(map_str));
        chunk.emit(Op::Eq);
        fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

        let has_si = chunk.intern_string("map.has".into());
        for (key, sub_pat) in entries {
            chunk.emit(Op::Dup);
            let key_cidx = chunk.add_constant(Value::string(key.clone()));
            chunk.emit(Op::Const(key_cidx));
            chunk.emit(Op::Call(has_si, 2));
            fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

            self.compile_map_value_pattern(key, sub_pat, &mut fail_jumps, chunk);
        }
        fail_jumps
    }

    fn compile_elem_pattern(&mut self, pat: &Pattern, idx: usize, fail_jumps: &mut Vec<usize>, chunk: &mut Chunk) {
        match pat {
            Pattern::Wildcard => {}
            Pattern::Ident(name) => {
                chunk.emit(Op::Dup);
                let cidx = chunk.add_constant(Value::Int(idx as i64));
                chunk.emit(Op::Const(cidx));
                chunk.emit(Op::Index);
                let local_idx = self.locals.len();
                self.locals.push(name.clone());
                chunk.emit(Op::SetLocal(local_idx));
            }
            Pattern::Literal(lit) => {
                chunk.emit(Op::Dup);
                let cidx = chunk.add_constant(Value::Int(idx as i64));
                chunk.emit(Op::Const(cidx));
                chunk.emit(Op::Index);
                let val = match lit {
                    Literal::Int(v) => Value::Int(*v),
                    Literal::Float(v) => Value::Float(*v),
                    Literal::String(v) => Value::string(v.clone()),
                    Literal::Bool(v) => Value::Bool(*v),
                };
                let ecidx = chunk.add_constant(val);
                chunk.emit(Op::Const(ecidx));
                chunk.emit(Op::Eq);
                fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));
            }
            Pattern::Array(_) | Pattern::Map(_) => {
                chunk.emit(Op::Dup);
                let cidx = chunk.add_constant(Value::Int(idx as i64));
                chunk.emit(Op::Const(cidx));
                chunk.emit(Op::Index);
                let tmp = self.locals.len();
                self.locals.push("__pat_tmp__".into());
                chunk.emit(Op::SetLocal(tmp));
                self.compile_nested_structural(pat, tmp, fail_jumps, chunk);
            }
            Pattern::Constructor(_, _) => {}
        }
    }

    fn compile_map_value_pattern(&mut self, key: &str, pat: &Pattern, fail_jumps: &mut Vec<usize>, chunk: &mut Chunk) {
        match pat {
            Pattern::Wildcard => {}
            Pattern::Ident(name) => {
                chunk.emit(Op::Dup);
                let key_cidx = chunk.add_constant(Value::string(key.to_string()));
                chunk.emit(Op::Const(key_cidx));
                chunk.emit(Op::Index);
                let local_idx = self.locals.len();
                self.locals.push(name.clone());
                chunk.emit(Op::SetLocal(local_idx));
            }
            Pattern::Literal(lit) => {
                chunk.emit(Op::Dup);
                let key_cidx = chunk.add_constant(Value::string(key.to_string()));
                chunk.emit(Op::Const(key_cidx));
                chunk.emit(Op::Index);
                let val = match lit {
                    Literal::Int(v) => Value::Int(*v),
                    Literal::Float(v) => Value::Float(*v),
                    Literal::String(v) => Value::string(v.clone()),
                    Literal::Bool(v) => Value::Bool(*v),
                };
                let cidx = chunk.add_constant(val);
                chunk.emit(Op::Const(cidx));
                chunk.emit(Op::Eq);
                fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));
            }
            Pattern::Array(_) | Pattern::Map(_) => {
                chunk.emit(Op::Dup);
                let key_cidx = chunk.add_constant(Value::string(key.to_string()));
                chunk.emit(Op::Const(key_cidx));
                chunk.emit(Op::Index);
                let tmp = self.locals.len();
                self.locals.push("__pat_tmp__".into());
                chunk.emit(Op::SetLocal(tmp));
                self.compile_nested_structural(pat, tmp, fail_jumps, chunk);
            }
            Pattern::Constructor(_, _) => {}
        }
    }

    fn compile_nested_structural(&mut self, pat: &Pattern, tmp_slot: usize, fail_jumps: &mut Vec<usize>, chunk: &mut Chunk) {
        match pat {
            Pattern::Array(elems) => {
                let mut has_rest = false;
                let mut fixed_count = 0usize;
                for elem in elems {
                    match elem {
                        ArrayPatElem::Pat(_) => fixed_count += 1,
                        ArrayPatElem::Rest(_) => { has_rest = true; }
                    }
                }

                let typeof_si = chunk.intern_string("type_of".into());
                chunk.emit(Op::GetLocal(tmp_slot));
                chunk.emit(Op::Call(typeof_si, 1));
                let arr_str = chunk.add_constant(Value::string("array".into()));
                chunk.emit(Op::Const(arr_str));
                chunk.emit(Op::Eq);
                fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

                let len_si = chunk.intern_string("len".into());
                chunk.emit(Op::GetLocal(tmp_slot));
                chunk.emit(Op::Call(len_si, 1));
                let expected_len = chunk.add_constant(Value::Int(fixed_count as i64));
                chunk.emit(Op::Const(expected_len));
                if has_rest {
                    chunk.emit(Op::GtEq);
                } else {
                    chunk.emit(Op::Eq);
                }
                fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

                let mut idx = 0usize;
                for elem in elems {
                    match elem {
                        ArrayPatElem::Pat(sub) => {
                            match sub {
                                Pattern::Wildcard => {}
                                Pattern::Ident(name) => {
                                    chunk.emit(Op::GetLocal(tmp_slot));
                                    let cidx = chunk.add_constant(Value::Int(idx as i64));
                                    chunk.emit(Op::Const(cidx));
                                    chunk.emit(Op::Index);
                                    let local_idx = self.locals.len();
                                    self.locals.push(name.clone());
                                    chunk.emit(Op::SetLocal(local_idx));
                                }
                                Pattern::Literal(lit) => {
                                    chunk.emit(Op::GetLocal(tmp_slot));
                                    let cidx = chunk.add_constant(Value::Int(idx as i64));
                                    chunk.emit(Op::Const(cidx));
                                    chunk.emit(Op::Index);
                                    let val = match lit {
                                        Literal::Int(v) => Value::Int(*v),
                                        Literal::Float(v) => Value::Float(*v),
                                        Literal::String(v) => Value::string(v.clone()),
                                        Literal::Bool(v) => Value::Bool(*v),
                                    };
                                    let ecidx = chunk.add_constant(val);
                                    chunk.emit(Op::Const(ecidx));
                                    chunk.emit(Op::Eq);
                                    fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));
                                }
                                Pattern::Array(_) | Pattern::Map(_) => {
                                    chunk.emit(Op::GetLocal(tmp_slot));
                                    let cidx = chunk.add_constant(Value::Int(idx as i64));
                                    chunk.emit(Op::Const(cidx));
                                    chunk.emit(Op::Index);
                                    let inner_tmp = self.locals.len();
                                    self.locals.push("__pat_tmp__".into());
                                    chunk.emit(Op::SetLocal(inner_tmp));
                                    self.compile_nested_structural(sub, inner_tmp, fail_jumps, chunk);
                                }
                                _ => {}
                            }
                            idx += 1;
                        }
                        ArrayPatElem::Rest(name) => {
                            let drop_si = chunk.intern_string("drop".into());
                            chunk.emit(Op::GetLocal(tmp_slot));
                            let n_cidx = chunk.add_constant(Value::Int(idx as i64));
                            chunk.emit(Op::Const(n_cidx));
                            chunk.emit(Op::Call(drop_si, 2));
                            let local_idx = self.locals.len();
                            self.locals.push(name.clone());
                            chunk.emit(Op::SetLocal(local_idx));
                        }
                    }
                }
            }
            Pattern::Map(entries) => {
                let typeof_si = chunk.intern_string("type_of".into());
                chunk.emit(Op::GetLocal(tmp_slot));
                chunk.emit(Op::Call(typeof_si, 1));
                let map_str = chunk.add_constant(Value::string("map".into()));
                chunk.emit(Op::Const(map_str));
                chunk.emit(Op::Eq);
                fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

                let has_si = chunk.intern_string("map.has".into());
                for (key, sub_pat) in entries {
                    chunk.emit(Op::GetLocal(tmp_slot));
                    let key_cidx = chunk.add_constant(Value::string(key.clone()));
                    chunk.emit(Op::Const(key_cidx));
                    chunk.emit(Op::Call(has_si, 2));
                    fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));

                    match sub_pat {
                        Pattern::Wildcard => {}
                        Pattern::Ident(name) => {
                            chunk.emit(Op::GetLocal(tmp_slot));
                            let kc = chunk.add_constant(Value::string(key.clone()));
                            chunk.emit(Op::Const(kc));
                            chunk.emit(Op::Index);
                            let local_idx = self.locals.len();
                            self.locals.push(name.clone());
                            chunk.emit(Op::SetLocal(local_idx));
                        }
                        Pattern::Literal(lit) => {
                            chunk.emit(Op::GetLocal(tmp_slot));
                            let kc = chunk.add_constant(Value::string(key.clone()));
                            chunk.emit(Op::Const(kc));
                            chunk.emit(Op::Index);
                            let val = match lit {
                                Literal::Int(v) => Value::Int(*v),
                                Literal::Float(v) => Value::Float(*v),
                                Literal::String(v) => Value::string(v.clone()),
                                Literal::Bool(v) => Value::Bool(*v),
                            };
                            let cidx = chunk.add_constant(val);
                            chunk.emit(Op::Const(cidx));
                            chunk.emit(Op::Eq);
                            fail_jumps.push(chunk.emit(Op::JumpIfFalse(0)));
                        }
                        Pattern::Array(_) | Pattern::Map(_) => {
                            chunk.emit(Op::GetLocal(tmp_slot));
                            let kc = chunk.add_constant(Value::string(key.clone()));
                            chunk.emit(Op::Const(kc));
                            chunk.emit(Op::Index);
                            let inner_tmp = self.locals.len();
                            self.locals.push("__pat_tmp__".into());
                            chunk.emit(Op::SetLocal(inner_tmp));
                            self.compile_nested_structural(sub_pat, inner_tmp, fail_jumps, chunk);
                        }
                        _ => {}
                    }
                }
            }
            _ => {}
        }
    }

    fn optimize_tail_calls(chunk: &mut Chunk) {
        let len = chunk.code.len();
        if len < 2 { return; }
        for i in 0..len - 1 {
            if !matches!(chunk.code[i + 1], Op::Return) { continue; }
            match chunk.code[i] {
                Op::Call(name_idx, nargs) => {
                    chunk.code[i] = Op::TailCall(name_idx, nargs);
                    chunk.code[i + 1] = Op::Pop;
                }
                Op::CallClosure(nargs) => {
                    chunk.code[i] = Op::TailCallClosure(nargs);
                    chunk.code[i + 1] = Op::Pop;
                }
                _ => {}
            }
        }
    }

    fn resolve_local(&self, name: &str) -> Option<usize> {
        self.locals.iter().rposition(|n| n == name)
    }

    fn collect_free_vars(&self, expr: &Expr, params: &[Param]) -> Vec<String> {
        let mut used = Vec::new();
        let mut defined: std::collections::HashSet<String> = params.iter().map(|p| p.name.clone()).collect();
        Self::walk_expr_free(expr, &mut used, &mut defined);
        let mut captures = Vec::new();
        let mut seen = std::collections::HashSet::new();
        for name in used {
            if seen.contains(&name) { continue; }
            if self.resolve_local(&name).is_some() {
                seen.insert(name.clone());
                captures.push(name);
            }
        }
        captures
    }

    fn walk_expr_free(expr: &Expr, used: &mut Vec<String>, defined: &mut std::collections::HashSet<String>) {
        match &expr.kind {
            ExprKind::Ident(name) => {
                if !defined.contains(name) {
                    used.push(name.clone());
                }
            }
            ExprKind::Literal(_) => {}
            ExprKind::BinOp { left, right, .. } => {
                Self::walk_expr_free(left, used, defined);
                Self::walk_expr_free(right, used, defined);
            }
            ExprKind::UnaryOp { expr: inner, .. } => {
                Self::walk_expr_free(inner, used, defined);
            }
            ExprKind::Call { func, args } => {
                Self::walk_expr_free(func, used, defined);
                for a in args { Self::walk_expr_free(a, used, defined); }
            }
            ExprKind::FieldAccess { expr: inner, .. } => {
                Self::walk_expr_free(inner, used, defined);
            }
            ExprKind::Index { expr: arr, index } => {
                Self::walk_expr_free(arr, used, defined);
                Self::walk_expr_free(index, used, defined);
            }
            ExprKind::Array(elems) => {
                for e in elems { Self::walk_expr_free(e, used, defined); }
            }
            ExprKind::Record(fields) => {
                for (_, e) in fields { Self::walk_expr_free(e, used, defined); }
            }
            ExprKind::MapLiteral(entries) => {
                for (k, v) in entries {
                    Self::walk_expr_free(k, used, defined);
                    Self::walk_expr_free(v, used, defined);
                }
            }
            ExprKind::If { cond, then_block, else_block } => {
                Self::walk_expr_free(cond, used, defined);
                Self::walk_block_free(then_block, used, defined);
                Self::walk_block_free(else_block, used, defined);
            }
            ExprKind::Match { expr: inner, arms } => {
                Self::walk_expr_free(inner, used, defined);
                for arm in arms {
                    let mut arm_defined = defined.clone();
                    Self::collect_pattern_bindings(&arm.pattern, &mut arm_defined);
                    if let Some(g) = &arm.guard {
                        Self::walk_expr_free(g, used, &mut arm_defined);
                    }
                    match &arm.body {
                        MatchBody::Expr(e) => Self::walk_expr_free(e, used, &mut arm_defined),
                        MatchBody::Block(b) => Self::walk_block_free(b, used, &mut arm_defined),
                    }
                }
            }
            ExprKind::Block(block) => {
                Self::walk_block_free(block, used, defined);
            }
            ExprKind::Lambda { params: inner_params, body } => {
                let mut inner_defined = defined.clone();
                for p in inner_params { inner_defined.insert(p.name.clone()); }
                Self::walk_expr_free(body, used, &mut inner_defined);
            }
            ExprKind::Pipe { left, right } => {
                Self::walk_expr_free(left, used, defined);
                Self::walk_expr_free(right, used, defined);
            }
            ExprKind::Interpolation { parts } => {
                for part in parts {
                    if let StringPart::Expr(e) = part {
                        Self::walk_expr_free(e, used, defined);
                    }
                }
            }
            ExprKind::Try(inner) => { Self::walk_expr_free(inner, used, defined); }
            ExprKind::TryBlock(block) => { Self::walk_block_free(block, used, defined); }
            ExprKind::Spread(inner) => { Self::walk_expr_free(inner, used, defined); }
        }
    }

    fn walk_block_free(block: &Block, used: &mut Vec<String>, defined: &mut std::collections::HashSet<String>) {
        for stmt in &block.stmts {
            Self::walk_stmt_free(stmt, used, defined);
        }
    }

    fn walk_stmt_free(stmt: &Stmt, used: &mut Vec<String>, defined: &mut std::collections::HashSet<String>) {
        match &stmt.kind {
            StmtKind::Let { name, value, .. } => {
                Self::walk_expr_free(value, used, defined);
                defined.insert(name.clone());
            }
            StmtKind::Assign { target, value } => {
                Self::walk_expr_free(target, used, defined);
                Self::walk_expr_free(value, used, defined);
            }
            StmtKind::Return(e) | StmtKind::Expr(e) => {
                Self::walk_expr_free(e, used, defined);
            }
            StmtKind::If { cond, then_block, else_block } => {
                Self::walk_expr_free(cond, used, defined);
                Self::walk_block_free(then_block, used, defined);
                if let Some(eb) = else_block {
                    match eb {
                        ElseBranch::Block(b) => Self::walk_block_free(b, used, defined),
                        ElseBranch::If(s) => Self::walk_stmt_free(s, used, defined),
                    }
                }
            }
            StmtKind::Match { expr, arms } => {
                Self::walk_expr_free(expr, used, defined);
                for arm in arms {
                    let mut arm_defined = defined.clone();
                    Self::collect_pattern_bindings(&arm.pattern, &mut arm_defined);
                    if let Some(g) = &arm.guard {
                        Self::walk_expr_free(g, used, &mut arm_defined);
                    }
                    match &arm.body {
                        MatchBody::Expr(e) => Self::walk_expr_free(e, used, &mut arm_defined),
                        MatchBody::Block(b) => Self::walk_block_free(b, used, &mut arm_defined),
                    }
                }
            }
            StmtKind::For { var, iter, body, .. } => {
                Self::walk_expr_free(iter, used, defined);
                let mut inner_defined = defined.clone();
                inner_defined.insert(var.clone());
                Self::walk_block_free(body, used, &mut inner_defined);
            }
            StmtKind::While { cond, body } => {
                Self::walk_expr_free(cond, used, defined);
                Self::walk_block_free(body, used, defined);
            }
            StmtKind::LetDestructure { value, bindings, rest } => {
                Self::walk_expr_free(value, used, defined);
                for b in bindings {
                    if let Some(name) = b { defined.insert(name.clone()); }
                }
                if let Some(name) = rest { defined.insert(name.clone()); }
            }
            StmtKind::ForDestructure { bindings, iter, body } => {
                Self::walk_expr_free(iter, used, defined);
                let mut inner_defined = defined.clone();
                for name in bindings { inner_defined.insert(name.clone()); }
                Self::walk_block_free(body, used, &mut inner_defined);
            }
            StmtKind::Break | StmtKind::Continue => {}
        }
    }

    fn collect_pattern_bindings(pat: &Pattern, defined: &mut std::collections::HashSet<String>) {
        match pat {
            Pattern::Ident(name) => { defined.insert(name.clone()); }
            Pattern::Constructor(_, subs) => {
                for p in subs { Self::collect_pattern_bindings(p, defined); }
            }
            Pattern::Array(elems) => {
                for elem in elems {
                    match elem {
                        ArrayPatElem::Pat(p) => Self::collect_pattern_bindings(p, defined),
                        ArrayPatElem::Rest(name) => { defined.insert(name.clone()); }
                    }
                }
            }
            Pattern::Map(entries) => {
                for (_, p) in entries { Self::collect_pattern_bindings(p, defined); }
            }
            _ => {}
        }
    }
}
