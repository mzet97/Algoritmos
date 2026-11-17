use bib1::Leitor;

fn gcd(a: i64, b: i64) -> i64 {
    let mut a = a.abs();
    let mut b = b.abs();
    while b != 0 {
        let t = b;
        b = a % b;
        a = t;
    }
    a
}

fn reduz(p: i64, q: i64) -> (i64, i64) {
    let d = gcd(p, q);
    (p / d, q / d)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let p: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let q: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let (rp, rq) = reduz(p, q);
        println!("({}, {})", rp, rq);
    }
}
