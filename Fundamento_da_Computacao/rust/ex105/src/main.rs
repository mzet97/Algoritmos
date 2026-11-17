use bib1::Leitor;

fn soma_lista(x: &[i32], y: &[i32]) -> Vec<i32> {
    let mut resultado: Vec<i32> = Vec::new();
    let mut carry: i32 = 0;
    let len = x.len().max(y.len());
    for i in 0..len {
        let a = if i < x.len() { x[i] } else { 0 };
        let b = if i < y.len() { y[i] } else { 0 };
        let s = a + b + carry;
        resultado.push(s % 10);
        carry = s / 10;
    }
    if carry > 0 {
        resultado.push(carry);
    }
    resultado
}

fn make_big() -> String {
    "9".repeat(1000000)
}

fn parse_reversed(s: &str) -> Vec<i32> {
    s.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect()
}

fn fmt_result(r: &[i32]) -> String {
    r.iter().map(|d| char::from_digit(*d as u32, 10).unwrap()).collect()
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let (xs, ys) = if x_str != "@entradagrande" {
            let y_str = match leitor.entrada() {
                Some(s) => s,
                None => break,
            };
            (x_str, y_str)
        } else {
            let big = make_big();
            let big2 = make_big();
            (big, big2)
        };
        let x_lista = parse_reversed(&xs);
        let y_lista = parse_reversed(&ys);
        let mut r = soma_lista(&x_lista, &y_lista);
        r.reverse();
        println!("{}", fmt_result(&r));
    }
}
