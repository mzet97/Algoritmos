use bib1::Leitor;

fn mdc(x: i64, y: i64) -> i64 {
    if y == 0 {
        x
    } else {
        mdc(y, x % y)
    }
}

fn mmc(x: i64, y: i64) -> i64 {
    x / mdc(x, y) * y
}

fn multiplos(a: i64, b: i64, n: i64) -> i64 {
    let lcm = mmc(a, b);
    if lcm == 0 {
        return 0;
    }
    let count = (n - 1) / lcm;
    count
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let a: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let b: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        println!("{}", multiplos(a, b, n));
    }
}
