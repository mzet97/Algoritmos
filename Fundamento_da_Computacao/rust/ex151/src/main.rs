use bib1::Leitor;

fn multi_matriz(r: usize, s: usize, t: usize, m1: &[Vec<i64>], m2: &[Vec<i64>]) -> Vec<Vec<i64>> {
    let mut mr = vec![vec![0i64; t]; r];
    for i in 0..r {
        for j in 0..t {
            for k in 0..s {
                mr[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    mr
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let r: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let s: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let t: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let mut m1: Vec<Vec<i64>> = Vec::with_capacity(r);
        for _ in 0..r {
            let linha_str = match leitor.entrada() { Some(s) => s, None => break };
            let linha: Vec<i64> = linha_str.split_whitespace().filter_map(|x| x.parse().ok()).collect();
            m1.push(linha);
        }
        let mut m2: Vec<Vec<i64>> = Vec::with_capacity(s);
        for _ in 0..s {
            let linha_str = match leitor.entrada() { Some(s) => s, None => break };
            let linha: Vec<i64> = linha_str.split_whitespace().filter_map(|x| x.parse().ok()).collect();
            m2.push(linha);
        }
        let mr = multi_matriz(r, s, t, &m1, &m2);
        for i in 0..r {
            if i > 0 { print!(" | "); }
            for j in 0..t {
                if j > 0 { print!(" "); }
                print!("{}", mr[i][j]);
            }
        }
        println!();
    }
}
