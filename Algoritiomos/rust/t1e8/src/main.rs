use bib1::Leitor;

fn fibonacci(n: usize) -> u64 {
    if n == 0 {
        return 0;
    }
    if n == 1 {
        return 1;
    }
    const MOD: u64 = 1_000_000_000;
    let mut a: u64 = 0;
    let mut b: u64 = 1;
    for _ in 2..=n {
        let c = (a + b) % MOD;
        a = b;
        b = c;
    }
    b
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: usize = line.trim().parse().unwrap();
        println!("{}", fibonacci(n));
    }
}
