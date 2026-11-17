use bib1::Leitor;

fn calcula_soma(n: i64, m: &[i64], k: usize) -> i64 {
    let mut soma: i64 = 0;
    for i in 1..=n {
        for j in 0..k {
            if i % m[j] == 0 {
                soma += i;
                break;
            }
        }
    }
    soma
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<i64> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
        let n = parts[0];
        let k = parts[1] as usize;
        let mut m = vec![0i64; k];
        for i in 0..k {
            m[i] = leitor.entrada().unwrap().trim().parse().unwrap();
        }
        println!("{}", calcula_soma(n, &m, k));
    }
}
