use bib1::Leitor;

fn calc_dist(v: f64, a: f64, t: f64) -> f64 {
    v * t + 0.5 * a * t * t
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let v: f64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let a: f64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let t: f64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let result = calc_dist(v, a, t);
        // Python prints float: if it's a whole number, prints like "99.0"
        if result == result.floor() && result.is_finite() {
            println!("{:.1}", result);
        } else {
            println!("{}", result);
        }
    }
}
