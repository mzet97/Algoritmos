use bib1::Leitor;

fn acima_media(l: &[i64]) -> i64 {
    if l.is_empty() { return 0; }
    let soma: i64 = l.iter().sum();
    let media = soma as f64 / l.len() as f64;
    l.iter().filter(|&&x| (x as f64) > media).count() as i64
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        if n_str == "@entradagrande" {
            let l: Vec<i64> = (0..1000000).collect();
            println!("{}", acima_media(&l));
        } else {
            let n: usize = match n_str.parse() { Ok(v) => v, Err(_) => break };
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
            println!("{}", acima_media(&l));
        }
    }
}
