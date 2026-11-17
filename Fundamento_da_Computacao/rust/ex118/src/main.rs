use bib1::Leitor;

fn mdc(x: i64, y: i64) -> i64 {
    if y == 0 {
        x
    } else {
        mdc(y, x % y)
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let y: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        println!("{}", mdc(x, y));
    }
}
