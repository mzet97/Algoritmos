use bib1::Leitor;

fn maior(a: &[i32], n: usize) -> i32 {
    let mut m = a[0];
    for i in 1..n {
        if a[i] > m {
            m = a[i];
        }
    }
    m
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: usize = line.trim().parse().unwrap();
        let mut a = vec![0i32; n];
        for i in 0..n {
            a[i] = leitor.entrada().unwrap().trim().parse().unwrap();
        }
        println!("{}", maior(&a, n));
    }
}
