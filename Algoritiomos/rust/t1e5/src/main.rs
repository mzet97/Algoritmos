use bib1::Leitor;

fn conta_misteriosa(n: i32) -> f64 {
    let limite = 10i64.pow(n as u32);
    let mut soma = 0.0;
    for k in 1..=limite {
        let sinal: f64 = if (k + 1) % 2 == 0 { 1.0 } else { -1.0 };
        soma += sinal / (2.0 * k as f64 - 1.0);
    }
    4.0 * soma
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        println!("{:.8}", conta_misteriosa(n));
    }
}
