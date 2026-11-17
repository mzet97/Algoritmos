use bib1::Leitor;

fn maximo(a: &[i32], n: usize) -> i32 {
    if n == 1 {
        a[0]
    } else {
        let m = maximo(a, n - 1);
        if a[n - 1] > m { a[n - 1] } else { m }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n >= 0 {
            let Some(l2) = leitor.entrada() else { break };
            let a: Vec<i32> = l2.split_whitespace()
                .map(|x| x.parse().unwrap())
                .collect();
            println!("{}", maximo(&a, a.len()));
        } else {
            let n = 50000;
            let mut a: Vec<i32> = vec![0; n];
            for i in 0..n / 2 {
                a[i] = i as i32;
            }
            for i in (n / 2 + 1)..n {
                a[i] = (n - i) as i32;
            }
            for _ in 0..20 { maximo(&a, n); }
            println!("{}", maximo(&a, n));
        }
    }
}
