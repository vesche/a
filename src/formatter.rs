use crate::ast::*;
use crate::ast::StringPart;

pub fn format_program(program: &Program) -> String {
    let mut out = String::new();
    for (i, item) in program.items.iter().enumerate() {
        if i > 0 { out.push('\n'); }
        format_top_level(&mut out, item, 0);
    }
    out
}

fn indent(out: &mut String, level: usize) {
    for _ in 0..level {
        out.push_str("  ");
    }
}

fn format_top_level(out: &mut String, item: &TopLevel, level: usize) {
    match &item.kind {
        TopLevelKind::FnDecl(f) => format_fn(out, f, level),
        TopLevelKind::TypeDecl(t) => format_type_decl(out, t, level),
        TopLevelKind::ModDecl(m) => format_mod(out, m, level),
        TopLevelKind::UseDecl(u) => format_use(out, u, level),
    }
}

fn format_mod(out: &mut String, m: &ModDecl, level: usize) {
    indent(out, level);
    out.push_str(&format!("mod {} {{\n", m.name));
    for item in &m.items {
        format_top_level(out, item, level + 1);
        out.push('\n');
    }
    indent(out, level);
    out.push_str("}\n");
}

fn format_use(out: &mut String, u: &UseDecl, level: usize) {
    indent(out, level);
    out.push_str(&format!("use {}\n", u.path.join(".")));
}

fn format_type_decl(out: &mut String, t: &TypeDecl, level: usize) {
    indent(out, level);
    out.push_str(&format!("ty {}", t.name));
    if !t.params.is_empty() {
        out.push_str(&format!("<{}>", t.params.join(", ")));
    }
    out.push_str(" = ");
    match &t.body {
        TypeBody::Record(fields) => {
            out.push_str("{ ");
            for (i, f) in fields.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                out.push_str(&format!("{}: {}", f.name, format_type_expr(&f.ty)));
            }
            out.push_str(" }");
        }
        TypeBody::Sum(variants) => {
            for (i, v) in variants.iter().enumerate() {
                if i > 0 { out.push_str(" | "); }
                out.push_str(&v.name);
                if !v.fields.is_empty() {
                    out.push('(');
                    for (j, f) in v.fields.iter().enumerate() {
                        if j > 0 { out.push_str(", "); }
                        out.push_str(&format_type_expr(f));
                    }
                    out.push(')');
                }
            }
        }
        TypeBody::Alias(ty, where_clause) => {
            out.push_str(&format_type_expr(ty));
            if let Some(expr) = where_clause {
                out.push_str(" where { ");
                format_expr(out, expr);
                out.push_str(" }");
            }
        }
    }
    out.push('\n');
}

fn format_fn(out: &mut String, f: &FnDecl, level: usize) {
    indent(out, level);
    out.push_str(&format!("fn {}(", f.name));
    for (i, p) in f.params.iter().enumerate() {
        if i > 0 { out.push_str(", "); }
        out.push_str(&format!("{}: {}", p.name, format_type_expr(&p.ty)));
    }
    out.push_str(&format!(") -> {}", format_type_expr(&f.ret_type)));
    if !f.effects.is_empty() {
        out.push_str(&format!("\n  effects [{}]", f.effects.join(", ")));
    }
    if let Some(pre) = &f.pre {
        out.push_str("\n  pre { ");
        format_expr(out, pre);
        out.push_str(" }");
    }
    if let Some(post) = &f.post {
        out.push_str("\n  post { ");
        format_expr(out, post);
        out.push_str(" }");
    }
    out.push('\n');
    format_block(out, &f.body, level);
    out.push('\n');
}

fn format_type_expr(ty: &TypeExpr) -> String {
    match ty {
        TypeExpr::I8 => "i8".into(), TypeExpr::I16 => "i16".into(),
        TypeExpr::I32 => "i32".into(), TypeExpr::I64 => "i64".into(),
        TypeExpr::U8 => "u8".into(), TypeExpr::U16 => "u16".into(),
        TypeExpr::U32 => "u32".into(), TypeExpr::U64 => "u64".into(),
        TypeExpr::F32 => "f32".into(), TypeExpr::F64 => "f64".into(),
        TypeExpr::Bool => "bool".into(), TypeExpr::Str => "str".into(),
        TypeExpr::Bytes => "bytes".into(), TypeExpr::Void => "void".into(),
        TypeExpr::Array(inner) => format!("[{}]", format_type_expr(inner)),
        TypeExpr::Tuple(types) => {
            let inner: Vec<String> = types.iter().map(format_type_expr).collect();
            format!("({})", inner.join(", "))
        }
        TypeExpr::Record(fields) => {
            let inner: Vec<String> = fields.iter()
                .map(|f| format!("{}: {}", f.name, format_type_expr(&f.ty)))
                .collect();
            format!("{{ {} }}", inner.join(", "))
        }
        TypeExpr::FnType(params, ret) => {
            let ps: Vec<String> = params.iter().map(format_type_expr).collect();
            format!("fn({}) -> {}", ps.join(", "), format_type_expr(ret))
        }
        TypeExpr::Map(k, v) => format!("#{{{}: {}}}", format_type_expr(k), format_type_expr(v)),
        TypeExpr::Named(name, args) => {
            if args.is_empty() {
                name.clone()
            } else {
                let a: Vec<String> = args.iter().map(format_type_expr).collect();
                format!("{}<{}>", name, a.join(", "))
            }
        }
        TypeExpr::Infer => "_".into(),
    }
}

fn format_block(out: &mut String, block: &Block, level: usize) {
    indent(out, level);
    out.push_str("{\n");
    for stmt in &block.stmts {
        format_stmt(out, stmt, level + 1);
    }
    indent(out, level);
    out.push('}');
}

fn format_stmt(out: &mut String, stmt: &Stmt, level: usize) {
    indent(out, level);
    match &stmt.kind {
        StmtKind::Let { mutable, name, ty, value } => {
            out.push_str("let ");
            if *mutable { out.push_str("mut "); }
            if matches!(ty, TypeExpr::Infer) {
                out.push_str(&format!("{} = ", name));
            } else {
                out.push_str(&format!("{}: {} = ", name, format_type_expr(ty)));
            }
            format_expr(out, value);
            out.push('\n');
        }
        StmtKind::Assign { target, value } => {
            format_expr(out, target);
            out.push_str(" = ");
            format_expr(out, value);
            out.push('\n');
        }
        StmtKind::Return(expr) => {
            out.push_str("ret ");
            format_expr(out, expr);
            out.push('\n');
        }
        StmtKind::Expr(expr) => {
            format_expr(out, expr);
            out.push('\n');
        }
        StmtKind::If { cond, then_block, else_block } => {
            out.push_str("if ");
            format_expr(out, cond);
            out.push(' ');
            out.push('\n');
            format_block(out, then_block, level);
            if let Some(eb) = else_block {
                out.push_str(" else ");
                match eb {
                    ElseBranch::Block(b) => {
                        out.push('\n');
                        format_block(out, b, level);
                    }
                    ElseBranch::If(s) => {
                        format_stmt(out, s, level);
                        return;
                    }
                }
            }
            out.push('\n');
        }
        StmtKind::Match { expr, arms } => {
            out.push_str("match ");
            format_expr(out, expr);
            out.push_str(" {\n");
            for arm in arms {
                indent(out, level + 1);
                format_pattern(out, &arm.pattern);
                if let Some(guard) = &arm.guard {
                    out.push_str(" if ");
                    format_expr(out, guard);
                }
                out.push_str(" => ");
                match &arm.body {
                    MatchBody::Expr(e) => {
                        format_expr(out, e);
                        out.push('\n');
                    }
                    MatchBody::Block(b) => {
                        out.push('\n');
                        format_block(out, b, level + 1);
                        out.push('\n');
                    }
                }
            }
            indent(out, level);
            out.push_str("}\n");
        }
        StmtKind::For { var, ty, iter, body } => {
            if matches!(ty, TypeExpr::Infer) {
                out.push_str(&format!("for {} in ", var));
            } else {
                out.push_str(&format!("for {}: {} in ", var, format_type_expr(ty)));
            }
            format_expr(out, iter);
            out.push('\n');
            format_block(out, body, level);
            out.push('\n');
        }
        StmtKind::While { cond, body } => {
            out.push_str("while ");
            format_expr(out, cond);
            out.push('\n');
            format_block(out, body, level);
            out.push('\n');
        }
        StmtKind::LetDestructure { bindings, rest, value } => {
            out.push_str("let [");
            for (i, b) in bindings.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                match b {
                    Some(name) => out.push_str(name),
                    None => out.push('_'),
                }
            }
            if let Some(rest_name) = rest {
                if !bindings.is_empty() { out.push_str(", "); }
                out.push_str("...");
                out.push_str(rest_name);
            }
            out.push_str("] = ");
            format_expr(out, value);
            out.push('\n');
        }
        StmtKind::ForDestructure { bindings, iter, body } => {
            out.push_str("for [");
            for (i, name) in bindings.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                out.push_str(name);
            }
            out.push_str("] in ");
            format_expr(out, iter);
            out.push_str(" {\n");
            format_block(out, body, level + 1);
            indent(out, level);
            out.push_str("}\n");
        }
        StmtKind::Break => {
            out.push_str("break\n");
        }
        StmtKind::Continue => {
            out.push_str("continue\n");
        }
    }
}

fn format_expr(out: &mut String, expr: &Expr) {
    match &expr.kind {
        ExprKind::Literal(lit) => match lit {
            Literal::Int(v) => out.push_str(&v.to_string()),
            Literal::Float(v) => out.push_str(&format!("{v}")),
            Literal::String(v) => out.push_str(&format!("\"{v}\"")),
            Literal::Bool(v) => out.push_str(&v.to_string()),
        },
        ExprKind::Ident(name) => out.push_str(name),
        ExprKind::BinOp { op, left, right } => {
            out.push('(');
            format_expr(out, left);
            out.push_str(&format!(" {} ", format_binop(op)));
            format_expr(out, right);
            out.push(')');
        }
        ExprKind::UnaryOp { op, expr } => {
            match op {
                UnaryOp::Neg => out.push('-'),
                UnaryOp::Not => out.push('!'),
            }
            format_expr(out, expr);
        }
        ExprKind::Call { func, args } => {
            format_expr(out, func);
            out.push('(');
            for (i, a) in args.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                format_expr(out, a);
            }
            out.push(')');
        }
        ExprKind::FieldAccess { expr, field } => {
            format_expr(out, expr);
            out.push('.');
            out.push_str(field);
        }
        ExprKind::Index { expr, index } => {
            format_expr(out, expr);
            out.push('[');
            format_expr(out, index);
            out.push(']');
        }
        ExprKind::Try(inner) => {
            format_expr(out, inner);
            out.push('?');
        }
        ExprKind::TryBlock(block) => {
            out.push_str("try ");
            format_block(out, block, 0);
        }
        ExprKind::Array(elems) => {
            out.push('[');
            for (i, e) in elems.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                format_expr(out, e);
            }
            out.push(']');
        }
        ExprKind::Record(fields) => {
            out.push_str("{ ");
            for (i, (name, expr)) in fields.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                out.push_str(&format!("{name}: "));
                format_expr(out, expr);
            }
            out.push_str(" }");
        }
        ExprKind::If { cond, then_block, else_block } => {
            out.push_str("if ");
            format_expr(out, cond);
            out.push_str(" { ... } else { ... }");
            let _ = (then_block, else_block);
        }
        ExprKind::Match { expr, arms } => {
            out.push_str("match ");
            format_expr(out, expr);
            out.push_str(" { ... }");
            let _ = arms;
        }
        ExprKind::Block(_) => {
            out.push_str("{ ... }");
        }
        ExprKind::Lambda { params, body } => {
            out.push_str("fn(");
            for (i, p) in params.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                out.push_str(&format!("{}: {}", p.name, format_type_expr(&p.ty)));
            }
            out.push_str(") => ");
            format_expr(out, body);
        }
        ExprKind::Pipe { left, right } => {
            format_expr(out, left);
            out.push_str(" |> ");
            format_expr(out, right);
        }
        ExprKind::Interpolation { parts } => {
            out.push('"');
            for part in parts {
                match part {
                    StringPart::Lit(s) => out.push_str(s),
                    StringPart::Expr(e) => {
                        out.push('{');
                        format_expr(out, e);
                        out.push('}');
                    }
                }
            }
            out.push('"');
        }
        ExprKind::MapLiteral(entries) => {
            out.push_str("#{");
            for (i, (k, v)) in entries.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                format_expr(out, k);
                out.push_str(": ");
                format_expr(out, v);
            }
            out.push('}');
        }
        ExprKind::Spread(inner) => {
            out.push_str("...");
            format_expr(out, inner);
        }
    }
}

fn format_pattern(out: &mut String, pattern: &Pattern) {
    match pattern {
        Pattern::Wildcard => out.push('_'),
        Pattern::Ident(name) => out.push_str(name),
        Pattern::Literal(lit) => match lit {
            Literal::Int(v) => out.push_str(&v.to_string()),
            Literal::Float(v) => out.push_str(&format!("{v}")),
            Literal::String(v) => out.push_str(&format!("\"{v}\"")),
            Literal::Bool(v) => out.push_str(&v.to_string()),
        },
        Pattern::Constructor(name, subs) => {
            out.push_str(name);
            if !subs.is_empty() {
                out.push('(');
                for (i, p) in subs.iter().enumerate() {
                    if i > 0 { out.push_str(", "); }
                    format_pattern(out, p);
                }
                out.push(')');
            }
        }
        Pattern::Array(elems) => {
            out.push('[');
            for (i, elem) in elems.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                match elem {
                    ArrayPatElem::Pat(p) => format_pattern(out, p),
                    ArrayPatElem::Rest(name) => {
                        out.push_str("...");
                        out.push_str(name);
                    }
                }
            }
            out.push(']');
        }
        Pattern::Map(entries) => {
            out.push_str("#{");
            for (i, (key, pat)) in entries.iter().enumerate() {
                if i > 0 { out.push_str(", "); }
                out.push_str(&format!("\"{key}\": "));
                format_pattern(out, pat);
            }
            out.push('}');
        }
    }
}

fn format_binop(op: &BinOp) -> &'static str {
    match op {
        BinOp::Add => "+", BinOp::Sub => "-", BinOp::Mul => "*",
        BinOp::Div => "/", BinOp::Mod => "%",
        BinOp::Eq => "==", BinOp::NotEq => "!=",
        BinOp::Lt => "<", BinOp::Gt => ">",
        BinOp::LtEq => "<=", BinOp::GtEq => ">=",
        BinOp::And => "&&", BinOp::Or => "||",
    }
}
