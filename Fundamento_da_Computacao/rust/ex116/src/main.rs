use bib1::Leitor;

fn raiz(n: i64) -> i64 {
    let mut i: i64 = 0;
    while (i + 1) * (i + 1) <= n {
        i += 1;
    }
    i
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        println!("{}", raiz(n));
    }
}
