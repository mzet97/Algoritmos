use bib1::Leitor;

// Ultimo - last element of circular list
// Circular list: last element's prox points back to first
// We store elements in a Vec for simplicity

fn main() {
    let mut leitor = Leitor::new();
    let mut elementos: Vec<i32> = Vec::new();

    while let Some(line) = leitor.entrada() {
        let parts: Vec<&str> = line.split_whitespace().collect();
        let op = parts[0];
        if op.as_bytes()[0] != b'a' {
            if op == "i" {
                let e: i32 = parts[1].parse().unwrap();
                elementos.push(e);
            } else {
                if elementos.is_empty() {
                    println!("-1");
                } else {
                    println!("{}", elementos.last().unwrap());
                }
            }
        } else {
            elementos.clear();
            for i in 1..=2000000i32 {
                elementos.push(i);
            }
            println!("{}", elementos.last().unwrap());
        }
    }
}
