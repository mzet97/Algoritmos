use bib1::Leitor;

fn fibonacci(n: i64) -> i64 {
    if n <= 0 { return 0; }
    if n <= 2 { return 1; }
    let mut a: i64 = 1;
    let mut b: i64 = 1;
    for _ in 2..n {
        let temp = a + b;
        a = b;
        b = temp;
    }
    b
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", fibonacci(n));
    }
}
