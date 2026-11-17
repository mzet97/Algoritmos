use bib1::Leitor;

fn doishorarios(h: i64, m: i64) -> (String, String) {
    let hour = h % 12;
    let min = (m * 5) % 60;
    let h1 = format!("{:02}:{:02}", hour, min);
    let h2 = format!("{:02}:{:02}", hour + 12, min);
    (h1, h2)
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let h: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let m: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let (h1, h2) = doishorarios(h, m);
        println!("{} / {}", h1, h2);
    }
}
