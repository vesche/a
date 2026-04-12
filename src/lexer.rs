use crate::errors::{AError, AResult};
use crate::tokens::{Span, Token, TokenKind};

pub struct Lexer {
    src: Vec<char>,
    pos: usize,
    line: usize,
    col: usize,
    interp_depth: usize,
    interp_parts: Vec<TokenKind>,
}

impl Lexer {
    pub fn new(source: &str) -> Self {
        Lexer {
            src: source.chars().collect(),
            pos: 0,
            line: 1,
            col: 1,
            interp_depth: 0,
            interp_parts: Vec::new(),
        }
    }

    pub fn tokenize(&mut self) -> AResult<Vec<Token>> {
        let mut tokens = Vec::new();
        loop {
            self.skip_whitespace_and_comments();
            if self.at_end() {
                tokens.push(self.make_token(TokenKind::Eof, self.pos, 0));
                break;
            }
            let tok = self.next_token()?;
            tokens.push(tok);
        }
        Ok(tokens)
    }

    fn at_end(&self) -> bool {
        self.pos >= self.src.len()
    }

    fn peek(&self) -> char {
        if self.at_end() { '\0' } else { self.src[self.pos] }
    }

    fn peek_next(&self) -> char {
        if self.pos + 1 >= self.src.len() { '\0' } else { self.src[self.pos + 1] }
    }

    fn advance(&mut self) -> char {
        let c = self.src[self.pos];
        self.pos += 1;
        if c == '\n' {
            self.line += 1;
            self.col = 1;
        } else {
            self.col += 1;
        }
        c
    }

    fn span_at(&self, offset: usize, len: usize) -> Span {
        let mut line = 1;
        let mut col = 1;
        for i in 0..offset.min(self.src.len()) {
            if self.src[i] == '\n' {
                line += 1;
                col = 1;
            } else {
                col += 1;
            }
        }
        Span { line, col, offset, len }
    }

    fn make_token(&self, kind: TokenKind, start: usize, len: usize) -> Token {
        Token { kind, span: self.span_at(start, len) }
    }

    fn skip_whitespace_and_comments(&mut self) {
        while !self.at_end() {
            match self.peek() {
                ' ' | '\t' | '\r' => { self.advance(); }
                ';' => {
                    while !self.at_end() && self.peek() != '\n' {
                        self.advance();
                    }
                }
                _ => break,
            }
        }
    }

    fn next_token(&mut self) -> AResult<Token> {
        let start = self.pos;
        let c = self.advance();

        match c {
            '\n' => Ok(self.make_token(TokenKind::Newline, start, 1)),
            '(' => Ok(self.make_token(TokenKind::LParen, start, 1)),
            ')' => Ok(self.make_token(TokenKind::RParen, start, 1)),
            '{' => Ok(self.make_token(TokenKind::LBrace, start, 1)),
            '}' if self.interp_depth > 0 => {
                return self.lex_interp_continue();
            }
            '}' => Ok(self.make_token(TokenKind::RBrace, start, 1)),
            '[' => Ok(self.make_token(TokenKind::LBracket, start, 1)),
            ']' => Ok(self.make_token(TokenKind::RBracket, start, 1)),
            ',' => Ok(self.make_token(TokenKind::Comma, start, 1)),
            ':' => Ok(self.make_token(TokenKind::Colon, start, 1)),
            '.' => {
                if self.peek() == '.' && self.peek_next() == '.' {
                    self.advance();
                    self.advance();
                    Ok(self.make_token(TokenKind::DotDotDot, start, 3))
                } else {
                    Ok(self.make_token(TokenKind::Dot, start, 1))
                }
            }
            '+' => Ok(self.make_token(TokenKind::Plus, start, 1)),
            '*' => Ok(self.make_token(TokenKind::Star, start, 1)),
            '/' => Ok(self.make_token(TokenKind::Slash, start, 1)),
            '%' => Ok(self.make_token(TokenKind::Percent, start, 1)),
            '|' if self.peek() == '|' => {
                self.advance();
                Ok(self.make_token(TokenKind::Or, start, 2))
            }
            '|' if self.peek() == '>' => {
                self.advance();
                Ok(self.make_token(TokenKind::PipeArrow, start, 2))
            }
            '|' => Ok(self.make_token(TokenKind::Pipe, start, 1)),
            '#' => Ok(self.make_token(TokenKind::Hash, start, 1)),
            '&' if self.peek() == '&' => {
                self.advance();
                Ok(self.make_token(TokenKind::And, start, 2))
            }
            '-' if self.peek() == '>' => {
                self.advance();
                Ok(self.make_token(TokenKind::Arrow, start, 2))
            }
            '-' => Ok(self.make_token(TokenKind::Minus, start, 1)),
            '=' if self.peek() == '>' => {
                self.advance();
                Ok(self.make_token(TokenKind::FatArrow, start, 2))
            }
            '=' if self.peek() == '=' => {
                self.advance();
                Ok(self.make_token(TokenKind::EqEq, start, 2))
            }
            '=' => Ok(self.make_token(TokenKind::Eq, start, 1)),
            '!' if self.peek() == '=' => {
                self.advance();
                Ok(self.make_token(TokenKind::NotEq, start, 2))
            }
            '!' => Ok(self.make_token(TokenKind::Bang, start, 1)),
            '<' if self.peek() == '=' => {
                self.advance();
                Ok(self.make_token(TokenKind::LessEq, start, 2))
            }
            '<' => Ok(self.make_token(TokenKind::LAngle, start, 1)),
            '>' if self.peek() == '=' => {
                self.advance();
                Ok(self.make_token(TokenKind::GreaterEq, start, 2))
            }
            '>' => Ok(self.make_token(TokenKind::RAngle, start, 1)),

            '"' => self.lex_string(start),
            '?' => Ok(self.make_token(TokenKind::Question, start, 1)),

            '`' if self.peek() == '`' && self.peek_next() == '`' => {
                self.advance();
                self.advance();
                self.lex_heredoc(start)
            }

            c if c.is_ascii_digit() => self.lex_number(start),

            c if c.is_ascii_alphabetic() || c == '_' => {
                if c == 'r' && self.peek() == '"' {
                    self.advance();
                    self.lex_raw_string(start, 0)
                } else if c == 'r' && self.peek() == '#' {
                    let save_pos = self.pos;
                    let save_col = self.col;
                    let mut hashes = 0;
                    while self.peek() == '#' {
                        self.advance();
                        hashes += 1;
                    }
                    if self.peek() == '"' {
                        self.advance();
                        self.lex_raw_string(start, hashes)
                    } else {
                        self.pos = save_pos;
                        self.col = save_col;
                        self.lex_ident_or_keyword(start)
                    }
                } else {
                    self.lex_ident_or_keyword(start)
                }
            }

            _ => Err(AError::lex(
                format!("unexpected character '{c}'"),
                self.span_at(start, 1),
            )),
        }
    }

    fn lex_string(&mut self, start: usize) -> AResult<Token> {
        let mut s = String::new();
        loop {
            if self.at_end() {
                return Err(AError::lex("unterminated string", self.span_at(start, self.pos - start)));
            }
            let c = self.advance();
            match c {
                '"' => break,
                '{' => {
                    let len = self.pos - start;
                    self.interp_parts.push(TokenKind::InterpStart(s));
                    self.interp_depth += 1;
                    return self.finish_interp_start(start, len);
                }
                '\\' => {
                    if self.at_end() {
                        return Err(AError::lex("unterminated escape", self.span_at(start, self.pos - start)));
                    }
                    let esc = self.advance();
                    match esc {
                        'n' => s.push('\n'),
                        't' => s.push('\t'),
                        'r' => s.push('\r'),
                        '\\' => s.push('\\'),
                        '"' => s.push('"'),
                        '{' => s.push('{'),
                        '}' => s.push('}'),
                        _ => return Err(AError::lex(
                            format!("unknown escape '\\{esc}'"),
                            self.span_at(self.pos - 2, 2),
                        )),
                    }
                }
                _ => s.push(c),
            }
        }
        let len = self.pos - start;
        Ok(self.make_token(TokenKind::StringLit(s), start, len))
    }

    fn finish_interp_start(&mut self, start: usize, len: usize) -> AResult<Token> {
        let kind = self.interp_parts.pop().unwrap();
        Ok(self.make_token(kind, start, len))
    }

    fn lex_interp_continue(&mut self) -> AResult<Token> {
        let start = self.pos;
        let mut s = String::new();
        loop {
            if self.at_end() {
                return Err(AError::lex("unterminated interpolated string", self.span_at(start, self.pos - start)));
            }
            let c = self.advance();
            match c {
                '"' => {
                    self.interp_depth -= 1;
                    let len = self.pos - start;
                    return Ok(self.make_token(TokenKind::InterpEnd(s), start, len));
                }
                '{' => {
                    let len = self.pos - start;
                    return Ok(self.make_token(TokenKind::InterpMid(s), start, len));
                }
                '\\' => {
                    if self.at_end() {
                        return Err(AError::lex("unterminated escape in interpolation", self.span_at(start, self.pos - start)));
                    }
                    let esc = self.advance();
                    match esc {
                        'n' => s.push('\n'),
                        't' => s.push('\t'),
                        'r' => s.push('\r'),
                        '\\' => s.push('\\'),
                        '"' => s.push('"'),
                        '{' => s.push('{'),
                        '}' => s.push('}'),
                        _ => return Err(AError::lex(
                            format!("unknown escape '\\{esc}'"),
                            self.span_at(self.pos - 2, 2),
                        )),
                    }
                }
                _ => s.push(c),
            }
        }
    }

    fn lex_raw_string(&mut self, start: usize, num_hashes: usize) -> AResult<Token> {
        let mut s = String::new();
        loop {
            if self.at_end() {
                return Err(AError::lex("unterminated raw string", self.span_at(start, self.pos - start)));
            }
            let c = self.advance();
            if c == '"' {
                let mut matched = true;
                for i in 0..num_hashes {
                    let peek_pos = self.pos + i;
                    if peek_pos >= self.src.len() || self.src[peek_pos] != '#' {
                        matched = false;
                        break;
                    }
                }
                if matched {
                    for _ in 0..num_hashes {
                        self.advance();
                    }
                    let len = self.pos - start;
                    return Ok(self.make_token(TokenKind::StringLit(s), start, len));
                }
                s.push('"');
                continue;
            }
            s.push(c);
        }
    }

    fn lex_heredoc(&mut self, start: usize) -> AResult<Token> {
        let mut s = String::new();
        if !self.at_end() && self.peek() == '\n' {
            self.advance();
        }
        loop {
            if self.at_end() {
                return Err(AError::lex("unterminated heredoc", self.span_at(start, self.pos - start)));
            }
            if self.peek() == '`' {
                let save = self.pos;
                self.advance();
                if !self.at_end() && self.peek() == '`' {
                    self.advance();
                    if !self.at_end() && self.peek() == '`' {
                        self.advance();
                        if s.ends_with('\n') {
                            s.pop();
                        }
                        let len = self.pos - start;
                        return Ok(self.make_token(TokenKind::StringLit(s), start, len));
                    }
                }
                for i in save..self.pos {
                    s.push(self.src[i]);
                }
                continue;
            }
            let c = self.advance();
            s.push(c);
        }
    }

    fn lex_number(&mut self, start: usize) -> AResult<Token> {
        while !self.at_end() && self.peek().is_ascii_digit() {
            self.advance();
        }
        if !self.at_end() && self.peek() == '.' && self.peek_next().is_ascii_digit() {
            self.advance();
            while !self.at_end() && self.peek().is_ascii_digit() {
                self.advance();
            }
            let text: String = self.src[start..self.pos].iter().collect();
            let val: f64 = text.parse().map_err(|_| {
                AError::lex("invalid float literal", self.span_at(start, self.pos - start))
            })?;
            Ok(self.make_token(TokenKind::FloatLit(val), start, self.pos - start))
        } else {
            let text: String = self.src[start..self.pos].iter().collect();
            let val: i64 = text.parse().map_err(|_| {
                AError::lex("invalid integer literal", self.span_at(start, self.pos - start))
            })?;
            Ok(self.make_token(TokenKind::IntLit(val), start, self.pos - start))
        }
    }

    fn lex_ident_or_keyword(&mut self, start: usize) -> AResult<Token> {
        while !self.at_end() && (self.peek().is_ascii_alphanumeric() || self.peek() == '_') {
            self.advance();
        }
        let text: String = self.src[start..self.pos].iter().collect();
        let len = self.pos - start;
        let kind = match text.as_str() {
            "fn" => TokenKind::Fn,
            "ty" => TokenKind::Ty,
            "mod" => TokenKind::Mod,
            "let" => TokenKind::Let,
            "mut" => TokenKind::Mut,
            "if" => TokenKind::If,
            "else" => TokenKind::Else,
            "match" => TokenKind::Match,
            "for" => TokenKind::For,
            "in" => TokenKind::In,
            "while" => TokenKind::While,
            "break" => TokenKind::Break,
            "continue" => TokenKind::Continue,
            "ret" => TokenKind::Ret,
            "use" => TokenKind::Use,
            "try" => TokenKind::Try,
            "pub" => TokenKind::Pub,
            "effects" => TokenKind::Effects,
            "pre" => TokenKind::Pre,
            "post" => TokenKind::Post,
            "where" => TokenKind::Where,
            "extern" => TokenKind::Extern,
            "true" => TokenKind::True,
            "false" => TokenKind::False,
            "i8" => TokenKind::I8,
            "i16" => TokenKind::I16,
            "i32" => TokenKind::I32,
            "i64" => TokenKind::I64,
            "u8" => TokenKind::U8,
            "u16" => TokenKind::U16,
            "u32" => TokenKind::U32,
            "u64" => TokenKind::U64,
            "f32" => TokenKind::F32,
            "f64" => TokenKind::F64,
            "bool" => TokenKind::Bool,
            "str" => TokenKind::Str,
            "bytes" => TokenKind::Bytes,
            "void" => TokenKind::Void,
            "ptr" => TokenKind::Ptr,
            "_" => TokenKind::Underscore,
            _ => TokenKind::Ident(text),
        };
        Ok(self.make_token(kind, start, len))
    }
}
