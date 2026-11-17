use bib1::Leitor;

fn pg(a: i64, q: i64, n: usize) -> Vec<i64> {
    let mut resultado: Vec<i64> = Vec::with_capacity(n);
    let mut val = a;
    for _ in 0..n {
        resultado.push(val);
        val *= q;
    }
    resultado
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let a: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let q: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let n: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{:?}", pg(a, q, n));
    }
}
