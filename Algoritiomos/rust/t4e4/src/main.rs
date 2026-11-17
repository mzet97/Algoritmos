use bib1::Leitor;

fn raiz_quad(n: i32) -> i32 {
    if n == 0 {
        0
    } else {
        let r = raiz_quad(n - 1);
        if (r + 1) * (r + 1) <= n {
            r + 1
        } else {
            r
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n >= 0 {
            println!("{}", raiz_quad(n));
        } else {
            let n = 50000;
            for _ in 1..=200 { raiz_quad(n); }
            println!("{}", raiz_quad(n));
        }
    }
}
