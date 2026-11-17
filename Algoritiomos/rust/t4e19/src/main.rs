use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn soma2a2(l: &Option<Box<No>>) -> Option<Box<No>> {
    let mut vals: Vec<i32> = Vec::new();
    let mut p = l;
    while let Some(node) = p {
        vals.push(node.e);
        p = &node.prox;
    }
    let mut result: Option<Box<No>> = None;
    let mut i = 0;
    while i + 1 < vals.len() {
        result = Some(Box::new(No { e: vals[i] + vals[i + 1], prox: result.take() }));
        i += 2;
    }
    if i < vals.len() {
        result = Some(Box::new(No { e: vals[i], prox: result.take() }));
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
    print!("[ ");
    for v in &vals {
        print!("{} ", v);
    }
    println!("]");
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        let mut l: Option<Box<No>> = None;
        if n != -1 {
            for _ in 0..n {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                l = Some(Box::new(No { e, prox: l.take() }));
            }
            let lr = soma2a2(&l);
            escreve(&lr);
        } else {
            let n = 1000;
            for i in 0..n {
                l = Some(Box::new(No { e: i, prox: l.take() }));
            }
            let lr = soma2a2(&l);
            escreve(&lr);
        }
    }
}
