use bib1::Leitor;
use std::collections::BinaryHeap;
use std::cmp::Reverse;

fn maiores(l: &[i32], k: usize) -> Vec<i32> {
    let mut heap: BinaryHeap<Reverse<i32>> = BinaryHeap::with_capacity(k);
    for &x in l {
        if heap.len() < k {
            heap.push(Reverse(x));
        } else if x > heap.peek().unwrap().0 {
            heap.pop();
            heap.push(Reverse(x));
        }
    }
    let mut result: Vec<i32> = heap.into_iter().map(|r| r.0).collect();
    result.sort_by(|a, b| b.cmp(a));
    result
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            let Some(lin) = leitor.entrada() else { break };
            let k: i32 = lin.trim().parse().unwrap();
            let m = maiores(&l, k as usize);
            for v in &m {
                print!("{} ", v);
            }
            println!();
        } else {
            let n = 2000000;
            let mut l: Vec<i32> = Vec::with_capacity(n);
            for i in 0..n {
                l.push(i as i32);
            }
            let k = 100;
            let m = maiores(&l, k);
            for v in &m {
                print!("{} ", v);
            }
            println!();
        }
    }
}
