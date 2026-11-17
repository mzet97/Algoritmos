use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn maior(l: &Option<Box<No>>) -> i32 {
    let mut max_val = i32::MIN;
    let mut p: Option<&No> = l.as_ref().map(|b| b.as_ref());
    while let Some(no) = p {
        if no.e > max_val {
            max_val = no.e;
        }
        p = no.prox.as_ref().map(|b| b.as_ref());
    }
    max_val
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
                println!("{}", maior(&l));
            }
        } else {
            let n = 1000000;
            for i in (n / 2)..=n {
                insere(&mut l, i);
            }
            for i in 1..(n / 2) {
                insere(&mut l, i);
            }
            println!("{}", maior(&l));
        }
    }
}
