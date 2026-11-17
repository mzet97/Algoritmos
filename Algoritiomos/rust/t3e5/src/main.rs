use bib1::Leitor;
use std::collections::VecDeque;

fn filas_identicas(f1: &mut VecDeque<i32>, f2: &mut VecDeque<i32>) -> bool {
    if f1.len() != f2.len() {
        return false;
    }
    let len = f1.len();
    for _ in 0..len {
        if f1.pop_front() != f2.pop_front() {
            return false;
        }
    }
    true
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        let mut f1: VecDeque<i32> = VecDeque::new();
        let mut f2: VecDeque<i32> = VecDeque::new();
        if n != -1 {
            let n1 = n as usize;
            let Some(l1) = leitor.entrada() else { break };
            let vals1: Vec<i32> = l1.split_whitespace()
                .map(|x| x.parse().unwrap())
                .collect();
            for i in 0..n1.min(vals1.len()) {
                f1.push_back(vals1[i]);
            }
            let Some(l2) = leitor.entrada() else { break };
            let vals2: Vec<i32> = l2.split_whitespace()
                .map(|x| x.parse().unwrap())
                .collect();
            for i in 0..vals2.len() {
                f2.push_back(vals2[i]);
            }
        } else {
            let nn = 1000000;
            for i in 0..nn {
                f1.push_back((i + 1) as i32);
                f2.push_back((i + 1) as i32);
            }
        }
        println!("{}", if filas_identicas(&mut f1, &mut f2) { 1 } else { 0 });
    }
}
