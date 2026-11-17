use bib1::Leitor;

fn remove(l: &str, c: char) -> String {
    let mut result = String::with_capacity(l.len());
    for ch in l.chars() {
        if ch != c {
            result.push(ch);
        }
    }
    result
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let l_raw = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let l = if l_raw == "@entradagrande" {
            "ca".repeat(500000)
        } else {
            l_raw
        };
        let c_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let c = match c_str.chars().next() {
            Some(ch) => ch,
            None => break,
        };
        println!("{}", remove(&l, c));
    }
}
