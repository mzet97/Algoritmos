use bib1::Leitor;

fn imposto(l: &[i64]) -> f64 {
    let mut total: f64 = 0.0;
    for &s in l {
        if s <= 3000 {
            continue;
        } else if s <= 8000 {
            total += (s - 3000) as f64 * 0.15;
        } else {
            total += 5000.0 * 0.15 + (s - 8000) as f64 * 0.27;
        }
    }
    total
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let mut l: Vec<i64> = Vec::with_capacity(n);
        let mut ok = true;
        for _ in 0..n {
            match leitor.entrada() {
                Some(s) => match s.parse::<i64>() {
                    Ok(v) => l.push(v),
                    Err(_) => { ok = false; break; }
                },
                None => { ok = false; break; }
            }
        }
        if !ok { break; }
        println!("{}", imposto(&l));
    }
}
