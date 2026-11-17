use bib1::Leitor;

struct No {
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn maior_subarvore(t: &Option<Box<No>>) -> i32 {
    match t {
        None => 0,
        Some(node) => {
            let se = maior_subarvore(&node.esq);
            let sd = maior_subarvore(&node.dir);
            let s = node.e + se + sd;
            if s > se && s > sd { s } else if se > sd { se } else { sd }
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

fn preenche_cheia(t: &mut Option<Box<No>>, h: i32) {
    if h == 0 {
        *t = None;
    } else {
        let val = if h > 1 { -2 } else { 1 };
        *t = Some(Box::new(No { e: val, esq: None, dir: None }));
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
        let val = if h > 1 { -2 } else { 1 };
        *t = Some(Box::new(No { e: val, esq: None, dir: None }));
        if let Some(ref mut node) = *t {
            preenche_zigzag(&mut node.esq, h - 1);
            node.dir = None;
        }
    }
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
            println!("{}", maior_subarvore(&t));
        } else {
            if v == -2000 {
                preenche_zigzag(&mut t, 50000);
            } else {
                preenche_cheia(&mut t, 19);
            }
            println!("{}", maior_subarvore(&t));
        }
    }
}
