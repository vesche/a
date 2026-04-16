use crate::tokens::Span;
use serde::Serialize;

#[derive(Debug, Clone, Serialize)]
pub struct Program {
    pub items: Vec<TopLevel>,
}

#[derive(Debug, Clone, Serialize)]
pub struct TopLevel {
    pub kind: TopLevelKind,
    pub span: Option<Span>,
}

#[derive(Debug, Clone, Serialize)]
pub enum TopLevelKind {
    FnDecl(FnDecl),
    TypeDecl(TypeDecl),
    ModDecl(ModDecl),
    UseDecl(UseDecl),
    ExternFn(ExternFnDecl),
}

#[derive(Debug, Clone, Serialize)]
pub struct ModDecl {
    pub name: String,
    pub items: Vec<TopLevel>,
}

#[derive(Debug, Clone, Serialize)]
pub struct UseDecl {
    pub path: Vec<String>,
}

#[derive(Debug, Clone, Serialize)]
pub struct TypeDecl {
    pub name: String,
    pub params: Vec<String>,
    pub body: TypeBody,
}

#[derive(Debug, Clone, Serialize)]
pub enum TypeBody {
    Record(Vec<Field>),
    Sum(Vec<Variant>),
    Alias(TypeExpr, Option<Expr>),
}

#[derive(Debug, Clone, Serialize)]
pub struct Field {
    pub name: String,
    pub ty: TypeExpr,
}

#[derive(Debug, Clone, Serialize)]
pub struct Variant {
    pub name: String,
    pub fields: Vec<TypeExpr>,
}

#[derive(Debug, Clone, Serialize)]
pub enum TypeExpr {
    Named(String, Vec<TypeExpr>),
    Array(Box<TypeExpr>),
    Tuple(Vec<TypeExpr>),
    Record(Vec<Field>),
    FnType(Vec<TypeExpr>, Box<TypeExpr>),
    Map(Box<TypeExpr>, Box<TypeExpr>),
    I8, I16, I32, I64,
    U8, U16, U32, U64,
    F32, F64,
    Bool, Str, Bytes, Void, Ptr,
    Infer,
}

#[derive(Debug, Clone, Serialize)]
pub struct ExternFnDecl {
    pub name: String,
    pub params: Vec<Param>,
    pub ret_type: TypeExpr,
}

#[derive(Debug, Clone, Serialize)]
pub struct FnDecl {
    pub name: String,
    pub params: Vec<Param>,
    pub ret_type: TypeExpr,
    pub effects: Vec<String>,
    pub pre: Option<Expr>,
    pub post: Option<Expr>,
    pub body: Block,
}

#[derive(Debug, Clone, Serialize)]
pub struct Param {
    pub name: String,
    pub ty: TypeExpr,
}

#[derive(Debug, Clone, Serialize)]
pub struct Block {
    pub stmts: Vec<Stmt>,
}

#[derive(Debug, Clone, Serialize)]
pub struct Stmt {
    pub kind: StmtKind,
    pub span: Option<Span>,
}

#[derive(Debug, Clone, Serialize)]
pub enum StmtKind {
    Let {
        mutable: bool,
        name: String,
        ty: TypeExpr,
        value: Expr,
    },
    Assign {
        target: Expr,
        value: Expr,
    },
    Return(Expr),
    Expr(Expr),
    If {
        cond: Expr,
        then_block: Block,
        else_block: Option<ElseBranch>,
    },
    Match {
        expr: Expr,
        arms: Vec<MatchArm>,
    },
    For {
        var: String,
        ty: TypeExpr,
        iter: Expr,
        body: Block,
    },
    While {
        cond: Expr,
        body: Block,
    },
    LetDestructure {
        bindings: Vec<Option<String>>,
        rest: Option<String>,
        value: Expr,
    },
    ForDestructure {
        bindings: Vec<String>,
        iter: Expr,
        body: Block,
    },
    Break,
    Continue,
}

#[derive(Debug, Clone, Serialize)]
pub enum ElseBranch {
    Block(Block),
    If(Box<Stmt>),
}

#[derive(Debug, Clone, Serialize)]
pub struct MatchArm {
    pub pattern: Pattern,
    pub guard: Option<Expr>,
    pub body: MatchBody,
}

#[derive(Debug, Clone, Serialize)]
pub enum MatchBody {
    Expr(Expr),
    Block(Block),
}

#[derive(Debug, Clone, Serialize)]
pub enum ArrayPatElem {
    Pat(Pattern),
    Rest(String),
}

#[derive(Debug, Clone, Serialize)]
pub enum Pattern {
    Ident(String),
    Constructor(String, Vec<Pattern>),
    Literal(Literal),
    Wildcard,
    Array(Vec<ArrayPatElem>),
    Map(Vec<(String, Pattern)>),
}

#[derive(Debug, Clone, Serialize)]
pub struct Expr {
    pub kind: ExprKind,
    pub span: Option<Span>,
}

#[derive(Debug, Clone, Serialize)]
pub enum ExprKind {
    Literal(Literal),
    Ident(String),
    BinOp {
        op: BinOp,
        left: Box<Expr>,
        right: Box<Expr>,
    },
    UnaryOp {
        op: UnaryOp,
        expr: Box<Expr>,
    },
    Call {
        func: Box<Expr>,
        args: Vec<Expr>,
    },
    FieldAccess {
        expr: Box<Expr>,
        field: String,
    },
    Index {
        expr: Box<Expr>,
        index: Box<Expr>,
    },
    Try(Box<Expr>),
    TryBlock(Block),
    If {
        cond: Box<Expr>,
        then_block: Block,
        else_block: Block,
    },
    Match {
        expr: Box<Expr>,
        arms: Vec<MatchArm>,
    },
    Block(Block),
    Array(Vec<Expr>),
    Record(Vec<(String, Expr)>),
    Lambda {
        params: Vec<Param>,
        body: Box<Expr>,
    },
    Pipe {
        left: Box<Expr>,
        right: Box<Expr>,
    },
    Interpolation {
        parts: Vec<StringPart>,
    },
    MapLiteral(Vec<(Expr, Expr)>),
    Spread(Box<Expr>),
}

#[derive(Debug, Clone, Serialize)]
pub enum StringPart {
    Lit(String),
    Expr(Expr),
}

#[derive(Debug, Clone, Serialize)]
pub enum Literal {
    Int(i64),
    Float(f64),
    String(String),
    Bool(bool),
}

#[derive(Debug, Clone, Copy, Serialize)]
pub enum BinOp {
    Add, Sub, Mul, Div, Mod,
    Eq, NotEq, Lt, Gt, LtEq, GtEq,
    And, Or,
}

#[derive(Debug, Clone, Copy, Serialize)]
pub enum UnaryOp {
    Neg,
    Not,
}
