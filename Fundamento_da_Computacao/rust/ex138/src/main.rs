use bib1::Leitor;

fn pa(a: i64, r: i64, n: usize) -> Vec<i64> {
    (0..n).map(|i| a + i as i64 * r).collect()
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let a: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let r: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let n: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{:?}", pa(a, r, n));
    }
}
