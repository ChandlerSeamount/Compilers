use std::env;
use std::fs;
use std::process;
use std::io::{BufReader, BufRead};

#[derive(Clone)]
struct TokenId {
    table: Vec<Vec<String>>,
    name: String,
    alt: String,
    state: i32,
}

struct Token {
    val: String,
    id: TokenId,
    st_ln: i32,
    st_idx: i32,
    idx: i32,
    end_ln: i32,
    end_idx: i32,
}

fn make_alph(s:String) -> Vec<char> {
    let mut v:Vec<char> = vec![];
    let mut count = 0;
    while s.as_bytes()[count] as char != '\n' {
        
        if s.as_bytes()[count] as char == 'x' {
            let temp = &s[count + 1..count + 3];
            v.push(u8::from_str_radix(temp, 16).map(|n| n as char).expect("Bad value"));
            count += 3;
        } else if s.as_bytes()[count] as char == ' ' {
            count += 1;
        } else {
            v.push(s.as_bytes()[count] as char);
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
        println!("{}", lin);
        tables.push(make_token(lin));
    }
    tables.reverse();
    let scource_string = fs::read_to_string(&args[2])?;
    let mut tokens:Vec<Token> = vec![];
    let mut start_line = 0;
    let mut start_idx = 0;
    let mut cur_line = 0;
    let mut cur_idx = 0;
    let mut i = 0;
    let mut val_sub = "".to_string();
    let mut temp_tab = tables.to_vec();
    let mut data_file = fs::OpenOptions::new().append(true).open(&args[3]).expect("cannot open file");
    while i < scource_string.len() {
        let next = scource_string.chars().nth(i).unwrap();
        let mut next_tab:Vec<TokenId> = vec![];
        val_sub.push(next);
        
        for v in temp_tab {
            
            if v.table[v.state][alph.find(next.to_string()) + 2] != "E".to_string() {
                v.state = v.table[v.state][alph.find(next.to_string()) + 2].parse::<i32>().unwrap();
                next_tab.push(v);
                if v.table[v.state][0] == "+".to_string() {
                    tokens.push(
                        Token {
                            val: val_sub,
                            id: v,
                            st_ln: start_line,
                            st_idx: start_idx,
                            idx: i,
                            end_ln: cur_line,
                            end_idx: cur_idx,
                        }
                    )
                }
            }
        }
        
        temp_tab = next_tab.to_vec();
        next_tab.clear();
        if temp_tab.len() == 0 {
            let Some(tok) = tokens.pop();
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
        }
        
    }
}
