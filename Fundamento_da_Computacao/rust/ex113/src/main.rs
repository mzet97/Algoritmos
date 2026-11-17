use bib1::Leitor;

fn conta_num(lista: &[i64]) -> (i64, i64) {
    let mut qp: i64 = 0;
    let mut qn: i64 = 0;
    for &x in lista {
        if x > 0 { qp += 1; }
        else if x < 0 { qn += 1; }
    }
    (qp, qn)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let mut lista: Vec<i64> = Vec::with_capacity(n);
        let mut ok = true;
        for _ in 0..n {
            match leitor.entrada() {
                Some(s) => match s.parse::<i64>() {
                    Ok(v) => lista.push(v),
                    Err(_) => { ok = false; break; }
                },
                None => { ok = false; break; }
            }
        }
        if !ok { break; }
        let (qp, qn) = conta_num(&lista);
        println!("({}, {})", qp, qn);
    }
}
