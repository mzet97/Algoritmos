use bib1::Leitor;

fn serie(n: i64) -> f64 {
    let mut sum = 0.0_f64;
    for i in 1..=n {
        sum += i as f64 / (n - i + 1) as f64;
    }
    sum
}

fn print_python_float(val: f64) {
    // Python prints floats like: 1.0, 2.5, 4.333333333333333
    // For whole numbers it prints X.0
    if val == val.floor() && val.is_finite() {
        println!("{:.1}", val);
    } else {
        println!("{}", val);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        print_python_float(serie(n));
    }
}
