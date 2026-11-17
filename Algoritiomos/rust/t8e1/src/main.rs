use bib1::Leitor;
use std::collections::HashMap;

fn numero_mais_frequente(l: &[i64]) -> i64 {
    let mut freq: HashMap<i64, i32> = HashMap::new();
    for &x in l {
        *freq.entry(x).or_insert(0) += 1;
    }
    let mut best = l[0];
    let mut best_count = 0;
    for (&x, &count) in &freq {
        if count > best_count || (count == best_count && x < best) {
            best = x;
            best_count = count;
        }
    }
    best
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Vec<i64> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i64 = lin.trim().parse().unwrap();
                l.push(e);
            }
            println!("{}", numero_mais_frequente(&l));
        } else {
            let max_n = 3000000;
            let mut l: Vec<i64> = Vec::with_capacity(max_n);
            for i in (1..=max_n as i64).rev() {
                l.push(1000000 * i);
            }
            l[100000] = 1000000 * 191191;
            println!("{}", numero_mais_frequente(&l));
        }
    }
}
