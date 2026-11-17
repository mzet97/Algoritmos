use bib1::Leitor;

fn concatena_prefixos(a: &[u8], b: &[u8], n: usize, m: usize) -> Vec<u8> {
    let mut c = Vec::with_capacity(n + m);
    for i in 0..n {
        c.push(a[i]);
    }
    for i in 0..m {
        c.push(b[i]);
    }
    c
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(linha) = leitor.entrada() {
        let parts: Vec<&str> = linha.split_whitespace().collect();
        let n: usize = parts[0].parse().unwrap();
        let m: usize = parts[1].parse().unwrap();
        let a = leitor.entrada().unwrap();
        let b = leitor.entrada().unwrap();
        let c = concatena_prefixos(a.as_bytes(), b.as_bytes(), n, m);
        let s = String::from_utf8_lossy(&c).to_string();
        println!("{}", s);
    }
}
