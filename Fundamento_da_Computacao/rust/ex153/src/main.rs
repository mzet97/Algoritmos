use bib1::Leitor;

fn raiz(n: i64, inf: i64, sup: i64) -> i64 {
    let mut inf = inf;
    let mut sup = sup;
    while sup - inf > 1 {
        let mid = (inf + sup) / 2;
        if mid * mid <= n {
            inf = mid;
        } else {
            sup = mid;
        }
    }
    inf
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", raiz(n, 0, n + 1));
    }
}
