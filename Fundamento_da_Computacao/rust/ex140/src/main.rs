use bib1::Leitor;

fn perfeito(n: i64) -> bool {
    if n < 2 { return false; }
    let mut soma: i64 = 1;
    let mut i: i64 = 2;
    while i * i <= n {
        if n % i == 0 {
            soma += i;
            if i != n / i {
                soma += n / i;
            }
        }
        i += 1;
    }
    soma == n
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", perfeito(n));
    }
}
