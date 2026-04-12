use serde::Serialize;
use std::fmt;

#[derive(Debug, Clone, PartialEq, Serialize)]
pub struct Span {
    pub line: usize,
    pub col: usize,
    pub offset: usize,
    pub len: usize,
}

#[derive(Debug, Clone, PartialEq, Serialize)]
pub struct Token {
    pub kind: TokenKind,
    pub span: Span,
}

#[derive(Debug, Clone, PartialEq, Serialize)]
pub enum TokenKind {
    // Keywords
    Fn,
    Ty,
    Mod,
    Let,
    Mut,
    If,
    Else,
    Match,
    For,
    In,
    While,
    Break,
    Continue,
    Ret,
    Use,
    Try,
    Pub,
    Effects,
    Pre,
    Post,
    Where,

    // Literals
    IntLit(i64),
    FloatLit(f64),
    StringLit(String),
    True,
    False,

    // Identifier
    Ident(String),

    // Type keywords
    I8, I16, I32, I64,
    U8, U16, U32, U64,
    F32, F64,
    Bool,
    Str,
    Bytes,
    Void,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    LAngle,
    RAngle,
    Comma,
    Colon,
    Arrow,     // ->
    FatArrow,  // =>
    Dot,
    Pipe,      // |
    PipeArrow, // |>
    Underscore,
    Hash,      // #

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    EqEq,
    NotEq,
    LessEq,
    GreaterEq,
    And,       // &&
    Or,        // ||
    Bang,
    Eq,        // =

    // Interpolated string parts
    InterpStart(String),  // text before first {
    InterpMid(String),    // text between } and next {
    InterpEnd(String),    // text after last }

    Question,   // ?
    DotDotDot,  // ...

    // Special
    Newline,
    Eof,
}

impl fmt::Display for TokenKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            TokenKind::Fn => write!(f, "fn"),
            TokenKind::Ty => write!(f, "ty"),
            TokenKind::Mod => write!(f, "mod"),
            TokenKind::Let => write!(f, "let"),
            TokenKind::Mut => write!(f, "mut"),
            TokenKind::If => write!(f, "if"),
            TokenKind::Else => write!(f, "else"),
            TokenKind::Match => write!(f, "match"),
            TokenKind::For => write!(f, "for"),
            TokenKind::In => write!(f, "in"),
            TokenKind::While => write!(f, "while"),
            TokenKind::Break => write!(f, "break"),
            TokenKind::Continue => write!(f, "continue"),
            TokenKind::Ret => write!(f, "ret"),
            TokenKind::Use => write!(f, "use"),
            TokenKind::Try => write!(f, "try"),
            TokenKind::Pub => write!(f, "pub"),
            TokenKind::Effects => write!(f, "effects"),
            TokenKind::Pre => write!(f, "pre"),
            TokenKind::Post => write!(f, "post"),
            TokenKind::Where => write!(f, "where"),
            TokenKind::IntLit(v) => write!(f, "{v}"),
            TokenKind::FloatLit(v) => write!(f, "{v}"),
            TokenKind::StringLit(v) => write!(f, "\"{v}\""),
            TokenKind::True => write!(f, "true"),
            TokenKind::False => write!(f, "false"),
            TokenKind::Ident(s) => write!(f, "{s}"),
            TokenKind::I8 => write!(f, "i8"),
            TokenKind::I16 => write!(f, "i16"),
            TokenKind::I32 => write!(f, "i32"),
            TokenKind::I64 => write!(f, "i64"),
            TokenKind::U8 => write!(f, "u8"),
            TokenKind::U16 => write!(f, "u16"),
            TokenKind::U32 => write!(f, "u32"),
            TokenKind::U64 => write!(f, "u64"),
            TokenKind::F32 => write!(f, "f32"),
            TokenKind::F64 => write!(f, "f64"),
            TokenKind::Bool => write!(f, "bool"),
            TokenKind::Str => write!(f, "str"),
            TokenKind::Bytes => write!(f, "bytes"),
            TokenKind::Void => write!(f, "void"),
            TokenKind::LParen => write!(f, "("),
            TokenKind::RParen => write!(f, ")"),
            TokenKind::LBrace => write!(f, "{{"),
            TokenKind::RBrace => write!(f, "}}"),
            TokenKind::LBracket => write!(f, "["),
            TokenKind::RBracket => write!(f, "]"),
            TokenKind::LAngle => write!(f, "<"),
            TokenKind::RAngle => write!(f, ">"),
            TokenKind::Comma => write!(f, ","),
            TokenKind::Colon => write!(f, ":"),
            TokenKind::Arrow => write!(f, "->"),
            TokenKind::FatArrow => write!(f, "=>"),
            TokenKind::Dot => write!(f, "."),
            TokenKind::Pipe => write!(f, "|"),
            TokenKind::PipeArrow => write!(f, "|>"),
            TokenKind::Underscore => write!(f, "_"),
            TokenKind::Hash => write!(f, "#"),
            TokenKind::Plus => write!(f, "+"),
            TokenKind::Minus => write!(f, "-"),
            TokenKind::Star => write!(f, "*"),
            TokenKind::Slash => write!(f, "/"),
            TokenKind::Percent => write!(f, "%"),
            TokenKind::EqEq => write!(f, "=="),
            TokenKind::NotEq => write!(f, "!="),
            TokenKind::LessEq => write!(f, "<="),
            TokenKind::GreaterEq => write!(f, ">="),
            TokenKind::And => write!(f, "&&"),
            TokenKind::Or => write!(f, "||"),
            TokenKind::Bang => write!(f, "!"),
            TokenKind::Eq => write!(f, "="),
            TokenKind::Question => write!(f, "?"),
            TokenKind::DotDotDot => write!(f, "..."),
            TokenKind::InterpStart(s) => write!(f, "\"{s}{{"),
            TokenKind::InterpMid(s) => write!(f, "}}{s}{{"),
            TokenKind::InterpEnd(s) => write!(f, "}}{s}\""),
            TokenKind::Newline => write!(f, "\\n"),
            TokenKind::Eof => write!(f, "EOF"),
        }
    }
}
