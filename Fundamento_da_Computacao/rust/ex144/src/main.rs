use bib1::Leitor;

fn pi(n: i64) -> f64 {
    let mut s = 0.0_f64;
    for i in 0..n {
        let denom = (2 * i + 1) as f64;
        let term = 1.0 / (denom * denom * denom);
        if i % 2 == 0 {
            s += term;
        } else {
            s -= term;
        }
    }
    (32.0 * s).cbrt()
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        println!("{:.10}", pi(n));
    }
}
