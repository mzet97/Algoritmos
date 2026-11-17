use bib1::Leitor;

fn tv(d: i64, pl: i64, ph: i64) -> (f64, f64) {
    let diag_cm = d as f64 * 2.54;
    let ratio = pl as f64 / ph as f64;
    let h = diag_cm / (1.0 + ratio * ratio).sqrt();
    let w = h * ratio;
    let w = (w * 10.0).round() / 10.0;
    let h = (h * 10.0).round() / 10.0;
    (w, h)
}

fn print_float(val: f64) {
    if val == val.floor() && val.is_finite() && val.abs() < 1e15 {
        println!("{}", val as i64);
    } else {
        println!("{}", val);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let d: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let pl: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let ph: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let (w, h) = tv(d, pl, ph);
        print!("(");
        print_float(w);
        print!(", ");
        print_float(h);
        println!(")");
    }
}
