use bib1::Leitor;

fn calcula_soma(n: i64) -> i64 {
    n * (n + 1) / 2
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i64 = line.trim().parse().unwrap();
        println!("{}", calcula_soma(n));
    }
}
