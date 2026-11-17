use bib1::Leitor;

fn troco(m: &[i64], q: &[i64]) -> f64 {
    let total: i64 = m.iter().zip(q.iter()).map(|(&mi, &qi)| mi * qi).sum();
    total as f64 / 100.0
}

fn print_float(val: f64) {
    if val == val.floor() && val.is_finite() {
        println!("{}", val as i64);
    } else {
        println!("{}", val);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let mut m: Vec<i64> = Vec::with_capacity(n);
        let mut ok = true;
        for _ in 0..n {
            match leitor.entrada() {
                Some(s) => match s.parse::<i64>() {
                    Ok(v) => m.push(v),
                    Err(_) => { ok = false; break; }
                },
                None => { ok = false; break; }
            }
        }
        if !ok { break; }
        let mut q: Vec<i64> = Vec::with_capacity(n);
        for _ in 0..n {
            match leitor.entrada() {
                Some(s) => match s.parse::<i64>() {
                    Ok(v) => q.push(v),
                    Err(_) => { ok = false; break; }
                },
                None => { ok = false; break; }
            }
        }
        if !ok { break; }
        let result = troco(&m, &q);
        print_float(result);
    }
}
