use bib1::Leitor;

fn multiplica_lista_lista(x: &[i32], y: &[i32]) -> Vec<i32> {
    let mut resultado = vec![0i32; x.len() + y.len()];
    for i in 0..x.len() {
        let mut carry: i32 = 0;
        for j in 0..y.len() {
            let p = resultado[i + j] + x[i] * y[j] + carry;
            resultado[i + j] = p % 10;
            carry = p / 10;
        }
        let mut k = i + y.len();
        while carry > 0 {
            let p = resultado[k] + carry;
            resultado[k] = p % 10;
            carry = p / 10;
            k += 1;
        }
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
        let y_str = if x_str != "@entradagrande" {
            match leitor.entrada() {
                Some(s) => s,
                None => break,
            }
        } else {
            "9".repeat(1000)
        };
        let x_str = if x_str == "@entradagrande" { "9".repeat(1000) } else { x_str };
        let x_lista: Vec<i32> = x_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let y_lista: Vec<i32> = y_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let mut r = multiplica_lista_lista(&x_lista, &y_lista);
        r.reverse();
        let s: String = r.iter().map(|d| char::from_digit(*d as u32, 10).unwrap()).collect();
        println!("{}", s);
    }
}
