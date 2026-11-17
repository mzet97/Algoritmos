use bib1::Leitor;

fn calcula_soma(n: i64) -> i64 {
    let mut soma: i64 = 0;
    for i in 1..=n {
        soma += i;
    }
    soma
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i64 = line.trim().parse().unwrap();
        println!("{}", calcula_soma(n));
    }
}
