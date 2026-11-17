use bib1::Leitor;

struct No {
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn maior(t: &Option<Box<No>>) -> i32 {
    match t {
        None => i32::MIN,
        Some(node) => {
            let me = maior(&node.esq);
            let md = maior(&node.dir);
            let mut m = node.e;
            if me > m { m = me; }
            if md > m { m = md; }
            m
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

fn preenche_cheia_busca(t: &mut Option<Box<No>>, i: &mut i32, h: i32) {
    if h == 0 {
        *t = None;
    } else {
        *t = Some(Box::new(No { e: 0, esq: None, dir: None }));
        if let Some(ref mut node) = *t {
            preenche_cheia_busca(&mut node.esq, i, h - 1);
            node.e = *i;
            *i += 1;
            preenche_cheia_busca(&mut node.dir, i, h - 1);
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
                e.push(x);
                if x == -1 { break; }
            }
            let mut i = 0;
            insere(&mut t, &e, &mut i);
            println!("{}", maior(&t));
        } else {
            let mut i = 1;
            preenche_cheia_busca(&mut t, &mut i, 20);
            println!("{}", maior(&t));
        }
    }
}
