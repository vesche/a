use crate::ast::*;
use crate::errors::{AError, AResult};
use crate::tokens::{Span, Token, TokenKind};

pub struct Parser {
    tokens: Vec<Token>,
    pos: usize,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Self {
        Parser { tokens, pos: 0 }
    }

    pub fn parse_program(&mut self) -> AResult<Program> {
        let mut items = Vec::new();
        self.skip_newlines();
        while !self.at_end() {
            items.push(self.parse_top_level()?);
            self.skip_newlines();
        }
        Ok(Program { items })
    }

    fn at_end(&self) -> bool {
        self.pos >= self.tokens.len() || self.peek_kind() == &TokenKind::Eof
    }

    fn peek(&self) -> &Token {
        if self.pos < self.tokens.len() {
            &self.tokens[self.pos]
        } else {
            self.tokens.last().unwrap()
        }
    }

    fn peek_kind(&self) -> &TokenKind {
        &self.peek().kind
    }

    fn peek_at(&self, offset: usize) -> Option<&Token> {
        let idx = self.pos + offset;
        if idx < self.tokens.len() { Some(&self.tokens[idx]) } else { None }
    }

    fn peek_span(&self) -> Option<Span> {
        Some(self.peek().span.clone())
    }

    fn advance(&mut self) -> &Token {
        let tok = &self.tokens[self.pos];
        self.pos += 1;
        tok
    }

    fn expect(&mut self, kind: &TokenKind) -> AResult<Token> {
        if self.peek_kind() == kind {
            Ok(self.advance().clone())
        } else {
            Err(AError::parse(
                format!("expected {kind}, found {}", self.peek_kind()),
                self.peek_span(),
            ))
        }
    }

    fn expect_ident(&mut self) -> AResult<String> {
        match self.peek_kind().clone() {
            TokenKind::Ident(name) => {
                let name = name.clone();
                self.advance();
                Ok(name)
            }
            _ => Err(AError::parse(
                format!("expected identifier, found {}", self.peek_kind()),
                self.peek_span(),
            )),
        }
    }

    fn skip_newlines(&mut self) {
        while self.pos < self.tokens.len() && self.peek_kind() == &TokenKind::Newline {
            self.advance();
        }
    }

    fn expect_newline_or_eof(&mut self) -> AResult<()> {
        if self.at_end() || self.peek_kind() == &TokenKind::RBrace {
            return Ok(());
        }
        if self.peek_kind() == &TokenKind::Newline {
            self.skip_newlines();
            Ok(())
        } else {
            Err(AError::parse(
                format!("expected newline, found {}", self.peek_kind()),
                self.peek_span(),
            ))
        }
    }

    fn parse_top_level(&mut self) -> AResult<TopLevel> {
        let span = self.peek_span();
        let kind = match self.peek_kind() {
            TokenKind::Fn => TopLevelKind::FnDecl(self.parse_fn_decl()?),
            TokenKind::Ty => TopLevelKind::TypeDecl(self.parse_type_decl()?),
            TokenKind::Mod => TopLevelKind::ModDecl(self.parse_mod_decl()?),
            TokenKind::Use => TopLevelKind::UseDecl(self.parse_use_decl()?),
            _ => return Err(AError::parse(
                format!("expected fn, ty, mod, or use; found {}", self.peek_kind()),
                self.peek_span(),
            )),
        };
        Ok(TopLevel { kind, span })
    }

    fn parse_mod_decl(&mut self) -> AResult<ModDecl> {
        self.expect(&TokenKind::Mod)?;
        let name = self.expect_ident()?;
        self.skip_newlines();
        self.expect(&TokenKind::LBrace)?;
        self.skip_newlines();
        let mut items = Vec::new();
        while self.peek_kind() != &TokenKind::RBrace {
            items.push(self.parse_top_level()?);
            self.skip_newlines();
        }
        self.expect(&TokenKind::RBrace)?;
        Ok(ModDecl { name, items })
    }

    fn parse_use_decl(&mut self) -> AResult<UseDecl> {
        self.expect(&TokenKind::Use)?;
        let mut path = vec![self.expect_ident()?];
        while self.peek_kind() == &TokenKind::Dot {
            self.advance();
            path.push(self.expect_ident()?);
        }
        self.expect_newline_or_eof()?;
        Ok(UseDecl { path })
    }

    fn parse_type_decl(&mut self) -> AResult<TypeDecl> {
        self.expect(&TokenKind::Ty)?;
        let name = self.expect_ident()?;
        let params = if self.peek_kind() == &TokenKind::LAngle {
            self.parse_type_params()?
        } else {
            Vec::new()
        };
        self.expect(&TokenKind::Eq)?;
        self.skip_newlines();
        let body = self.parse_type_body()?;
        self.expect_newline_or_eof()?;
        Ok(TypeDecl { name, params, body })
    }

    fn parse_type_params(&mut self) -> AResult<Vec<String>> {
        self.expect(&TokenKind::LAngle)?;
        let mut params = vec![self.expect_ident()?];
        while self.peek_kind() == &TokenKind::Comma {
            self.advance();
            params.push(self.expect_ident()?);
        }
        self.expect(&TokenKind::RAngle)?;
        Ok(params)
    }

    fn parse_type_body(&mut self) -> AResult<TypeBody> {
        if self.peek_kind() == &TokenKind::LBrace {
            let record = self.parse_record_fields()?;
            return Ok(TypeBody::Record(record));
        }

        if self.looks_like_sum_type() {
            let mut variants = vec![self.parse_variant()?];
            while self.peek_kind() == &TokenKind::Pipe {
                self.advance();
                self.skip_newlines();
                variants.push(self.parse_variant()?);
            }
            return Ok(TypeBody::Sum(variants));
        }

        let ty = self.parse_type_expr()?;

        let where_clause = if self.peek_kind() == &TokenKind::Where {
            self.advance();
            self.expect(&TokenKind::LBrace)?;
            self.skip_newlines();
            let expr = self.parse_expr()?;
            self.skip_newlines();
            self.expect(&TokenKind::RBrace)?;
            Some(expr)
        } else {
            None
        };

        Ok(TypeBody::Alias(ty, where_clause))
    }

    fn looks_like_sum_type(&self) -> bool {
        let mut i = self.pos;
        if let TokenKind::Ident(_) = &self.tokens[i].kind {
            i += 1;
            if i < self.tokens.len() {
                if let TokenKind::LParen = &self.tokens[i].kind {
                    return true;
                }
                if let TokenKind::Pipe = &self.tokens[i].kind {
                    return true;
                }
                if let TokenKind::Newline = &self.tokens[i].kind {
                    let mut j = i;
                    while j < self.tokens.len() && matches!(self.tokens[j].kind, TokenKind::Newline) {
                        j += 1;
                    }
                    if j < self.tokens.len() {
                        if let TokenKind::Pipe = &self.tokens[j].kind {
                            return true;
                        }
                    }
                }
            }
        }
        false
    }

    fn parse_variant(&mut self) -> AResult<Variant> {
        let name = self.expect_ident()?;
        let fields = if self.peek_kind() == &TokenKind::LParen {
            self.advance();
            let mut types = vec![self.parse_type_expr()?];
            while self.peek_kind() == &TokenKind::Comma {
                self.advance();
                types.push(self.parse_type_expr()?);
            }
            self.expect(&TokenKind::RParen)?;
            types
        } else {
            Vec::new()
        };
        Ok(Variant { name, fields })
    }

    fn parse_record_fields(&mut self) -> AResult<Vec<Field>> {
        self.expect(&TokenKind::LBrace)?;
        self.skip_newlines();
        let mut fields = Vec::new();
        if self.peek_kind() != &TokenKind::RBrace {
            fields.push(self.parse_field()?);
            while self.peek_kind() == &TokenKind::Comma {
                self.advance();
                self.skip_newlines();
                if self.peek_kind() == &TokenKind::RBrace {
                    break;
                }
                fields.push(self.parse_field()?);
            }
        }
        self.skip_newlines();
        self.expect(&TokenKind::RBrace)?;
        Ok(fields)
    }

    fn parse_field(&mut self) -> AResult<Field> {
        let name = self.expect_ident()?;
        self.expect(&TokenKind::Colon)?;
        let ty = self.parse_type_expr()?;
        Ok(Field { name, ty })
    }

    fn parse_type_expr(&mut self) -> AResult<TypeExpr> {
        match self.peek_kind().clone() {
            TokenKind::I8 => { self.advance(); Ok(TypeExpr::I8) }
            TokenKind::I16 => { self.advance(); Ok(TypeExpr::I16) }
            TokenKind::I32 => { self.advance(); Ok(TypeExpr::I32) }
            TokenKind::I64 => { self.advance(); Ok(TypeExpr::I64) }
            TokenKind::U8 => { self.advance(); Ok(TypeExpr::U8) }
            TokenKind::U16 => { self.advance(); Ok(TypeExpr::U16) }
            TokenKind::U32 => { self.advance(); Ok(TypeExpr::U32) }
            TokenKind::U64 => { self.advance(); Ok(TypeExpr::U64) }
            TokenKind::F32 => { self.advance(); Ok(TypeExpr::F32) }
            TokenKind::F64 => { self.advance(); Ok(TypeExpr::F64) }
            TokenKind::Bool => { self.advance(); Ok(TypeExpr::Bool) }
            TokenKind::Str => { self.advance(); Ok(TypeExpr::Str) }
            TokenKind::Bytes => { self.advance(); Ok(TypeExpr::Bytes) }
            TokenKind::Void => { self.advance(); Ok(TypeExpr::Void) }
            TokenKind::LBracket => {
                self.advance();
                let inner = self.parse_type_expr()?;
                self.expect(&TokenKind::RBracket)?;
                Ok(TypeExpr::Array(Box::new(inner)))
            }
            TokenKind::LParen => {
                self.advance();
                let mut types = Vec::new();
                if self.peek_kind() != &TokenKind::RParen {
                    types.push(self.parse_type_expr()?);
                    while self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                        types.push(self.parse_type_expr()?);
                    }
                }
                self.expect(&TokenKind::RParen)?;
                Ok(TypeExpr::Tuple(types))
            }
            TokenKind::Fn => {
                self.advance();
                self.expect(&TokenKind::LParen)?;
                let mut params = Vec::new();
                if self.peek_kind() != &TokenKind::RParen {
                    params.push(self.parse_type_expr()?);
                    while self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                        params.push(self.parse_type_expr()?);
                    }
                }
                self.expect(&TokenKind::RParen)?;
                self.expect(&TokenKind::Arrow)?;
                let ret = self.parse_type_expr()?;
                Ok(TypeExpr::FnType(params, Box::new(ret)))
            }
            TokenKind::Hash => {
                self.advance();
                self.expect(&TokenKind::LBrace)?;
                let key = self.parse_type_expr()?;
                self.expect(&TokenKind::Colon)?;
                let val = self.parse_type_expr()?;
                self.expect(&TokenKind::RBrace)?;
                Ok(TypeExpr::Map(Box::new(key), Box::new(val)))
            }
            TokenKind::Ident(name) => {
                let name = name.clone();
                self.advance();
                let type_args = if self.peek_kind() == &TokenKind::LAngle {
                    self.advance();
                    let mut args = vec![self.parse_type_expr()?];
                    while self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                        args.push(self.parse_type_expr()?);
                    }
                    self.expect(&TokenKind::RAngle)?;
                    args
                } else {
                    Vec::new()
                };
                Ok(TypeExpr::Named(name, type_args))
            }
            _ => Err(AError::parse(
                format!("expected type, found {}", self.peek_kind()),
                self.peek_span(),
            )),
        }
    }

    fn parse_fn_decl(&mut self) -> AResult<FnDecl> {
        self.expect(&TokenKind::Fn)?;
        let name = self.expect_ident()?;
        self.expect(&TokenKind::LParen)?;
        let params = if self.peek_kind() != &TokenKind::RParen {
            self.parse_param_list()?
        } else {
            Vec::new()
        };
        self.expect(&TokenKind::RParen)?;
        let ret_type = if self.peek_kind() == &TokenKind::Arrow {
            self.advance();
            self.parse_type_expr()?
        } else {
            TypeExpr::Infer
        };
        self.skip_newlines();

        let effects = if self.peek_kind() == &TokenKind::Effects {
            self.parse_effects()?
        } else {
            Vec::new()
        };
        self.skip_newlines();

        let pre = if self.peek_kind() == &TokenKind::Pre {
            self.advance();
            self.expect(&TokenKind::LBrace)?;
            self.skip_newlines();
            let e = self.parse_expr()?;
            self.skip_newlines();
            self.expect(&TokenKind::RBrace)?;
            self.skip_newlines();
            Some(e)
        } else {
            None
        };

        let post = if self.peek_kind() == &TokenKind::Post {
            self.advance();
            self.expect(&TokenKind::LBrace)?;
            self.skip_newlines();
            let e = self.parse_expr()?;
            self.skip_newlines();
            self.expect(&TokenKind::RBrace)?;
            self.skip_newlines();
            Some(e)
        } else {
            None
        };

        let body = self.parse_block()?;
        Ok(FnDecl { name, params, ret_type, effects, pre, post, body })
    }

    fn parse_param_list(&mut self) -> AResult<Vec<Param>> {
        let mut params = vec![self.parse_param()?];
        while self.peek_kind() == &TokenKind::Comma {
            self.advance();
            params.push(self.parse_param()?);
        }
        Ok(params)
    }

    fn parse_param(&mut self) -> AResult<Param> {
        let name = self.expect_ident()?;
        let ty = if self.peek_kind() == &TokenKind::Colon {
            self.advance();
            self.parse_type_expr()?
        } else {
            TypeExpr::Infer
        };
        Ok(Param { name, ty })
    }

    fn parse_effects(&mut self) -> AResult<Vec<String>> {
        self.expect(&TokenKind::Effects)?;
        self.expect(&TokenKind::LBracket)?;
        let mut effects = vec![self.expect_ident()?];
        while self.peek_kind() == &TokenKind::Comma {
            self.advance();
            effects.push(self.expect_ident()?);
        }
        self.expect(&TokenKind::RBracket)?;
        Ok(effects)
    }

    fn parse_block(&mut self) -> AResult<Block> {
        self.expect(&TokenKind::LBrace)?;
        self.skip_newlines();
        let mut stmts = Vec::new();
        while self.peek_kind() != &TokenKind::RBrace {
            stmts.push(self.parse_stmt()?);
            self.skip_newlines();
        }
        self.expect(&TokenKind::RBrace)?;
        Ok(Block { stmts })
    }

    fn parse_stmt(&mut self) -> AResult<Stmt> {
        let span = self.peek_span();
        let kind = match self.peek_kind() {
            TokenKind::Let => self.parse_let_stmt()?,
            TokenKind::Ret => self.parse_ret_stmt()?,
            TokenKind::If => self.parse_if_stmt()?,
            TokenKind::Match => self.parse_match_stmt()?,
            TokenKind::For => self.parse_for_stmt()?,
            TokenKind::While => self.parse_while_stmt()?,
            TokenKind::Break => {
                self.advance();
                self.expect_newline_or_eof()?;
                StmtKind::Break
            }
            TokenKind::Continue => {
                self.advance();
                self.expect_newline_or_eof()?;
                StmtKind::Continue
            }
            _ => {
                let expr = self.parse_expr()?;
                if self.peek_kind() == &TokenKind::Eq {
                    self.advance();
                    self.skip_newlines();
                    let value = self.parse_expr()?;
                    self.expect_newline_or_eof()?;
                    StmtKind::Assign { target: expr, value }
                } else {
                    self.expect_newline_or_eof()?;
                    StmtKind::Expr(expr)
                }
            }
        };
        Ok(Stmt { kind, span })
    }

    fn parse_let_stmt(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::Let)?;
        let mutable = if self.peek_kind() == &TokenKind::Mut {
            self.advance();
            true
        } else {
            false
        };
        if self.peek_kind() == &TokenKind::LBracket {
            return self.parse_let_destructure();
        }
        let name = self.expect_ident()?;
        let ty = if self.peek_kind() == &TokenKind::Colon {
            self.advance();
            self.parse_type_expr()?
        } else {
            TypeExpr::Infer
        };
        self.expect(&TokenKind::Eq)?;
        self.skip_newlines();
        let value = self.parse_expr()?;
        self.expect_newline_or_eof()?;
        Ok(StmtKind::Let { mutable, name, ty, value })
    }

    fn parse_let_destructure(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::LBracket)?;
        self.skip_newlines();
        let mut bindings: Vec<Option<String>> = Vec::new();
        let mut rest: Option<String> = None;
        while self.peek_kind() != &TokenKind::RBracket {
            if self.peek_kind() == &TokenKind::DotDotDot {
                self.advance();
                if self.peek_kind() != &TokenKind::RBracket && self.peek_kind() != &TokenKind::Comma {
                    rest = Some(self.expect_ident()?);
                }
                if self.peek_kind() == &TokenKind::Comma { self.advance(); self.skip_newlines(); }
                break;
            }
            if self.peek_kind() == &TokenKind::Underscore {
                self.advance();
                bindings.push(None);
            } else {
                let name = self.expect_ident()?;
                bindings.push(Some(name));
            }
            if self.peek_kind() == &TokenKind::Comma {
                self.advance();
                self.skip_newlines();
            } else {
                break;
            }
        }
        self.skip_newlines();
        self.expect(&TokenKind::RBracket)?;
        self.expect(&TokenKind::Eq)?;
        self.skip_newlines();
        let value = self.parse_expr()?;
        self.expect_newline_or_eof()?;
        Ok(StmtKind::LetDestructure { bindings, rest, value })
    }

    fn parse_ret_stmt(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::Ret)?;
        let expr = self.parse_expr()?;
        self.expect_newline_or_eof()?;
        Ok(StmtKind::Return(expr))
    }

    fn parse_if_stmt(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::If)?;
        let cond = self.parse_expr()?;
        self.skip_newlines();
        let then_block = self.parse_block()?;
        self.skip_newlines();
        let else_block = if self.peek_kind() == &TokenKind::Else {
            self.advance();
            self.skip_newlines();
            if self.peek_kind() == &TokenKind::If {
                let if_stmt = self.parse_if_stmt()?;
                Some(ElseBranch::If(Box::new(Stmt { kind: if_stmt, span: None })))
            } else {
                Some(ElseBranch::Block(self.parse_block()?))
            }
        } else {
            None
        };
        Ok(StmtKind::If { cond, then_block, else_block })
    }

    fn parse_match_stmt(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::Match)?;
        let expr = self.parse_expr()?;
        self.skip_newlines();
        self.expect(&TokenKind::LBrace)?;
        self.skip_newlines();
        let mut arms = Vec::new();
        while self.peek_kind() != &TokenKind::RBrace {
            arms.push(self.parse_match_arm()?);
            self.skip_newlines();
        }
        self.expect(&TokenKind::RBrace)?;
        Ok(StmtKind::Match { expr, arms })
    }

    fn parse_match_arm(&mut self) -> AResult<MatchArm> {
        let pattern = self.parse_pattern()?;
        let guard = if self.peek_kind() == &TokenKind::If {
            self.advance();
            Some(self.parse_expr()?)
        } else {
            None
        };
        self.expect(&TokenKind::FatArrow)?;
        self.skip_newlines();
        let body = if self.peek_kind() == &TokenKind::LBrace {
            MatchBody::Block(self.parse_block()?)
        } else {
            MatchBody::Expr(self.parse_expr()?)
        };
        self.skip_newlines();
        Ok(MatchArm { pattern, guard, body })
    }

    fn parse_pattern(&mut self) -> AResult<Pattern> {
        match self.peek_kind().clone() {
            TokenKind::Underscore => {
                self.advance();
                Ok(Pattern::Wildcard)
            }
            TokenKind::IntLit(v) => {
                self.advance();
                Ok(Pattern::Literal(Literal::Int(v)))
            }
            TokenKind::FloatLit(v) => {
                self.advance();
                Ok(Pattern::Literal(Literal::Float(v)))
            }
            TokenKind::StringLit(s) => {
                let s = s.clone();
                self.advance();
                Ok(Pattern::Literal(Literal::String(s)))
            }
            TokenKind::True => {
                self.advance();
                Ok(Pattern::Literal(Literal::Bool(true)))
            }
            TokenKind::False => {
                self.advance();
                Ok(Pattern::Literal(Literal::Bool(false)))
            }
            TokenKind::LBracket => {
                self.advance();
                let mut elems = Vec::new();
                while self.peek_kind() != &TokenKind::RBracket {
                    if self.peek_kind() == &TokenKind::DotDotDot {
                        self.advance();
                        let rest_name = match self.peek_kind().clone() {
                            TokenKind::Ident(n) => {
                                let n = n.clone();
                                self.advance();
                                n
                            }
                            _ => return Err(AError::parse(
                                "expected identifier after ...".to_string(),
                                self.peek_span(),
                            )),
                        };
                        elems.push(ArrayPatElem::Rest(rest_name));
                        break;
                    }
                    elems.push(ArrayPatElem::Pat(self.parse_pattern()?));
                    if self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                    } else {
                        break;
                    }
                }
                self.expect(&TokenKind::RBracket)?;
                Ok(Pattern::Array(elems))
            }
            TokenKind::Hash => {
                self.advance();
                self.expect(&TokenKind::LBrace)?;
                let mut entries = Vec::new();
                while self.peek_kind() != &TokenKind::RBrace {
                    let key = match self.peek_kind().clone() {
                        TokenKind::StringLit(s) => {
                            let s = s.clone();
                            self.advance();
                            s
                        }
                        TokenKind::Ident(n) => {
                            let n = n.clone();
                            self.advance();
                            n
                        }
                        _ => return Err(AError::parse(
                            "expected string key in map pattern".to_string(),
                            self.peek_span(),
                        )),
                    };
                    self.expect(&TokenKind::Colon)?;
                    let pat = self.parse_pattern()?;
                    entries.push((key, pat));
                    if self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                    } else {
                        break;
                    }
                }
                self.expect(&TokenKind::RBrace)?;
                Ok(Pattern::Map(entries))
            }
            TokenKind::Ident(name) => {
                let name = name.clone();
                self.advance();
                if self.peek_kind() == &TokenKind::LParen {
                    self.advance();
                    let mut patterns = Vec::new();
                    if self.peek_kind() != &TokenKind::RParen {
                        patterns.push(self.parse_pattern()?);
                        while self.peek_kind() == &TokenKind::Comma {
                            self.advance();
                            patterns.push(self.parse_pattern()?);
                        }
                    }
                    self.expect(&TokenKind::RParen)?;
                    Ok(Pattern::Constructor(name, patterns))
                } else {
                    Ok(Pattern::Ident(name))
                }
            }
            _ => Err(AError::parse(
                format!("expected pattern, found {}", self.peek_kind()),
                self.peek_span(),
            )),
        }
    }

    fn parse_for_stmt(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::For)?;
        if self.peek_kind() == &TokenKind::LBracket {
            return self.parse_for_destructure();
        }
        let var = self.expect_ident()?;
        let ty = if self.peek_kind() == &TokenKind::Colon {
            self.advance();
            self.parse_type_expr()?
        } else {
            TypeExpr::Infer
        };
        self.expect(&TokenKind::In)?;
        let iter = self.parse_expr()?;
        self.skip_newlines();
        let body = self.parse_block()?;
        Ok(StmtKind::For { var, ty, iter, body })
    }

    fn parse_for_destructure(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::LBracket)?;
        self.skip_newlines();
        let mut bindings: Vec<String> = Vec::new();
        while self.peek_kind() != &TokenKind::RBracket {
            bindings.push(self.expect_ident()?);
            if self.peek_kind() == &TokenKind::Comma {
                self.advance();
                self.skip_newlines();
            } else {
                break;
            }
        }
        self.skip_newlines();
        self.expect(&TokenKind::RBracket)?;
        self.expect(&TokenKind::In)?;
        let iter = self.parse_expr()?;
        self.skip_newlines();
        let body = self.parse_block()?;
        Ok(StmtKind::ForDestructure { bindings, iter, body })
    }

    fn parse_while_stmt(&mut self) -> AResult<StmtKind> {
        self.expect(&TokenKind::While)?;
        let cond = self.parse_expr()?;
        self.skip_newlines();
        let body = self.parse_block()?;
        Ok(StmtKind::While { cond, body })
    }

    // --- Expression parsing with precedence climbing ---

    fn parse_expr(&mut self) -> AResult<Expr> {
        self.parse_pipe_expr()
    }

    fn parse_pipe_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_or_expr()?;
        loop {
            if self.peek_kind() == &TokenKind::PipeArrow {
                self.advance();
                self.skip_newlines();
                let right = self.parse_or_expr()?;
                left = Expr {
                    span: left.span.clone(),
                    kind: ExprKind::Pipe {
                        left: Box::new(left),
                        right: Box::new(right),
                    },
                };
            } else if self.peek_kind() == &TokenKind::Newline {
                let saved = self.pos;
                self.skip_newlines();
                if self.peek_kind() == &TokenKind::PipeArrow {
                    self.advance();
                    self.skip_newlines();
                    let right = self.parse_or_expr()?;
                    left = Expr {
                        span: left.span.clone(),
                        kind: ExprKind::Pipe {
                            left: Box::new(left),
                            right: Box::new(right),
                        },
                    };
                } else {
                    self.pos = saved;
                    break;
                }
            } else {
                break;
            }
        }
        Ok(left)
    }

    fn parse_or_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_and_expr()?;
        while self.peek_kind() == &TokenKind::Or {
            self.advance();
            self.skip_newlines();
            let right = self.parse_and_expr()?;
            left = Expr {
                span: left.span.clone(),
                kind: ExprKind::BinOp {
                    op: BinOp::Or,
                    left: Box::new(left),
                    right: Box::new(right),
                },
            };
        }
        Ok(left)
    }

    fn parse_and_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_equality_expr()?;
        while self.peek_kind() == &TokenKind::And {
            self.advance();
            self.skip_newlines();
            let right = self.parse_equality_expr()?;
            left = Expr {
                span: left.span.clone(),
                kind: ExprKind::BinOp {
                    op: BinOp::And,
                    left: Box::new(left),
                    right: Box::new(right),
                },
            };
        }
        Ok(left)
    }

    fn parse_equality_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_comparison_expr()?;
        loop {
            let op = match self.peek_kind() {
                TokenKind::EqEq => BinOp::Eq,
                TokenKind::NotEq => BinOp::NotEq,
                _ => break,
            };
            self.advance();
            self.skip_newlines();
            let right = self.parse_comparison_expr()?;
            left = Expr {
                span: left.span.clone(),
                kind: ExprKind::BinOp { op, left: Box::new(left), right: Box::new(right) },
            };
        }
        Ok(left)
    }

    fn parse_comparison_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_additive_expr()?;
        loop {
            let op = match self.peek_kind() {
                TokenKind::LAngle => BinOp::Lt,
                TokenKind::RAngle => BinOp::Gt,
                TokenKind::LessEq => BinOp::LtEq,
                TokenKind::GreaterEq => BinOp::GtEq,
                _ => break,
            };
            self.advance();
            self.skip_newlines();
            let right = self.parse_additive_expr()?;
            left = Expr {
                span: left.span.clone(),
                kind: ExprKind::BinOp { op, left: Box::new(left), right: Box::new(right) },
            };
        }
        Ok(left)
    }

    fn parse_additive_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_multiplicative_expr()?;
        loop {
            let op = match self.peek_kind() {
                TokenKind::Plus => BinOp::Add,
                TokenKind::Minus => BinOp::Sub,
                _ => break,
            };
            self.advance();
            self.skip_newlines();
            let right = self.parse_multiplicative_expr()?;
            left = Expr {
                span: left.span.clone(),
                kind: ExprKind::BinOp { op, left: Box::new(left), right: Box::new(right) },
            };
        }
        Ok(left)
    }

    fn parse_multiplicative_expr(&mut self) -> AResult<Expr> {
        let mut left = self.parse_unary_expr()?;
        loop {
            let op = match self.peek_kind() {
                TokenKind::Star => BinOp::Mul,
                TokenKind::Slash => BinOp::Div,
                TokenKind::Percent => BinOp::Mod,
                _ => break,
            };
            self.advance();
            self.skip_newlines();
            let right = self.parse_unary_expr()?;
            left = Expr {
                span: left.span.clone(),
                kind: ExprKind::BinOp { op, left: Box::new(left), right: Box::new(right) },
            };
        }
        Ok(left)
    }

    fn parse_unary_expr(&mut self) -> AResult<Expr> {
        match self.peek_kind() {
            TokenKind::Minus => {
                let span = self.peek_span();
                self.advance();
                let expr = self.parse_unary_expr()?;
                Ok(Expr { span, kind: ExprKind::UnaryOp { op: UnaryOp::Neg, expr: Box::new(expr) } })
            }
            TokenKind::Bang => {
                let span = self.peek_span();
                self.advance();
                let expr = self.parse_unary_expr()?;
                Ok(Expr { span, kind: ExprKind::UnaryOp { op: UnaryOp::Not, expr: Box::new(expr) } })
            }
            TokenKind::Try => {
                let span = self.peek_span();
                self.advance();
                if self.peek_kind() == &TokenKind::LBrace {
                    self.skip_newlines();
                    let block = self.parse_block()?;
                    Ok(Expr { span, kind: ExprKind::TryBlock(block) })
                } else {
                    let expr = self.parse_unary_expr()?;
                    Ok(Expr { span, kind: ExprKind::Try(Box::new(expr)) })
                }
            }
            _ => self.parse_postfix_expr(),
        }
    }

    fn parse_postfix_expr(&mut self) -> AResult<Expr> {
        let mut expr = self.parse_primary_expr()?;
        loop {
            match self.peek_kind() {
                TokenKind::LParen => {
                    self.advance();
                    self.skip_newlines();
                    let mut args = Vec::new();
                    if self.peek_kind() != &TokenKind::RParen {
                        args.push(self.parse_expr()?);
                        while self.peek_kind() == &TokenKind::Comma {
                            self.advance();
                            self.skip_newlines();
                            args.push(self.parse_expr()?);
                        }
                    }
                    self.skip_newlines();
                    self.expect(&TokenKind::RParen)?;
                    expr = Expr {
                        span: expr.span.clone(),
                        kind: ExprKind::Call { func: Box::new(expr), args },
                    };
                }
                TokenKind::Dot => {
                    self.advance();
                    let field = self.expect_ident()?;
                    expr = Expr {
                        span: expr.span.clone(),
                        kind: ExprKind::FieldAccess { expr: Box::new(expr), field },
                    };
                }
                TokenKind::LBracket => {
                    self.advance();
                    let index = self.parse_expr()?;
                    self.expect(&TokenKind::RBracket)?;
                    expr = Expr {
                        span: expr.span.clone(),
                        kind: ExprKind::Index { expr: Box::new(expr), index: Box::new(index) },
                    };
                }
                TokenKind::Question => {
                    self.advance();
                    expr = Expr {
                        span: expr.span.clone(),
                        kind: ExprKind::Try(Box::new(expr)),
                    };
                }
                _ => break,
            }
        }
        Ok(expr)
    }

    fn parse_primary_expr(&mut self) -> AResult<Expr> {
        let span = self.peek_span();
        match self.peek_kind().clone() {
            TokenKind::IntLit(v) => {
                self.advance();
                Ok(Expr { span, kind: ExprKind::Literal(Literal::Int(v)) })
            }
            TokenKind::FloatLit(v) => {
                self.advance();
                Ok(Expr { span, kind: ExprKind::Literal(Literal::Float(v)) })
            }
            TokenKind::StringLit(s) => {
                let s = s.clone();
                self.advance();
                Ok(Expr { span, kind: ExprKind::Literal(Literal::String(s)) })
            }
            TokenKind::True => {
                self.advance();
                Ok(Expr { span, kind: ExprKind::Literal(Literal::Bool(true)) })
            }
            TokenKind::False => {
                self.advance();
                Ok(Expr { span, kind: ExprKind::Literal(Literal::Bool(false)) })
            }
            TokenKind::Ident(name) => {
                let name = name.clone();
                self.advance();
                Ok(Expr { span, kind: ExprKind::Ident(name) })
            }
            TokenKind::Ret => {
                self.advance();
                Ok(Expr { span, kind: ExprKind::Ident("ret".into()) })
            }
            TokenKind::Str if self.peek_at(1).map(|t| &t.kind) == Some(&TokenKind::Dot) => {
                self.advance();
                Ok(Expr { span, kind: ExprKind::Ident("str".into()) })
            }
            TokenKind::Fn => {
                self.advance();
                self.expect(&TokenKind::LParen)?;
                let params = if self.peek_kind() != &TokenKind::RParen {
                    self.parse_param_list()?
                } else {
                    Vec::new()
                };
                self.expect(&TokenKind::RParen)?;
                self.skip_newlines();
                let body = if self.peek_kind() == &TokenKind::FatArrow {
                    self.advance();
                    self.skip_newlines();
                    self.parse_expr()?
                } else {
                    let block = self.parse_block()?;
                    Expr { span: None, kind: ExprKind::Block(block) }
                };
                Ok(Expr { span, kind: ExprKind::Lambda { params, body: Box::new(body) } })
            }
            TokenKind::InterpStart(_) => {
                self.parse_interp_string()
            }
            TokenKind::Hash => {
                self.advance();
                self.expect(&TokenKind::LBrace)?;
                self.skip_newlines();
                let mut entries = Vec::new();
                if self.peek_kind() != &TokenKind::RBrace {
                    let key = self.parse_expr()?;
                    self.expect(&TokenKind::Colon)?;
                    self.skip_newlines();
                    let val = self.parse_expr()?;
                    entries.push((key, val));
                    while self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                        self.skip_newlines();
                        if self.peek_kind() == &TokenKind::RBrace { break; }
                        let key = self.parse_expr()?;
                        self.expect(&TokenKind::Colon)?;
                        self.skip_newlines();
                        let val = self.parse_expr()?;
                        entries.push((key, val));
                    }
                }
                self.skip_newlines();
                self.expect(&TokenKind::RBrace)?;
                Ok(Expr { span, kind: ExprKind::MapLiteral(entries) })
            }
            TokenKind::LParen => {
                self.advance();
                self.skip_newlines();
                let expr = self.parse_expr()?;
                self.skip_newlines();
                self.expect(&TokenKind::RParen)?;
                Ok(expr)
            }
            TokenKind::LBracket => {
                self.advance();
                self.skip_newlines();
                let mut elems = Vec::new();
                if self.peek_kind() != &TokenKind::RBracket {
                    elems.push(self.parse_array_element()?);
                    while self.peek_kind() == &TokenKind::Comma {
                        self.advance();
                        self.skip_newlines();
                        if self.peek_kind() == &TokenKind::RBracket {
                            break;
                        }
                        elems.push(self.parse_array_element()?);
                    }
                }
                self.skip_newlines();
                self.expect(&TokenKind::RBracket)?;
                Ok(Expr { span, kind: ExprKind::Array(elems) })
            }
            _ => Err(AError::parse(
                format!("expected expression, found {}", self.peek_kind()),
                self.peek_span(),
            )),
        }
    }

    fn parse_array_element(&mut self) -> AResult<Expr> {
        if self.peek_kind() == &TokenKind::DotDotDot {
            let span = self.peek_span();
            self.advance();
            let expr = self.parse_expr()?;
            Ok(Expr { span, kind: ExprKind::Spread(Box::new(expr)) })
        } else {
            self.parse_expr()
        }
    }

    fn parse_interp_string(&mut self) -> AResult<Expr> {
        let span = self.peek_span();
        let mut parts: Vec<StringPart> = Vec::new();

        if let TokenKind::InterpStart(s) = self.peek_kind().clone() {
            if !s.is_empty() { parts.push(StringPart::Lit(s)); }
            self.advance();
        } else {
            return Err(AError::parse("expected interpolated string", self.peek_span()));
        }

        loop {
            let expr = self.parse_expr()?;
            parts.push(StringPart::Expr(expr));

            match self.peek_kind().clone() {
                TokenKind::InterpMid(s) => {
                    if !s.is_empty() { parts.push(StringPart::Lit(s)); }
                    self.advance();
                }
                TokenKind::InterpEnd(s) => {
                    if !s.is_empty() { parts.push(StringPart::Lit(s)); }
                    self.advance();
                    break;
                }
                _ => return Err(AError::parse(
                    format!("expected interpolation continuation, found {}", self.peek_kind()),
                    self.peek_span(),
                )),
            }
        }

        Ok(Expr { span, kind: ExprKind::Interpolation { parts } })
    }
}
