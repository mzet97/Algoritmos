use bib1::Leitor;

struct No {
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn criar_arvore_altura_minima(x: &[i32], ini: usize, fim: usize) -> Option<Box<No>> {
    if ini > fim || ini >= x.len() {
        return None;
    }
    let m = (ini + fim) / 2;
    let mut node = Box::new(No { e: x[m], esq: None, dir: None });
    node.esq = criar_arvore_altura_minima(x, ini, m.saturating_sub(1));
    node.dir = criar_arvore_altura_minima(x, m + 1, fim);
    Some(node)
}

fn escrever(t: &Option<Box<No>>) {
    if let Some(ref node) = *t {
        print!("({} ", node.e);
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
    let mut e: Vec<i32> = Vec::new();
    while let Some(line) = leitor.entrada() {
        let v: i32 = line.trim().parse().unwrap();
        if v > -1 {
            e.clear();
            e.push(v);
            loop {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                if x == -1 { break; }
                e.push(x);
            }
        } else {
            e.clear();
            for n in 1..=100000 {
                e.push(n);
            }
        }
        let t = criar_arvore_altura_minima(&e, 0, e.len().saturating_sub(1));
        escrever(&t);
        println!();
    }
}
