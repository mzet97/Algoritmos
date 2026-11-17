use bib1::Leitor;

fn lista_para_int(lst: &[i32]) -> u64 {
    let mut val: u64 = 0;
    for i in (0..lst.len()).rev() {
        val = val * 10 + lst[i] as u64;
    }
    val
}

fn int_para_lista(val: u64) -> Vec<i32> {
    if val == 0 { return vec![0]; }
    let mut lst: Vec<i32> = Vec::new();
    let mut v = val;
    while v > 0 {
        lst.push((v % 10) as i32);
        v /= 10;
    }
    lst
}

fn divide_lista(x: &[i32], y: &[i32]) -> (Vec<i32>, Vec<i32>) {
    let dx = lista_para_int(x);
    let dy = lista_para_int(y);
    let q = dx / dy;
    let r = dx % dy;
    (int_para_lista(q), int_para_lista(r))
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let y_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let x_lista: Vec<i32> = x_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let y_lista: Vec<i32> = y_str.chars().rev().filter_map(|c| c.to_digit(10).map(|d| d as i32)).collect();
        let (mut q, mut r) = divide_lista(&x_lista, &y_lista);
        q.reverse();
        r.reverse();
        let qs: String = q.iter().map(|d| char::from_digit(*d as u32, 10).unwrap()).collect();
        let rs: String = r.iter().map(|d| char::from_digit(*d as u32, 10).unwrap()).collect();
        println!("({},{})", qs, rs);
    }
}
