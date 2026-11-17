use bib1::Leitor;

fn sistema(a: &[Vec<i64>], b: &[i64], n: usize) -> Vec<f64> {
    let mut x: Vec<f64> = vec![0.0; n];
    for i in 0..n {
        let mut s = b[i] as f64;
        for j in 0..i {
            s -= a[i][j] as f64 * x[j];
        }
        x[i] = s / a[i][i] as f64;
    }
    x
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n_val: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let (a, b, n) = if n_val >= 1 {
            let n = n_val as usize;
            let mut a: Vec<Vec<i64>> = Vec::with_capacity(n);
            let mut b: Vec<i64> = Vec::with_capacity(n);
            let mut ok = true;
            for _ in 0..n {
                let linha_str = match leitor.entrada() { Some(s) => s, None => { ok = false; break; } };
                let parts: Vec<i64> = linha_str.split_whitespace().filter_map(|x| x.parse().ok()).collect();
                let a_row: Vec<i64> = parts[..parts.len()-1].to_vec();
                let bi = parts[parts.len()-1];
                a.push(a_row);
                b.push(bi);
            }
            if !ok { break; }
            (a, b, n)
        } else {
            let n: usize = 1000;
            let a: Vec<Vec<i64>> = (1..=n).map(|i| vec![1; i]).collect();
            let b: Vec<i64> = vec![1; n];
            (a, b, n)
        };
        let x = sistema(&a, &b, n);
        let parts: Vec<String> = x.iter().map(|e| format!("{:.1}", e)).collect();
        println!("{}", parts.join(" "));
    }
}
