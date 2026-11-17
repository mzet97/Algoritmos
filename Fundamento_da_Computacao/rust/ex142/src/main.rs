use bib1::Leitor;

fn ponto_reta(a: f64, b: f64, ponto: (f64, f64)) -> char {
    let (x, y) = ponto;
    let y_reta = a * x + b;
    let diff = y - y_reta;
    if diff.abs() <= 0.00001 { '=' }
    else if diff > 0.0 { '>' }
    else { '<' }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let a: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let b: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let x: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let y: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        println!("{}", ponto_reta(a, b, (x, y)));
    }
}
