use bib1::Leitor;

fn numero_elementos(l: &Option<Box<No>>) -> i32 {
    match l {
        None => 0,
        Some(node) => 1 + numero_elementos(&node.prox),
    }
}

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn main() {
    let mut leitor = Leitor::new();
    let mut l: Option<Box<No>> = None;
    while let Some(line) = leitor.entrada() {
        if line != "a" {
            let op: Vec<char> = line.chars().collect();
            if op[0] == 'i' {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                l = Some(Box::new(No { e, prox: l.take() }));
            } else {
                println!("{}", numero_elementos(&l));
            }
        } else {
            for i in 1..=50000 {
                l = Some(Box::new(No { e: i, prox: l.take() }));
            }
            for _ in 1..=100 { numero_elementos(&l); }
            println!("{}", numero_elementos(&l));
        }
    }
}
