use bib1::Leitor;

fn multiplica_lista(x: &[i32], y: i32) -> Vec<i32> {
    let mut resultado: Vec<i32> = Vec::new();
    let mut carry: i32 = 0;
    for &d in x {
        let p = d * y + carry;
        resultado.push(p % 10);
        carry = p / 10;
    }
    while carry > 0 {
        resultado.push(carry % 10);
        carry /= 10;
    }
    if resultado.is_empty() {
        resultado.push(0);
    }
    resultado
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let y: i32 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let x_lista: Vec<i32> = x_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let mut r = multiplica_lista(&x_lista, y);
        r.reverse();
        let s: String = r.iter().map(|d| char::from_digit(*d as u32, 10).unwrap()).collect();
        println!("{}", s);
    }
}
