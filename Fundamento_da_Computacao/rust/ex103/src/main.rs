use bib1::Leitor;

fn calc_sec(h1: i64, m1: i64, h2: i64, m2: i64) -> i64 {
    let t1 = h1 * 3600 + m1 * 60;
    let t2 = h2 * 3600 + m2 * 60;
    (t1 - t2).abs()
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let h1: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let m1: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let h2: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let m2: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        println!("{}", calc_sec(h1, m1, h2, m2));
    }
}
