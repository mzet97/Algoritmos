use bib1::Leitor;

fn saque(s: i64) -> Vec<(i64, i64)> {
    let notas = [200, 100, 50, 20, 10, 5];
    let mut resultado: Vec<(i64, i64)> = Vec::new();
    let mut restante = s;
    for &n in &notas {
        if restante >= n {
            let q = restante / n;
            resultado.push((q, n));
            restante -= q * n;
        }
    }
    if restante != 0 {
        return Vec::new();
    }
    resultado
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let s: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{:?}", saque(s));
    }
}
