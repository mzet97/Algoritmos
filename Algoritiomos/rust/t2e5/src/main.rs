use bib1::Leitor;

// K-ultimo O(k) - doubly linked circular list
// Since it's doubly linked, we can walk k steps backwards from the last node
// Using Vec representation, we just index from the end
fn k_ultimo(elementos: &[i32], k: i32) -> i32 {
    let n = elementos.len();
    if k < 1 || (k as usize) > n {
        return -1;
    }
    elementos[n - k as usize]
}

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
                let k: i32 = parts[1].parse().unwrap();
                println!("{}", k_ultimo(&elementos, k));
            }
        } else {
            elementos.clear();
            let n = 2000000;
            for i in 1..=n {
                elementos.push(i);
            }
            if op == "a1" {
                println!("{}", k_ultimo(&elementos, n / 2));
            } else {
                println!("{}", k_ultimo(&elementos, 3));
                for _ in 0..1000 {
                    k_ultimo(&elementos, 3);
                }
            }
        }
    }
}
