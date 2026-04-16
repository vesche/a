use std::collections::HashMap;
use std::sync::Arc;

use crate::bytecode::*;
use crate::interpreter::Value;

fn get_str(map: &HashMap<String, Value>, key: &str) -> String {
    match map.get(key) {
        Some(Value::String(s)) => (**s).clone(),
        _ => String::new(),
    }
}

fn get_int(map: &HashMap<String, Value>, key: &str) -> i64 {
    match map.get(key) {
        Some(Value::Int(i)) => *i,
        _ => -1,
    }
}

fn get_array<'a>(map: &'a HashMap<String, Value>, key: &str) -> &'a [Value] {
    match map.get(key) {
        Some(Value::Array(a)) => a.as_slice(),
        _ => &[],
    }
}

fn get_map<'a>(map: &'a HashMap<String, Value>, key: &str) -> Option<&'a Arc<HashMap<String, Value>>> {
    match map.get(key) {
        Some(Value::Map(m)) => Some(m),
        _ => None,
    }
}

fn val_to_usize(val: &Value) -> usize {
    match val {
        Value::Int(i) => *i as usize,
        _ => 0,
    }
}

fn value_to_op(val: &Value) -> Result<Op, String> {
    let arr = match val {
        Value::Array(a) => a,
        _ => return Err("opcode must be an array".into()),
    };
    if arr.is_empty() {
        return Err("empty opcode array".into());
    }
    let name = match &arr[0] {
        Value::String(s) => s.as_str(),
        _ => return Err("opcode name must be a string".into()),
    };
    let a = || val_to_usize(arr.get(1).unwrap_or(&Value::Int(0)));
    let b = || val_to_usize(arr.get(2).unwrap_or(&Value::Int(0)));
    let c = || val_to_usize(arr.get(3).unwrap_or(&Value::Int(0)));

    match name {
        "Const" => Ok(Op::Const(a())),
        "Pop" => Ok(Op::Pop),
        "GetLocal" => Ok(Op::GetLocal(a())),
        "SetLocal" => Ok(Op::SetLocal(a())),
        "GetGlobal" => Ok(Op::GetGlobal(a())),
        "SetGlobal" => Ok(Op::SetGlobal(a())),
        "Add" => Ok(Op::Add),
        "Sub" => Ok(Op::Sub),
        "Mul" => Ok(Op::Mul),
        "Div" => Ok(Op::Div),
        "Mod" => Ok(Op::Mod),
        "Neg" => Ok(Op::Neg),
        "Eq" => Ok(Op::Eq),
        "NotEq" => Ok(Op::NotEq),
        "Lt" => Ok(Op::Lt),
        "Gt" => Ok(Op::Gt),
        "LtEq" => Ok(Op::LtEq),
        "GtEq" => Ok(Op::GtEq),
        "Not" => Ok(Op::Not),
        "JumpIfFalseKeep" => Ok(Op::JumpIfFalseKeep(a())),
        "Jump" => Ok(Op::Jump(a())),
        "JumpIfFalse" => Ok(Op::JumpIfFalse(a())),
        "Loop" => Ok(Op::Loop(a())),
        "Call" => Ok(Op::Call(a(), b())),
        "CallMethod" => Ok(Op::CallMethod(a(), b(), c())),
        "CallClosure" => Ok(Op::CallClosure(a())),
        "TailCall" => Ok(Op::TailCall(a(), b())),
        "TailCallClosure" => Ok(Op::TailCallClosure(a())),
        "Return" => Ok(Op::Return),
        "Array" => Ok(Op::Array(a())),
        "Index" => Ok(Op::Index),
        "IndexSet" => Ok(Op::IndexSet),
        "Field" => Ok(Op::Field(a())),
        "MapNew" => Ok(Op::MapNew(a())),
        "Record" => Ok(Op::Record(a())),
        "Concat" => Ok(Op::Concat(a())),
        "MakeOk" => Ok(Op::MakeOk),
        "MakeErr" => Ok(Op::MakeErr),
        "Unwrap" => Ok(Op::Unwrap),
        "UnwrapOr" => Ok(Op::UnwrapOr),
        "IsOk" => Ok(Op::IsOk),
        "IsErr" => Ok(Op::IsErr),
        "TryStart" => Ok(Op::TryStart(a())),
        "TryEnd" => Ok(Op::TryEnd),
        "Dup" => Ok(Op::Dup),
        "MatchVariant" => Ok(Op::MatchVariant(a(), b())),
        "MatchLiteral" => Ok(Op::MatchLiteral(a(), b())),
        "BindMatchVar" => Ok(Op::BindMatchVar(a())),
        "PopScope" => Ok(Op::PopScope),
        "MakeClosure" => Ok(Op::MakeClosure(a(), b())),
        _ => Err(format!("unknown opcode: {name}")),
    }
}

fn value_to_chunk(map: &HashMap<String, Value>) -> Result<Chunk, String> {
    let mut chunk = Chunk::new();

    for op_val in get_array(map, "code") {
        let op = value_to_op(op_val)?;
        chunk.code.push(op);
    }

    for line_val in get_array(map, "lines") {
        chunk.lines.push(val_to_usize(line_val) as u32);
    }

    for const_val in get_array(map, "constants") {
        chunk.constants.push(const_val.clone());
    }

    for str_val in get_array(map, "strings") {
        match str_val {
            Value::String(s) => chunk.strings.push((**s).clone()),
            _ => chunk.strings.push(String::new()),
        }
    }

    Ok(chunk)
}

fn value_to_fn(val: &Value) -> Result<CompiledFn, String> {
    let map = match val {
        Value::Map(m) => m,
        _ => return Err("function must be a map".into()),
    };

    let name = get_str(map, "name");

    let params: Vec<String> = get_array(map, "params")
        .iter()
        .filter_map(|v| match v {
            Value::String(s) => Some((**s).clone()),
            _ => None,
        })
        .collect();

    let locals: Vec<String> = get_array(map, "locals")
        .iter()
        .filter_map(|v| match v {
            Value::String(s) => Some((**s).clone()),
            _ => None,
        })
        .collect();

    let chunk = match get_map(map, "chunk") {
        Some(chunk_map) => value_to_chunk(chunk_map)?,
        None => return Err("function missing chunk".into()),
    };

    Ok(CompiledFn { name, params, chunk, locals })
}

pub fn value_to_program(val: &Value) -> Result<CompiledProgram, String> {
    let map = match val {
        Value::Map(m) => m,
        _ => return Err("program must be a map".into()),
    };

    let mut functions = Vec::new();
    for fn_val in get_array(map, "functions") {
        functions.push(value_to_fn(fn_val)?);
    }

    let main_idx_raw = get_int(map, "main_idx");
    let main_idx = if main_idx_raw >= 0 {
        Some(main_idx_raw as usize)
    } else {
        None
    };

    Ok(CompiledProgram { functions, main_idx })
}

// --- Inverse: Rust structs -> Value::Map ---

fn op_to_value(op: &Op) -> Value {
    let s = |name: &str| Value::string(name.to_string());
    let u = |n: usize| Value::Int(n as i64);
    match op {
        Op::Const(a)            => Value::array(vec![s("Const"), u(*a)]),
        Op::Pop                 => Value::array(vec![s("Pop")]),
        Op::GetLocal(a)         => Value::array(vec![s("GetLocal"), u(*a)]),
        Op::SetLocal(a)         => Value::array(vec![s("SetLocal"), u(*a)]),
        Op::GetGlobal(a)        => Value::array(vec![s("GetGlobal"), u(*a)]),
        Op::SetGlobal(a)        => Value::array(vec![s("SetGlobal"), u(*a)]),
        Op::Add                 => Value::array(vec![s("Add")]),
        Op::Sub                 => Value::array(vec![s("Sub")]),
        Op::Mul                 => Value::array(vec![s("Mul")]),
        Op::Div                 => Value::array(vec![s("Div")]),
        Op::Mod                 => Value::array(vec![s("Mod")]),
        Op::Neg                 => Value::array(vec![s("Neg")]),
        Op::Eq                  => Value::array(vec![s("Eq")]),
        Op::NotEq               => Value::array(vec![s("NotEq")]),
        Op::Lt                  => Value::array(vec![s("Lt")]),
        Op::Gt                  => Value::array(vec![s("Gt")]),
        Op::LtEq                => Value::array(vec![s("LtEq")]),
        Op::GtEq                => Value::array(vec![s("GtEq")]),
        Op::Not                 => Value::array(vec![s("Not")]),
        Op::JumpIfFalseKeep(a)  => Value::array(vec![s("JumpIfFalseKeep"), u(*a)]),
        Op::Jump(a)             => Value::array(vec![s("Jump"), u(*a)]),
        Op::JumpIfFalse(a)      => Value::array(vec![s("JumpIfFalse"), u(*a)]),
        Op::Loop(a)             => Value::array(vec![s("Loop"), u(*a)]),
        Op::Call(a, b)          => Value::array(vec![s("Call"), u(*a), u(*b)]),
        Op::CallMethod(a, b, c) => Value::array(vec![s("CallMethod"), u(*a), u(*b), u(*c)]),
        Op::CallClosure(a)      => Value::array(vec![s("CallClosure"), u(*a)]),
        Op::TailCall(a, b)      => Value::array(vec![s("TailCall"), u(*a), u(*b)]),
        Op::TailCallClosure(a)  => Value::array(vec![s("TailCallClosure"), u(*a)]),
        Op::Return              => Value::array(vec![s("Return")]),
        Op::Array(a)            => Value::array(vec![s("Array"), u(*a)]),
        Op::Index               => Value::array(vec![s("Index")]),
        Op::IndexSet            => Value::array(vec![s("IndexSet")]),
        Op::Field(a)            => Value::array(vec![s("Field"), u(*a)]),
        Op::MapNew(a)           => Value::array(vec![s("MapNew"), u(*a)]),
        Op::Record(a)           => Value::array(vec![s("Record"), u(*a)]),
        Op::Concat(a)           => Value::array(vec![s("Concat"), u(*a)]),
        Op::MakeOk              => Value::array(vec![s("MakeOk")]),
        Op::MakeErr             => Value::array(vec![s("MakeErr")]),
        Op::Unwrap              => Value::array(vec![s("Unwrap")]),
        Op::UnwrapOr            => Value::array(vec![s("UnwrapOr")]),
        Op::IsOk                => Value::array(vec![s("IsOk")]),
        Op::IsErr               => Value::array(vec![s("IsErr")]),
        Op::TryStart(a)         => Value::array(vec![s("TryStart"), u(*a)]),
        Op::TryEnd              => Value::array(vec![s("TryEnd")]),
        Op::Dup                 => Value::array(vec![s("Dup")]),
        Op::MatchVariant(a, b)  => Value::array(vec![s("MatchVariant"), u(*a), u(*b)]),
        Op::MatchLiteral(a, b)  => Value::array(vec![s("MatchLiteral"), u(*a), u(*b)]),
        Op::BindMatchVar(a)     => Value::array(vec![s("BindMatchVar"), u(*a)]),
        Op::PopScope            => Value::array(vec![s("PopScope")]),
        Op::MakeClosure(a, b)   => Value::array(vec![s("MakeClosure"), u(*a), u(*b)]),
    }
}

fn chunk_to_value(chunk: &Chunk) -> Value {
    let code: Vec<Value> = chunk.code.iter().map(op_to_value).collect();
    let lines: Vec<Value> = chunk.lines.iter().map(|l| Value::Int(*l as i64)).collect();
    let constants: Vec<Value> = chunk.constants.clone();
    let strings: Vec<Value> = chunk.strings.iter().map(|s| Value::string(s.clone())).collect();

    let mut map = HashMap::new();
    map.insert("code".to_string(), Value::array(code));
    map.insert("lines".to_string(), Value::array(lines));
    map.insert("constants".to_string(), Value::array(constants));
    map.insert("strings".to_string(), Value::array(strings));
    Value::Map(Arc::new(map))
}

fn fn_to_value(f: &CompiledFn) -> Value {
    let params: Vec<Value> = f.params.iter().map(|s| Value::string(s.clone())).collect();
    let locals: Vec<Value> = f.locals.iter().map(|s| Value::string(s.clone())).collect();

    let mut map = HashMap::new();
    map.insert("name".to_string(), Value::string(f.name.clone()));
    map.insert("params".to_string(), Value::array(params));
    map.insert("locals".to_string(), Value::array(locals));
    map.insert("chunk".to_string(), chunk_to_value(&f.chunk));
    Value::Map(Arc::new(map))
}

pub fn program_to_value(prog: &CompiledProgram) -> Value {
    let functions: Vec<Value> = prog.functions.iter().map(fn_to_value).collect();
    let main_idx = match prog.main_idx {
        Some(i) => Value::Int(i as i64),
        None => Value::Int(-1),
    };

    let mut map = HashMap::new();
    map.insert("version".to_string(), Value::Int(1));
    map.insert("functions".to_string(), Value::array(functions));
    map.insert("main_idx".to_string(), main_idx);
    Value::Map(Arc::new(map))
}
