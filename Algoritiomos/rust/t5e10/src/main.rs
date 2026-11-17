use bib1::Leitor;

struct No {
    e: i32,
    q: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn insere(t: &mut Option<Box<No>>, x: i32) {
    match t {
        None => {
            *t = Some(Box::new(No { e: x, q: 1, esq: None, dir: None }));
        }
        Some(ref mut node) => {
            if x == node.e {
                // duplicate, insert to the right per C++ code logic
                insere(&mut node.dir, x);
                node.q += 1;
            } else if x < node.e {
                insere(&mut node.esq, x);
                node.q += 1;
            } else {
                insere(&mut node.dir, x);
                node.q += 1;
            }
        }
    }
}

fn insere_bal(t: &mut Option<Box<No>>, i: i32, j: i32) {
    if i <= j {
        let m = (i + j) / 2;
        insere(t, m);
        insere_bal(t, i, m - 1);
        insere_bal(t, m + 1, j);
    }
}

fn escrever(t: &Option<Box<No>>) {
    if let Some(ref node) = *t {
        print!("(Q={},E={} ", node.q, node.e);
        print!("(");
        escrever(&node.esq);
        print!(")");
        print!("(");
        escrever(&node.dir);
        print!("))");
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut t: Option<Box<No>> = None;
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        t = None;
        if n > 0 {
            for _ in 0..n {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                insere(&mut t, x);
            }
        } else {
            insere_bal(&mut t, 1, 50000);
            insere_bal(&mut t, 1, 50000);
        }
        escrever(&t);
        println!();
    }
}
