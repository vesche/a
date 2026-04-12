use crate::tokens::Span;
use serde::Serialize;
use std::fmt;

#[derive(Debug, Clone, Serialize)]
pub struct StackFrame {
    pub function: String,
    pub line: u32,
}

#[derive(Debug, Clone, Serialize)]
pub struct AError {
    pub kind: ErrorKind,
    pub message: String,
    pub span: Option<Span>,
    #[serde(skip_serializing_if = "Vec::is_empty")]
    pub stack: Vec<StackFrame>,
}

#[derive(Debug, Clone, Serialize)]
pub enum ErrorKind {
    LexError,
    ParseError,
    TypeError,
    EffectError,
    ContractError,
    RuntimeError,
}

impl fmt::Display for AError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if let Some(span) = &self.span {
            write!(f, "[{}:{}] {:?}: {}", span.line, span.col, self.kind, self.message)
        } else {
            write!(f, "{:?}: {}", self.kind, self.message)
        }
    }
}

impl std::error::Error for AError {}

impl AError {
    pub fn lex(msg: impl Into<String>, span: Span) -> Self {
        AError { kind: ErrorKind::LexError, message: msg.into(), span: Some(span), stack: Vec::new() }
    }

    pub fn parse(msg: impl Into<String>, span: Option<Span>) -> Self {
        AError { kind: ErrorKind::ParseError, message: msg.into(), span, stack: Vec::new() }
    }

    pub fn type_err(msg: impl Into<String>, span: Option<Span>) -> Self {
        AError { kind: ErrorKind::TypeError, message: msg.into(), span, stack: Vec::new() }
    }

    pub fn effect_err(msg: impl Into<String>, span: Option<Span>) -> Self {
        AError { kind: ErrorKind::EffectError, message: msg.into(), span, stack: Vec::new() }
    }

    pub fn runtime(msg: impl Into<String>, span: Option<Span>) -> Self {
        AError { kind: ErrorKind::RuntimeError, message: msg.into(), span, stack: Vec::new() }
    }

    pub fn with_stack(mut self, stack: Vec<StackFrame>) -> Self {
        self.stack = stack;
        self
    }

    pub fn to_json(&self) -> String {
        serde_json::to_string(self).unwrap_or_else(|_| format!("{{\"error\":\"{}\"}}", self.message))
    }
}

pub type AResult<T> = Result<T, AError>;
