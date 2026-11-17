use bib1::Leitor;

fn subtrai_lista(x: &[i32], y: &[i32]) -> Vec<i32> {
    let mut resultado: Vec<i32> = Vec::new();
    let mut borrow: i32 = 0;
    for i in 0..x.len() {
        let mut a = x[i];
        let b = if i < y.len() { y[i] } else { 0 };
        a -= borrow;
        if a < b {
            a += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultado.push(a - b);
    }
    while resultado.len() > 1 && *resultado.last().unwrap() == 0 {
        resultado.pop();
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
        let y_str = if x_str == "@entradagrande1" {
            "9".repeat(1000000 - 1)
        } else if x_str == "@entradagrande2" {
            "1".repeat(1000000)
        } else {
            match leitor.entrada() {
                Some(s) => s,
                None => break,
            }
        };
        let x_str = if x_str == "@entradagrande1" { "1".repeat(1000000) }
                     else if x_str == "@entradagrande2" { "1".repeat(1000000) }
                     else { x_str };
        let x_lista: Vec<i32> = x_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let y_lista: Vec<i32> = y_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let mut r = subtrai_lista(&x_lista, &y_lista);
        r.reverse();
        let s: String = r.iter().map(|d| char::from_digit(*d as u32, 10).unwrap()).collect();
        println!("{}", s);
    }
}
