use bib1::Leitor;

fn raiz(a: f64, b: f64, c: f64) -> Option<(f64, Option<f64>)> {
    let delta = b * b - 4.0 * a * c;
    if delta < 0.0 {
        return None;
    }
    let r1 = (-b + delta.sqrt()) / (2.0 * a);
    if delta == 0.0 {
        return Some((r1, None));
    }
    let r2 = (-b - delta.sqrt()) / (2.0 * a);
    Some((r1, Some(r2)))
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
        let a: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let b: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let c: f64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        match raiz(a, b, c) {
            None => println!("None"),
            Some((r1, None)) => {
                print!("(");
                print_float(r1);
                print!(", None)");
                println!();
            }
            Some((r1, Some(r2))) => {
                print!("(");
                print_float(r1);
                print!(", ");
                print_float(r2);
                println!(")");
            }
        }
    }
}
