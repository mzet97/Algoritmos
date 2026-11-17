use bib1::Leitor;

fn transposta(m: &[Vec<i64>], n: usize, mm: usize) -> Vec<Vec<i64>> {
    (0..mm).map(|j| (0..n).map(|i| m[i][j]).collect()).collect()
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let (n, mm, m) = if n > -1 {
            let n = n as usize;
            let mm: usize = match leitor.entrada() {
                Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
                None => break,
            };
            let mut m: Vec<Vec<i64>> = Vec::with_capacity(n);
            let mut ok = true;
            for _ in 0..n {
                let mut linha: Vec<i64> = Vec::with_capacity(mm);
                for _ in 0..mm {
                    match leitor.entrada() {
                        Some(s) => match s.parse::<i64>() {
                            Ok(v) => linha.push(v),
                            Err(_) => { ok = false; break; }
                        },
                        None => { ok = false; break; }
                    }
                }
                if !ok { break; }
                m.push(linha);
            }
            (n, mm, m)
        } else {
            let n: usize = 1000;
            let mm: usize = 1000;
            let m: Vec<Vec<i64>> = (0..n).map(|i| (0..mm).map(|j| (i*n+j) as i64).collect()).collect();
            (n, mm, m)
        };
        println!("{:?}", transposta(&m, n, mm));
    }
}
