use crate::interpreter::Value;

#[derive(Debug, Clone, Copy)]
pub enum Op {
    Const(usize),
    Pop,

    GetLocal(usize),
    SetLocal(usize),
    GetGlobal(usize),
    SetGlobal(usize),

    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Neg,

    Eq,
    NotEq,
    Lt,
    Gt,
    LtEq,
    GtEq,

    Not,
    JumpIfFalseKeep(usize),

    Jump(usize),
    JumpIfFalse(usize),
    Loop(usize),

    Call(usize, usize),
    CallMethod(usize, usize, usize),
    CallClosure(usize),
    TailCall(usize, usize),
    TailCallClosure(usize),
    Return,

    Array(usize),
    Index,
    IndexSet,
    Field(usize),
    MapNew(usize),
    Record(usize),

    Concat(usize),

    MakeOk,
    MakeErr,
    Unwrap,
    UnwrapOr,
    IsOk,
    IsErr,
    TryStart(usize),
    TryEnd,

    Dup,
    MatchVariant(usize, usize),
    MatchLiteral(usize, usize),
    BindMatchVar(usize),
    PopScope,

    MakeClosure(usize, usize),
}

#[derive(Debug, Clone)]
pub struct Chunk {
    pub code: Vec<Op>,
    pub lines: Vec<u32>,
    pub constants: Vec<Value>,
    pub strings: Vec<String>,
    current_line: u32,
}

impl Chunk {
    pub fn new() -> Self {
        Chunk {
            code: Vec::new(),
            lines: Vec::new(),
            constants: Vec::new(),
            strings: Vec::new(),
            current_line: 0,
        }
    }

    pub fn set_line(&mut self, line: u32) {
        self.current_line = line;
    }

    pub fn emit(&mut self, op: Op) -> usize {
        let idx = self.code.len();
        self.code.push(op);
        self.lines.push(self.current_line);
        idx
    }

    pub fn add_constant(&mut self, val: Value) -> usize {
        self.constants.push(val);
        self.constants.len() - 1
    }

    pub fn intern_string(&mut self, s: String) -> usize {
        if let Some(pos) = self.strings.iter().position(|e| e == &s) {
            return pos;
        }
        self.strings.push(s);
        self.strings.len() - 1
    }

    pub fn patch_jump(&mut self, idx: usize) {
        let target = self.code.len();
        match &mut self.code[idx] {
            Op::Jump(ref mut t)
            | Op::JumpIfFalse(ref mut t)
            | Op::JumpIfFalseKeep(ref mut t)
            | Op::MatchVariant(_, ref mut t)
            | Op::MatchLiteral(_, ref mut t)
            | Op::TryStart(ref mut t) => *t = target,
            _ => {}
        }
    }
}

#[derive(Debug, Clone)]
pub struct CompiledFn {
    pub name: String,
    pub params: Vec<String>,
    pub chunk: Chunk,
    pub locals: Vec<String>,
}

#[derive(Debug, Clone)]
pub struct CompiledProgram {
    pub functions: Vec<CompiledFn>,
    pub main_idx: Option<usize>,
}
