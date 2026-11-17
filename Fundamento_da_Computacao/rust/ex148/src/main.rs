use bib1::Leitor;

fn fatorial(n: i64) -> i64 {
    let mut r: i64 = 1;
    for i in 2..=n {
        r *= i;
    }
    r
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", fatorial(n));
    }
}
