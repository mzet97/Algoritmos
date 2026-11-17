use bib1::Leitor;

struct No {
    op: char,
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn avaliar_expressao(t: &Option<Box<No>>) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.op == '@' {
                node.e
            } else {
                let le = avaliar_expressao(&node.esq);
                let ri = avaliar_expressao(&node.dir);
                match node.op {
                    '+' => le + ri,
                    '-' => le - ri,
                    '*' => le * ri,
                    '/' => le / ri,
                    _ => 0,
                }
            }
        }
    }
}

fn insere(t: &mut Option<Box<No>>, e: &[char], i: &mut usize) {
    if *i >= e.len() || e[*i] == '.' {
        if *i < e.len() { *i += 1; }
        *t = None;
    } else {
        let op;
        let val;
        if e[*i] >= '0' && e[*i] <= '9' {
            val = (e[*i] as i32) - ('0' as i32);
            op = '@';
        } else {
            op = e[*i];
            val = 0;
        }
        *t = Some(Box::new(No { op, e: val, esq: None, dir: None }));
        *i += 1;
        if let Some(ref mut node) = *t {
            insere(&mut node.esq, e, i);
            insere(&mut node.dir, e, i);
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let chars: Vec<char> = line.trim().chars().collect();
        let mut i = 0;
        let mut t: Option<Box<No>> = None;
        insere(&mut t, &chars, &mut i);
        println!("{}", avaliar_expressao(&t));
    }
}
