use std::collections::HashMap;
use crate::ast::*;
use crate::errors::AError;

#[derive(Debug, Clone, PartialEq)]
pub enum Type {
    I8, I16, I32, I64,
    U8, U16, U32, U64,
    F32, F64,
    Bool, Str, Bytes, Void,
    Array(Box<Type>),
    Tuple(Vec<Type>),
    Record(Vec<(String, Type)>),
    Named(String, Vec<Type>),
    Fn(Vec<Type>, Box<Type>),
    Map(Box<Type>, Box<Type>),
    Result(Box<Type>, Box<Type>),
    Unknown,
}

#[derive(Debug, Clone)]
pub struct FnSig {
    pub params: Vec<(String, Type)>,
    pub ret: Type,
    pub effects: Vec<String>,
}

#[derive(Debug)]
pub struct Checker {
    type_env: HashMap<String, TypeDef>,
    fn_sigs: HashMap<String, FnSig>,
    scopes: Vec<HashMap<String, (Type, bool)>>,
    current_effects: Vec<String>,
    errors: Vec<AError>,
}

#[derive(Debug, Clone)]
#[allow(dead_code)]
enum TypeDef {
    Record(Vec<(String, Type)>),
    Sum(Vec<(String, Vec<Type>)>),
    Alias(Type),
}

impl Checker {
    pub fn new() -> Self {
        Checker {
            type_env: HashMap::new(),
            fn_sigs: HashMap::new(),
            scopes: vec![HashMap::new()],
            current_effects: Vec::new(),
            errors: Vec::new(),
        }
    }

    pub fn check_program(&mut self, program: &Program) -> Result<(), Vec<AError>> {
        self.register_builtins();

        for item in &program.items {
            self.register_top_level(item);
        }

        for item in &program.items {
            self.check_top_level(item);
        }

        if self.errors.is_empty() {
            Ok(())
        } else {
            Err(self.errors.clone())
        }
    }

    fn register_builtins(&mut self) {
        self.fn_sigs.insert("print".into(), FnSig {
            params: vec![("msg".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("println".into(), FnSig {
            params: vec![("msg".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("to_str".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("len".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::I64,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("sqrt".into(), FnSig {
            params: vec![("x".into(), Type::F64)],
            ret: Type::F64,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("abs".into(), FnSig {
            params: vec![("x".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("map".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("f".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("filter".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("f".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("reduce".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("init".into(), Type::Unknown), ("f".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("each".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("f".into(), Type::Unknown)],
            ret: Type::Void,
            effects: vec!["pure".into()],
        });
        for name in &["str.contains", "str.starts_with", "str.ends_with"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("s".into(), Type::Str), ("sub".into(), Type::Str)],
                ret: Type::Bool,
                effects: vec!["pure".into()],
            });
        }
        self.fn_sigs.insert("str.replace".into(), FnSig {
            params: vec![("s".into(), Type::Str), ("from".into(), Type::Str), ("to".into(), Type::Str)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        for name in &["str.trim", "str.upper", "str.lower"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("s".into(), Type::Str)],
                ret: Type::Str,
                effects: vec!["pure".into()],
            });
        }
        self.fn_sigs.insert("str.join".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("sep".into(), Type::Str)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("str.chars".into(), FnSig {
            params: vec![("s".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("str.slice".into(), FnSig {
            params: vec![("s".into(), Type::Str), ("start".into(), Type::I64), ("end".into(), Type::I64)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("str.lines".into(), FnSig {
            params: vec![("s".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("str.find".into(), FnSig {
            params: vec![("s".into(), Type::Str), ("sub".into(), Type::Str)],
            ret: Type::I64,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("str.count".into(), FnSig {
            params: vec![("s".into(), Type::Str), ("sub".into(), Type::Str)],
            ret: Type::I64,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("args".into(), FnSig {
            params: vec![],
            ret: Type::Array(Box::new(Type::Str)),
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("exit".into(), FnSig {
            params: vec![("code".into(), Type::I64)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("eprintln".into(), FnSig {
            params: vec![("s".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("io.read_stdin".into(), FnSig {
            params: vec![],
            ret: Type::Str,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("io.read_line".into(), FnSig {
            params: vec![],
            ret: Type::Str,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("io.read_bytes".into(), FnSig {
            params: vec![("n".into(), Type::I64)],
            ret: Type::Str,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("io.flush".into(), FnSig {
            params: vec![],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("exec".into(), FnSig {
            params: vec![("cmd".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("proc.spawn".into(), FnSig {
            params: vec![("cmd".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("proc.write".into(), FnSig {
            params: vec![("handle".into(), Type::I64), ("data".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("proc.read_line".into(), FnSig {
            params: vec![("handle".into(), Type::I64)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("proc.kill".into(), FnSig {
            params: vec![("handle".into(), Type::I64)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("json.parse".into(), FnSig {
            params: vec![("s".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("json.stringify".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("json.pretty".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("env.get".into(), FnSig {
            params: vec![("key".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("env.set".into(), FnSig {
            params: vec![("key".into(), Type::Str), ("val".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("env.all".into(), FnSig {
            params: vec![],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("type_of".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("int".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::I64,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("float".into(), FnSig {
            params: vec![("val".into(), Type::Unknown)],
            ret: Type::F64,
            effects: vec!["pure".into()],
        });
        for name in &["http.get", "http.delete"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("url".into(), Type::Str)],
                ret: Type::Unknown,
                effects: vec!["io".into()],
            });
        }
        for name in &["http.post", "http.put", "http.patch"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("url".into(), Type::Str), ("body".into(), Type::Unknown)],
                ret: Type::Unknown,
                effects: vec!["io".into()],
            });
        }
        self.fn_sigs.insert("http.stream".into(), FnSig {
            params: vec![("url".into(), Type::Str), ("body".into(), Type::Unknown), ("headers".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("http.stream_read".into(), FnSig {
            params: vec![("handle".into(), Type::I64)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("http.stream_close".into(), FnSig {
            params: vec![("handle".into(), Type::I64)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("ws.connect".into(), FnSig {
            params: vec![("url".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("ws.send".into(), FnSig {
            params: vec![("handle".into(), Type::I64), ("msg".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("ws.recv".into(), FnSig {
            params: vec![("handle".into(), Type::I64)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("ws.close".into(), FnSig {
            params: vec![("handle".into(), Type::I64)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("uuid.v4".into(), FnSig {
            params: vec![],
            ret: Type::Str,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("signal.on".into(), FnSig {
            params: vec![("name".into(), Type::Str), ("handler".into(), Type::Unknown)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("http.serve".into(), FnSig {
            params: vec![("port".into(), Type::I64), ("handler".into(), Type::Unknown)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("http.serve_static".into(), FnSig {
            params: vec![("port".into(), Type::I64), ("dir".into(), Type::Str)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("db.open".into(), FnSig {
            params: vec![("path".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("db.close".into(), FnSig {
            params: vec![("db".into(), Type::Unknown)],
            ret: Type::Void,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("db.exec".into(), FnSig {
            params: vec![("db".into(), Type::Unknown), ("sql".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("db.query".into(), FnSig {
            params: vec![("db".into(), Type::Unknown), ("sql".into(), Type::Str), ("params".into(), Type::Array(Box::new(Type::Unknown)))],
            ret: Type::Array(Box::new(Type::Unknown)),
            effects: vec!["io".into()],
        });
        for name in &["fs.exists", "fs.is_dir", "fs.is_file"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("path".into(), Type::Str)],
                ret: Type::Bool,
                effects: vec!["io".into()],
            });
        }
        self.fn_sigs.insert("fs.ls".into(), FnSig {
            params: vec![("path".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        for name in &["fs.mkdir", "fs.rm"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("path".into(), Type::Str)],
                ret: Type::Void,
                effects: vec!["io".into()],
            });
        }
        for name in &["fs.mv", "fs.cp"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("from".into(), Type::Str), ("to".into(), Type::Str)],
                ret: Type::Void,
                effects: vec!["io".into()],
            });
        }
        self.fn_sigs.insert("fs.cwd".into(), FnSig {
            params: vec![],
            ret: Type::Str,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("fs.abs".into(), FnSig {
            params: vec![("path".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        self.fn_sigs.insert("fs.glob".into(), FnSig {
            params: vec![("pattern".into(), Type::Str)],
            ret: Type::Unknown,
            effects: vec!["io".into()],
        });
        for name in &["unwrap", "unwrap_or", "expect"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("result".into(), Type::Unknown)],
                ret: Type::Unknown,
                effects: vec!["pure".into()],
            });
        }
        for name in &["is_ok", "is_err"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("result".into(), Type::Unknown)],
                ret: Type::Bool,
                effects: vec!["pure".into()],
            });
        }
        for name in &["Ok", "Err"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("val".into(), Type::Unknown)],
                ret: Type::Unknown,
                effects: vec!["pure".into()],
            });
        }
        self.fn_sigs.insert("char_code".into(), FnSig {
            params: vec![("s".into(), Type::Str)],
            ret: Type::I64,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("from_code".into(), FnSig {
            params: vec![("n".into(), Type::I64)],
            ret: Type::Str,
            effects: vec!["pure".into()],
        });
        for name in &["is_alpha", "is_digit", "is_alnum"] {
            self.fn_sigs.insert((*name).into(), FnSig {
                params: vec![("s".into(), Type::Str)],
                ret: Type::Bool,
                effects: vec!["pure".into()],
            });
        }
        self.fn_sigs.insert("push".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("val".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("slice".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("start".into(), Type::I64), ("end".into(), Type::I64)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("sort".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("reverse_arr".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown)],
            ret: Type::Unknown,
            effects: vec!["pure".into()],
        });
        self.fn_sigs.insert("contains".into(), FnSig {
            params: vec![("arr".into(), Type::Unknown), ("val".into(), Type::Unknown)],
            ret: Type::Bool,
            effects: vec!["pure".into()],
        });
    }

    fn register_top_level(&mut self, item: &TopLevel) {
        match &item.kind {
            TopLevelKind::FnDecl(f) => {
                let params: Vec<(String, Type)> = f.params.iter()
                    .map(|p| (p.name.clone(), self.resolve_type(&p.ty)))
                    .collect();
                let ret = self.resolve_type(&f.ret_type);
                let effects = if f.effects.is_empty() {
                    vec!["pure".into()]
                } else {
                    f.effects.clone()
                };
                self.fn_sigs.insert(f.name.clone(), FnSig { params, ret, effects });
            }
            TopLevelKind::TypeDecl(t) => {
                let def = match &t.body {
                    TypeBody::Record(fields) => {
                        TypeDef::Record(fields.iter().map(|f| (f.name.clone(), self.resolve_type(&f.ty))).collect())
                    }
                    TypeBody::Sum(variants) => {
                        TypeDef::Sum(variants.iter().map(|v| {
                            (v.name.clone(), v.fields.iter().map(|f| self.resolve_type(f)).collect())
                        }).collect())
                    }
                    TypeBody::Alias(ty, _) => {
                        TypeDef::Alias(self.resolve_type(ty))
                    }
                };
                self.type_env.insert(t.name.clone(), def);
            }
            TopLevelKind::ModDecl(m) => {
                for sub in &m.items {
                    self.register_top_level(sub);
                }
            }
            TopLevelKind::UseDecl(_) => {}
            TopLevelKind::ExternFn(_) => {}
        }
    }

    fn check_top_level(&mut self, item: &TopLevel) {
        match &item.kind {
            TopLevelKind::FnDecl(f) => self.check_fn(f),
            TopLevelKind::ModDecl(m) => {
                for sub in &m.items {
                    self.check_top_level(sub);
                }
            }
            _ => {}
        }
    }

    fn check_fn(&mut self, f: &FnDecl) {
        let prev_effects = self.current_effects.clone();
        self.current_effects = if f.effects.is_empty() {
            vec!["pure".into()]
        } else {
            f.effects.clone()
        };

        self.push_scope();
        for p in &f.params {
            let ty = self.resolve_type(&p.ty);
            self.define_var(&p.name, ty, false);
        }

        let ret_type = self.resolve_type(&f.ret_type);
        self.define_var("ret", ret_type, false);

        for stmt in &f.body.stmts {
            self.check_stmt(stmt);
        }

        self.pop_scope();
        self.current_effects = prev_effects;
    }

    fn check_stmt(&mut self, stmt: &Stmt) {
        match &stmt.kind {
            StmtKind::Let { mutable, name, ty, value } => {
                let inferred = self.check_expr(value);
                let declared = self.resolve_type(ty);
                if declared == Type::Unknown && matches!(ty, TypeExpr::Infer) {
                    self.define_var(name, inferred, *mutable);
                } else {
                    if !self.types_compatible(&declared, &inferred) {
                        self.errors.push(AError::type_err(
                            format!("type mismatch: expected {declared:?}, found {inferred:?}"),
                            stmt.span.clone(),
                        ));
                    }
                    self.define_var(name, declared, *mutable);
                }
            }
            StmtKind::Assign { target, value } => {
                let target_ty = self.check_expr(target);
                let value_ty = self.check_expr(value);
                if !self.types_compatible(&target_ty, &value_ty) {
                    self.errors.push(AError::type_err(
                        format!("assignment type mismatch: expected {target_ty:?}, found {value_ty:?}"),
                        stmt.span.clone(),
                    ));
                }
            }
            StmtKind::Return(expr) => {
                self.check_expr(expr);
            }
            StmtKind::Expr(expr) => {
                self.check_expr(expr);
            }
            StmtKind::If { cond, then_block, else_block } => {
                let cond_ty = self.check_expr(cond);
                if cond_ty != Type::Bool && cond_ty != Type::Unknown {
                    self.errors.push(AError::type_err(
                        format!("if condition must be bool, found {cond_ty:?}"),
                        stmt.span.clone(),
                    ));
                }
                self.push_scope();
                for s in &then_block.stmts { self.check_stmt(s); }
                self.pop_scope();
                if let Some(ElseBranch::Block(b)) = else_block {
                    self.push_scope();
                    for s in &b.stmts { self.check_stmt(s); }
                    self.pop_scope();
                } else if let Some(ElseBranch::If(s)) = else_block {
                    self.check_stmt(s);
                }
            }
            StmtKind::Match { expr, arms } => {
                self.check_expr(expr);
                for arm in arms {
                    self.push_scope();
                    self.bind_pattern(&arm.pattern);
                    if let Some(g) = &arm.guard { self.check_expr(g); }
                    match &arm.body {
                        MatchBody::Expr(e) => { self.check_expr(e); }
                        MatchBody::Block(b) => {
                            for s in &b.stmts { self.check_stmt(s); }
                        }
                    }
                    self.pop_scope();
                }
            }
            StmtKind::For { var, ty, iter, body } => {
                let iter_ty = self.check_expr(iter);
                let elem_ty = if matches!(ty, TypeExpr::Infer) {
                    if let Type::Array(inner) = iter_ty {
                        *inner
                    } else {
                        Type::Unknown
                    }
                } else {
                    self.resolve_type(ty)
                };
                self.push_scope();
                self.define_var(var, elem_ty, false);
                for s in &body.stmts { self.check_stmt(s); }
                self.pop_scope();
            }
            StmtKind::While { cond, body } => {
                self.check_expr(cond);
                self.push_scope();
                for s in &body.stmts { self.check_stmt(s); }
                self.pop_scope();
            }
            StmtKind::LetDestructure { bindings, rest, value } => {
                self.check_expr(value);
                for b in bindings {
                    if let Some(name) = b {
                        self.define_var(name, Type::Unknown, false);
                    }
                }
                if let Some(name) = rest {
                    self.define_var(name, Type::Unknown, false);
                }
            }
            StmtKind::ForDestructure { bindings, iter, body } => {
                self.check_expr(iter);
                self.push_scope();
                for name in bindings {
                    self.define_var(name, Type::Unknown, false);
                }
                for s in &body.stmts { self.check_stmt(s); }
                self.pop_scope();
            }
            StmtKind::Break | StmtKind::Continue => {}
        }
    }

    fn check_expr(&mut self, expr: &Expr) -> Type {
        match &expr.kind {
            ExprKind::Literal(lit) => match lit {
                Literal::Int(_) => Type::I64,
                Literal::Float(_) => Type::F64,
                Literal::String(_) => Type::Str,
                Literal::Bool(_) => Type::Bool,
            },
            ExprKind::Ident(name) => {
                self.lookup_var(name).unwrap_or_else(|| {
                    self.errors.push(AError::type_err(
                        format!("undefined variable '{name}'"),
                        expr.span.clone(),
                    ));
                    Type::Unknown
                })
            }
            ExprKind::BinOp { op, left, right } => {
                let lt = self.check_expr(left);
                let rt = self.check_expr(right);
                match op {
                    BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div | BinOp::Mod => {
                        if self.is_numeric(&lt) || lt == Type::Unknown {
                            lt
                        } else if self.is_numeric(&rt) || rt == Type::Unknown {
                            rt
                        } else {
                            self.errors.push(AError::type_err(
                                format!("arithmetic on non-numeric types: {lt:?}, {rt:?}"),
                                expr.span.clone(),
                            ));
                            Type::Unknown
                        }
                    }
                    BinOp::Eq | BinOp::NotEq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq => {
                        Type::Bool
                    }
                    BinOp::And | BinOp::Or => Type::Bool,
                }
            }
            ExprKind::UnaryOp { op, expr: inner } => {
                let t = self.check_expr(inner);
                match op {
                    UnaryOp::Neg => t,
                    UnaryOp::Not => Type::Bool,
                }
            }
            ExprKind::Call { func, args } => {
                if let ExprKind::FieldAccess { expr: obj, field } = &func.kind {
                    if let ExprKind::Ident(module) = &obj.kind {
                        let full_name = format!("{module}.{field}");
                        for arg in args { self.check_expr(arg); }
                        self.check_effect_call(&full_name, expr);
                        return self.fn_return_type(&full_name);
                    }
                }
                if let ExprKind::Ident(name) = &func.kind {
                    for arg in args { self.check_expr(arg); }
                    self.check_effect_call(name, expr);
                    return self.fn_return_type(name);
                }
                for arg in args { self.check_expr(arg); }
                Type::Unknown
            }
            ExprKind::FieldAccess { expr: inner, field } => {
                let t = self.check_expr(inner);
                if let Type::Record(fields) = &t {
                    for (n, ty) in fields {
                        if n == field { return ty.clone(); }
                    }
                }
                Type::Unknown
            }
            ExprKind::Index { expr: inner, index } => {
                let t = self.check_expr(inner);
                self.check_expr(index);
                match t {
                    Type::Array(elem) => *elem,
                    Type::Map(_, val) => *val,
                    _ => Type::Unknown,
                }
            }
            ExprKind::Try(inner) => {
                let t = self.check_expr(inner);
                if let Type::Result(ok, _) = t {
                    *ok
                } else {
                    Type::Unknown
                }
            }
            ExprKind::TryBlock(block) => {
                for stmt in &block.stmts {
                    self.check_stmt(stmt);
                }
                Type::Unknown
            }
            ExprKind::Array(elems) => {
                let elem_type = if let Some(first) = elems.first() {
                    self.check_expr(first)
                } else {
                    Type::Unknown
                };
                for e in elems.iter().skip(1) { self.check_expr(e); }
                Type::Array(Box::new(elem_type))
            }
            ExprKind::Record(fields) => {
                let typed: Vec<(String, Type)> = fields.iter()
                    .map(|(n, e)| (n.clone(), self.check_expr(e)))
                    .collect();
                Type::Record(typed)
            }
            ExprKind::If { cond, then_block, else_block } => {
                self.check_expr(cond);
                self.push_scope();
                let mut t = Type::Void;
                for s in &then_block.stmts { self.check_stmt(s); }
                if let Some(last) = then_block.stmts.last() {
                    if let StmtKind::Expr(e) = &last.kind {
                        t = self.check_expr(e);
                    }
                }
                self.pop_scope();
                self.push_scope();
                for s in &else_block.stmts { self.check_stmt(s); }
                self.pop_scope();
                t
            }
            ExprKind::Match { expr: inner, arms } => {
                self.check_expr(inner);
                let mut result_type = Type::Unknown;
                for arm in arms {
                    self.push_scope();
                    self.bind_pattern(&arm.pattern);
                    if let Some(g) = &arm.guard { self.check_expr(g); }
                    match &arm.body {
                        MatchBody::Expr(e) => { result_type = self.check_expr(e); }
                        MatchBody::Block(b) => {
                            for s in &b.stmts { self.check_stmt(s); }
                        }
                    }
                    self.pop_scope();
                }
                result_type
            }
            ExprKind::Block(block) => {
                self.push_scope();
                for s in &block.stmts { self.check_stmt(s); }
                self.pop_scope();
                Type::Void
            }
            ExprKind::Lambda { params, body } => {
                self.push_scope();
                let param_types: Vec<Type> = params.iter()
                    .map(|p| {
                        let t = self.resolve_type(&p.ty);
                        self.define_var(&p.name, t.clone(), false);
                        t
                    })
                    .collect();
                let ret = self.check_expr(body);
                self.pop_scope();
                Type::Fn(param_types, Box::new(ret))
            }
            ExprKind::Pipe { left, right } => {
                self.check_expr(left);
                self.check_expr(right);
                Type::Unknown
            }
            ExprKind::Interpolation { parts } => {
                for part in parts {
                    if let StringPart::Expr(e) = part {
                        self.check_expr(e);
                    }
                }
                Type::Str
            }
            ExprKind::MapLiteral(entries) => {
                let mut key_type = Type::Unknown;
                let mut val_type = Type::Unknown;
                for (k, v) in entries {
                    let kt = self.check_expr(k);
                    let vt = self.check_expr(v);
                    if key_type == Type::Unknown { key_type = kt; }
                    if val_type == Type::Unknown { val_type = vt; }
                }
                Type::Map(Box::new(key_type), Box::new(val_type))
            }
            ExprKind::Spread(inner) => {
                self.check_expr(inner);
                Type::Unknown
            }
        }
    }

    fn check_effect_call(&mut self, name: &str, expr: &Expr) {
        if let Some(sig) = self.fn_sigs.get(name).cloned() {
            for required_effect in &sig.effects {
                if required_effect == "pure" { continue; }
                if !self.current_effects.contains(required_effect) && !self.current_effects.contains(&"*".to_string()) {
                    self.errors.push(AError::effect_err(
                        format!(
                            "function '{name}' requires effect '{required_effect}' but current context only allows {:?}",
                            self.current_effects
                        ),
                        expr.span.clone(),
                    ));
                }
            }
        }
    }

    fn fn_return_type(&self, name: &str) -> Type {
        self.fn_sigs.get(name).map(|s| s.ret.clone()).unwrap_or(Type::Unknown)
    }

    fn resolve_type(&self, ty: &TypeExpr) -> Type {
        match ty {
            TypeExpr::I8 => Type::I8, TypeExpr::I16 => Type::I16,
            TypeExpr::I32 => Type::I32, TypeExpr::I64 => Type::I64,
            TypeExpr::U8 => Type::U8, TypeExpr::U16 => Type::U16,
            TypeExpr::U32 => Type::U32, TypeExpr::U64 => Type::U64,
            TypeExpr::F32 => Type::F32, TypeExpr::F64 => Type::F64,
            TypeExpr::Bool => Type::Bool, TypeExpr::Str => Type::Str,
            TypeExpr::Bytes => Type::Bytes, TypeExpr::Void => Type::Void, TypeExpr::Ptr => Type::Unknown,
            TypeExpr::Array(inner) => Type::Array(Box::new(self.resolve_type(inner))),
            TypeExpr::Tuple(types) => Type::Tuple(types.iter().map(|t| self.resolve_type(t)).collect()),
            TypeExpr::Record(fields) => Type::Record(fields.iter().map(|f| (f.name.clone(), self.resolve_type(&f.ty))).collect()),
            TypeExpr::FnType(params, ret) => Type::Fn(
                params.iter().map(|t| self.resolve_type(t)).collect(),
                Box::new(self.resolve_type(ret)),
            ),
            TypeExpr::Map(k, v) => Type::Map(
                Box::new(self.resolve_type(k)),
                Box::new(self.resolve_type(v)),
            ),
            TypeExpr::Named(name, args) => {
                if name == "Result" && args.len() == 2 {
                    Type::Result(
                        Box::new(self.resolve_type(&args[0])),
                        Box::new(self.resolve_type(&args[1])),
                    )
                } else {
                    Type::Named(name.clone(), args.iter().map(|t| self.resolve_type(t)).collect())
                }
            }
            TypeExpr::Infer => Type::Unknown,
        }
    }

    fn types_compatible(&self, expected: &Type, actual: &Type) -> bool {
        if *expected == Type::Unknown || *actual == Type::Unknown { return true; }
        match (expected, actual) {
            (Type::Array(a), Type::Array(b)) => self.types_compatible(a, b),
            (Type::Map(ak, av), Type::Map(bk, bv)) => {
                self.types_compatible(ak, bk) && self.types_compatible(av, bv)
            }
            (Type::Result(ao, ae), Type::Result(bo, be)) => {
                self.types_compatible(ao, bo) && self.types_compatible(ae, be)
            }
            _ => expected == actual,
        }
    }

    fn is_numeric(&self, t: &Type) -> bool {
        matches!(t, Type::I8 | Type::I16 | Type::I32 | Type::I64 |
                    Type::U8 | Type::U16 | Type::U32 | Type::U64 |
                    Type::F32 | Type::F64)
    }

    fn push_scope(&mut self) {
        self.scopes.push(HashMap::new());
    }

    fn pop_scope(&mut self) {
        self.scopes.pop();
    }

    fn define_var(&mut self, name: &str, ty: Type, mutable: bool) {
        if let Some(scope) = self.scopes.last_mut() {
            scope.insert(name.to_string(), (ty, mutable));
        }
    }

    fn lookup_var(&self, name: &str) -> Option<Type> {
        for scope in self.scopes.iter().rev() {
            if let Some((ty, _)) = scope.get(name) {
                return Some(ty.clone());
            }
        }
        None
    }

    fn bind_pattern(&mut self, pattern: &Pattern) {
        match pattern {
            Pattern::Ident(name) => {
                self.define_var(name, Type::Unknown, false);
            }
            Pattern::Constructor(_, sub) => {
                for p in sub { self.bind_pattern(p); }
            }
            Pattern::Array(elems) => {
                for elem in elems {
                    match elem {
                        ArrayPatElem::Pat(p) => self.bind_pattern(p),
                        ArrayPatElem::Rest(name) => { self.define_var(name, Type::Unknown, false); }
                    }
                }
            }
            Pattern::Map(entries) => {
                for (_, p) in entries { self.bind_pattern(p); }
            }
            _ => {}
        }
    }
}
