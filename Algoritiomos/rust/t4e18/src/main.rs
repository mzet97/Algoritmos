use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn inverte(l: &Option<Box<No>>) -> Option<Box<No>> {
    let mut result: Option<Box<No>> = None;
    let mut p = l;
    while let Some(node) = p {
        result = Some(Box::new(No { e: node.e, prox: result.take() }));
        p = &node.prox;
    }
    result
}

fn escreve(l: &Option<Box<No>>) {
    print!("[ ");
    let mut p = l;
    while let Some(node) = p {
        print!("{} ", node.e);
        p = &node.prox;
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
            let lr = inverte(&l);
            escreve(&lr);
        } else {
            let n = 1000;
            for i in 0..n {
                l = Some(Box::new(No { e: i % 100, prox: l.take() }));
            }
            for _ in 1..=1000 {
                let _lr = inverte(&l);
            }
            let lr = inverte(&l);
            escreve(&lr);
        }
    }
}
