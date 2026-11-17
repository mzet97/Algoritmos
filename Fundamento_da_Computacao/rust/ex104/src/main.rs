use bib1::Leitor;

fn contagem(x: i64, y: i64) -> i64 {
    let s = x.abs().to_string();
    let target = (y.abs() % 10) as u8 + b'0';
    let mut count: i64 = 0;
    for b in s.bytes() {
        if b == target {
            count += 1;
        }
    }
    count
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let y: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", contagem(x, y));
    }
}
