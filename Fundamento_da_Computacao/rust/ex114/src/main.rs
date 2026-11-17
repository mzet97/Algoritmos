use bib1::Leitor;

fn raiz(a: i64, b: i64) -> Option<f64> {
    if a == 0 {
        return None;
    }
    Some(-b as f64 / a as f64)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let a: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let b: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        match raiz(a, b) {
            Some(r) => {
                if r == r.floor() && r.is_finite() {
                    println!("{:.1}", r);
                } else {
                    println!("{}", r);
                }
            }
            None => println!("None"),
        }
    }
}
