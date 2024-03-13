use std::env;
use std::fs;
use std::process;
use std::io::{BufReader, BufRead};
use std::collections::HashMap;
use std::io::Write;

#[derive(Clone, Debug)]
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

fn char_hex_map(s:String) -> HashMap<char, String> {
    let mut v = HashMap::new();
    let mut count = 0;
    while s.as_bytes()[count] as char != '\n' {
        
        if s.as_bytes()[count] as char == 'x' {
            let temp = &s[count + 1..count + 3];
            v.insert(u8::from_str_radix(temp, 16).map(|n| n as char).expect("Bad value"), temp.to_string());
            count += 3;
        } else if s.as_bytes()[count] as char == ' ' {
            count += 1;
        } else {
            count += 1;
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

fn copy_2d_vec(table:Vec<Vec<String>>) -> Vec<Vec<String>> {
    let mut nvec:Vec<Vec<String>> = vec![];
    for a in table {
        let mut mvec:Vec<String> = vec![];
        for b in a {
            mvec.push(b);
        }
        nvec.push(mvec);
    }
    nvec
}

fn copy_vec_token_id(tokens:Vec<TokenId>) -> Vec<TokenId>{
    let mut nvec:Vec<TokenId> = vec![];
    for a in tokens {
        nvec.push(
            TokenId {
                table: copy_2d_vec(a.table.clone()),
                name: a.name.clone(),
                alt: a.alt.clone(),
                state: a.state,
            }
        );
    }
    nvec
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
    
    let alph = make_alph(contents.clone());
    let char_hex_map = char_hex_map(contents.clone());
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
        if lin.len() > 0 {
            tables.push(make_token(lin));
        }
        
    }
    // for (key, value) in &char_hex_map {
    //     println!("{} / {}", key, value);
    // }
    tables.reverse();
    let scource_string = fs::read_to_string(&args[2]).expect("Failed to read file");
    let mut tokens:Vec<Token> = vec![];
    let mut start_line = 1;
    let mut start_idx = 1;
    let mut cur_line = 1;
    let mut cur_idx = 1;
    let mut i = 0;
    let mut val_sub = "".to_string();
    let mut temp_tab = copy_vec_token_id(tables.clone());
    let mut out_file = fs::OpenOptions::new().write(true).create(true).truncate(true).open(&args[3]).expect("cannot open file");
    // println!("{temp_tab:?}");
    while i < scource_string.len() {
        let next = scource_string.chars().nth(i).unwrap();
        let mut next_tab:Vec<TokenId> = vec![];
        val_sub.push(next);
        
        for mut v in temp_tab {
            // println!("{} / {}",i,next);
            // println!("{:?}", v.table);
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
                            end_idx: cur_idx+1,
                        }
                    )
                }
                next_tab.push(v.clone());
            }
        }
        
        temp_tab = copy_vec_token_id(next_tab.clone());
        // next_tab.clear();
        i += 1;
        if next_tab.len() == 0 || i >= scource_string.len() {
            // println!("{}", i);
            let Some(tok) = tokens.pop() else {panic!("Bad token")};
            let mut new_line = "".to_string();
            new_line.push_str(&tok.id.name);
            new_line.push(' ');
            if tok.id.alt == "".to_string() {
                for chs in tok.val.chars() {
                    if (chs < '9' && chs > '0') || 
                    (chs <= 'Z' && chs >= 'A') || (chs <= 'w' && chs >= 'a') ||
                    (chs <= 'z' && chs >= 'y') {
                        new_line.push(chs);
                    } else {
                        new_line.push('x');
                        new_line.push_str(char_hex_map.get(&chs).unwrap());
                    }
                }
                // new_line.push_str(&tok.val);
            } else {
                new_line.push_str(&tok.id.alt);
            }
            new_line.push(' ');
            new_line.push_str(&(tok.st_ln).to_string());
            new_line.push(' ');
            new_line.push_str(&tok.st_idx.to_string());
            new_line.push('\n');
            out_file.write(new_line.as_bytes()).expect("Write failed");
            start_line = tok.end_ln;
            start_idx = tok.end_idx;
            // if scource_string.chars().nth(tok.idx).unwrap() == '\n' {
            //     start_line += 1;
            //     start_idx = 1;
            // }
            cur_idx = start_idx-1;
            cur_line = start_line;
            i = tok.idx+1;
            val_sub = "".to_string();
            // temp_tab.clear();
            for v in &mut tables {
                v.state = 0;
            }
            
            temp_tab = copy_vec_token_id(tables.clone());
            // println!("{temp_tab:?}");
            
            
        } 
        cur_idx += 1;
        if i < scource_string.len() && scource_string.chars().nth(i).unwrap() == '\n' {
            cur_line += 1;
            cur_idx = 0;
        }
        
    }
}
