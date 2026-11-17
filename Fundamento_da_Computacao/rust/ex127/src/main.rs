use bib1::Leitor;

fn duplica(l: &str) -> String {
    let mut result = String::with_capacity(l.len() * 2);
    for ch in l.chars() {
        result.push(ch);
        if "aeiouAEIOU".contains(ch) {
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
            "a".repeat(500000)
        } else {
            l_raw
        };
        println!("{}", duplica(&l));
    }
}
