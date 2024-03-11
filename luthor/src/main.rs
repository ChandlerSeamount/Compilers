use std::env;
use std::fs;

fn main() {
    let args:Vec<String> = env::args().collect();
    // dbg!(args);
    let scan_def_path = &args[1];
    let out_path = &args[2];
    // let mut file_in = fs::File::open(scan_def_path);
    // let mut contents = vec![];
    // file_in.read_to_end(contents);
    let contents = fs::read_to_string(scan_def_path).expect("Things");
    println!("{}", contents);
    // match contents {
    //     Ok(s) => s,
    //     Err(s) => panic!("{}", s)
    // }
}
