use bib1::Leitor;

fn conta_unicos(p: &str) -> i64 {
    let mut count_map = std::collections::HashMap::new();
    for ch in p.chars() {
        *count_map.entry(ch).or_insert(0i64) += 1;
    }
    count_map.values().filter(|&&v| v == 1).count() as i64
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let p = if n >= 0 {
            match leitor.entrada() {
                Some(s) => s,
                None => break,
            }
        } else {
            "abcde".repeat(10000)
        };
        println!("{}", conta_unicos(&p));
    }
}
