use bib1::Leitor;

fn representacao_decimal(mut n: i64) -> Vec<i32> {
    if n == 0 {
        return vec![0];
    }
    let mut digitos = Vec::new();
    while n > 0 {
        digitos.push((n % 10) as i32);
        n /= 10;
    }
    digitos.reverse();
    digitos
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i64 = line.trim().parse().unwrap();
        let r = representacao_decimal(n);
        let s: Vec<String> = r.iter().map(|d| d.to_string()).collect();
        println!("{}", s.join(" "));
    }
}
