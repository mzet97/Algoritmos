use bib1::Leitor;

struct No {
    e: i32,
    q: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn quantidade_menor_igual(t: &Option<Box<No>>, a: i32) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.e <= a {
                let left_count = match &node.esq {
                    Some(n) => n.q,
                    None => 0,
                };
                1 + left_count + quantidade_menor_igual(&node.dir, a)
            } else {
                quantidade_menor_igual(&node.esq, a)
            }
        }
    }
}

fn insere(t: &mut Option<Box<No>>, x: i32) {
    match t {
        None => {
            *t = Some(Box::new(No { e: x, q: 1, esq: None, dir: None }));
        }
        Some(ref mut node) => {
            if x < node.e {
                insere(&mut node.esq, x);
            } else {
                insere(&mut node.dir, x);
            }
            node.q += 1;
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
            let Some(la) = leitor.entrada() else { break };
            let a: i32 = la.trim().parse().unwrap();
            println!("{}", quantidade_menor_igual(&t, a));
        } else {
            insere_bal(&mut t, 1, 500000);
            let a = 358254;
            println!("{}", quantidade_menor_igual(&t, a));
        }
    }
}
