use bib1::Leitor;

struct No {
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn eh_arvore_busca(t: &Option<Box<No>>) -> bool {
    eh_arvore_busca_rec(t, i32::MIN, i32::MAX)
}

fn eh_arvore_busca_rec(t: &Option<Box<No>>, min_val: i32, max_val: i32) -> bool {
    match t {
        None => true,
        Some(ref node) => {
            node.e > min_val && node.e < max_val
                && eh_arvore_busca_rec(&node.esq, min_val, node.e)
                && eh_arvore_busca_rec(&node.dir, node.e, max_val)
        }
    }
}

fn insere(t: &mut Option<Box<No>>, e: &[i32], i: &mut usize) {
    if e[*i] == 0 {
        *i += 1;
        *t = None;
    } else {
        *t = Some(Box::new(No { e: e[*i], esq: None, dir: None }));
        *i += 1;
        if let Some(ref mut node) = *t {
            insere(&mut node.esq, e, i);
            insere(&mut node.dir, e, i);
        }
    }
}

fn insere_bal(t: &mut Option<Box<No>>, i: i32, j: i32) {
    if i <= j {
        let m = (i + j) / 2;
        let val = if m == 900000 { 1 } else { m };
        *t = Some(Box::new(No { e: val, esq: None, dir: None }));
        if let Some(ref mut node) = *t {
            insere_bal(&mut node.esq, i, m - 1);
            insere_bal(&mut node.dir, m + 1, j);
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut t: Option<Box<No>> = None;
    while let Some(line) = leitor.entrada() {
        let v: i32 = line.trim().parse().unwrap();
        if v > -1 {
            let mut e: Vec<i32> = vec![v];
            loop {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                if x == -1 { break; }
                e.push(x);
            }
            let mut i = 0;
            insere(&mut t, &e, &mut i);
        } else {
            insere_bal(&mut t, 1, 900000);
        }
        println!("{}", if eh_arvore_busca(&t) { 1 } else { 0 });
    }
}
