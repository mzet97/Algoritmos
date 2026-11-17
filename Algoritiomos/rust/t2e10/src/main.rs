use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn insere(l: &mut Option<Box<No>>, e: i32) {
    let novo = Box::new(No { e, prox: l.take() });
    *l = Some(novo);
}

fn inverte(l: Option<Box<No>>) -> Option<Box<No>> {
    let mut result: Option<Box<No>> = None;
    let mut p = l;
    while let Some(mut no) = p {
        p = no.prox.take();
        no.prox = result;
        result = Some(no);
    }
    result
}

fn escreve(l: &Option<Box<No>>) {
    print!("[ ");
    let mut p: Option<&No> = l.as_ref().map(|b| b.as_ref());
    while let Some(no) = p {
        print!("{} ", no.e);
        p = no.prox.as_ref().map(|b| b.as_ref());
    }
    println!("]");
}

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut l: Option<Box<No>> = None;
            for _ in 0..n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                insere(&mut l, e);
            }
            let lr = inverte(l);
            escreve(&lr);
        } else {
            let n_test = 500000;
            let mut l: Option<Box<No>> = None;
            for i in 0..n_test {
                insere(&mut l, i % 100);
            }
            let lr = inverte(l);
            escreve(&lr);
        }
    }
}
