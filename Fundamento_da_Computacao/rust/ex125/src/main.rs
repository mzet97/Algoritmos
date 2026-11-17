use bib1::Leitor;

fn contar(l: &str, c: char) -> i64 {
    let mut count: i64 = 0;
    for ch in l.chars() {
        if ch == c {
            count += 1;
        }
    }
    count
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let l = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let c_str = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let c = match c_str.chars().next() {
            Some(ch) => ch,
            None => break,
        };
        println!("{}", contar(&l, c));
    }
}
