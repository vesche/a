use crate::errors::{AError, AResult};
use crate::interpreter::{Interpreter, Value};
use std::collections::HashMap;
use std::io::Read;
use std::process::Command;
#[allow(unused_imports)]
use std::sync::Arc;

pub fn call_builtin(name: &str, args: &[Value], interp: &mut Interpreter) -> AResult<Value> {
    match name {
        "print" => {
            let msg = val_to_display(&args[0]);
            print!("{msg}");
            Ok(Value::Void)
        }
        "println" => {
            let msg = val_to_display(&args[0]);
            println!("{msg}");
            Ok(Value::Void)
        }
        "eprintln" => {
            let msg = val_to_display(&args[0]);
            eprintln!("{msg}");
            Ok(Value::Void)
        }
        "to_str" => {
            Ok(Value::string(val_to_display(&args[0])))
        }
        "len" => match &args[0] {
            Value::String(s) => Ok(Value::Int(s.len() as i64)),
            Value::Array(a) => Ok(Value::Int(a.len() as i64)),
            Value::Map(m) => Ok(Value::Int(m.len() as i64)),
            _ => Err(AError::runtime("len: expected string, array, or map", None)),
        },
        "sqrt" => match &args[0] {
            Value::Float(f) => Ok(Value::Float(f.sqrt())),
            Value::Int(i) => Ok(Value::Float((*i as f64).sqrt())),
            _ => Err(AError::runtime("sqrt: expected numeric", None)),
        },
        "abs" => match &args[0] {
            Value::Int(i) => Ok(Value::Int(i.abs())),
            Value::Float(f) => Ok(Value::Float(f.abs())),
            _ => Err(AError::runtime("abs: expected numeric", None)),
        },
        "floor" => match &args[0] {
            Value::Float(f) => Ok(Value::Int(f.floor() as i64)),
            Value::Int(i) => Ok(Value::Int(*i)),
            _ => Err(AError::runtime("floor: expected numeric", None)),
        },
        "ceil" => match &args[0] {
            Value::Float(f) => Ok(Value::Int(f.ceil() as i64)),
            Value::Int(i) => Ok(Value::Int(*i)),
            _ => Err(AError::runtime("ceil: expected numeric", None)),
        },
        "round" => match &args[0] {
            Value::Float(f) => Ok(Value::Int(f.round() as i64)),
            Value::Int(i) => Ok(Value::Int(*i)),
            _ => Err(AError::runtime("round: expected numeric", None)),
        },

        "map" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("map: first arg must be array", None)),
            };
            let func = &args[1];
            let mut result = Vec::with_capacity(arr.len());
            for elem in arr.iter() {
                result.push(interp.call_closure(func, &[elem.clone()], &None)?);
            }
            Ok(Value::array(result))
        }
        "filter" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("filter: first arg must be array", None)),
            };
            let func = &args[1];
            let mut result = Vec::new();
            for elem in arr.iter() {
                let keep = interp.call_closure(func, &[elem.clone()], &None)?;
                if is_truthy(&keep) {
                    result.push(elem.clone());
                }
            }
            Ok(Value::array(result))
        }
        "reduce" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("reduce: first arg must be array", None)),
            };
            let mut acc = args[1].clone();
            let func = &args[2];
            for elem in arr.iter() {
                acc = interp.call_closure(func, &[acc, elem.clone()], &None)?;
            }
            Ok(acc)
        }
        "each" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("each: first arg must be array", None)),
            };
            let func = &args[1];
            for elem in arr.iter() {
                interp.call_closure(func, &[elem.clone()], &None)?;
            }
            Ok(Value::Void)
        }
        "sort_by" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("sort_by: first arg must be array", None)),
            };
            let func = &args[1];
            let mut items: Vec<Value> = arr.iter().cloned().collect();
            let mut err: Option<AError> = None;
            items.sort_by(|a, b| {
                if err.is_some() { return std::cmp::Ordering::Equal; }
                match interp.call_closure(func, &[a.clone(), b.clone()], &None) {
                    Ok(Value::Int(n)) => {
                        if n < 0 { std::cmp::Ordering::Less }
                        else if n > 0 { std::cmp::Ordering::Greater }
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
            Ok(Value::array(items))
        }
        "find" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("find: first arg must be array", None)),
            };
            let func = &args[1];
            for elem in arr.iter() {
                let v = interp.call_closure(func, &[elem.clone()], &None)?;
                if is_truthy(&v) {
                    return Ok(Value::Result(Ok(Box::new(elem.clone()))));
                }
            }
            Ok(Value::Result(Err(Box::new(Value::string("not found".into())))))
        }
        "any" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("any: first arg must be array", None)),
            };
            let func = &args[1];
            for elem in arr.iter() {
                let v = interp.call_closure(func, &[elem.clone()], &None)?;
                if is_truthy(&v) { return Ok(Value::Bool(true)); }
            }
            Ok(Value::Bool(false))
        }
        "all" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("all: first arg must be array", None)),
            };
            let func = &args[1];
            for elem in arr.iter() {
                let v = interp.call_closure(func, &[elem.clone()], &None)?;
                if !is_truthy(&v) { return Ok(Value::Bool(false)); }
            }
            Ok(Value::Bool(true))
        }
        "flat_map" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("flat_map: first arg must be array", None)),
            };
            let func = &args[1];
            let mut result = Vec::new();
            for elem in arr.iter() {
                let v = interp.call_closure(func, &[elem.clone()], &None)?;
                match v {
                    Value::Array(inner) => result.extend(inner.iter().cloned()),
                    other => result.push(other),
                }
            }
            Ok(Value::array(result))
        }
        "min_by" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("min_by: first arg must be array", None)),
            };
            if arr.is_empty() {
                return Ok(Value::Result(Err(Box::new(Value::string("empty array".into())))));
            }
            let func = &args[1];
            let mut best = arr[0].clone();
            let mut best_key = interp.call_closure(func, &[best.clone()], &None)?;
            for elem in arr.iter().skip(1) {
                let key = interp.call_closure(func, &[elem.clone()], &None)?;
                if val_less_than(&key, &best_key) {
                    best = elem.clone();
                    best_key = key;
                }
            }
            Ok(Value::Result(Ok(Box::new(best))))
        }
        "max_by" => {
            let arr = match &args[0] {
                Value::Array(a) => a.clone(),
                _ => return Err(AError::runtime("max_by: first arg must be array", None)),
            };
            if arr.is_empty() {
                return Ok(Value::Result(Err(Box::new(Value::string("empty array".into())))));
            }
            let func = &args[1];
            let mut best = arr[0].clone();
            let mut best_key = interp.call_closure(func, &[best.clone()], &None)?;
            for elem in arr.iter().skip(1) {
                let key = interp.call_closure(func, &[elem.clone()], &None)?;
                if val_less_than(&best_key, &key) {
                    best = elem.clone();
                    best_key = key;
                }
            }
            Ok(Value::Result(Ok(Box::new(best))))
        }

        "map.get" => {
            if let Value::Map(m) = &args[0] {
                let key = val_to_key(&args[1]);
                Ok(m.get(&key).cloned().unwrap_or(Value::Void))
            } else {
                Err(AError::runtime("map.get: expected map", None))
            }
        }
        "map.set" => {
            if let Value::Map(m) = &args[0] {
                let key = val_to_key(&args[1]);
                let val = args[2].clone();
                let mut new_m = (**m).clone();
                new_m.insert(key, val);
                Ok(Value::map(new_m))
            } else {
                Err(AError::runtime("map.set: expected map", None))
            }
        }
        "map.keys" => {
            if let Value::Map(m) = &args[0] {
                Ok(Value::array(m.keys().map(|k| Value::string(k.clone())).collect()))
            } else {
                Err(AError::runtime("map.keys: expected map", None))
            }
        }
        "map.values" => {
            if let Value::Map(m) = &args[0] {
                Ok(Value::array(m.values().cloned().collect()))
            } else {
                Err(AError::runtime("map.values: expected map", None))
            }
        }
        "map.has" => {
            if let Value::Map(m) = &args[0] {
                let key = val_to_key(&args[1]);
                Ok(Value::Bool(m.contains_key(&key)))
            } else {
                Err(AError::runtime("map.has: expected map", None))
            }
        }
        "map.delete" => {
            if let Value::Map(m) = &args[0] {
                let key = val_to_key(&args[1]);
                let mut new_m = (**m).clone();
                new_m.remove(&key);
                Ok(Value::map(new_m))
            } else {
                Err(AError::runtime("map.delete: expected map", None))
            }
        }
        "map.merge" => {
            if let (Value::Map(a), Value::Map(b)) = (&args[0], &args[1]) {
                let mut merged = (**a).clone();
                for (k, v) in b.iter() {
                    merged.insert(k.clone(), v.clone());
                }
                Ok(Value::map(merged))
            } else {
                Err(AError::runtime("map.merge: expected two maps", None))
            }
        }
        "map.entries" => {
            if let Value::Map(m) = &args[0] {
                Ok(Value::array(m.iter().map(|(k, v)| Value::array(vec![Value::string(k.clone()), v.clone()])).collect()))
            } else {
                Err(AError::runtime("map.entries: expected map", None))
            }
        }
        "map.from_entries" => {
            if let Value::Array(arr) = &args[0] {
                let mut m = HashMap::new();
                for entry in arr.iter() {
                    if let Value::Array(pair) = entry {
                        if pair.len() >= 2 {
                            let key = val_to_key(&pair[0]);
                            m.insert(key, pair[1].clone());
                        }
                    }
                }
                Ok(Value::map(m))
            } else {
                Err(AError::runtime("map.from_entries: expected array", None))
            }
        }

        "io.write" => {
            if args.len() >= 2 {
                let msg = val_to_display(&args[1]);
                print!("{msg}");
            }
            Ok(Value::Void)
        }
        "io.read_file" => {
            if let Value::String(path) = &args[0] {
                match std::fs::read_to_string(path.as_str()) {
                    Ok(contents) => Ok(Value::string(contents)),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("io.read_file: expected string path", None))
            }
        }
        "io.write_file" => {
            if args.len() >= 2 {
                if let (Value::String(path), Value::String(contents)) = (&args[0], &args[1]) {
                    match std::fs::write(path.as_str(), contents.as_str()) {
                        Ok(()) => Ok(Value::Result(Ok(Box::new(Value::Void)))),
                        Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                    }
                } else {
                    Err(AError::runtime("io.write_file: expected (string, string)", None))
                }
            } else {
                Err(AError::runtime("io.write_file: expected 2 arguments", None))
            }
        }
        "io.read_stdin" => {
            let mut buf = String::new();
            match std::io::stdin().read_to_string(&mut buf) {
                Ok(_) => Ok(Value::string(buf)),
                Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
            }
        }
        "io.read_line" => {
            let mut buf = String::new();
            match std::io::stdin().read_line(&mut buf) {
                Ok(0) => Ok(Value::Result(Err(Box::new(Value::string("eof".to_string()))))),
                Ok(_) => {
                    if buf.ends_with('\n') { buf.pop(); }
                    if buf.ends_with('\r') { buf.pop(); }
                    Ok(Value::string(buf))
                }
                Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
            }
        }
        "io.read_bytes" => {
            let n = match &args[0] {
                Value::Int(i) => *i as usize,
                _ => 0,
            };
            let mut buf = vec![0u8; n];
            use std::io::Read;
            let mut total = 0;
            while total < n {
                match std::io::stdin().read(&mut buf[total..]) {
                    Ok(0) => break,
                    Ok(got) => total += got,
                    Err(_) => break,
                }
            }
            buf.truncate(total);
            Ok(Value::string(String::from_utf8_lossy(&buf).to_string()))
        }
        "io.flush" => {
            use std::io::Write;
            let _ = std::io::stdout().flush();
            Ok(Value::Void)
        }
        "str.concat" => {
            let mut result = String::new();
            for a in args {
                result.push_str(&val_to_display(a));
            }
            Ok(Value::string(result))
        }
        "str.split" => {
            if let (Value::String(s), Value::String(delim)) = (&args[0], &args[1]) {
                let parts: Vec<Value> = s.split(delim.as_str())
                    .map(|p| Value::string(p.to_string()))
                    .collect();
                Ok(Value::array(parts))
            } else {
                Err(AError::runtime("str.split: expected (string, string)", None))
            }
        }
        "str.contains" => {
            if let (Value::String(s), Value::String(sub)) = (&args[0], &args[1]) {
                Ok(Value::Bool(s.contains(sub.as_str())))
            } else {
                Err(AError::runtime("str.contains: expected (string, string)", None))
            }
        }
        "str.starts_with" => {
            if let (Value::String(s), Value::String(pre)) = (&args[0], &args[1]) {
                Ok(Value::Bool(s.starts_with(pre.as_str())))
            } else {
                Err(AError::runtime("str.starts_with: expected (string, string)", None))
            }
        }
        "str.ends_with" => {
            if let (Value::String(s), Value::String(suf)) = (&args[0], &args[1]) {
                Ok(Value::Bool(s.ends_with(suf.as_str())))
            } else {
                Err(AError::runtime("str.ends_with: expected (string, string)", None))
            }
        }
        "str.replace" => {
            if let (Value::String(s), Value::String(from), Value::String(to)) = (&args[0], &args[1], &args[2]) {
                Ok(Value::string(s.replace(from.as_str(), to.as_str())))
            } else {
                Err(AError::runtime("str.replace: expected (string, string, string)", None))
            }
        }
        "str.trim" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::string(s.trim().to_string()))
            } else {
                Err(AError::runtime("str.trim: expected string", None))
            }
        }
        "str.upper" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::string(s.to_uppercase()))
            } else {
                Err(AError::runtime("str.upper: expected string", None))
            }
        }
        "str.lower" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::string(s.to_lowercase()))
            } else {
                Err(AError::runtime("str.lower: expected string", None))
            }
        }
        "str.join" => {
            if let (Value::Array(arr), Value::String(sep)) = (&args[0], &args[1]) {
                let parts: Vec<String> = arr.iter().map(val_to_display).collect();
                Ok(Value::string(parts.join(sep.as_str())))
            } else {
                Err(AError::runtime("str.join: expected (array, string)", None))
            }
        }
        "str.chars" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::array(s.chars().map(|c| Value::string(c.to_string())).collect()))
            } else {
                Err(AError::runtime("str.chars: expected string", None))
            }
        }
        "str.slice" => {
            if let (Value::String(s), Value::Int(start), Value::Int(end)) = (&args[0], &args[1], &args[2]) {
                let start = (*start).max(0) as usize;
                let end = (*end).min(s.len() as i64) as usize;
                let sliced: String = s.chars().skip(start).take(end.saturating_sub(start)).collect();
                Ok(Value::string(sliced))
            } else {
                Err(AError::runtime("str.slice: expected (string, int, int)", None))
            }
        }
        "str.lines" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::array(s.lines().map(|l| Value::string(l.to_string())).collect()))
            } else {
                Err(AError::runtime("str.lines: expected string", None))
            }
        }
        "str.find" => {
            if let (Value::String(s), Value::String(sub)) = (&args[0], &args[1]) {
                match s.find(sub.as_str()) {
                    Some(pos) => Ok(Value::Int(pos as i64)),
                    None => Ok(Value::Int(-1)),
                }
            } else {
                Err(AError::runtime("str.find: expected (string, string)", None))
            }
        }
        "str.count" => {
            if let (Value::String(s), Value::String(sub)) = (&args[0], &args[1]) {
                Ok(Value::Int(s.matches(sub.as_str()).count() as i64))
            } else {
                Err(AError::runtime("str.count: expected (string, string)", None))
            }
        }

        // --- Regex ---
        "regex.is_match" => {
            if let (Value::String(pat), Value::String(text)) = (&args[0], &args[1]) {
                match regex::Regex::new(pat) {
                    Ok(re) => Ok(Value::Bool(re.is_match(text))),
                    Err(e) => Err(AError::runtime(format!("regex.is_match: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.is_match: expected (string, string)", None))
            }
        }
        "regex.find" => {
            if let (Value::String(pat), Value::String(text)) = (&args[0], &args[1]) {
                match regex::Regex::new(pat) {
                    Ok(re) => {
                        match re.find(text) {
                            Some(m) => {
                                let mut map = HashMap::new();
                                map.insert("match".into(), Value::string(m.as_str().to_string()));
                                map.insert("start".into(), Value::Int(m.start() as i64));
                                map.insert("end".into(), Value::Int(m.end() as i64));
                                Ok(Value::Map(Arc::new(map)))
                            }
                            None => Ok(Value::Bool(false)),
                        }
                    }
                    Err(e) => Err(AError::runtime(format!("regex.find: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.find: expected (string, string)", None))
            }
        }
        "regex.find_all" => {
            if let (Value::String(pat), Value::String(text)) = (&args[0], &args[1]) {
                match regex::Regex::new(pat) {
                    Ok(re) => {
                        let matches: Vec<Value> = re.find_iter(text)
                            .map(|m| Value::string(m.as_str().to_string()))
                            .collect();
                        Ok(Value::array(matches))
                    }
                    Err(e) => Err(AError::runtime(format!("regex.find_all: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.find_all: expected (string, string)", None))
            }
        }
        "regex.replace" => {
            if let (Value::String(pat), Value::String(text), Value::String(rep)) = (&args[0], &args[1], &args[2]) {
                match regex::Regex::new(pat) {
                    Ok(re) => Ok(Value::string(re.replace(text, rep.as_str()).into_owned())),
                    Err(e) => Err(AError::runtime(format!("regex.replace: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.replace: expected (string, string, string)", None))
            }
        }
        "regex.replace_all" => {
            if let (Value::String(pat), Value::String(text), Value::String(rep)) = (&args[0], &args[1], &args[2]) {
                match regex::Regex::new(pat) {
                    Ok(re) => Ok(Value::string(re.replace_all(text, rep.as_str()).into_owned())),
                    Err(e) => Err(AError::runtime(format!("regex.replace_all: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.replace_all: expected (string, string, string)", None))
            }
        }
        "regex.split" => {
            if let (Value::String(pat), Value::String(text)) = (&args[0], &args[1]) {
                match regex::Regex::new(pat) {
                    Ok(re) => {
                        let parts: Vec<Value> = re.split(text)
                            .map(|s| Value::string(s.to_string()))
                            .collect();
                        Ok(Value::array(parts))
                    }
                    Err(e) => Err(AError::runtime(format!("regex.split: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.split: expected (string, string)", None))
            }
        }
        "regex.captures" => {
            if let (Value::String(pat), Value::String(text)) = (&args[0], &args[1]) {
                match regex::Regex::new(pat) {
                    Ok(re) => {
                        match re.captures(text) {
                            Some(caps) => {
                                let groups: Vec<Value> = caps.iter()
                                    .map(|m| match m {
                                        Some(m) => Value::string(m.as_str().to_string()),
                                        None => Value::Void,
                                    })
                                    .collect();
                                Ok(Value::array(groups))
                            }
                            None => Ok(Value::Bool(false)),
                        }
                    }
                    Err(e) => Err(AError::runtime(format!("regex.captures: {e}"), None)),
                }
            } else {
                Err(AError::runtime("regex.captures: expected (string, string)", None))
            }
        }

        // --- Shell execution ---
        "exec" => {
            if let Value::String(cmd) = &args[0] {
                match Command::new("sh").arg("-c").arg(cmd.as_str()).output() {
                    Ok(output) => {
                        let stdout = String::from_utf8_lossy(&output.stdout).to_string();
                        let stderr = String::from_utf8_lossy(&output.stderr).to_string();
                        let code = output.status.code().unwrap_or(-1) as i64;
                        Ok(Value::Record(vec![
                            ("stdout".into(), Value::string(stdout)),
                            ("stderr".into(), Value::string(stderr)),
                            ("code".into(), Value::Int(code)),
                        ]))
                    }
                    Err(e) => Ok(Value::Record(vec![
                        ("stdout".into(), Value::string(String::new())),
                        ("stderr".into(), Value::string(e.to_string())),
                        ("code".into(), Value::Int(-1)),
                    ])),
                }
            } else {
                Err(AError::runtime("exec: expected string command", None))
            }
        }

        // --- JSON ---
        "json.parse" => {
            if let Value::String(s) = &args[0] {
                match serde_json::from_str::<serde_json::Value>(s.as_str()) {
                    Ok(jval) => Ok(json_to_value(&jval)),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("json.parse: expected string", None))
            }
        }
        "json.stringify" => {
            let jval = value_to_json(&args[0]);
            match serde_json::to_string(&jval) {
                Ok(s) => Ok(Value::string(s)),
                Err(e) => Err(AError::runtime(format!("json.stringify failed: {e}"), None)),
            }
        }
        "json.pretty" => {
            let jval = value_to_json(&args[0]);
            match serde_json::to_string_pretty(&jval) {
                Ok(s) => Ok(Value::string(s)),
                Err(e) => Err(AError::runtime(format!("json.pretty failed: {e}"), None)),
            }
        }

        // --- Environment ---
        "env.get" => {
            if let Value::String(key) = &args[0] {
                match std::env::var(key.as_str()) {
                    Ok(val) => Ok(Value::string(val)),
                    Err(_) => Ok(Value::Void),
                }
            } else {
                Err(AError::runtime("env.get: expected string", None))
            }
        }
        "env.set" => {
            if let (Value::String(key), Value::String(val)) = (&args[0], &args[1]) {
                std::env::set_var(key.as_str(), val.as_str());
                Ok(Value::Void)
            } else {
                Err(AError::runtime("env.set: expected (string, string)", None))
            }
        }
        "env.all" => {
            let entries: HashMap<String, Value> = std::env::vars()
                .map(|(k, v)| (k, Value::string(v)))
                .collect();
            Ok(Value::map(entries))
        }

        // --- Type checking / conversion ---
        "type_of" => {
            let t = match &args[0] {
                Value::Int(_) => "int",
                Value::Float(_) => "float",
                Value::String(_) => "str",
                Value::Bool(_) => "bool",
                Value::Void => "void",
                Value::Array(_) => "array",
                Value::Record(_) => "record",
                Value::Map(_) => "map",
                Value::Result(_) => "result",
                Value::Variant(..) => "variant",
                Value::Closure { .. } | Value::VMClosure { .. } => "fn",
                Value::TaskHandle(_) => "task",
            };
            Ok(Value::string(t.to_string()))
        }
        "int" => {
            match &args[0] {
                Value::Int(i) => Ok(Value::Int(*i)),
                Value::Float(f) => Ok(Value::Int(*f as i64)),
                Value::String(s) => s.trim().parse::<i64>()
                    .map(Value::Int)
                    .map_err(|_| AError::runtime(format!("int: cannot parse '{}'", s.as_str()), None)),
                Value::Bool(b) => Ok(Value::Int(if *b { 1 } else { 0 })),
                _ => Err(AError::runtime("int: unsupported type", None)),
            }
        }
        "float" => {
            match &args[0] {
                Value::Float(f) => Ok(Value::Float(*f)),
                Value::Int(i) => Ok(Value::Float(*i as f64)),
                Value::String(s) => s.trim().parse::<f64>()
                    .map(Value::Float)
                    .map_err(|_| AError::runtime(format!("float: cannot parse '{}'", s.as_str()), None)),
                _ => Err(AError::runtime("float: unsupported type", None)),
            }
        }

        // --- HTTP ---
        "http.get" => {
            if let Value::String(url) = &args[0] {
                Ok(do_http_request("GET", url.as_str(), None, args.get(1)))
            } else {
                Err(AError::runtime("http.get: expected string url", None))
            }
        }
        "http.post" => {
            if let Value::String(url) = &args[0] {
                let body = args.get(1);
                let headers = args.get(2);
                Ok(do_http_request("POST", url.as_str(), body, headers))
            } else {
                Err(AError::runtime("http.post: expected string url", None))
            }
        }
        "http.put" => {
            if let Value::String(url) = &args[0] {
                let body = args.get(1);
                let headers = args.get(2);
                Ok(do_http_request("PUT", url.as_str(), body, headers))
            } else {
                Err(AError::runtime("http.put: expected string url", None))
            }
        }
        "http.patch" => {
            if let Value::String(url) = &args[0] {
                let body = args.get(1);
                let headers = args.get(2);
                Ok(do_http_request("PATCH", url.as_str(), body, headers))
            } else {
                Err(AError::runtime("http.patch: expected string url", None))
            }
        }
        "http.delete" => {
            if let Value::String(url) = &args[0] {
                Ok(do_http_request("DELETE", url.as_str(), None, args.get(1)))
            } else {
                Err(AError::runtime("http.delete: expected string url", None))
            }
        }

        // --- Filesystem ---
        "fs.exists" => {
            if let Value::String(p) = &args[0] {
                Ok(Value::Bool(std::path::Path::new(p.as_str()).exists()))
            } else {
                Err(AError::runtime("fs.exists: expected string path", None))
            }
        }
        "fs.is_dir" => {
            if let Value::String(p) = &args[0] {
                Ok(Value::Bool(std::path::Path::new(p.as_str()).is_dir()))
            } else {
                Err(AError::runtime("fs.is_dir: expected string path", None))
            }
        }
        "fs.is_file" => {
            if let Value::String(p) = &args[0] {
                Ok(Value::Bool(std::path::Path::new(p.as_str()).is_file()))
            } else {
                Err(AError::runtime("fs.is_file: expected string path", None))
            }
        }
        "fs.ls" => {
            if let Value::String(p) = &args[0] {
                match std::fs::read_dir(p.as_str()) {
                    Ok(entries) => {
                        let mut items = Vec::new();
                        for entry in entries.flatten() {
                            let name = entry.file_name().to_string_lossy().to_string();
                            let is_dir = entry.file_type().map(|t| t.is_dir()).unwrap_or(false);
                            items.push(Value::Record(vec![
                                ("name".into(), Value::string(name)),
                                ("is_dir".into(), Value::Bool(is_dir)),
                            ]));
                        }
                        Ok(Value::array(items))
                    }
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("fs.ls: expected string path", None))
            }
        }
        "fs.mkdir" => {
            if let Value::String(p) = &args[0] {
                match std::fs::create_dir_all(p.as_str()) {
                    Ok(()) => Ok(Value::Void),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("fs.mkdir: expected string path", None))
            }
        }
        "fs.rm" => {
            if let Value::String(p) = &args[0] {
                let path = std::path::Path::new(p.as_str());
                let result = if path.is_dir() {
                    std::fs::remove_dir_all(path)
                } else {
                    std::fs::remove_file(path)
                };
                match result {
                    Ok(()) => Ok(Value::Void),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("fs.rm: expected string path", None))
            }
        }
        "fs.mv" => {
            if let (Value::String(from), Value::String(to)) = (&args[0], &args[1]) {
                match std::fs::rename(from.as_str(), to.as_str()) {
                    Ok(()) => Ok(Value::Void),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("fs.mv: expected (string, string)", None))
            }
        }
        "fs.cp" => {
            if let (Value::String(from), Value::String(to)) = (&args[0], &args[1]) {
                match std::fs::copy(from.as_str(), to.as_str()) {
                    Ok(_) => Ok(Value::Void),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("fs.cp: expected (string, string)", None))
            }
        }
        "fs.cwd" => {
            match std::env::current_dir() {
                Ok(p) => Ok(Value::string(p.to_string_lossy().to_string())),
                Err(e) => Err(AError::runtime(format!("fs.cwd failed: {e}"), None)),
            }
        }
        "fs.abs" => {
            if let Value::String(p) = &args[0] {
                match std::fs::canonicalize(p.as_str()) {
                    Ok(abs) => Ok(Value::string(abs.to_string_lossy().to_string())),
                    Err(e) => Ok(Value::Result(Err(Box::new(Value::string(e.to_string()))))),
                }
            } else {
                Err(AError::runtime("fs.abs: expected string path", None))
            }
        }
        "fs.glob" => {
            if let Value::String(pattern) = &args[0] {
                let base = std::env::current_dir().unwrap_or_default();
                let mut matches = Vec::new();
                fn glob_walk(dir: &std::path::Path, pattern: &str, matches: &mut Vec<Value>) {
                    if let Ok(entries) = std::fs::read_dir(dir) {
                        for entry in entries.flatten() {
                            let path = entry.path();
                            let name = path.to_string_lossy().to_string();
                            if simple_glob_match(pattern, &name) {
                                matches.push(Value::string(name.clone()));
                            }
                            if path.is_dir() {
                                glob_walk(&path, pattern, matches);
                            }
                        }
                    }
                }
                glob_walk(&base, pattern.as_str(), &mut matches);
                Ok(Value::array(matches))
            } else {
                Err(AError::runtime("fs.glob: expected string pattern", None))
            }
        }

        // --- Result builtins ---
        "unwrap" => {
            match &args[0] {
                Value::Result(Ok(v)) => Ok(*v.clone()),
                Value::Result(Err(e)) => Err(AError::runtime(
                    format!("unwrap on Err: {}", val_to_display(e)), None)),
                other => Ok(other.clone()),
            }
        }
        "unwrap_or" => {
            match &args[0] {
                Value::Result(Ok(v)) => Ok(*v.clone()),
                Value::Result(Err(_)) => Ok(args[1].clone()),
                other => Ok(other.clone()),
            }
        }
        "is_ok" => {
            Ok(Value::Bool(matches!(&args[0], Value::Result(Ok(_)))))
        }
        "is_err" => {
            Ok(Value::Bool(matches!(&args[0], Value::Result(Err(_)))))
        }
        "expect" => {
            match &args[0] {
                Value::Result(Ok(v)) => Ok(*v.clone()),
                Value::Result(Err(e)) => {
                    let msg = if args.len() > 1 {
                        val_to_display(&args[1])
                    } else {
                        val_to_display(e)
                    };
                    Err(AError::runtime(msg, None))
                }
                other => Ok(other.clone()),
            }
        }

        "char_code" => {
            if let Value::String(s) = &args[0] {
                if let Some(c) = s.chars().next() {
                    Ok(Value::Int(c as i64))
                } else {
                    Err(AError::runtime("char_code: empty string", None))
                }
            } else {
                Err(AError::runtime("char_code: expected str", None))
            }
        }
        "from_code" => {
            if let Value::Int(n) = &args[0] {
                if let Some(c) = char::from_u32(*n as u32) {
                    Ok(Value::string(c.to_string()))
                } else {
                    Err(AError::runtime(format!("from_code: invalid code point {n}"), None))
                }
            } else {
                Err(AError::runtime("from_code: expected i64", None))
            }
        }
        "is_alpha" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::Bool(!s.is_empty() && s.chars().all(|c| c.is_ascii_alphabetic())))
            } else {
                Err(AError::runtime("is_alpha: expected str", None))
            }
        }
        "is_digit" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::Bool(!s.is_empty() && s.chars().all(|c| c.is_ascii_digit())))
            } else {
                Err(AError::runtime("is_digit: expected str", None))
            }
        }
        "is_alnum" => {
            if let Value::String(s) = &args[0] {
                Ok(Value::Bool(!s.is_empty() && s.chars().all(|c| c.is_ascii_alphanumeric() || c == '_')))
            } else {
                Err(AError::runtime("is_alnum: expected str", None))
            }
        }
        "push" => {
            if let Value::Array(arr) = &args[0] {
                let mut new_arr = (**arr).clone();
                new_arr.push(args[1].clone());
                Ok(Value::array(new_arr))
            } else {
                Err(AError::runtime("push: expected array", None))
            }
        }
        "slice" => {
            if let Value::Array(arr) = &args[0] {
                let start = if let Value::Int(n) = &args[1] { *n as usize } else { 0 };
                let end = if args.len() > 2 {
                    if let Value::Int(n) = &args[2] { *n as usize } else { arr.len() }
                } else { arr.len() };
                let start = start.min(arr.len());
                let end = end.min(arr.len());
                Ok(Value::array(arr[start..end].to_vec()))
            } else {
                Err(AError::runtime("slice: expected array", None))
            }
        }

        "sort" => {
            if let Value::Array(arr) = &args[0] {
                let mut sorted = (**arr).clone();
                sorted.sort_by(|a, b| {
                    match (a, b) {
                        (Value::Int(x), Value::Int(y)) => x.cmp(y),
                        (Value::Float(x), Value::Float(y)) => x.partial_cmp(y).unwrap_or(std::cmp::Ordering::Equal),
                        (Value::String(x), Value::String(y)) => x.as_str().cmp(y.as_str()),
                        _ => std::cmp::Ordering::Equal,
                    }
                });
                Ok(Value::array(sorted))
            } else {
                Err(AError::runtime("sort: expected array", None))
            }
        }
        "concat_arr" => {
            if let (Value::Array(a), Value::Array(b)) = (&args[0], &args[1]) {
                let mut v = (**a).clone();
                v.extend(b.iter().cloned());
                Ok(Value::array(v))
            } else {
                Err(AError::runtime("concat_arr: expected two arrays", None))
            }
        }
        "reverse_arr" => {
            if let Value::Array(arr) = &args[0] {
                let mut rev = (**arr).clone();
                rev.reverse();
                Ok(Value::array(rev))
            } else {
                Err(AError::runtime("reverse_arr: expected array", None))
            }
        }
        "contains" => {
            if let Value::Array(arr) = &args[0] {
                Ok(Value::Bool(arr.contains(&args[1])))
            } else {
                Err(AError::runtime("contains: expected array", None))
            }
        }

        "zip" => {
            if let (Value::Array(a), Value::Array(b)) = (&args[0], &args[1]) {
                let len = a.len().min(b.len());
                let result: Vec<Value> = (0..len)
                    .map(|i| Value::array(vec![a[i].clone(), b[i].clone()]))
                    .collect();
                Ok(Value::array(result))
            } else {
                Err(AError::runtime("zip: expected two arrays", None))
            }
        }
        "enumerate" => {
            if let Value::Array(a) = &args[0] {
                let result: Vec<Value> = a.iter().enumerate()
                    .map(|(i, v)| Value::array(vec![Value::Int(i as i64), v.clone()]))
                    .collect();
                Ok(Value::array(result))
            } else {
                Err(AError::runtime("enumerate: expected array", None))
            }
        }
        "take" => {
            if let Value::Array(a) = &args[0] {
                let n = if let Value::Int(n) = &args[1] { (*n).max(0) as usize } else { 0 };
                let n = n.min(a.len());
                Ok(Value::array(a[..n].to_vec()))
            } else {
                Err(AError::runtime("take: expected array", None))
            }
        }
        "drop" => {
            if let Value::Array(a) = &args[0] {
                let n = if let Value::Int(n) = &args[1] { (*n).max(0) as usize } else { 0 };
                let n = n.min(a.len());
                Ok(Value::array(a[n..].to_vec()))
            } else {
                Err(AError::runtime("drop: expected array", None))
            }
        }
        "chunk" => {
            if let Value::Array(a) = &args[0] {
                let size = if let Value::Int(n) = &args[1] { (*n).max(1) as usize } else { 1 };
                let result: Vec<Value> = a.chunks(size)
                    .map(|c| Value::array(c.to_vec()))
                    .collect();
                Ok(Value::array(result))
            } else {
                Err(AError::runtime("chunk: expected array", None))
            }
        }
        "unique" => {
            if let Value::Array(a) = &args[0] {
                let mut seen = Vec::new();
                let mut result = Vec::new();
                for elem in a.iter() {
                    if !seen.iter().any(|s| s == elem) {
                        seen.push(elem.clone());
                        result.push(elem.clone());
                    }
                }
                Ok(Value::array(result))
            } else {
                Err(AError::runtime("unique: expected array", None))
            }
        }

        "time.now" => {
            use std::time::{SystemTime, UNIX_EPOCH};
            let ms = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .map(|d| d.as_millis() as i64)
                .unwrap_or(0);
            Ok(Value::Int(ms))
        }
        "time.sleep" => {
            if let Value::Int(ms) = &args[0] {
                std::thread::sleep(std::time::Duration::from_millis(*ms as u64));
                Ok(Value::Void)
            } else {
                Err(AError::runtime("time.sleep: expected int milliseconds", None))
            }
        }
        "hash.sha256" => {
            if let Value::String(s) = &args[0] {
                use sha2::Digest;
                let mut hasher = sha2::Sha256::new();
                hasher.update(s.as_bytes());
                let result = hasher.finalize();
                let hex: String = result.iter().map(|b| format!("{:02x}", b)).collect();
                Ok(Value::string(hex))
            } else {
                Err(AError::runtime("hash.sha256: expected string", None))
            }
        }
        "hash.md5" => {
            if let Value::String(s) = &args[0] {
                use md5::Digest;
                let mut hasher = md5::Md5::new();
                hasher.update(s.as_bytes());
                let result = hasher.finalize();
                let hex: String = result.iter().map(|b| format!("{:02x}", b)).collect();
                Ok(Value::string(hex))
            } else {
                Err(AError::runtime("hash.md5: expected string", None))
            }
        }

        "__bridge_convert__" => {
            match crate::bridge::value_to_program(&args[0]) {
                Ok(program) => {
                    let n = program.functions.len();
                    let has_main = program.main_idx.is_some();
                    let mut map = std::collections::HashMap::new();
                    map.insert("ok".into(), Value::Bool(true));
                    map.insert("functions".into(), Value::Int(n as i64));
                    map.insert("has_main".into(), Value::Bool(has_main));
                    Ok(Value::Map(std::sync::Arc::new(map)))
                }
                Err(e) => Err(AError::runtime(format!("bridge_convert: {e}"), None)),
            }
        }
        "__bridge_exec__" => {
            match crate::bridge::value_to_program(&args[0]) {
                Ok(program) => {
                    let mut vm = crate::vm::VM::new(program);
                    match vm.run() {
                        Ok(v) => Ok(v),
                        Err(e) => Err(AError::runtime(format!("bridge_exec: {}", e.message), None)),
                    }
                }
                Err(e) => Err(AError::runtime(format!("bridge_exec: {e}"), None)),
            }
        }

        _ => Err(AError::runtime(format!("unknown builtin: {name}"), None)),
    }
}

pub fn call_builtin_no_interp(name: &str, args: &[Value]) -> AResult<Value> {
    let mut dummy_interp = crate::interpreter::Interpreter::new();
    call_builtin(name, args, &mut dummy_interp)
}

fn do_http_request(method: &str, url: &str, body: Option<&Value>, headers: Option<&Value>) -> Value {
    let build_result = || -> Result<Value, String> {
        let mut req = match method {
            "GET" => ureq::get(url),
            "POST" => ureq::post(url),
            "PUT" => ureq::put(url),
            "PATCH" => ureq::patch(url),
            "DELETE" => ureq::delete(url),
            _ => return Err(format!("unsupported method: {method}")),
        };

        if let Some(Value::Map(entries)) = headers {
            for (k, v) in entries.iter() {
                if let Value::String(hv) = v {
                    req = req.set(k.as_str(), hv.as_str());
                }
            }
        }

        let response = match body {
            Some(Value::String(s)) => {
                if !has_header(headers, "content-type") {
                    req = req.set("Content-Type", "application/json");
                }
                req.send_string(s.as_str())
            }
            Some(val) => {
                let json = value_to_json(val);
                req = req.set("Content-Type", "application/json");
                req.send_string(&json.to_string())
            }
            None => req.call(),
        };

        match response {
            Ok(resp) => Ok(response_to_value(resp)),
            Err(ureq::Error::Status(code, resp)) => {
                let mut rec = response_to_value(resp);
                if let Value::Record(ref mut fields) = rec {
                    for field in fields.iter_mut() {
                        if field.0 == "status" {
                            field.1 = Value::Int(code as i64);
                        }
                    }
                }
                Ok(rec)
            }
            Err(ureq::Error::Transport(t)) => {
                Ok(Value::Record(vec![
                    ("status".into(), Value::Int(0)),
                    ("body".into(), Value::string(t.to_string())),
                    ("headers".into(), Value::map(HashMap::new())),
                ]))
            }
        }
    };

    match build_result() {
        Ok(v) => v,
        Err(e) => Value::Record(vec![
            ("status".into(), Value::Int(0)),
            ("body".into(), Value::string(e)),
            ("headers".into(), Value::map(HashMap::new())),
        ]),
    }
}

fn response_to_value(resp: ureq::Response) -> Value {
    let status = resp.status() as i64;
    let mut hdrs: HashMap<String, Value> = HashMap::new();
    for name in resp.headers_names() {
        if let Some(val) = resp.header(&name) {
            hdrs.insert(name, Value::string(val.to_string()));
        }
    }
    let body = resp.into_string().unwrap_or_default();
    Value::Record(vec![
        ("status".into(), Value::Int(status)),
        ("body".into(), Value::string(body)),
        ("headers".into(), Value::map(hdrs)),
    ])
}

fn has_header(headers: Option<&Value>, name: &str) -> bool {
    if let Some(Value::Map(entries)) = headers {
        entries.keys().any(|k| k.eq_ignore_ascii_case(name))
    } else {
        false
    }
}

fn simple_glob_match(pattern: &str, path: &str) -> bool {
    if pattern == "*" { return true; }
    if let Some(ext) = pattern.strip_prefix("*.") {
        return path.ends_with(&format!(".{ext}"));
    }
    if let Some(prefix) = pattern.strip_suffix("/*") {
        return path.starts_with(prefix);
    }
    path.contains(pattern)
}

pub fn json_to_value(j: &serde_json::Value) -> Value {
    match j {
        serde_json::Value::Null => Value::Void,
        serde_json::Value::Bool(b) => Value::Bool(*b),
        serde_json::Value::Number(n) => {
            if let Some(i) = n.as_i64() { Value::Int(i) }
            else if let Some(f) = n.as_f64() { Value::Float(f) }
            else { Value::Void }
        }
        serde_json::Value::String(s) => Value::string(s.clone()),
        serde_json::Value::Array(arr) => {
            Value::array(arr.iter().map(json_to_value).collect())
        }
        serde_json::Value::Object(obj) => {
            Value::map(obj.iter().map(|(k, v)| (k.clone(), json_to_value(v))).collect())
        }
    }
}

pub fn value_to_json(v: &Value) -> serde_json::Value {
    match v {
        Value::Void => serde_json::Value::Null,
        Value::Bool(b) => serde_json::Value::Bool(*b),
        Value::Int(i) => serde_json::Value::Number((*i).into()),
        Value::Float(f) => serde_json::json!(*f),
        Value::String(s) => serde_json::Value::String((**s).clone()),
        Value::Array(arr) => {
            serde_json::Value::Array(arr.iter().map(value_to_json).collect())
        }
        Value::Map(entries) => {
            let obj: serde_json::Map<String, serde_json::Value> = entries.iter()
                .map(|(k, v)| (k.clone(), value_to_json(v)))
                .collect();
            serde_json::Value::Object(obj)
        }
        Value::Record(fields) => {
            let obj: serde_json::Map<String, serde_json::Value> = fields.iter()
                .map(|(k, v)| (k.clone(), value_to_json(v)))
                .collect();
            serde_json::Value::Object(obj)
        }
        Value::Result(Ok(v)) => value_to_json(v),
        Value::Result(Err(v)) => serde_json::json!({"error": value_to_json(v)}),
        Value::Variant(name, vals) => {
            if vals.is_empty() { serde_json::Value::String(name.clone()) }
            else { serde_json::json!({name: vals.iter().map(value_to_json).collect::<Vec<_>>()}) }
        }
        Value::Closure { .. } | Value::VMClosure { .. } | Value::TaskHandle(_) => serde_json::Value::Null,
    }
}

pub fn is_builtin(name: &str) -> bool {
    matches!(name,
        "print" | "println" | "eprintln" | "to_str" | "len" | "sqrt" | "abs" | "floor" | "ceil" | "round" |
        "map" | "filter" | "reduce" | "each" |
        "sort_by" | "find" | "any" | "all" | "flat_map" | "min_by" | "max_by" |
        "zip" | "enumerate" | "take" | "drop" | "chunk" | "unique" |
        "map.get" | "map.set" | "map.keys" | "map.values" | "map.has" |
        "map.delete" | "map.merge" | "map.entries" | "map.from_entries" |
        "io.write" | "io.read_file" | "io.write_file" | "io.read_stdin" | "io.read_line" | "io.read_bytes" | "io.flush" |
        "str.concat" | "str.split" | "str.contains" | "str.starts_with" | "str.ends_with" |
        "str.replace" | "str.trim" | "str.upper" | "str.lower" |
        "str.join" | "str.chars" | "str.slice" | "str.lines" | "str.find" | "str.count" |
        "exec" |
        "json.parse" | "json.stringify" | "json.pretty" |
        "env.get" | "env.set" | "env.all" |
        "type_of" | "int" | "float" |
        "http.get" | "http.post" | "http.put" | "http.patch" | "http.delete" |
        "fs.exists" | "fs.is_dir" | "fs.is_file" | "fs.ls" | "fs.mkdir" |
        "fs.rm" | "fs.mv" | "fs.cp" | "fs.cwd" | "fs.abs" | "fs.glob" |
        "unwrap" | "unwrap_or" | "is_ok" | "is_err" | "expect" |
        "char_code" | "from_code" | "is_alpha" | "is_digit" | "is_alnum" | "push" | "slice" |
        "sort" | "reverse_arr" | "contains" | "concat_arr" |
        "__bridge_exec__" | "__bridge_convert__" | "eval" |
        "regex.is_match" | "regex.find" | "regex.find_all" |
        "regex.replace" | "regex.replace_all" | "regex.split" | "regex.captures" |
        "spawn" | "await" | "await_all" | "parallel_map" | "parallel_each" | "timeout" |
        "time.now" | "time.sleep" | "hash.sha256" | "hash.md5"
    )
}

fn val_to_display(val: &Value) -> String {
    match val {
        Value::Int(i) => i.to_string(),
        Value::Float(f) => format!("{f}"),
        Value::String(s) => (**s).clone(),
        Value::Bool(b) => b.to_string(),
        Value::Void => "void".to_string(),
        Value::Array(elems) => {
            let inner: Vec<String> = elems.iter().map(val_to_display).collect();
            format!("[{}]", inner.join(", "))
        }
        Value::Record(fields) => {
            let inner: Vec<String> = fields.iter()
                .map(|(k, v)| format!("{k}: {}", val_to_display(v)))
                .collect();
            format!("{{{}}}", inner.join(", "))
        }
        Value::Map(entries) => {
            let inner: Vec<String> = entries.iter()
                .map(|(k, v)| format!("\"{}\": {}", k, val_to_display(v)))
                .collect();
            format!("#{{{}}}", inner.join(", "))
        }
        Value::Result(Ok(v)) => format!("Ok({})", val_to_display(v)),
        Value::Result(Err(v)) => format!("Err({})", val_to_display(v)),
        Value::Variant(name, vals) => {
            if vals.is_empty() {
                name.clone()
            } else {
                let inner: Vec<String> = vals.iter().map(val_to_display).collect();
                format!("{name}({})", inner.join(", "))
            }
        }
        Value::Closure { .. } | Value::VMClosure { .. } => "<closure>".to_string(),
        Value::TaskHandle(id) => format!("<task:{id}>"),
    }
}


fn val_less_than(a: &Value, b: &Value) -> bool {
    match (a, b) {
        (Value::Int(x), Value::Int(y)) => x < y,
        (Value::Float(x), Value::Float(y)) => x < y,
        (Value::Int(x), Value::Float(y)) => (*x as f64) < *y,
        (Value::Float(x), Value::Int(y)) => *x < (*y as f64),
        (Value::String(x), Value::String(y)) => x.as_str() < y.as_str(),
        _ => false,
    }
}

fn is_truthy(val: &Value) -> bool {
    match val {
        Value::Bool(b) => *b,
        Value::Int(i) => *i != 0,
        Value::Void => false,
        _ => true,
    }
}

fn val_to_key(val: &Value) -> String {
    match val {
        Value::String(s) => s.as_ref().clone(),
        Value::Int(i) => i.to_string(),
        Value::Bool(b) => b.to_string(),
        Value::Float(f) => format!("{f}"),
        other => val_to_display(other),
    }
}
