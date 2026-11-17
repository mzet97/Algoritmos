use bib1::Leitor;

fn cientifico(n: f64) -> String {
    if n == 0.0 { return "0.0x10^0".to_string(); }
    let sinal = if n < 0.0 { "-" } else { "" };
    let mut val = n.abs();
    let mut exp: i32 = 0;
    while val >= 10.0 { val /= 10.0; exp += 1; }
    while val < 1.0 { val *= 10.0; exp -= 1; }
    format!("{}{}x10^{}", sinal, val as f64, exp)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", cientifico(n));
    }
}
