use bib1::Leitor;
use std::collections::HashSet;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn sem_repeticoes(l: &Option<Box<No>>) -> Option<Box<No>> {
    let mut seen: HashSet<i32> = HashSet::new();
    let mut result: Option<Box<No>> = None;
    let mut p = l;
    while let Some(node) = p {
        if !seen.contains(&node.e) {
            seen.insert(node.e);
            result = Some(Box::new(No { e: node.e, prox: result.take() }));
        }
        p = &node.prox;
    }
    result
}

fn escreve(l: &Option<Box<No>>) {
    let mut vals: Vec<i32> = Vec::new();
    let mut p = l;
    while let Some(node) = p {
        vals.push(node.e);
        p = &node.prox;
    }
    vals.sort();
    print!("[");
    for v in &vals {
        print!("{}", v);
    }
    println!("]");
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        let mut l: Option<Box<No>> = None;
        if n > -1 {
            for _ in 0..n {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                l = Some(Box::new(No { e, prox: l.take() }));
            }
            let lr = sem_repeticoes(&l);
            escreve(&lr);
        } else if n == -1 {
            let nn = 1000;
            for i in 0..nn {
                let e = if i % 2 == 0 { 2 } else { 3 };
                l = Some(Box::new(No { e, prox: l.take() }));
            }
            let lr = sem_repeticoes(&l);
            escreve(&lr);
        } else {
            let nn = 1000;
            for i in 0..nn {
                l = Some(Box::new(No { e: i, prox: l.take() }));
            }
            let lr = sem_repeticoes(&l);
            escreve(&lr);
        }
    }
}
