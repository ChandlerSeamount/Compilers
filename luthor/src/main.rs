use std::env;
use std::fs;
use std::process;
use std::io::{BufReader, BufRead};
use std::collections::HashMap;
use std::io::Write;

#[derive(Clone)]
struct TokenId {
    table: Vec<Vec<String>>,
    name: String,
    alt: String,
    state: usize,
}

struct Token {
    val: String,
    id: TokenId,
    st_ln: i32,
    st_idx: i32,
    idx: usize,
    end_ln: i32,
    end_idx: i32,
}

fn make_alph(s:String) -> HashMap<char, usize> {
    let mut v = HashMap::new();
    let mut count = 0;
    let mut other = 0;
    while s.as_bytes()[count] as char != '\n' {
        
        if s.as_bytes()[count] as char == 'x' {
            let temp = &s[count + 1..count + 3];
            v.insert(u8::from_str_radix(temp, 16).map(|n| n as char).expect("Bad value"), other);
            count += 3;
            other += 1;
        } else if s.as_bytes()[count] as char == ' ' {
            count += 1;
        } else {
            v.insert(s.as_bytes()[count] as char, other);
            count += 1;
            other += 1;
        }
        
    }
    v
}

fn make_token(s:String) -> TokenId {
    let mut v:Vec<Vec<String>> = vec![];
    let mut t:String = "".to_string();
    let mut a:String = "".to_string();
    for (i, byte) in s.split_whitespace().enumerate() {
        if i == 0 {
            let file_in = match fs::File::open(byte) {
                Ok(file) => file,
                Err(_) => process::exit(1),
            };
            for line in BufReader::new(file_in).lines() {
                let mut vc:Vec<String> = vec![];
                for b in line.expect("Bad table").split_whitespace() {
                    vc.push(b.to_string());
                }
                v.push(vc);
            }
        } else if i == 1 {
            t = byte.to_string();
        } else {
            a = byte.to_string();
        }
    }
    TokenId {
        table: v,
        name: t,
        alt: a,
        state: 0,
    }
}

fn main() {
    let args:Vec<String> = env::args().collect();
    // dbg!(args);
    let scan_def_path = &args[1];
    // let out_path = &args[2];
    let file_in = match fs::File::open(scan_def_path) {
        Ok(file) => file,
        Err(_) => process::exit(1),
    };
    let mut reader = BufReader::new(file_in);
    let mut contents = String::new();
    if reader.read_line(&mut contents).expect("stuff") == 0 {process::exit(1)};
    
    let alph = make_alph(contents);
    // println!("{}", contents);
    // println!("{:?}", make_alph(contents));
    let mut tables: Vec<TokenId> = vec![];
    for line in reader.lines() {
        let lin = 
        match line {
            Ok(l) => l,
            Err(_) => process::exit(1),
        };
        // println!("{}", lin);
        tables.push(make_token(lin));
    }
    for (key, value) in &alph {
        println!("{} / {}", key, value);
    }
    tables.reverse();
    let scource_string = fs::read_to_string(&args[2]).expect("Failed to read file");
    let mut tokens:Vec<Token> = vec![];
    let mut start_line = 0;
    let mut start_idx = 0;
    let mut cur_line = 0;
    let mut cur_idx = 0;
    let mut i = 0;
    let mut val_sub = "".to_string();
    let mut temp_tab = tables.to_vec();
    let mut out_file = fs::OpenOptions::new().append(true).create(true).open(&args[3]).expect("cannot open file");
    while i < scource_string.len() {
        let next = scource_string.chars().nth(i).unwrap();
        let mut next_tab:Vec<TokenId> = vec![];
        val_sub.push(next);
        
        for mut v in temp_tab {
            println!("{} / {}",v.state,next);
            println!("{:?}", v.table);
            if v.table[v.state][alph.get(&next).unwrap() + 2] != "E".to_string() {
                v.state = v.table[v.state][alph.get(&next).unwrap() + 2].parse::<usize>().unwrap();
                
                if v.table[v.state][0] == "+".to_string() {
                    tokens.push(
                        Token {
                            val: val_sub.clone(),
                            id: v.clone(),
                            st_ln: start_line,
                            st_idx: start_idx,
                            idx: i,
                            end_ln: cur_line,
                            end_idx: cur_idx,
                        }
                    )
                }
                next_tab.push(v);
            }
        }
        
        temp_tab = next_tab.to_vec();
        next_tab.clear();
        i += 1;
        if temp_tab.len() == 0 {
            let Some(tok) = tokens.pop() else {panic!("Bad token")};
            let mut new_line = "".to_string();
            new_line.push_str(&tok.id.name);
            new_line.push(' ');
            if tok.id.alt == "".to_string() {
                new_line.push_str(&tok.val);
            } else {
                new_line.push_str(&tok.id.alt);
            }
            new_line.push(' ');
            new_line.push_str(&tok.st_ln.to_string());
            new_line.push(' ');
            new_line.push_str(&tok.st_idx.to_string());
            new_line.push('\n');
            out_file.write(new_line.as_bytes()).expect("Write failed");
            start_line = tok.end_ln;
            start_idx = tok.end_idx;
            i = tok.idx;
            val_sub = "".to_string();
        }
        cur_idx += 1;
        if next == '\n' {
            cur_line += 1;
            cur_idx = 0;
        }
        
    }
}
