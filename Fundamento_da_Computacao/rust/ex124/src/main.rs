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

fn divide(p1: i64, q1: i64, p2: i64, q2: i64) -> (i64, i64) {
    let p = p1 * q2;
    let q = q1 * p2;
    reduz(p, q)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let p1: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let q1: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let p2: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let q2: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let (rp, rq) = divide(p1, q1, p2, q2);
        println!("({}, {})", rp, rq);
    }
}
