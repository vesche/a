use std::collections::HashMap;
use std::sync::Arc;

use crate::bytecode::*;
use crate::builtins;
use crate::compiler::Compiler;
use crate::errors::{AError, AResult, StackFrame};
use crate::interpreter::Value;
use crate::lexer::Lexer;
use crate::parser::Parser as AParser;

fn val_less(a: &Value, b: &Value) -> bool {
    match (a, b) {
        (Value::Int(x), Value::Int(y)) => x < y,
        (Value::Float(x), Value::Float(y)) => x < y,
        (Value::Int(x), Value::Float(y)) => (*x as f64) < *y,
        (Value::Float(x), Value::Int(y)) => *x < (*y as f64),
        (Value::String(x), Value::String(y)) => x.as_str() < y.as_str(),
        _ => false,
    }
}

fn cmp_values(a: Value, b: Value, test: impl Fn(std::cmp::Ordering) -> bool) -> Value {
    let ord = match (&a, &b) {
        (Value::Int(x), Value::Int(y)) => x.cmp(y),
        (Value::Float(x), Value::Float(y)) => x.partial_cmp(y).unwrap_or(std::cmp::Ordering::Equal),
        (Value::Int(x), Value::Float(y)) => (*x as f64).partial_cmp(y).unwrap_or(std::cmp::Ordering::Equal),
        (Value::Float(x), Value::Int(y)) => x.partial_cmp(&(*y as f64)).unwrap_or(std::cmp::Ordering::Equal),
        (Value::String(x), Value::String(y)) => x.as_str().cmp(y.as_str()),
        _ => return Value::Bool(false),
    };
    Value::Bool(test(ord))
}

struct CallFrame {
    fn_idx: usize,
    ip: usize,
    stack_base: usize,
}

struct TryFrame {
    catch_ip: usize,
    fn_idx: usize,
}

pub struct VM {
    stack: Vec<Value>,
    frames: Vec<CallFrame>,
    try_stack: Vec<TryFrame>,
    program: Arc<CompiledProgram>,
    fn_index: HashMap<String, usize>,
    program_args: Vec<String>,
    test_mode: bool,
    task_handles: HashMap<usize, std::thread::JoinHandle<Result<Value, String>>>,
    next_task_id: usize,
}

impl VM {
    pub fn new(program: CompiledProgram) -> Self {
        Self::new_shared(Arc::new(program))
    }

    pub fn new_shared(program: Arc<CompiledProgram>) -> Self {
        let mut fn_index = HashMap::with_capacity(program.functions.len());
        for (i, f) in program.functions.iter().enumerate() {
            fn_index.insert(f.name.clone(), i);
        }
        VM {
            stack: Vec::with_capacity(256),
            frames: Vec::new(),
            try_stack: Vec::new(),
            program,
            fn_index,
            program_args: Vec::new(),
            test_mode: false,
            task_handles: HashMap::new(),
            next_task_id: 0,
        }
    }

    pub fn set_args(&mut self, args: Vec<String>) {
        self.program_args = args;
    }

    pub fn set_test_mode(&mut self, enabled: bool) {
        self.test_mode = enabled;
    }

    pub fn run_function(&mut self, name: &str) -> AResult<Value> {
        let fn_idx = self.find_function(name).ok_or_else(|| {
            AError::runtime(format!("function '{}' not found", name), None)
        })?;

        self.stack.clear();
        self.frames.clear();
        self.try_stack.clear();

        self.frames.push(CallFrame {
            fn_idx,
            ip: 0,
            stack_base: 0,
        });

        let num_locals = self.program.functions[fn_idx].locals.len();
        for _ in 0..num_locals {
            self.stack.push(Value::Void);
        }

        self.execute()
    }

    pub fn run(&mut self) -> AResult<Value> {
        let main_idx = self.program.main_idx.ok_or_else(|| {
            AError::runtime("no main function", None)
        })?;

        self.frames.push(CallFrame {
            fn_idx: main_idx,
            ip: 0,
            stack_base: 0,
        });

        let num_locals = self.program.functions[main_idx].locals.len();
        for _ in 0..num_locals {
            self.stack.push(Value::Void);
        }

        self.execute()
    }

    fn execute(&mut self) -> AResult<Value> {
        self.execute_loop(0).map_err(|mut e| {
            if e.stack.is_empty() {
                let trace = self.capture_stack_trace();
                if e.span.is_none() {
                    if let Some(first) = trace.first() {
                        e.span = Some(crate::tokens::Span {
                            line: first.line as usize,
                            col: 0,
                            offset: 0,
                            len: 0,
                        });
                    }
                }
                e.with_stack(trace)
            } else {
                e
            }
        })
    }

    fn capture_stack_trace(&self) -> Vec<StackFrame> {
        let mut trace = Vec::new();
        for frame in self.frames.iter().rev() {
            let func = &self.program.functions[frame.fn_idx];
            let line = if frame.ip > 0 && frame.ip <= func.chunk.lines.len() {
                func.chunk.lines[frame.ip - 1]
            } else if !func.chunk.lines.is_empty() {
                func.chunk.lines[0]
            } else {
                0
            };
            trace.push(StackFrame { function: func.name.clone(), line });
        }
        trace
    }

    fn execute_loop(&mut self, min_depth: usize) -> AResult<Value> {
        loop {
            if self.frames.len() <= min_depth {
                return Ok(self.stack.pop().unwrap_or(Value::Void));
            }

            let frame = self.frames.last().unwrap();
            let fn_idx = frame.fn_idx;
            let ip = frame.ip;
            let stack_base = frame.stack_base;

            let chunk = &self.program.functions[fn_idx].chunk;
            if ip >= chunk.code.len() {
                self.frames.pop();
                continue;
            }

            let op = chunk.code[ip];
            self.frames.last_mut().unwrap().ip += 1;

            match op {
                Op::Const(idx) => {
                    let val = self.program.functions[fn_idx].chunk.constants[idx].clone();
                    self.stack.push(val);
                }
                Op::Pop => { self.stack.pop(); }
                Op::Dup => {
                    let val = self.stack.last().cloned().unwrap_or(Value::Void);
                    self.stack.push(val);
                }

                Op::GetLocal(idx) => {
                    let slot = stack_base + idx;
                    let val = if slot < self.stack.len() {
                        self.stack[slot].clone()
                    } else {
                        Value::Void
                    };
                    self.stack.push(val);
                }
                Op::SetLocal(idx) => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let slot = stack_base + idx;
                    while self.stack.len() <= slot {
                        self.stack.push(Value::Void);
                    }
                    self.stack[slot] = val;
                }
                Op::GetGlobal(_) => {
                    self.stack.push(Value::Void);
                }
                Op::SetGlobal(_) => {
                    self.stack.pop();
                }

                Op::Add => { self.binary_op(|a, b| match (a, b) {
                    (Value::Int(x), Value::Int(y)) => Value::Int(x + y),
                    (Value::Float(x), Value::Float(y)) => Value::Float(x + y),
                    (Value::Int(x), Value::Float(y)) => Value::Float(x as f64 + y),
                    (Value::Float(x), Value::Int(y)) => Value::Float(x + y as f64),
                    (Value::String(x), Value::String(y)) => Value::string(format!("{x}{y}")),
                    _ => Value::Void,
                }); }
                Op::Sub => { self.binary_op(|a, b| match (a, b) {
                    (Value::Int(x), Value::Int(y)) => Value::Int(x - y),
                    (Value::Float(x), Value::Float(y)) => Value::Float(x - y),
                    (Value::Int(x), Value::Float(y)) => Value::Float(x as f64 - y),
                    (Value::Float(x), Value::Int(y)) => Value::Float(x - y as f64),
                    _ => Value::Void,
                }); }
                Op::Mul => { self.binary_op(|a, b| match (a, b) {
                    (Value::Int(x), Value::Int(y)) => Value::Int(x * y),
                    (Value::Float(x), Value::Float(y)) => Value::Float(x * y),
                    (Value::Int(x), Value::Float(y)) => Value::Float(x as f64 * y),
                    (Value::Float(x), Value::Int(y)) => Value::Float(x * y as f64),
                    _ => Value::Void,
                }); }
                Op::Div => { self.binary_op(|a, b| match (a, b) {
                    (Value::Int(x), Value::Int(y)) => if y == 0 { Value::Void } else { Value::Int(x / y) },
                    (Value::Float(x), Value::Float(y)) => Value::Float(x / y),
                    (Value::Int(x), Value::Float(y)) => Value::Float(x as f64 / y),
                    (Value::Float(x), Value::Int(y)) => Value::Float(x / y as f64),
                    _ => Value::Void,
                }); }
                Op::Mod => { self.binary_op(|a, b| match (a, b) {
                    (Value::Int(x), Value::Int(y)) => if y == 0 { Value::Void } else { Value::Int(x % y) },
                    _ => Value::Void,
                }); }
                Op::Neg => {
                    let v = self.stack.pop().unwrap_or(Value::Void);
                    self.stack.push(match v {
                        Value::Int(i) => Value::Int(-i),
                        Value::Float(f) => Value::Float(-f),
                        _ => Value::Void,
                    });
                }

                Op::Eq => { self.binary_op(|a, b| Value::Bool(a == b)); }
                Op::NotEq => { self.binary_op(|a, b| Value::Bool(a != b)); }
                Op::Lt => { self.binary_op(|a, b| cmp_values(a, b, |o| o.is_lt())); }
                Op::Gt => { self.binary_op(|a, b| cmp_values(a, b, |o| o.is_gt())); }
                Op::LtEq => { self.binary_op(|a, b| cmp_values(a, b, |o| o.is_le())); }
                Op::GtEq => { self.binary_op(|a, b| cmp_values(a, b, |o| o.is_ge())); }

                Op::Not => {
                    let v = self.stack.pop().unwrap_or(Value::Void);
                    self.stack.push(Value::Bool(!self.is_truthy(&v)));
                }
                Op::JumpIfFalseKeep(target) => {
                    let val = self.stack.last().cloned().unwrap_or(Value::Void);
                    if !self.is_truthy(&val) {
                        self.frames.last_mut().unwrap().ip = target;
                    }
                }

                Op::Jump(target) => {
                    self.frames.last_mut().unwrap().ip = target;
                }
                Op::JumpIfFalse(target) => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    if !self.is_truthy(&val) {
                        self.frames.last_mut().unwrap().ip = target;
                    }
                }
                Op::Loop(target) => {
                    self.frames.last_mut().unwrap().ip = target;
                }

                Op::Call(name_idx, nargs) => {
                    let name = self.program.functions[fn_idx].chunk.strings[name_idx].clone();
                    self.do_call(&name, nargs)?;
                }
                Op::CallMethod(mod_idx, func_idx, nargs) => {
                    let chunk = &self.program.functions[fn_idx].chunk;
                    let full = format!("{}.{}", chunk.strings[mod_idx], chunk.strings[func_idx]);
                    self.do_call(&full, nargs)?;
                }
                Op::CallClosure(nargs) => {
                    let closure_val = self.stack.pop().unwrap_or(Value::Void);
                    match closure_val {
                        Value::String(fn_name) => {
                            self.do_call(fn_name.as_str(), nargs)?;
                        }
                        Value::VMClosure { name, captures } => {
                            let n_caps = captures.len();
                            let arg_start = self.stack.len() - nargs;
                            let args: Vec<Value> = self.stack.drain(arg_start..).collect();
                            for c in captures.iter() {
                                self.stack.push(c.clone());
                            }
                            for a in args {
                                self.stack.push(a);
                            }
                            self.do_call(name.as_str(), nargs + n_caps)?;
                        }
                        _ => {
                            let start = self.stack.len() - nargs;
                            self.stack.drain(start..);
                            self.stack.push(Value::Void);
                        }
                    }
                }
                Op::TailCall(name_idx, nargs) => {
                    let name = self.program.functions[fn_idx].chunk.strings[name_idx].clone();
                    if let Some(fi) = self.find_function(&name) {
                        let arg_start = self.stack.len() - nargs;
                        for i in 0..nargs {
                            self.stack[stack_base + i] = self.stack[arg_start + i].clone();
                        }
                        let num_locals = self.program.functions[fi].locals.len();
                        self.stack.truncate(stack_base + nargs);
                        while self.stack.len() < stack_base + num_locals {
                            self.stack.push(Value::Void);
                        }
                        let frame = self.frames.last_mut().unwrap();
                        frame.fn_idx = fi;
                        frame.ip = 0;
                        continue;
                    }
                    self.do_call(&name, nargs)?;
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let frame = self.frames.pop().unwrap();
                    self.stack.truncate(frame.stack_base);
                    self.stack.push(val);
                }
                Op::TailCallClosure(nargs) => {
                    let closure_val = self.stack.pop().unwrap_or(Value::Void);
                    let (resolved_name, total_nargs) = match closure_val {
                        Value::String(fn_name) => {
                            (fn_name, nargs)
                        }
                        Value::VMClosure { name, captures } => {
                            let n_caps = captures.len();
                            let arg_start = self.stack.len() - nargs;
                            let args: Vec<Value> = self.stack.drain(arg_start..).collect();
                            for c in captures.iter() {
                                self.stack.push(c.clone());
                            }
                            for a in args {
                                self.stack.push(a);
                            }
                            (name, nargs + n_caps)
                        }
                        _ => {
                            let start = self.stack.len() - nargs;
                            self.stack.drain(start..);
                            self.stack.push(Value::Void);
                            let val = self.stack.pop().unwrap_or(Value::Void);
                            let frame = self.frames.pop().unwrap();
                            self.stack.truncate(frame.stack_base);
                            self.stack.push(val);
                            continue;
                        }
                    };
                    if let Some(fi) = self.find_function(resolved_name.as_str()) {
                        let arg_start = self.stack.len() - total_nargs;
                        for i in 0..total_nargs {
                            self.stack[stack_base + i] = self.stack[arg_start + i].clone();
                        }
                        let num_locals = self.program.functions[fi].locals.len();
                        self.stack.truncate(stack_base + total_nargs);
                        while self.stack.len() < stack_base + num_locals {
                            self.stack.push(Value::Void);
                        }
                        let frame = self.frames.last_mut().unwrap();
                        frame.fn_idx = fi;
                        frame.ip = 0;
                        continue;
                    }
                    self.do_call(resolved_name.as_str(), total_nargs)?;
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let frame = self.frames.pop().unwrap();
                    self.stack.truncate(frame.stack_base);
                    self.stack.push(val);
                }
                Op::Return => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let frame = self.frames.pop().unwrap();
                    self.stack.truncate(frame.stack_base);
                    self.stack.push(val);
                }

                Op::Array(n) => {
                    let start = self.stack.len() - n;
                    let elems: Vec<Value> = self.stack.drain(start..).collect();
                    self.stack.push(Value::array(elems));
                }
                Op::Index => {
                    let idx = self.stack.pop().unwrap_or(Value::Int(0));
                    let coll = self.stack.pop().unwrap_or(Value::Void);
                    match (&coll, &idx) {
                        (Value::Array(a), Value::Int(i)) => {
                            self.stack.push(a.get(*i as usize).cloned().unwrap_or(Value::Void));
                        }
                        (Value::String(s), Value::Int(i)) => {
                            let ch = s.chars().nth(*i as usize)
                                .map(|c| Value::string(c.to_string()))
                                .unwrap_or(Value::Void);
                            self.stack.push(ch);
                        }
                        (Value::Map(m), Value::String(k)) => {
                            self.stack.push(m.get(k.as_str()).cloned().unwrap_or(Value::Void));
                        }
                        (Value::Map(m), Value::Int(i)) => {
                            let entry = m.iter().nth(*i as usize);
                            match entry {
                                Some((k, v)) => self.stack.push(Value::array(vec![Value::string(k.clone()), v.clone()])),
                                None => self.stack.push(Value::Void),
                            }
                        }
                        _ => self.stack.push(Value::Void),
                    }
                }
                Op::IndexSet => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let idx = self.stack.pop().unwrap_or(Value::Int(0));
                    let coll = self.stack.pop().unwrap_or(Value::Void);
                    match (coll, idx) {
                        (Value::Array(a), Value::Int(i)) => {
                            let i = i as usize;
                            let mut v = Arc::try_unwrap(a).unwrap_or_else(|rc| (*rc).clone());
                            while v.len() <= i { v.push(Value::Void); }
                            v[i] = val;
                            self.stack.push(Value::array(v));
                        }
                        (Value::Map(m), Value::String(k)) => {
                            let mut new_m = Arc::try_unwrap(m).unwrap_or_else(|rc| (*rc).clone());
                            new_m.insert(k.as_ref().clone(), val);
                            self.stack.push(Value::map(new_m));
                        }
                        _ => self.stack.push(Value::Void),
                    }
                }
                Op::Field(name_idx) => {
                    let name = &self.program.functions[fn_idx].chunk.strings[name_idx];
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    match val {
                        Value::Record(fields) => {
                            let found = fields.iter()
                                .find(|(n, _)| n == name)
                                .map(|(_, v)| v.clone())
                                .unwrap_or(Value::Void);
                            self.stack.push(found);
                        }
                        Value::Map(m) => {
                            self.stack.push(m.get(name.as_str()).cloned().unwrap_or(Value::Void));
                        }
                        _ => self.stack.push(Value::Void),
                    }
                }
                Op::Record(n) => {
                    let start = self.stack.len() - n * 2;
                    let pairs: Vec<Value> = self.stack.drain(start..).collect();
                    let mut fields = Vec::new();
                    for chunk_pair in pairs.chunks(2) {
                        if let [Value::String(k), v] = chunk_pair {
                            fields.push((k.as_ref().clone(), v.clone()));
                        }
                    }
                    self.stack.push(Value::Record(fields));
                }
                Op::MapNew(n) => {
                    let start = self.stack.len() - n * 2;
                    let pairs: Vec<Value> = self.stack.drain(start..).collect();
                    let mut map = HashMap::new();
                    for chunk_pair in pairs.chunks(2) {
                        if chunk_pair.len() == 2 {
                            let key = match &chunk_pair[0] {
                                Value::String(s) => s.as_ref().clone(),
                                Value::Int(i) => i.to_string(),
                                Value::Bool(b) => b.to_string(),
                                other => self.val_display(other),
                            };
                            map.insert(key, chunk_pair[1].clone());
                        }
                    }
                    self.stack.push(Value::map(map));
                }
                Op::Concat(n) => {
                    let start = self.stack.len() - n;
                    let parts: Vec<Value> = self.stack.drain(start..).collect();
                    let mut result = String::new();
                    for p in parts {
                        result.push_str(&self.val_display(&p));
                    }
                    self.stack.push(Value::string(result));
                }

                Op::MakeOk => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    self.stack.push(Value::Result(Ok(Box::new(val))));
                }
                Op::MakeErr => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    self.stack.push(Value::Result(Err(Box::new(val))));
                }
                Op::IsOk => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let b = matches!(val, Value::Result(Ok(_)));
                    self.stack.push(Value::Bool(b));
                }
                Op::IsErr => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let b = matches!(val, Value::Result(Err(_)));
                    self.stack.push(Value::Bool(b));
                }
                Op::Unwrap => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    match val {
                        Value::Result(Ok(v)) => self.stack.push(*v),
                        Value::Result(Err(e)) => {
                            if let Some(tf) = self.try_stack.pop() {
                                self.stack.push(*e);
                                while self.frames.len() > 0 && self.frames.last().unwrap().fn_idx != tf.fn_idx {
                                    self.frames.pop();
                                }
                                if let Some(f) = self.frames.last_mut() {
                                    f.ip = tf.catch_ip;
                                }
                            } else {
                                self.stack.push(*e);
                            }
                        }
                        other => self.stack.push(other),
                    }
                }
                Op::UnwrapOr => {
                    let default = self.stack.pop().unwrap_or(Value::Void);
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    match val {
                        Value::Result(Ok(v)) => self.stack.push(*v),
                        _ => self.stack.push(default),
                    }
                }
                Op::TryStart(catch_ip) => {
                    let fn_idx = self.frames.last().unwrap().fn_idx;
                    self.try_stack.push(TryFrame {
                        catch_ip,
                        fn_idx,
                    });
                }
                Op::TryEnd => {
                    self.try_stack.pop();
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    self.stack.push(val);
                }

                Op::MatchVariant(tag_idx, skip_target) => {
                    let tag = &self.program.functions[fn_idx].chunk.strings[tag_idx];
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    match &val {
                        Value::Variant(name, _) if name == tag => {
                            self.stack.push(val);
                        }
                        Value::Result(Ok(inner)) if tag == "Ok" => {
                            self.stack.push(Value::Variant("Ok".into(), vec![*inner.clone()]));
                        }
                        Value::Result(Err(inner)) if tag == "Err" => {
                            self.stack.push(Value::Variant("Err".into(), vec![*inner.clone()]));
                        }
                        _ => {
                            self.stack.push(val);
                            self.frames.last_mut().unwrap().ip = skip_target;
                        }
                    }
                }
                Op::MatchLiteral(const_idx, skip_target) => {
                    let val = self.stack.pop().unwrap_or(Value::Void);
                    let fn_idx = self.frames.last().unwrap().fn_idx;
                    let expected = self.program.functions[fn_idx].chunk.constants[const_idx].clone();
                    if val == expected {
                        self.stack.push(val);
                    } else {
                        self.stack.push(val);
                        self.frames.last_mut().unwrap().ip = skip_target;
                    }
                }
                Op::BindMatchVar(_) => {}
                Op::PopScope => {}

                Op::MakeClosure(name_idx, n_captures) => {
                    let fn_idx = self.frames.last().unwrap().fn_idx;
                    let name_val = self.program.functions[fn_idx].chunk.constants[name_idx].clone();
                    if n_captures == 0 {
                        self.stack.push(name_val);
                    } else {
                        let start = self.stack.len() - n_captures;
                        let captures: Vec<Value> = self.stack.drain(start..).collect();
                        if let Value::String(name) = name_val {
                            self.stack.push(Value::VMClosure {
                                name,
                                captures: Arc::new(captures),
                            });
                        } else {
                            self.stack.push(Value::Void);
                        }
                    }
                }
            }
        }
    }

    fn try_compile(code: &str) -> Result<CompiledProgram, String> {
        let mut lexer = Lexer::new(code);
        let tokens = lexer.tokenize().map_err(|e| e.message.clone())?;
        let mut parser = AParser::new(tokens);
        let program = parser.parse_program().map_err(|e| e.message.clone())?;
        let mut compiler = Compiler::new();
        Ok(compiler.compile_program(&program))
    }

    fn eval_compile(code: &str) -> Result<CompiledProgram, String> {
        let first = Self::try_compile(code);
        if let Ok(ref compiled) = first {
            if compiled.main_idx.is_some() {
                return first;
            }
        }
        let wrapped = format!("fn main() {{ ret {code} }}");
        if let Ok(compiled) = Self::try_compile(&wrapped) {
            if compiled.main_idx.is_some() {
                return Ok(compiled);
            }
        }
        first
    }

    fn do_call(&mut self, name: &str, nargs: usize) -> AResult<()> {
        if name == "args" {
            self.stack.push(Value::array(
                self.program_args.iter().map(|s| Value::string(s.clone())).collect(),
            ));
            return Ok(());
        }
        if name == "exit" {
            let code = if nargs > 0 {
                match self.stack.pop() {
                    Some(Value::Int(n)) => n as i32,
                    _ => 0,
                }
            } else { 0 };
            if self.test_mode {
                return Err(AError::runtime(format!("exit({})", code), None));
            }
            std::process::exit(code);
        }
        if name == "fail" {
            let msg = if nargs > 0 {
                match self.stack.pop() {
                    Some(Value::String(s)) => (*s).clone(),
                    Some(v) => format!("{:?}", v),
                    None => "assertion failed".to_string(),
                }
            } else {
                "assertion failed".to_string()
            };
            return Err(AError::runtime(msg, None));
        }
        if name == "is_ok" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            self.stack.push(Value::Bool(matches!(val, Value::Result(Ok(_)))));
            return Ok(());
        }
        if name == "is_err" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            self.stack.push(Value::Bool(matches!(val, Value::Result(Err(_)))));
            return Ok(());
        }
        if name == "unwrap" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            match val {
                Value::Result(Ok(v)) => { self.stack.push(*v); return Ok(()); }
                Value::Result(Err(e)) => {
                    return Err(AError::runtime(format!("unwrap on Err: {}", self.val_display(&e)), None));
                }
                other => { self.stack.push(other); return Ok(()); }
            }
        }
        if name == "unwrap_or" && nargs == 2 {
            let default = self.stack.pop().unwrap_or(Value::Void);
            let val = self.stack.pop().unwrap_or(Value::Void);
            match val {
                Value::Result(Ok(v)) => { self.stack.push(*v); }
                _ => { self.stack.push(default); }
            }
            return Ok(());
        }
        if name == "expect" && nargs == 2 {
            let _msg = self.stack.pop();
            let val = self.stack.pop().unwrap_or(Value::Void);
            match val {
                Value::Result(Ok(v)) => { self.stack.push(*v); return Ok(()); }
                Value::Result(Err(e)) => {
                    return Err(AError::runtime(format!("expect failed: {}", self.val_display(&e)), None));
                }
                other => { self.stack.push(other); return Ok(()); }
            }
        }
        if name == "Ok" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            self.stack.push(Value::Result(Ok(Box::new(val))));
            return Ok(());
        }
        if name == "Err" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            self.stack.push(Value::Result(Err(Box::new(val))));
            return Ok(());
        }
        if name == "__variant_field__" && nargs == 2 {
            let idx_val = self.stack.pop().unwrap_or(Value::Int(0));
            let variant = self.stack.pop().unwrap_or(Value::Void);
            if let (Value::Variant(_, fields), Value::Int(i)) = (variant, idx_val) {
                self.stack.push(fields.get(i as usize).cloned().unwrap_or(Value::Void));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }

        if name == "push" && nargs == 2 {
            let elem = self.stack.pop().unwrap_or(Value::Void);
            let arr = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = arr {
                let mut v = Arc::try_unwrap(a).unwrap_or_else(|rc| (*rc).clone());
                v.push(elem);
                self.stack.push(Value::array(v));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "len" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            let result = match &val {
                Value::String(s) => Value::Int(s.len() as i64),
                Value::Array(a) => Value::Int(a.len() as i64),
                Value::Map(m) => Value::Int(m.len() as i64),
                _ => Value::Int(0),
            };
            self.stack.push(result);
            return Ok(());
        }
        if name == "contains" && nargs == 2 {
            let needle = self.stack.pop().unwrap_or(Value::Void);
            let haystack = self.stack.pop().unwrap_or(Value::Void);
            let found = match &haystack {
                Value::Array(a) => a.contains(&needle),
                _ => false,
            };
            self.stack.push(Value::Bool(found));
            return Ok(());
        }
        if name == "sort" && nargs == 1 {
            let arr = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = arr {
                let mut v = Arc::try_unwrap(a).unwrap_or_else(|rc| (*rc).clone());
                v.sort_by(|x, y| match (x, y) {
                    (Value::Int(a), Value::Int(b)) => a.cmp(b),
                    (Value::String(a), Value::String(b)) => a.as_str().cmp(b.as_str()),
                    _ => std::cmp::Ordering::Equal,
                });
                self.stack.push(Value::array(v));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "concat_arr" && nargs == 2 {
            let b = self.stack.pop().unwrap_or(Value::Void);
            let a = self.stack.pop().unwrap_or(Value::Void);
            match (a, b) {
                (Value::Array(a_arr), Value::Array(b_arr)) => {
                    let mut v = Arc::try_unwrap(a_arr).unwrap_or_else(|rc| (*rc).clone());
                    v.extend(b_arr.iter().cloned());
                    self.stack.push(Value::array(v));
                }
                _ => { self.stack.push(Value::Void); }
            }
            return Ok(());
        }
        if name == "reverse_arr" && nargs == 1 {
            let arr = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = arr {
                let mut v = Arc::try_unwrap(a).unwrap_or_else(|rc| (*rc).clone());
                v.reverse();
                self.stack.push(Value::array(v));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "slice" && nargs == 3 {
            let end = match self.stack.pop() { Some(Value::Int(n)) => n as usize, _ => 0 };
            let start = match self.stack.pop() { Some(Value::Int(n)) => n.max(0) as usize, _ => 0 };
            let val = self.stack.pop().unwrap_or(Value::Void);
            match val {
                Value::Array(a) => {
                    let end = end.min(a.len());
                    let start = start.min(end);
                    self.stack.push(Value::array(a[start..end].to_vec()));
                }
                Value::String(s) => {
                    let chars: Vec<char> = s.chars().collect();
                    let end = end.min(chars.len());
                    let start = start.min(end);
                    self.stack.push(Value::string(chars[start..end].iter().collect()));
                }
                _ => self.stack.push(Value::Void),
            }
            return Ok(());
        }
        if name == "zip" && nargs == 2 {
            let b = self.stack.pop().unwrap_or(Value::Void);
            let a = self.stack.pop().unwrap_or(Value::Void);
            if let (Value::Array(aa), Value::Array(bb)) = (&a, &b) {
                let len = aa.len().min(bb.len());
                let result: Vec<Value> = (0..len)
                    .map(|i| Value::array(vec![aa[i].clone(), bb[i].clone()]))
                    .collect();
                self.stack.push(Value::array(result));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "enumerate" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = val {
                let result: Vec<Value> = a.iter().enumerate()
                    .map(|(i, v)| Value::array(vec![Value::Int(i as i64), v.clone()]))
                    .collect();
                self.stack.push(Value::array(result));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "take" && nargs == 2 {
            let n = match self.stack.pop() { Some(Value::Int(n)) => n.max(0) as usize, _ => 0 };
            let val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = val {
                let n = n.min(a.len());
                self.stack.push(Value::array(a[..n].to_vec()));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "drop" && nargs == 2 {
            let n = match self.stack.pop() { Some(Value::Int(n)) => n.max(0) as usize, _ => 0 };
            let val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = val {
                let n = n.min(a.len());
                self.stack.push(Value::array(a[n..].to_vec()));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "chunk" && nargs == 2 {
            let size = match self.stack.pop() { Some(Value::Int(n)) => n.max(1) as usize, _ => 1 };
            let val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = val {
                let result: Vec<Value> = a.chunks(size)
                    .map(|c| Value::array(c.to_vec()))
                    .collect();
                self.stack.push(Value::array(result));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "unique" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(a) = val {
                let mut seen = Vec::new();
                let mut result = Vec::new();
                for elem in a.iter() {
                    if !seen.iter().any(|s| s == elem) {
                        seen.push(elem.clone());
                        result.push(elem.clone());
                    }
                }
                self.stack.push(Value::array(result));
            } else {
                self.stack.push(Value::Void);
            }
            return Ok(());
        }
        if name == "to_str" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            self.stack.push(Value::string(self.val_display(&val)));
            return Ok(());
        }
        if name == "type_of" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            let t = match &val {
                Value::Int(_) => "int",
                Value::Float(_) => "float",
                Value::String(_) => "str",
                Value::Bool(_) => "bool",
                Value::Void => "void",
                Value::Array(_) => "array",
                Value::Record(_) => "record",
                Value::Map(_) => "map",
                Value::Result(_) => "result",
                Value::Variant(_, _) => "variant",
                Value::Closure { .. } | Value::VMClosure { .. } => "closure",
                Value::TaskHandle(_) => "task",
            };
            self.stack.push(Value::string(t.to_string()));
            return Ok(());
        }
        if name == "println" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            println!("{}", self.val_display(&val));
            self.stack.push(Value::Void);
            return Ok(());
        }
        if name == "print" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            print!("{}", self.val_display(&val));
            self.stack.push(Value::Void);
            return Ok(());
        }
        if name == "eprintln" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            eprintln!("{}", self.val_display(&val));
            self.stack.push(Value::Void);
            return Ok(());
        }
        if name == "int" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            let result = match &val {
                Value::Int(i) => Value::Int(*i),
                Value::Float(f) => Value::Int(*f as i64),
                Value::String(s) => Value::Int(s.parse::<i64>().unwrap_or(0)),
                Value::Bool(b) => Value::Int(if *b { 1 } else { 0 }),
                _ => Value::Int(0),
            };
            self.stack.push(result);
            return Ok(());
        }
        if name == "float" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            let result = match &val {
                Value::Float(f) => Value::Float(*f),
                Value::Int(i) => Value::Float(*i as f64),
                Value::String(s) => Value::Float(s.parse::<f64>().unwrap_or(0.0)),
                _ => Value::Float(0.0),
            };
            self.stack.push(result);
            return Ok(());
        }

        if matches!(name, "map" | "filter" | "reduce" | "each" |
            "sort_by" | "find" | "any" | "all" | "flat_map" |
            "min_by" | "max_by") && nargs >= 2 {
            return self.do_hof(name, nargs);
        }

        if name == "__bridge_convert__" && nargs == 1 {
            let val = self.stack.pop().unwrap_or(Value::Void);
            match crate::bridge::value_to_program(&val) {
                Ok(program) => {
                    let n = program.functions.len();
                    let has_main = program.main_idx.is_some();
                    let mut map = HashMap::new();
                    map.insert("ok".into(), Value::Bool(true));
                    map.insert("functions".into(), Value::Int(n as i64));
                    map.insert("has_main".into(), Value::Bool(has_main));
                    self.stack.push(Value::Map(Arc::new(map)));
                }
                Err(e) => {
                    let mut map = HashMap::new();
                    map.insert("ok".into(), Value::Bool(false));
                    map.insert("error".into(), Value::string(e));
                    self.stack.push(Value::Map(Arc::new(map)));
                }
            }
            return Ok(());
        }
        if name == "__bridge_exec__" && nargs >= 1 {
            let extra_args: Vec<Value> = if nargs > 1 {
                let start = self.stack.len() - (nargs - 1);
                self.stack.drain(start..).collect()
            } else {
                Vec::new()
            };
            let val = self.stack.pop().unwrap_or(Value::Void);
            match crate::bridge::value_to_program(&val) {
                Ok(program) => {
                    let mut child_vm = VM::new(program);
                    let str_args: Vec<String> = extra_args.iter().map(|v| match v {
                        Value::String(s) => (**s).clone(),
                        _ => String::new(),
                    }).collect();
                    child_vm.set_args(str_args);
                    match child_vm.run() {
                        Ok(v) => self.stack.push(v),
                        Err(e) => {
                            self.stack.push(Value::string(format!("error: {}", e.message)));
                        }
                    }
                }
                Err(e) => {
                    self.stack.push(Value::string(format!("bridge error: {e}")));
                }
            }
            return Ok(());
        }

        if name == "eval" && nargs >= 1 {
            let extra_args: Vec<Value> = if nargs > 1 {
                let start = self.stack.len() - (nargs - 1);
                self.stack.drain(start..).collect()
            } else {
                Vec::new()
            };
            let code_val = self.stack.pop().unwrap_or(Value::Void);
            let code = match &code_val {
                Value::String(s) => (**s).clone(),
                _ => {
                    self.stack.push(Value::string("eval error: expected string argument".to_string()));
                    return Ok(());
                }
            };
            match Self::eval_compile(&code) {
                Ok(program) => {
                    let mut child_vm = VM::new(program);
                    let str_args: Vec<String> = extra_args.iter().map(|v| match v {
                        Value::String(s) => (**s).clone(),
                        Value::Int(i) => i.to_string(),
                        Value::Float(f) => f.to_string(),
                        Value::Bool(b) => b.to_string(),
                        _ => String::new(),
                    }).collect();
                    if !str_args.is_empty() {
                        child_vm.set_args(str_args);
                    }
                    match child_vm.run() {
                        Ok(v) => self.stack.push(v),
                        Err(e) => {
                            self.stack.push(Value::string(format!("eval error: {}", e.message)));
                        }
                    }
                }
                Err(e) => {
                    self.stack.push(Value::string(format!("eval error: {e}")));
                }
            }
            return Ok(());
        }

        if name == "spawn" && nargs == 1 {
            let closure_val = self.stack.pop().unwrap_or(Value::Void);
            let program = Arc::clone(&self.program);
            let id = self.next_task_id;
            self.next_task_id += 1;
            let handle = std::thread::spawn(move || {
                let mut vm = VM::new_shared(program);
                vm.call_value(&closure_val, &[]).map_err(|e| e.message)
            });
            self.task_handles.insert(id, handle);
            self.stack.push(Value::TaskHandle(id));
            return Ok(());
        }

        if name == "await" && nargs == 1 {
            let handle_val = self.stack.pop().unwrap_or(Value::Void);
            match handle_val {
                Value::TaskHandle(id) => {
                    if let Some(handle) = self.task_handles.remove(&id) {
                        match handle.join() {
                            Ok(Ok(v)) => self.stack.push(Value::Result(Ok(Box::new(v)))),
                            Ok(Err(e)) => self.stack.push(Value::Result(Err(Box::new(Value::string(e))))),
                            Err(_) => self.stack.push(Value::Result(Err(Box::new(Value::string("task panicked".into()))))),
                        }
                    } else {
                        self.stack.push(Value::Result(Err(Box::new(Value::string("invalid or already-awaited task handle".into())))));
                    }
                }
                _ => {
                    self.stack.push(Value::Result(Err(Box::new(Value::string("await expects a task handle".into())))));
                }
            }
            return Ok(());
        }

        if name == "await_all" && nargs == 1 {
            let arr_val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(handles) = arr_val {
                let mut results = Vec::with_capacity(handles.len());
                for h in handles.iter() {
                    match h {
                        Value::TaskHandle(id) => {
                            if let Some(handle) = self.task_handles.remove(id) {
                                match handle.join() {
                                    Ok(Ok(v)) => results.push(Value::Result(Ok(Box::new(v)))),
                                    Ok(Err(e)) => results.push(Value::Result(Err(Box::new(Value::string(e))))),
                                    Err(_) => results.push(Value::Result(Err(Box::new(Value::string("task panicked".into()))))),
                                }
                            } else {
                                results.push(Value::Result(Err(Box::new(Value::string("invalid or already-awaited task handle".into())))));
                            }
                        }
                        _ => results.push(Value::Result(Err(Box::new(Value::string("await_all expects array of task handles".into()))))),
                    }
                }
                self.stack.push(Value::array(results));
            } else {
                self.stack.push(Value::array(vec![]));
            }
            return Ok(());
        }

        if name == "parallel_map" && nargs == 2 {
            let func = self.stack.pop().unwrap_or(Value::Void);
            let arr_val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(items) = arr_val {
                let handles: Vec<_> = items.iter().map(|item| {
                    let prog = Arc::clone(&self.program);
                    let f = func.clone();
                    let arg = item.clone();
                    std::thread::spawn(move || {
                        let mut vm = VM::new_shared(prog);
                        vm.call_value(&f, &[arg]).map_err(|e| e.message)
                    })
                }).collect();
                let mut results = Vec::with_capacity(handles.len());
                for h in handles {
                    match h.join() {
                        Ok(Ok(v)) => results.push(v),
                        Ok(Err(e)) => results.push(Value::Result(Err(Box::new(Value::string(e))))),
                        Err(_) => results.push(Value::Result(Err(Box::new(Value::string("task panicked".into()))))),
                    }
                }
                self.stack.push(Value::array(results));
            } else {
                self.stack.push(Value::array(vec![]));
            }
            return Ok(());
        }

        if name == "parallel_each" && nargs == 2 {
            let func = self.stack.pop().unwrap_or(Value::Void);
            let arr_val = self.stack.pop().unwrap_or(Value::Void);
            if let Value::Array(items) = arr_val {
                let handles: Vec<_> = items.iter().map(|item| {
                    let prog = Arc::clone(&self.program);
                    let f = func.clone();
                    let arg = item.clone();
                    std::thread::spawn(move || {
                        let mut vm = VM::new_shared(prog);
                        vm.call_value(&f, &[arg]).map_err(|e| e.message)
                    })
                }).collect();
                for h in handles {
                    let _ = h.join();
                }
            }
            self.stack.push(Value::Void);
            return Ok(());
        }

        if name == "timeout" && nargs == 2 {
            let func = self.stack.pop().unwrap_or(Value::Void);
            let ms_val = self.stack.pop().unwrap_or(Value::Void);
            let ms = match ms_val {
                Value::Int(i) => i as u64,
                Value::Float(f) => f as u64,
                _ => 0,
            };
            let prog = Arc::clone(&self.program);
            let (tx, rx) = std::sync::mpsc::channel();
            std::thread::spawn(move || {
                let mut vm = VM::new_shared(prog);
                let result = vm.call_value(&func, &[]).map_err(|e| e.message);
                let _ = tx.send(result);
            });
            match rx.recv_timeout(std::time::Duration::from_millis(ms)) {
                Ok(Ok(v)) => self.stack.push(Value::Result(Ok(Box::new(v)))),
                Ok(Err(e)) => self.stack.push(Value::Result(Err(Box::new(Value::string(e))))),
                Err(std::sync::mpsc::RecvTimeoutError::Timeout) => {
                    self.stack.push(Value::Result(Err(Box::new(Value::string("timeout".into())))));
                }
                Err(_) => {
                    self.stack.push(Value::Result(Err(Box::new(Value::string("task failed".into())))));
                }
            }
            return Ok(());
        }

        if let Some(fi) = self.find_function(name) {
            let stack_base = self.stack.len() - nargs;
            let num_locals = self.program.functions[fi].locals.len();
            while self.stack.len() < stack_base + num_locals {
                self.stack.push(Value::Void);
            }
            self.frames.push(CallFrame {
                fn_idx: fi,
                ip: 0,
                stack_base,
            });
            return Ok(());
        }

        if let Some(result) = self.try_vm_builtin(name, nargs)? {
            self.stack.push(result);
            return Ok(());
        }

        if builtins::is_builtin(name) {
            let start = self.stack.len() - nargs;
            let args: Vec<Value> = self.stack.drain(start..).collect();
            let result = builtins::call_builtin_no_interp(name, &args)?;
            self.stack.push(result);
            return Ok(());
        }

        let start = if self.stack.len() >= nargs { self.stack.len() - nargs } else { 0 };
        self.stack.drain(start..);
        self.stack.push(Value::Void);
        Ok(())
    }

    fn do_hof(&mut self, name: &str, nargs: usize) -> AResult<()> {
        let start = self.stack.len() - nargs;
        let args: Vec<Value> = self.stack.drain(start..).collect();

        match name {
            "map" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                let mut result = Vec::new();
                for elem in arr.iter() {
                    let v = self.call_value(func, &[elem.clone()])?;
                    result.push(v);
                }
                self.stack.push(Value::array(result));
            }
            "filter" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                let mut result = Vec::new();
                for elem in arr.iter() {
                    let v = self.call_value(func, &[elem.clone()])?;
                    if self.is_truthy(&v) {
                        result.push(elem.clone());
                    }
                }
                self.stack.push(Value::array(result));
            }
            "reduce" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let mut acc = args[1].clone();
                let func = &args[2];
                for elem in arr.iter() {
                    acc = self.call_value(func, &[acc, elem.clone()])?;
                }
                self.stack.push(acc);
            }
            "each" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                for elem in arr.iter() {
                    self.call_value(func, &[elem.clone()])?;
                }
                self.stack.push(Value::Void);
            }
            "sort_by" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                let mut items: Vec<Value> = arr.iter().cloned().collect();
                let mut err: Option<AError> = None;
                let func_clone = func.clone();
                items.sort_by(|a, b| {
                    if err.is_some() { return std::cmp::Ordering::Equal; }
                    match self.call_value(&func_clone, &[a.clone(), b.clone()]) {
                        Ok(Value::Int(n)) => {
                            if n < 0 { std::cmp::Ordering::Less }
                            else if n > 0 { std::cmp::Ordering::Greater }
                            else { std::cmp::Ordering::Equal }
                        }
                        Ok(Value::Float(f)) => {
                            if f < 0.0 { std::cmp::Ordering::Less }
                            else if f > 0.0 { std::cmp::Ordering::Greater }
                            else { std::cmp::Ordering::Equal }
                        }
                        Ok(Value::Bool(b)) => {
                            if b { std::cmp::Ordering::Less } else { std::cmp::Ordering::Greater }
                        }
                        Ok(_) => std::cmp::Ordering::Equal,
                        Err(e) => { err = Some(e); std::cmp::Ordering::Equal }
                    }
                });
                if let Some(e) = err { return Err(e); }
                self.stack.push(Value::array(items));
            }
            "find" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                let mut found = None;
                for elem in arr.iter() {
                    let v = self.call_value(func, &[elem.clone()])?;
                    if self.is_truthy(&v) {
                        found = Some(elem.clone());
                        break;
                    }
                }
                match found {
                    Some(v) => self.stack.push(Value::Result(Ok(Box::new(v)))),
                    None => self.stack.push(Value::Result(Err(Box::new(Value::string("not found".into()))))),
                }
            }
            "any" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Bool(false)); return Ok(()); }
                };
                let func = &args[1];
                let mut result = false;
                for elem in arr.iter() {
                    let v = self.call_value(func, &[elem.clone()])?;
                    if self.is_truthy(&v) { result = true; break; }
                }
                self.stack.push(Value::Bool(result));
            }
            "all" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Bool(true)); return Ok(()); }
                };
                let func = &args[1];
                let mut result = true;
                for elem in arr.iter() {
                    let v = self.call_value(func, &[elem.clone()])?;
                    if !self.is_truthy(&v) { result = false; break; }
                }
                self.stack.push(Value::Bool(result));
            }
            "flat_map" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                let mut result = Vec::new();
                for elem in arr.iter() {
                    let v = self.call_value(func, &[elem.clone()])?;
                    match v {
                        Value::Array(inner) => result.extend(inner.iter().cloned()),
                        other => result.push(other),
                    }
                }
                self.stack.push(Value::array(result));
            }
            "min_by" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                if arr.is_empty() {
                    self.stack.push(Value::Result(Err(Box::new(Value::string("empty array".into())))));
                } else {
                    let mut best = arr[0].clone();
                    let mut best_key = self.call_value(func, &[best.clone()])?;
                    for elem in arr.iter().skip(1) {
                        let key = self.call_value(func, &[elem.clone()])?;
                        if val_less(&key, &best_key) {
                            best = elem.clone();
                            best_key = key;
                        }
                    }
                    self.stack.push(Value::Result(Ok(Box::new(best))));
                }
            }
            "max_by" => {
                let arr = match &args[0] {
                    Value::Array(a) => a.clone(),
                    _ => { self.stack.push(Value::Void); return Ok(()); }
                };
                let func = &args[1];
                if arr.is_empty() {
                    self.stack.push(Value::Result(Err(Box::new(Value::string("empty array".into())))));
                } else {
                    let mut best = arr[0].clone();
                    let mut best_key = self.call_value(func, &[best.clone()])?;
                    for elem in arr.iter().skip(1) {
                        let key = self.call_value(func, &[elem.clone()])?;
                        if val_less(&best_key, &key) {
                            best = elem.clone();
                            best_key = key;
                        }
                    }
                    self.stack.push(Value::Result(Ok(Box::new(best))));
                }
            }
            _ => { self.stack.push(Value::Void); }
        }
        Ok(())
    }

    fn call_value(&mut self, func: &Value, args: &[Value]) -> AResult<Value> {
        match func {
            Value::String(s) => {
                for a in args {
                    self.stack.push(a.clone());
                }
                self.do_call(s.as_str(), args.len())?;
                self.run_to_return()
            }
            Value::VMClosure { name, captures } => {
                for c in captures.iter() {
                    self.stack.push(c.clone());
                }
                for a in args {
                    self.stack.push(a.clone());
                }
                self.do_call(name.as_str(), args.len() + captures.len())?;
                self.run_to_return()
            }
            _ => Ok(Value::Void),
        }
    }

    fn run_to_return(&mut self) -> AResult<Value> {
        let target_depth = self.frames.len() - 1;
        self.execute_loop(target_depth)
    }

    fn find_function(&self, name: &str) -> Option<usize> {
        self.fn_index.get(name).copied()
    }

    fn try_vm_builtin(&mut self, name: &str, nargs: usize) -> AResult<Option<Value>> {
        match name {
            "str.split" if nargs == 2 => {
                let delim = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(d)) = (s, delim) {
                    Ok(Some(Value::array(s.split(d.as_str()).map(|p| Value::string(p.to_string())).collect())))
                } else { Ok(Some(Value::Void)) }
            }
            "str.contains" if nargs == 2 => {
                let sub = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(sub)) = (s, sub) {
                    Ok(Some(Value::Bool(s.contains(sub.as_str()))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "str.starts_with" if nargs == 2 => {
                let pre = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(p)) = (s, pre) {
                    Ok(Some(Value::Bool(s.starts_with(p.as_str()))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "str.ends_with" if nargs == 2 => {
                let suf = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(su)) = (s, suf) {
                    Ok(Some(Value::Bool(s.ends_with(su.as_str()))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "str.replace" if nargs == 3 => {
                let to = self.stack.pop().unwrap_or(Value::Void);
                let from = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(f), Value::String(t)) = (s, from, to) {
                    Ok(Some(Value::string(s.replace(f.as_str(), t.as_str()))))
                } else { Ok(Some(Value::Void)) }
            }
            "str.trim" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::string(s.trim().to_string())))
                } else { Ok(Some(Value::Void)) }
            }
            "str.upper" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::string(s.to_uppercase())))
                } else { Ok(Some(Value::Void)) }
            }
            "str.lower" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::string(s.to_lowercase())))
                } else { Ok(Some(Value::Void)) }
            }
            "str.join" if nargs == 2 => {
                let sep = self.stack.pop().unwrap_or(Value::Void);
                let arr = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::Array(a), Value::String(sep)) = (arr, sep) {
                    let parts: Vec<String> = a.iter().map(|v| self.val_display(v)).collect();
                    Ok(Some(Value::string(parts.join(sep.as_str()))))
                } else { Ok(Some(Value::Void)) }
            }
            "str.chars" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::array(s.chars().map(|c| Value::string(c.to_string())).collect())))
                } else { Ok(Some(Value::Void)) }
            }
            "str.slice" if nargs == 3 => {
                let end = self.stack.pop().unwrap_or(Value::Int(0));
                let start = self.stack.pop().unwrap_or(Value::Int(0));
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::Int(st), Value::Int(en)) = (s, start, end) {
                    let st = st.max(0) as usize;
                    let en = en.min(s.len() as i64) as usize;
                    let sliced: String = s.chars().skip(st).take(en.saturating_sub(st)).collect();
                    Ok(Some(Value::string(sliced)))
                } else { Ok(Some(Value::Void)) }
            }
            "str.lines" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::array(s.lines().map(|l| Value::string(l.to_string())).collect())))
                } else { Ok(Some(Value::Void)) }
            }
            "str.find" if nargs == 2 => {
                let sub = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(sub)) = (s, sub) {
                    Ok(Some(Value::Int(s.find(sub.as_str()).map(|p| p as i64).unwrap_or(-1))))
                } else { Ok(Some(Value::Int(-1))) }
            }
            "str.count" if nargs == 2 => {
                let sub = self.stack.pop().unwrap_or(Value::Void);
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::String(s), Value::String(sub)) = (s, sub) {
                    Ok(Some(Value::Int(s.matches(sub.as_str()).count() as i64)))
                } else { Ok(Some(Value::Int(0))) }
            }
            "str.concat" if nargs >= 1 => {
                let start = self.stack.len() - nargs;
                let args: Vec<Value> = self.stack.drain(start..).collect();
                let mut result = String::new();
                for a in &args { result.push_str(&self.val_display(a)); }
                Ok(Some(Value::string(result)))
            }
            "map.get" if nargs == 2 => {
                let key = self.stack.pop().unwrap_or(Value::Void);
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    let k = self.val_to_key(&key);
                    Ok(Some(map.get(&k).cloned().unwrap_or(Value::Void)))
                } else { Ok(Some(Value::Void)) }
            }
            "map.set" if nargs == 3 => {
                let val = self.stack.pop().unwrap_or(Value::Void);
                let key = self.stack.pop().unwrap_or(Value::Void);
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    let k = self.val_to_key(&key);
                    let mut new_m = Arc::try_unwrap(map).unwrap_or_else(|rc| (*rc).clone());
                    new_m.insert(k, val);
                    Ok(Some(Value::map(new_m)))
                } else { Ok(Some(Value::Void)) }
            }
            "map.keys" if nargs == 1 => {
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    Ok(Some(Value::array(map.keys().map(|k| Value::string(k.clone())).collect())))
                } else { Ok(Some(Value::Void)) }
            }
            "map.values" if nargs == 1 => {
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    Ok(Some(Value::array(map.values().cloned().collect())))
                } else { Ok(Some(Value::Void)) }
            }
            "map.has" if nargs == 2 => {
                let key = self.stack.pop().unwrap_or(Value::Void);
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    let k = self.val_to_key(&key);
                    Ok(Some(Value::Bool(map.contains_key(&k))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "map.delete" if nargs == 2 => {
                let key = self.stack.pop().unwrap_or(Value::Void);
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    let k = self.val_to_key(&key);
                    let mut new_m = Arc::try_unwrap(map).unwrap_or_else(|rc| (*rc).clone());
                    new_m.remove(&k);
                    Ok(Some(Value::map(new_m)))
                } else { Ok(Some(Value::Void)) }
            }
            "map.merge" if nargs == 2 => {
                let b = self.stack.pop().unwrap_or(Value::Void);
                let a = self.stack.pop().unwrap_or(Value::Void);
                if let (Value::Map(ma), Value::Map(mb)) = (a, b) {
                    let mut merged = Arc::try_unwrap(ma).unwrap_or_else(|rc| (*rc).clone());
                    for (k, v) in mb.iter() {
                        merged.insert(k.clone(), v.clone());
                    }
                    Ok(Some(Value::map(merged)))
                } else { Ok(Some(Value::Void)) }
            }
            "map.entries" if nargs == 1 => {
                let m = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Map(map) = m {
                    Ok(Some(Value::array(map.iter().map(|(k, v)| Value::array(vec![Value::string(k.clone()), v.clone()])).collect())))
                } else { Ok(Some(Value::Void)) }
            }
            "map.from_entries" if nargs == 1 => {
                let arr = self.stack.pop().unwrap_or(Value::Void);
                if let Value::Array(entries) = arr {
                    let mut map = HashMap::new();
                    for entry in entries.iter() {
                        if let Value::Array(pair) = entry {
                            if pair.len() >= 2 {
                                let key = self.val_to_key(&pair[0]);
                                map.insert(key, pair[1].clone());
                            }
                        }
                    }
                    Ok(Some(Value::map(map)))
                } else { Ok(Some(Value::Void)) }
            }
            "char_code" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    if let Some(c) = s.chars().next() {
                        Ok(Some(Value::Int(c as i64)))
                    } else { Ok(Some(Value::Int(0))) }
                } else { Ok(Some(Value::Int(0))) }
            }
            "from_code" if nargs == 1 => {
                let n = self.stack.pop().unwrap_or(Value::Int(0));
                if let Value::Int(n) = n {
                    if let Some(c) = char::from_u32(n as u32) {
                        Ok(Some(Value::string(c.to_string())))
                    } else { Ok(Some(Value::Void)) }
                } else { Ok(Some(Value::Void)) }
            }
            "is_alpha" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::Bool(!s.is_empty() && s.chars().all(|c| c.is_ascii_alphabetic()))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "is_digit" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::Bool(!s.is_empty() && s.chars().all(|c| c.is_ascii_digit()))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "is_alnum" if nargs == 1 => {
                let s = self.stack.pop().unwrap_or(Value::Void);
                if let Value::String(s) = s {
                    Ok(Some(Value::Bool(!s.is_empty() && s.chars().all(|c| c.is_ascii_alphanumeric() || c == '_'))))
                } else { Ok(Some(Value::Bool(false))) }
            }
            "sqrt" if nargs == 1 => {
                let v = self.stack.pop().unwrap_or(Value::Void);
                match v {
                    Value::Float(f) => Ok(Some(Value::Float(f.sqrt()))),
                    Value::Int(i) => Ok(Some(Value::Float((i as f64).sqrt()))),
                    _ => Ok(Some(Value::Void)),
                }
            }
            "abs" if nargs == 1 => {
                let v = self.stack.pop().unwrap_or(Value::Void);
                match v {
                    Value::Int(i) => Ok(Some(Value::Int(i.abs()))),
                    Value::Float(f) => Ok(Some(Value::Float(f.abs()))),
                    _ => Ok(Some(Value::Void)),
                }
            }
            "floor" if nargs == 1 => {
                let v = self.stack.pop().unwrap_or(Value::Void);
                match v {
                    Value::Float(f) => Ok(Some(Value::Int(f.floor() as i64))),
                    Value::Int(i) => Ok(Some(Value::Int(i))),
                    _ => Ok(Some(Value::Void)),
                }
            }
            "ceil" if nargs == 1 => {
                let v = self.stack.pop().unwrap_or(Value::Void);
                match v {
                    Value::Float(f) => Ok(Some(Value::Int(f.ceil() as i64))),
                    Value::Int(i) => Ok(Some(Value::Int(i))),
                    _ => Ok(Some(Value::Void)),
                }
            }
            "round" if nargs == 1 => {
                let v = self.stack.pop().unwrap_or(Value::Void);
                match v {
                    Value::Float(f) => Ok(Some(Value::Int(f.round() as i64))),
                    Value::Int(i) => Ok(Some(Value::Int(i))),
                    _ => Ok(Some(Value::Void)),
                }
            }
            _ => Ok(None),
        }
    }

    fn binary_op(&mut self, f: impl Fn(Value, Value) -> Value) {
        let b = self.stack.pop().unwrap_or(Value::Void);
        let a = self.stack.pop().unwrap_or(Value::Void);
        self.stack.push(f(a, b));
    }


    fn is_truthy(&self, val: &Value) -> bool {
        match val {
            Value::Bool(b) => *b,
            Value::Int(i) => *i != 0,
            Value::Void => false,
            Value::String(s) => !s.is_empty(),
            _ => true,
        }
    }

    fn val_to_key(&self, val: &Value) -> String {
        match val {
            Value::String(s) => s.as_ref().clone(),
            Value::Int(i) => i.to_string(),
            Value::Bool(b) => b.to_string(),
            Value::Float(f) => format!("{f}"),
            other => self.val_display(other),
        }
    }

    fn val_display(&self, val: &Value) -> String {
        match val {
            Value::Int(i) => i.to_string(),
            Value::Float(f) => format!("{f}"),
            Value::String(s) => (**s).clone(),
            Value::Bool(b) => b.to_string(),
            Value::Void => "void".to_string(),
            Value::Array(elems) => {
                let inner: Vec<String> = elems.iter().map(|v| self.val_display(v)).collect();
                format!("[{}]", inner.join(", "))
            }
            Value::Record(fields) => {
                let inner: Vec<String> = fields.iter().map(|(k, v)| format!("{k}: {}", self.val_display(v))).collect();
                format!("{{{}}}", inner.join(", "))
            }
            Value::Map(entries) => {
                let inner: Vec<String> = entries.iter().map(|(k, v)| format!("\"{}\": {}", k, self.val_display(v))).collect();
                format!("#{{{}}}", inner.join(", "))
            }
            Value::Result(Ok(v)) => format!("Ok({})", self.val_display(v)),
            Value::Result(Err(e)) => format!("Err({})", self.val_display(e)),
            Value::Variant(name, fields) => {
                if fields.is_empty() {
                    name.clone()
                } else {
                    let inner: Vec<String> = fields.iter().map(|v| self.val_display(v)).collect();
                    format!("{name}({})", inner.join(", "))
                }
            }
            Value::Closure { .. } | Value::VMClosure { .. } => "<closure>".to_string(),
            Value::TaskHandle(id) => format!("<task:{id}>"),
        }
    }
}
