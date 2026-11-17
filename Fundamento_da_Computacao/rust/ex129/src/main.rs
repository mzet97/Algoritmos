use bib1::Leitor;

fn reverte(l: &str) -> String {
    l.chars().rev().collect()
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let l_raw = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let l = if l_raw == "@entradagrande" {
            "ab".repeat(500000)
        } else {
            l_raw
        };
        println!("{}", reverte(&l));
    }
}
