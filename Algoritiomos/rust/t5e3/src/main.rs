use bib1::Leitor;

struct No {
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

struct NoL {
    e: i32,
    prox: Option<Box<NoL>>,
}

fn lista_nos_internos(t: &Option<Box<No>>) -> Option<Box<NoL>> {
    let mut result: Option<Box<NoL>> = None;
    lista_nos_internos_rec(t, &mut result);
    result
}

fn lista_nos_internos_rec(t: &Option<Box<No>>, result: &mut Option<Box<NoL>>) {
    if let Some(ref node) = *t {
        if node.esq.is_some() || node.dir.is_some() {
            *result = Some(Box::new(NoL { e: node.e, prox: result.take() }));
        }
        lista_nos_internos_rec(&node.esq, result);
        lista_nos_internos_rec(&node.dir, result);
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

fn preenche_cheia(t: &mut Option<Box<No>>, h: i32) {
    if h == 0 {
        *t = None;
    } else {
        *t = Some(Box::new(No { e: h, esq: None, dir: None }));
        if let Some(ref mut node) = *t {
            preenche_cheia(&mut node.esq, h - 1);
            preenche_cheia(&mut node.dir, h - 1);
        }
    }
}

fn preenche_zigzag(t: &mut Option<Box<No>>, h: i32) {
    if h == 0 {
        *t = None;
    } else {
        *t = Some(Box::new(No { e: h, esq: None, dir: None }));
        if let Some(ref mut node) = *t {
            preenche_zigzag(&mut node.esq, h - 1);
            node.dir = None;
        }
    }
}

fn escreve_lista(l: &Option<Box<NoL>>) {
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
    let mut t: Option<Box<No>> = None;
    while let Some(line) = leitor.entrada() {
        let v: i32 = line.trim().parse().unwrap();
        if v > -2000 {
            let mut e: Vec<i32> = vec![v];
            loop {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                e.push(x);
                if x == -1000 { break; }
            }
            let mut i = 0;
            insere(&mut t, &e, &mut i);
        } else {
            if v == -2000 {
                preenche_zigzag(&mut t, 50000);
            } else {
                preenche_cheia(&mut t, 19);
            }
        }
        let lr = lista_nos_internos(&t);
        escreve_lista(&lr);
    }
}
