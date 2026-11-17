use bib1::Leitor;

fn maiorigual_lista(x: &[i32], y: &[i32]) -> bool {
    // x[0], y[0] are least significant digits
    // Compare the numbers represented by x and y
    let len_x = x.len();
    let len_y = y.len();
    if len_x != len_y {
        return len_x > len_y;
    }
    // Same length: compare from most significant digit
    for i in (0..len_x).rev() {
        if x[i] != y[i] {
            return x[i] > y[i];
        }
    }
    true // equal
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
        // Convert string to list of digits (reversed, so x[0] is least significant)
        // Python: for i in reversed(x): x_lista.append(int(i))
        // reversed("999991") -> '1','9','9','9','9','9' -> x_lista = [1,9,9,9,9,9]
        let x_lista: Vec<i32> = x_str
            .chars()
            .rev()
            .filter_map(|c| c.to_digit(10).map(|d| d as i32))
            .collect();
        let y_lista: Vec<i32> = y_str
            .chars()
            .rev()
            .filter_map(|c| c.to_digit(10).map(|d| d as i32))
            .collect();
        if maiorigual_lista(&x_lista, &y_lista) {
            println!("1");
        } else {
            println!("0");
        }
    }
}
