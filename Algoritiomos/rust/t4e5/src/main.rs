use bib1::Leitor;

fn bin(n: i64, k: i64) -> i64 {
    if k == 0 || k == n {
        1
    } else {
        bin(n - 1, k - 1) + bin(n - 1, k)
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<i64> = line.split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        let n = parts[0];
        let k = parts[1];
        if n >= 0 {
            println!("{}", bin(n, k));
        } else {
            let n = 50000;
            let k = 49999;
            println!("{}", bin(n, k));
        }
    }
}
