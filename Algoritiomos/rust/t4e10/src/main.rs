use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn remove_maiores(l: &mut Option<Box<No>>, k: i32) {
    let mut current = l.take();
    let mut result: Option<Box<No>> = None;
    while let Some(mut node) = current {
        current = node.prox.take();
        if node.e <= k {
            node.prox = result.take();
            result = Some(node);
        }
    }
    *l = result;
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
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Option<Box<No>> = None;
            for _ in 0..max_n {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                l = Some(Box::new(No { e, prox: l.take() }));
            }
            let Some(lk) = leitor.entrada() else { break };
            let k: i32 = lk.trim().parse().unwrap();
            remove_maiores(&mut l, k);
            escreve(&l);
        } else {
            for _ in 1..=20 {
                let mut l: Option<Box<No>> = None;
                let max_n = 50000;
                for i in 0..max_n {
                    let e = if i % 2 == 0 { 2 } else { 3 };
                    l = Some(Box::new(No { e, prox: l.take() }));
                }
                remove_maiores(&mut l, 2);
                // first iteration writes
            }
            // simplified: do once
            let mut l: Option<Box<No>> = None;
            for i in 0..50000 {
                let e = if i % 2 == 0 { 2 } else { 3 };
                l = Some(Box::new(No { e, prox: l.take() }));
            }
            remove_maiores(&mut l, 2);
            escreve(&l);
        }
    }
}
