use bib1::Leitor;

fn notas(m: &[i64], n: &[f64]) -> (Vec<i64>, Vec<i64>) {
    let max_n = n.iter().cloned().fold(f64::NEG_INFINITY, f64::max);
    let min_n = n.iter().cloned().fold(f64::INFINITY, f64::min);
    let maiores: Vec<i64> = m.iter().zip(n.iter()).filter(|(_, &nota)| nota == max_n).map(|(&mat, _)| mat).collect();
    let menores: Vec<i64> = m.iter().zip(n.iter()).filter(|(_, &nota)| nota == min_n).map(|(&mat, _)| mat).collect();
    (maiores, menores)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n_val: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let (m, n) = if n_val >= 0 {
            let n = n_val as usize;
            let mut m: Vec<i64> = Vec::with_capacity(n);
            let mut notas_vec: Vec<f64> = Vec::with_capacity(n);
            let mut ok = true;
            for _ in 0..n {
                match leitor.entrada() {
                    Some(s) => match s.parse::<i64>() { Ok(v) => m.push(v), Err(_) => { ok = false; break; } },
                    None => { ok = false; break; }
                }
            }
            if !ok { break; }
            for _ in 0..n {
                match leitor.entrada() {
                    Some(s) => match s.parse::<f64>() { Ok(v) => notas_vec.push(v), Err(_) => { ok = false; break; } },
                    None => { ok = false; break; }
                }
            }
            if !ok { break; }
            (m, notas_vec)
        } else {
            let m: Vec<i64> = (0..1000000).collect();
            let mut notas_vec: Vec<f64> = vec![2.0; 999998];
            notas_vec.push(1.0);
            notas_vec.push(3.0);
            (m, notas_vec)
        };
        let (maiores, menores) = notas(&m, &n);
        println!("({:?}, {:?})", maiores, menores);
    }
}
