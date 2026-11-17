use bib1::Leitor;

fn exp(b: i64, n: i32) -> i64 {
    if n == 0 {
        1
    } else {
        b * exp(b, n - 1)
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<i64> = line.split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        let b = parts[0];
        let n = parts[1] as i32;
        if b >= 0 {
            println!("{}", exp(b, n));
        } else {
            let b = 1;
            let n = 50000;
            for _ in 1..=200 { exp(b, n); }
            println!("{}", exp(b, n));
        }
    }
}
