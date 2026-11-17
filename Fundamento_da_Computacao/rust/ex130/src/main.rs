use bib1::Leitor;

fn sub_string(l: &str, s: &str) -> bool {
    if s.is_empty() {
        return true;
    }
    l.contains(s)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let l_raw = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let (l, s) = if l_raw == "@entradagrande" {
            let l = "ab".repeat(2000);
            let s = "ab".repeat(1000) + "c";
            (l, s)
        } else {
            let s = match leitor.entrada() {
                Some(v) => v,
                None => break,
            };
            (l_raw, s)
        };
        println!("{}", if sub_string(&l, &s) { "True" } else { "False" });
    }
}
