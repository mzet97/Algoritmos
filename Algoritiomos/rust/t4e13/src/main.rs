use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn maior(l: &Option<Box<No>>) -> i32 {
    match l {
        None => i32::MIN,
        Some(node) => {
            let m = maior(&node.prox);
            if node.e > m { node.e } else { m }
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut l: Option<Box<No>> = None;
    while let Some(line) = leitor.entrada() {
        if line != "a" {
            if line.starts_with('i') {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                l = Some(Box::new(No { e, prox: l.take() }));
            } else {
                println!("{}", maior(&l));
            }
        } else {
            let n = 50000;
            for i in (n / 2)..=n {
                l = Some(Box::new(No { e: i, prox: l.take() }));
            }
            for i in 1..(n / 2) {
                l = Some(Box::new(No { e: i, prox: l.take() }));
            }
            for _ in 1..=100 { maior(&l); }
            println!("{}", maior(&l));
        }
    }
}
