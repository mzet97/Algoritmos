use bib1::Leitor;

fn impar(n: i32) -> i32 {
    if n == 0 {
        1
    } else {
        impar(n - 1) + 2
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n >= 0 {
            println!("{}", impar(n));
        } else {
            let n = 50000;
            for _ in 1..=200 { impar(n); }
            println!("{}", impar(n));
        }
    }
}
