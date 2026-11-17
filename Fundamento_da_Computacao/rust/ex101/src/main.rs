use bib1::Leitor;

fn operacao(x: i64, y: i64, op: &str) -> f64 {
    match op {
        "+" => (x + y) as f64,
        "-" => (x - y) as f64,
        "x" => (x * y) as f64,
        "/" => x as f64 / y as f64,
        _ => 0.0,
    }
}

fn print_result(val: f64) {
    if val == (val as i64) as f64 && val.is_finite() {
        println!("{}", val as i64);
    } else {
        println!("{}", val);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let y: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let op = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let result = operacao(x, y, &op);
        print_result(result);
    }
}
