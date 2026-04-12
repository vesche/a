use std::collections::HashMap;
use std::collections::HashSet;
use std::path::PathBuf;
use std::sync::Arc;
use crate::ast::*;
use crate::builtins;
use crate::errors::{AError, AResult};
use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::tokens::Span;

#[derive(Debug, Clone)]
pub enum Value {
    Int(i64),
    Float(f64),
    String(Arc<String>),
    Bool(bool),
    Void,
    Array(Arc<Vec<Value>>),
    Record(Vec<(String, Value)>),
    Map(Arc<HashMap<String, Value>>),
    Result(std::result::Result<Box<Value>, Box<Value>>),
    Variant(String, Vec<Value>),
    Closure {
        params: Vec<Param>,
        body: Box<Expr>,
        env: Vec<HashMap<String, Value>>,
    },
    VMClosure {
        name: Arc<String>,
        captures: Arc<Vec<Value>>,
    },
    TaskHandle(usize),
}

impl Value {
    pub fn string(s: String) -> Self { Value::String(Arc::new(s)) }
    pub fn array(v: Vec<Value>) -> Self { Value::Array(Arc::new(v)) }
    pub fn map(m: HashMap<String, Value>) -> Self { Value::Map(Arc::new(m)) }
}

impl PartialEq for Value {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (Value::Int(a), Value::Int(b)) => a == b,
            (Value::Float(a), Value::Float(b)) => a == b,
            (Value::String(a), Value::String(b)) => a == b,
            (Value::Bool(a), Value::Bool(b)) => a == b,
            (Value::Void, Value::Void) => true,
            (Value::Array(a), Value::Array(b)) => a == b,
            (Value::Record(a), Value::Record(b)) => a == b,
            (Value::Map(a), Value::Map(b)) => {
                if a.len() != b.len() { return false; }
                a.iter().all(|(k, v)| b.get(k).map_or(false, |bv| v == bv))
            }
            (Value::Result(a), Value::Result(b)) => a == b,
            (Value::Variant(na, va), Value::Variant(nb, vb)) => na == nb && va == vb,
            (Value::VMClosure { name: a, .. }, Value::VMClosure { name: b, .. }) => a == b,
            (Value::TaskHandle(a), Value::TaskHandle(b)) => a == b,
            _ => false,
        }
    }
}

enum ControlFlow {
    Return(Value),
    Continue,
    Break,
    LoopContinue,
}

pub struct Interpreter {
    fns: HashMap<String, FnDecl>,
    scopes: Vec<HashMap<String, Value>>,
    variants: HashMap<String, usize>,
    source_dir: PathBuf,
    loaded_modules: HashSet<PathBuf>,
    program_args: Vec<String>,
}

impl Interpreter {
    pub fn new() -> Self {
        Interpreter {
            fns: HashMap::new(),
            scopes: vec![HashMap::new()],
            variants: HashMap::new(),
            source_dir: std::env::current_dir().unwrap_or_default(),
            loaded_modules: HashSet::new(),
            program_args: Vec::new(),
        }
    }

    pub fn set_source_dir(&mut self, dir: PathBuf) {
        self.source_dir = dir;
    }

    pub fn set_args(&mut self, args: Vec<String>) {
        self.program_args = args;
    }

    pub fn run(&mut self, program: &Program) -> AResult<Value> {
        for item in &program.items {
            self.register_top_level(item);
        }

        if self.fns.contains_key("main") {
            self.call_fn("main", &[])
        } else {
            for item in &program.items {
                if let TopLevelKind::FnDecl(f) = &item.kind {
                    return self.call_fn(&f.name, &[]);
                }
            }
            Ok(Value::Void)
        }
    }

    fn register_top_level(&mut self, item: &TopLevel) {
        match &item.kind {
            TopLevelKind::FnDecl(f) => {
                self.fns.insert(f.name.clone(), f.clone());
            }
            TopLevelKind::TypeDecl(td) => {
                if let TypeBody::Sum(variants) = &td.body {
                    for v in variants {
                        self.variants.insert(v.name.clone(), v.fields.len());
                    }
                }
            }
            TopLevelKind::ModDecl(m) => {
                for sub in &m.items {
                    self.register_top_level(sub);
                }
            }
            TopLevelKind::UseDecl(u) => {
                self.load_module(&u.path);
            }
        }
    }

    fn load_module(&mut self, path: &[String]) {
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

        let source = match std::fs::read_to_string(&file_path) {
            Ok(s) => s,
            Err(_) => return,
        };

        let mut lexer = Lexer::new(&source);
        let tokens = match lexer.tokenize() {
            Ok(t) => t,
            Err(_) => return,
        };

        let mut parser = Parser::new(tokens);
        let program = match parser.parse_program() {
            Ok(p) => p,
            Err(_) => return,
        };

        for item in &program.items {
            match &item.kind {
                TopLevelKind::FnDecl(f) => {
                    let namespaced = format!("{namespace}.{}", f.name);
                    self.fns.insert(namespaced, f.clone());
                    self.fns.entry(f.name.clone()).or_insert_with(|| f.clone());
                }
                TopLevelKind::TypeDecl(td) => {
                    if let TypeBody::Sum(variants) = &td.body {
                        for v in variants {
                            self.variants.insert(
                                format!("{namespace}.{}", v.name),
                                v.fields.len(),
                            );
                            self.variants.insert(v.name.clone(), v.fields.len());
                        }
                    }
                }
                TopLevelKind::UseDecl(u) => {
                    self.load_module(&u.path);
                }
                _ => {}
            }
        }
    }

    fn call_fn(&mut self, name: &str, args: &[Value]) -> AResult<Value> {
        if name == "Ok" {
            return Ok(Value::Result(Ok(Box::new(args.first().cloned().unwrap_or(Value::Void)))));
        }
        if name == "Err" {
            return Ok(Value::Result(Err(Box::new(args.first().cloned().unwrap_or(Value::Void)))));
        }
        if self.variants.contains_key(name) {
            return Ok(Value::Variant(name.to_string(), args.to_vec()));
        }
        if name == "args" {
            return Ok(Value::array(
                self.program_args.iter().map(|s| Value::string(s.clone())).collect()
            ));
        }
        if name == "exit" {
            let code = match args.first() {
                Some(Value::Int(n)) => *n as i32,
                _ => 0,
            };
            std::process::exit(code);
        }
        if name == "fail" {
            let msg = match args.first() {
                Some(Value::String(s)) => s.to_string(),
                _ => "assertion failed".to_string(),
            };
            return Err(AError::runtime(msg, None));
        }
        if builtins::is_builtin(name) {
            return builtins::call_builtin(name, args, self);
        }

        if let Some(val) = self.lookup(name) {
            if let Value::Closure { .. } = &val {
                return self.call_closure(&val, args, &None);
            }
        }

        let f = self.fns.get(name).cloned().ok_or_else(|| {
            AError::runtime(format!("undefined function: {name}"), None)
        })?;

        self.push_scope();
        for (param, arg) in f.params.iter().zip(args.iter()) {
            self.define(&param.name, arg.clone());
        }

        let result = self.exec_block(&f.body)?;
        self.pop_scope();

        match result {
            ControlFlow::Return(v) => Ok(v),
            ControlFlow::Continue | ControlFlow::Break | ControlFlow::LoopContinue => Ok(Value::Void),
        }
    }

    pub fn call_closure(&mut self, closure: &Value, args: &[Value], span: &Option<Span>) -> AResult<Value> {
        if let Value::Closure { params, body, env } = closure {
            let saved_scopes = std::mem::replace(&mut self.scopes, env.clone());
            self.push_scope();
            for (param, arg) in params.iter().zip(args.iter()) {
                self.define(&param.name, arg.clone());
            }
            let result = self.eval_expr(body)?;
            self.pop_scope();
            self.scopes = saved_scopes;
            Ok(result)
        } else {
            Err(AError::runtime("cannot call non-function value", span.clone()))
        }
    }

    fn val_to_string(&self, val: &Value) -> String {
        match val {
            Value::Int(i) => i.to_string(),
            Value::Float(f) => format!("{f}"),
            Value::String(s) => s.to_string(),
            Value::Bool(b) => b.to_string(),
            Value::Void => "void".to_string(),
            Value::Array(elems) => {
                let inner: Vec<String> = elems.iter().map(|v| self.val_to_string(v)).collect();
                format!("[{}]", inner.join(", "))
            }
            Value::Record(fields) => {
                let inner: Vec<String> = fields.iter().map(|(k, v)| format!("{k}: {}", self.val_to_string(v))).collect();
                format!("{{{}}}", inner.join(", "))
            }
            Value::Map(entries) => {
                let inner: Vec<String> = entries.iter().map(|(k, v)| format!("\"{}\": {}", k, self.val_to_string(v))).collect();
                format!("#{{{}}}", inner.join(", "))
            }
            Value::Result(Ok(v)) => format!("Ok({})", self.val_to_string(v)),
            Value::Result(Err(v)) => format!("Err({})", self.val_to_string(v)),
            Value::Variant(name, vals) => {
                if vals.is_empty() { name.clone() }
                else {
                    let inner: Vec<String> = vals.iter().map(|v| self.val_to_string(v)).collect();
                    format!("{name}({})", inner.join(", "))
                }
            }
            Value::Closure { .. } | Value::VMClosure { .. } => "<closure>".to_string(),
            Value::TaskHandle(id) => format!("<task:{id}>"),
        }
    }

    fn exec_block(&mut self, block: &Block) -> AResult<ControlFlow> {
        for stmt in &block.stmts {
            match self.exec_stmt(stmt)? {
                ControlFlow::Return(v) => return Ok(ControlFlow::Return(v)),
                ControlFlow::Break => return Ok(ControlFlow::Break),
                ControlFlow::LoopContinue => return Ok(ControlFlow::LoopContinue),
                ControlFlow::Continue => {}
            }
        }
        Ok(ControlFlow::Continue)
    }

    fn exec_try_block(&mut self, block: &Block) -> AResult<Value> {
        let stmts = &block.stmts;
        if stmts.is_empty() { return Ok(Value::Void); }
        for stmt in &stmts[..stmts.len() - 1] {
            match self.exec_stmt(stmt)? {
                ControlFlow::Return(v) => return Ok(v),
                ControlFlow::Break | ControlFlow::LoopContinue => return Ok(Value::Void),
                ControlFlow::Continue => {}
            }
        }
        let last = &stmts[stmts.len() - 1];
        if let StmtKind::Expr(e) = &last.kind {
            self.eval_expr(e)
        } else {
            match self.exec_stmt(last)? {
                ControlFlow::Return(v) => Ok(v),
                ControlFlow::Continue | ControlFlow::Break | ControlFlow::LoopContinue => Ok(Value::Void),
            }
        }
    }

    fn exec_stmt(&mut self, stmt: &Stmt) -> AResult<ControlFlow> {
        match &stmt.kind {
            StmtKind::Let { name, value, .. } => {
                let val = self.eval_expr(value)?;
                self.define(name, val);
                Ok(ControlFlow::Continue)
            }
            StmtKind::Assign { target, value } => {
                let val = self.eval_expr(value)?;
                self.assign_target(target, val)?;
                Ok(ControlFlow::Continue)
            }
            StmtKind::Return(expr) => {
                let val = self.eval_expr(expr)?;
                Ok(ControlFlow::Return(val))
            }
            StmtKind::Expr(expr) => {
                self.eval_expr(expr)?;
                Ok(ControlFlow::Continue)
            }
            StmtKind::If { cond, then_block, else_block } => {
                let cond_val = self.eval_expr(cond)?;
                if self.is_truthy(&cond_val) {
                    self.push_scope();
                    let cf = self.exec_block(then_block)?;
                    self.pop_scope();
                    match cf {
                        ControlFlow::Continue => {}
                        other => return Ok(other),
                    }
                } else if let Some(eb) = else_block {
                    match eb {
                        ElseBranch::Block(b) => {
                            self.push_scope();
                            let cf = self.exec_block(b)?;
                            self.pop_scope();
                            match cf {
                                ControlFlow::Continue => {}
                                other => return Ok(other),
                            }
                        }
                        ElseBranch::If(s) => {
                            return self.exec_stmt(s);
                        }
                    }
                }
                Ok(ControlFlow::Continue)
            }
            StmtKind::Match { expr, arms } => {
                let val = self.eval_expr(expr)?;
                for arm in arms {
                    if let Some(bindings) = self.match_pattern(&arm.pattern, &val) {
                        self.push_scope();
                        for (name, bval) in &bindings {
                            self.define(name, bval.clone());
                        }
                        if let Some(guard) = &arm.guard {
                            let gval = self.eval_expr(guard)?;
                            if !self.is_truthy(&gval) {
                                self.pop_scope();
                                continue;
                            }
                        }
                        let result = match &arm.body {
                            MatchBody::Expr(e) => {
                                let v = self.eval_expr(e)?;
                                ControlFlow::Return(v)
                            }
                            MatchBody::Block(b) => self.exec_block(b)?,
                        };
                        self.pop_scope();
                        return Ok(result);
                    }
                }
                Err(AError::runtime("non-exhaustive match", stmt.span.clone()))
            }
            StmtKind::For { var, iter, body, .. } => {
                let iter_val = self.eval_expr(iter)?;
                let items: Vec<Value> = match &iter_val {
                    Value::Array(elems) => elems.iter().cloned().collect(),
                    Value::Map(m) => m.iter().map(|(k, v)| Value::array(vec![Value::string(k.clone()), v.clone()])).collect(),
                    _ => return Err(AError::runtime("for loop requires array or map", stmt.span.clone())),
                };
                for elem in items {
                    self.push_scope();
                    self.define(var, elem);
                    let cf = self.exec_block(body)?;
                    self.pop_scope();
                    match cf {
                        ControlFlow::Return(v) => return Ok(ControlFlow::Return(v)),
                        ControlFlow::Break => break,
                        ControlFlow::LoopContinue => continue,
                        ControlFlow::Continue => {}
                    }
                }
                Ok(ControlFlow::Continue)
            }
            StmtKind::While { cond, body } => {
                loop {
                    let cond_val = self.eval_expr(cond)?;
                    if !self.is_truthy(&cond_val) { break; }
                    self.push_scope();
                    let cf = self.exec_block(body)?;
                    self.pop_scope();
                    match cf {
                        ControlFlow::Return(v) => return Ok(ControlFlow::Return(v)),
                        ControlFlow::Break => break,
                        ControlFlow::LoopContinue => continue,
                        ControlFlow::Continue => {}
                    }
                }
                Ok(ControlFlow::Continue)
            }
            StmtKind::LetDestructure { bindings, rest, value } => {
                let val = self.eval_expr(value)?;
                if let Value::Array(arr) = &val {
                    for (i, binding) in bindings.iter().enumerate() {
                        if let Some(name) = binding {
                            let elem = arr.get(i).cloned().unwrap_or(Value::Void);
                            self.define(name, elem);
                        }
                    }
                    if let Some(rest_name) = rest {
                        let n = bindings.len();
                        let rest_arr = if n < arr.len() { arr[n..].to_vec() } else { Vec::new() };
                        self.define(rest_name, Value::array(rest_arr));
                    }
                }
                Ok(ControlFlow::Continue)
            }
            StmtKind::ForDestructure { bindings, iter, body } => {
                let iter_val = self.eval_expr(iter)?;
                let items: Vec<Value> = match &iter_val {
                    Value::Array(elems) => elems.iter().cloned().collect(),
                    Value::Map(m) => m.iter().map(|(k, v)| Value::array(vec![Value::string(k.clone()), v.clone()])).collect(),
                    _ => Vec::new(),
                };
                {
                    for elem in items {
                        self.push_scope();
                        if let Value::Array(inner) = &elem {
                            for (i, name) in bindings.iter().enumerate() {
                                let v = inner.get(i).cloned().unwrap_or(Value::Void);
                                self.define(name, v);
                            }
                        }
                        let cf = self.exec_block(body)?;
                        self.pop_scope();
                        match cf {
                            ControlFlow::Return(v) => return Ok(ControlFlow::Return(v)),
                            ControlFlow::Break => break,
                            ControlFlow::LoopContinue => continue,
                            ControlFlow::Continue => {}
                        }
                    }
                }
                Ok(ControlFlow::Continue)
            }
            StmtKind::Break => Ok(ControlFlow::Break),
            StmtKind::Continue => Ok(ControlFlow::LoopContinue),
        }
    }

    fn eval_expr(&mut self, expr: &Expr) -> AResult<Value> {
        match &expr.kind {
            ExprKind::Literal(lit) => Ok(match lit {
                Literal::Int(v) => Value::Int(*v),
                Literal::Float(v) => Value::Float(*v),
                Literal::String(v) => Value::string(v.clone()),
                Literal::Bool(v) => Value::Bool(*v),
            }),
            ExprKind::Ident(name) => {
                self.lookup(name).ok_or_else(|| {
                    AError::runtime(format!("undefined variable: {name}"), expr.span.clone())
                })
            }
            ExprKind::BinOp { op, left, right } => {
                let lv = self.eval_expr(left)?;
                let rv = self.eval_expr(right)?;
                self.eval_binop(*op, &lv, &rv, &expr.span)
            }
            ExprKind::UnaryOp { op, expr: inner } => {
                let v = self.eval_expr(inner)?;
                match op {
                    UnaryOp::Neg => match v {
                        Value::Int(i) => Ok(Value::Int(-i)),
                        Value::Float(f) => Ok(Value::Float(-f)),
                        _ => Err(AError::runtime("cannot negate non-numeric", expr.span.clone())),
                    },
                    UnaryOp::Not => match v {
                        Value::Bool(b) => Ok(Value::Bool(!b)),
                        _ => Err(AError::runtime("cannot apply ! to non-bool", expr.span.clone())),
                    },
                }
            }
            ExprKind::Call { func, args } => {
                let arg_vals: Vec<Value> = args.iter()
                    .map(|a| self.eval_expr(a))
                    .collect::<AResult<_>>()?;

                if let ExprKind::FieldAccess { expr: obj, field } = &func.kind {
                    if let ExprKind::Ident(module) = &obj.kind {
                        let full_name = format!("{module}.{field}");
                        return self.call_fn(&full_name, &arg_vals);
                    }
                }
                if let ExprKind::Ident(name) = &func.kind {
                    return self.call_fn(name, &arg_vals);
                }
                let func_val = self.eval_expr(func)?;
                self.call_closure(&func_val, &arg_vals, &expr.span)
            }
            ExprKind::FieldAccess { expr: inner, field } => {
                let val = self.eval_expr(inner)?;
                match val {
                    Value::Record(fields) => {
                        for (name, v) in &fields {
                            if name == field { return Ok(v.clone()); }
                        }
                        Err(AError::runtime(format!("no field '{field}'"), expr.span.clone()))
                    }
                    Value::Map(m) => {
                        Ok(m.get(field).cloned().unwrap_or(Value::Void))
                    }
                    _ => Err(AError::runtime("field access on non-record", expr.span.clone())),
                }
            }
            ExprKind::Index { expr: inner, index } => {
                let val = self.eval_expr(inner)?;
                let idx = self.eval_expr(index)?;
                match (&val, &idx) {
                    (Value::Array(arr), Value::Int(i)) => {
                        let i = *i as usize;
                        arr.get(i).cloned().ok_or_else(|| {
                            AError::runtime(format!("index {i} out of bounds"), expr.span.clone())
                        })
                    }
                    (Value::Map(m), Value::String(k)) => {
                        Ok(m.get(k.as_str()).cloned().unwrap_or(Value::Void))
                    }
                    (Value::Map(m), Value::Int(i)) => {
                        let entry = m.iter().nth(*i as usize);
                        match entry {
                            Some((k, v)) => Ok(Value::array(vec![Value::string(k.clone()), v.clone()])),
                            None => Ok(Value::Void),
                        }
                    }
                    _ => Err(AError::runtime("invalid index operation", expr.span.clone())),
                }
            }
            ExprKind::Try(inner) => {
                let val = self.eval_expr(inner)?;
                match val {
                    Value::Result(Ok(v)) => Ok(*v),
                    Value::Result(Err(e)) => Err(AError::runtime(
                        format!("unwrap on Err: {}", self.val_to_string(&e)),
                        expr.span.clone(),
                    )),
                    other => Ok(other),
                }
            }
            ExprKind::TryBlock(block) => {
                self.push_scope();
                let result = self.exec_try_block(block);
                self.pop_scope();
                match result {
                    Ok(val) => Ok(Value::Result(Ok(Box::new(val)))),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.message.clone()))))),
                }
            }
            ExprKind::If { cond, then_block, else_block } => {
                let cond_val = self.eval_expr(cond)?;
                if self.is_truthy(&cond_val) {
                    self.push_scope();
                    let cf = self.exec_block(then_block)?;
                    self.pop_scope();
                    match cf {
                        ControlFlow::Return(v) => Ok(v),
                        _ => Ok(Value::Void),
                    }
                } else {
                    self.push_scope();
                    let cf = self.exec_block(else_block)?;
                    self.pop_scope();
                    match cf {
                        ControlFlow::Return(v) => Ok(v),
                        _ => Ok(Value::Void),
                    }
                }
            }
            ExprKind::Match { expr: inner, arms } => {
                let val = self.eval_expr(inner)?;
                for arm in arms {
                    if let Some(bindings) = self.match_pattern(&arm.pattern, &val) {
                        self.push_scope();
                        for (name, bval) in &bindings {
                            self.define(name, bval.clone());
                        }
                        if let Some(guard) = &arm.guard {
                            let gval = self.eval_expr(guard)?;
                            if !self.is_truthy(&gval) {
                                self.pop_scope();
                                continue;
                            }
                        }
                        let result = match &arm.body {
                            MatchBody::Expr(e) => self.eval_expr(e)?,
                            MatchBody::Block(b) => {
                                match self.exec_block(b)? {
                                    ControlFlow::Return(v) => v,
                                    _ => Value::Void,
                                }
                            }
                        };
                        self.pop_scope();
                        return Ok(result);
                    }
                }
                Err(AError::runtime("non-exhaustive match", expr.span.clone()))
            }
            ExprKind::Block(block) => {
                self.push_scope();
                let cf = self.exec_block(block)?;
                self.pop_scope();
                match cf {
                    ControlFlow::Return(v) => Ok(v),
                    _ => Ok(Value::Void),
                }
            }
            ExprKind::Array(elems) => {
                let has_spread = elems.iter().any(|e| matches!(e.kind, ExprKind::Spread(_)));
                if !has_spread {
                    let vals: Vec<Value> = elems.iter()
                        .map(|e| self.eval_expr(e))
                        .collect::<AResult<_>>()?;
                    Ok(Value::array(vals))
                } else {
                    let mut result = Vec::new();
                    for e in elems {
                        if let ExprKind::Spread(inner) = &e.kind {
                            let val = self.eval_expr(inner)?;
                            if let Value::Array(arr) = val {
                                result.extend(arr.iter().cloned());
                            }
                        } else {
                            result.push(self.eval_expr(e)?);
                        }
                    }
                    Ok(Value::array(result))
                }
            }
            ExprKind::Spread(_) => Ok(Value::Void),
            ExprKind::Record(fields) => {
                let vals: Vec<(String, Value)> = fields.iter()
                    .map(|(n, e)| Ok((n.clone(), self.eval_expr(e)?)))
                    .collect::<AResult<_>>()?;
                Ok(Value::Record(vals))
            }
            ExprKind::Lambda { params, body } => {
                Ok(Value::Closure {
                    params: params.clone(),
                    body: body.clone(),
                    env: self.scopes.clone(),
                })
            }
            ExprKind::Pipe { left, right } => {
                let lv = self.eval_expr(left)?;
                match &right.kind {
                    ExprKind::Call { func, args } => {
                        let mut all_args = vec![lv];
                        for a in args {
                            all_args.push(self.eval_expr(a)?);
                        }
                        if let ExprKind::FieldAccess { expr: obj, field } = &func.kind {
                            if let ExprKind::Ident(module) = &obj.kind {
                                let full_name = format!("{module}.{field}");
                                return self.call_fn(&full_name, &all_args);
                            }
                        }
                        if let ExprKind::Ident(name) = &func.kind {
                            return self.call_fn(name, &all_args);
                        }
                        let func_val = self.eval_expr(func)?;
                        self.call_closure(&func_val, &all_args, &expr.span)
                    }
                    ExprKind::Ident(name) => {
                        self.call_fn(name, &[lv])
                    }
                    ExprKind::Lambda { .. } => {
                        let func_val = self.eval_expr(right)?;
                        self.call_closure(&func_val, &[lv], &expr.span)
                    }
                    _ => {
                        let func_val = self.eval_expr(right)?;
                        self.call_closure(&func_val, &[lv], &expr.span)
                    }
                }
            }
            ExprKind::Interpolation { parts } => {
                let mut result = String::new();
                for part in parts {
                    match part {
                        StringPart::Lit(s) => result.push_str(s),
                        StringPart::Expr(e) => {
                            let v = self.eval_expr(e)?;
                            result.push_str(&self.val_to_string(&v));
                        }
                    }
                }
                Ok(Value::string(result))
            }
            ExprKind::MapLiteral(entries) => {
                let mut map = HashMap::new();
                for (k, v) in entries {
                    let key = self.eval_expr(k)?;
                    let key_str = match &key {
                        Value::String(s) => s.as_ref().clone(),
                        other => self.val_to_string(other),
                    };
                    let val = self.eval_expr(v)?;
                    map.insert(key_str, val);
                }
                Ok(Value::map(map))
            }
        }
    }

    fn eval_binop(&self, op: BinOp, left: &Value, right: &Value, span: &Option<Span>) -> AResult<Value> {
        match (left, right) {
            (Value::Int(a), Value::Int(b)) => match op {
                BinOp::Add => Ok(Value::Int(a + b)),
                BinOp::Sub => Ok(Value::Int(a - b)),
                BinOp::Mul => Ok(Value::Int(a * b)),
                BinOp::Div => {
                    if *b == 0 { return Err(AError::runtime("division by zero", span.clone())); }
                    Ok(Value::Int(a / b))
                }
                BinOp::Mod => {
                    if *b == 0 { return Err(AError::runtime("modulo by zero", span.clone())); }
                    Ok(Value::Int(a % b))
                }
                BinOp::Eq => Ok(Value::Bool(a == b)),
                BinOp::NotEq => Ok(Value::Bool(a != b)),
                BinOp::Lt => Ok(Value::Bool(a < b)),
                BinOp::Gt => Ok(Value::Bool(a > b)),
                BinOp::LtEq => Ok(Value::Bool(a <= b)),
                BinOp::GtEq => Ok(Value::Bool(a >= b)),
                _ => Err(AError::runtime("invalid op on integers", span.clone())),
            },
            (Value::Float(a), Value::Float(b)) => match op {
                BinOp::Add => Ok(Value::Float(a + b)),
                BinOp::Sub => Ok(Value::Float(a - b)),
                BinOp::Mul => Ok(Value::Float(a * b)),
                BinOp::Div => Ok(Value::Float(a / b)),
                BinOp::Mod => Ok(Value::Float(a % b)),
                BinOp::Eq => Ok(Value::Bool(a == b)),
                BinOp::NotEq => Ok(Value::Bool(a != b)),
                BinOp::Lt => Ok(Value::Bool(a < b)),
                BinOp::Gt => Ok(Value::Bool(a > b)),
                BinOp::LtEq => Ok(Value::Bool(a <= b)),
                BinOp::GtEq => Ok(Value::Bool(a >= b)),
                _ => Err(AError::runtime("invalid op on floats", span.clone())),
            },
            (Value::Int(a), Value::Float(b)) => {
                self.eval_binop(op, &Value::Float(*a as f64), &Value::Float(*b), span)
            }
            (Value::Float(a), Value::Int(b)) => {
                self.eval_binop(op, &Value::Float(*a), &Value::Float(*b as f64), span)
            }
            (Value::String(a), Value::String(b)) => match op {
                BinOp::Add => Ok(Value::string(format!("{a}{b}"))),
                BinOp::Eq => Ok(Value::Bool(a == b)),
                BinOp::NotEq => Ok(Value::Bool(a != b)),
                _ => Err(AError::runtime("invalid op on strings", span.clone())),
            },
            (Value::Bool(a), Value::Bool(b)) => match op {
                BinOp::And => Ok(Value::Bool(*a && *b)),
                BinOp::Or => Ok(Value::Bool(*a || *b)),
                BinOp::Eq => Ok(Value::Bool(a == b)),
                BinOp::NotEq => Ok(Value::Bool(a != b)),
                _ => Err(AError::runtime("invalid op on booleans", span.clone())),
            },
            _ => Err(AError::runtime(
                format!("cannot apply {op:?} to {left:?} and {right:?}"),
                span.clone(),
            )),
        }
    }

    fn match_pattern(&self, pattern: &Pattern, value: &Value) -> Option<Vec<(String, Value)>> {
        match pattern {
            Pattern::Wildcard => Some(Vec::new()),
            Pattern::Ident(name) => Some(vec![(name.clone(), value.clone())]),
            Pattern::Literal(lit) => {
                let matches = match (lit, value) {
                    (Literal::Int(a), Value::Int(b)) => a == b,
                    (Literal::Float(a), Value::Float(b)) => a == b,
                    (Literal::String(a), Value::String(b)) => a == b.as_ref(),
                    (Literal::Bool(a), Value::Bool(b)) => a == b,
                    _ => false,
                };
                if matches { Some(Vec::new()) } else { None }
            }
            Pattern::Constructor(name, sub_patterns) => {
                match value {
                    Value::Variant(vname, vals) if vname == name && vals.len() == sub_patterns.len() => {
                        let mut bindings = Vec::new();
                        for (pat, val) in sub_patterns.iter().zip(vals.iter()) {
                            match self.match_pattern(pat, val) {
                                Some(b) => bindings.extend(b),
                                None => return None,
                            }
                        }
                        Some(bindings)
                    }
                    Value::Result(Ok(inner)) if name == "Ok" && sub_patterns.len() == 1 => {
                        self.match_pattern(&sub_patterns[0], inner)
                    }
                    Value::Result(Err(inner)) if name == "Err" && sub_patterns.len() == 1 => {
                        self.match_pattern(&sub_patterns[0], inner)
                    }
                    _ => {
                        if sub_patterns.is_empty() {
                            if let Value::Variant(vname, vals) = value {
                                if vname == name && vals.is_empty() {
                                    return Some(Vec::new());
                                }
                            }
                        }
                        None
                    }
                }
            }
            Pattern::Array(elems) => {
                let arr = match value {
                    Value::Array(a) => a,
                    _ => return None,
                };
                let mut has_rest = false;
                let mut fixed_count = 0usize;
                for elem in elems {
                    match elem {
                        ArrayPatElem::Pat(_) => fixed_count += 1,
                        ArrayPatElem::Rest(_) => { has_rest = true; }
                    }
                }
                if has_rest {
                    if arr.len() < fixed_count { return None; }
                } else {
                    if arr.len() != fixed_count { return None; }
                }
                let mut bindings = Vec::new();
                let mut idx = 0usize;
                for elem in elems {
                    match elem {
                        ArrayPatElem::Pat(pat) => {
                            match self.match_pattern(pat, &arr[idx]) {
                                Some(b) => bindings.extend(b),
                                None => return None,
                            }
                            idx += 1;
                        }
                        ArrayPatElem::Rest(name) => {
                            let rest: Vec<Value> = arr[idx..].to_vec();
                            bindings.push((name.clone(), Value::array(rest)));
                        }
                    }
                }
                Some(bindings)
            }
            Pattern::Map(entries) => {
                let map = match value {
                    Value::Map(m) => m,
                    _ => return None,
                };
                let mut bindings = Vec::new();
                for (key, sub_pat) in entries {
                    match map.get(key.as_str()) {
                        Some(v) => {
                            match self.match_pattern(sub_pat, v) {
                                Some(b) => bindings.extend(b),
                                None => return None,
                            }
                        }
                        None => return None,
                    }
                }
                Some(bindings)
            }
        }
    }

    fn is_truthy(&self, val: &Value) -> bool {
        match val {
            Value::Bool(b) => *b,
            Value::Int(i) => *i != 0,
            Value::Void => false,
            _ => true,
        }
    }

    fn assign_target(&mut self, target: &Expr, val: Value) -> AResult<()> {
        match &target.kind {
            ExprKind::Ident(name) => {
                for scope in self.scopes.iter_mut().rev() {
                    if scope.contains_key(name) {
                        scope.insert(name.clone(), val);
                        return Ok(());
                    }
                }
                Err(AError::runtime(format!("undefined variable: {name}"), target.span.clone()))
            }
            ExprKind::Index { expr: coll_expr, index } => {
                let idx = self.eval_expr(index)?;
                if let ExprKind::Ident(var_name) = &coll_expr.kind {
                    let coll = self.lookup(var_name).ok_or_else(|| {
                        AError::runtime(format!("undefined variable: {var_name}"), coll_expr.span.clone())
                    })?;
                    let new_coll = match (coll, &idx) {
                        (Value::Array(a), Value::Int(i)) => {
                            let i = *i as usize;
                            let mut v = Arc::try_unwrap(a).unwrap_or_else(|rc| (*rc).clone());
                            while v.len() <= i { v.push(Value::Void); }
                            v[i] = val;
                            Value::array(v)
                        }
                        (Value::Map(m), Value::String(k)) => {
                            let mut new_m = Arc::try_unwrap(m).unwrap_or_else(|rc| (*rc).clone());
                            new_m.insert(k.as_ref().clone(), val);
                            Value::map(new_m)
                        }
                        _ => return Err(AError::runtime("invalid index assignment", target.span.clone())),
                    };
                    for scope in self.scopes.iter_mut().rev() {
                        if scope.contains_key(var_name) {
                            scope.insert(var_name.clone(), new_coll);
                            return Ok(());
                        }
                    }
                    Err(AError::runtime(format!("undefined variable: {var_name}"), coll_expr.span.clone()))
                } else {
                    Err(AError::runtime("invalid assignment target", target.span.clone()))
                }
            }
            _ => Err(AError::runtime("invalid assignment target", target.span.clone())),
        }
    }

    fn push_scope(&mut self) {
        self.scopes.push(HashMap::new());
    }

    fn pop_scope(&mut self) {
        self.scopes.pop();
    }

    fn define(&mut self, name: &str, val: Value) {
        if let Some(scope) = self.scopes.last_mut() {
            scope.insert(name.to_string(), val);
        }
    }

    fn lookup(&self, name: &str) -> Option<Value> {
        for scope in self.scopes.iter().rev() {
            if let Some(val) = scope.get(name) {
                return Some(val.clone());
            }
        }
        None
    }
}
