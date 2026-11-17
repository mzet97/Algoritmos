use bib1::Leitor;
use std::collections::HashSet;

fn distintos(l: &[i32]) -> i32 {
    let set: HashSet<i32> = l.iter().copied().collect();
    set.len() as i32
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
            println!("{}", distintos(&l));
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                l.push((i % 997) as i32);
            }
            println!("{}", distintos(&l));
        }
    }
}
