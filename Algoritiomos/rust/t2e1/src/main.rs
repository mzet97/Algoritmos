use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn numero_elementos(l: &Option<Box<No>>) -> i32 {
    let mut count = 0;
    let mut p: Option<&No> = l.as_ref().map(|b| b.as_ref());
    while let Some(no) = p {
        count += 1;
        p = no.prox.as_ref().map(|b| b.as_ref());
    }
    count
}

fn insere(l: &mut Option<Box<No>>, e: i32) {
    let novo = Box::new(No { e, prox: l.take() });
    *l = Some(novo);
}

fn main() {
    let mut leitor = Leitor::new();
    let mut l: Option<Box<No>> = None;

    while let Some(line) = leitor.entrada() {
        let parts: Vec<&str> = line.split_whitespace().collect();
        let op = parts[0];
        if op.as_bytes()[0] != b'a' {
            if op == "i" {
                let e: i32 = parts[1].parse().unwrap();
                insere(&mut l, e);
            } else {
                println!("{}", numero_elementos(&l));
            }
        } else {
            for i in 1..=1000000 {
                insere(&mut l, i);
            }
            println!("{}", numero_elementos(&l));
        }
    }
}
