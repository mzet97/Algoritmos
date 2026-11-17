use bib1::Leitor;

fn divisores(n: i32, k: i32) -> i32 {
    if k == 0 {
        0
    } else if n % k == 0 {
        1 + divisores(n, k - 1)
    } else {
        divisores(n, k - 1)
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<i32> = line.split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        let n = parts[0];
        let k = parts[1];
        if n >= 0 {
            println!("{}", divisores(n, k));
        } else {
            let n = 50000;
            let k = 50000;
            for _ in 1..=200 { divisores(n, k); }
            println!("{}", divisores(n, k));
        }
    }
}
