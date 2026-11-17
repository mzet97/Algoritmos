use bib1::Leitor;
use std::collections::HashSet;

fn busca_em_lote(l: &[i32], b: &[i32]) -> i32 {
    let set_l: HashSet<i32> = l.iter().copied().collect();
    let mut count = 0;
    for &x in b {
        if set_l.contains(&x) {
            count += 1;
        }
    }
    count
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            let mut b: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                b.push(e);
            }
            println!("{}", busca_em_lote(&l, &b));
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                l.push(i as i32);
            }
            let mut b: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                b.push(2 * i as i32);
            }
            println!("{}", busca_em_lote(&l, &b));
        }
    }
}
