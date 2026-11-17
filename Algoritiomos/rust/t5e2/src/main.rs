use bib1::Leitor;

struct No {
    e: i32,
    h: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn atualiza_altura(t: &mut Option<Box<No>>) {
    if let Some(ref mut node) = *t {
        atualiza_altura(&mut node.esq);
        atualiza_altura(&mut node.dir);
        let he = match &node.esq {
            Some(n) => n.h,
            None => -1,
        };
        let hd = match &node.dir {
            Some(n) => n.h,
            None => -1,
        };
        node.h = if he > hd { he + 1 } else { hd + 1 };
    }
}

fn insere(t: &mut Option<Box<No>>, e: &[i32], i: &mut usize) {
    if e[*i] == 0 {
        *i += 1;
        *t = None;
    } else {
        *t = Some(Box::new(No { e: e[*i], h: 0, esq: None, dir: None }));
        *i += 1;
        if let Some(ref mut node) = *t {
            insere(&mut node.esq, e, i);
            insere(&mut node.dir, e, i);
        }
    }
}

fn preenche_cheia(t: &mut Option<Box<No>>, h: i32, i: &mut i32) {
    if h == 0 {
        *t = None;
    } else {
        *t = Some(Box::new(No { e: *i, h: 0, esq: None, dir: None }));
        *i += 1;
        if let Some(ref mut node) = *t {
            preenche_cheia(&mut node.esq, h - 1, i);
            preenche_cheia(&mut node.dir, h - 1, i);
        }
    }
}

fn preenche_zigzag(t: &mut Option<Box<No>>, h: i32, i: &mut i32) {
    if h == 0 {
        *t = None;
    } else {
        *t = Some(Box::new(No { e: *i, h: 0, esq: None, dir: None }));
        *i += 1;
        if let Some(ref mut node) = *t {
            preenche_zigzag(&mut node.esq, h - 1, i);
            node.dir = None;
        }
    }
}

fn escreve(t: &Option<Box<No>>) {
    if let Some(ref node) = *t {
        print!("h({})={} ", node.e, node.h);
        escreve(&node.esq);
        escreve(&node.dir);
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
        } else {
            let mut i = 1;
            if v == -2000 {
                preenche_cheia(&mut t, 19, &mut i);
            } else {
                preenche_zigzag(&mut t, 50000, &mut i);
            }
        }
        atualiza_altura(&mut t);
        escreve(&t);
        println!();
    }
}
