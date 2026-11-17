use bib1::Leitor;

fn subtrai(a: &[i32], n: usize, b: &[i32], m: usize) -> Vec<i32> {
    let mut r = vec![0i32; n];
    let mut emprestimo = 0i32;
    let mut j = m as i32 - 1;
    for i in (0..n).rev() {
        let mut diff = a[i] - emprestimo;
        if j >= 0 {
            diff -= b[j as usize];
            j -= 1;
        }
        if diff < 0 {
            diff += 10;
            emprestimo = 1;
        } else {
            emprestimo = 0;
        }
        r[i] = diff;
    }
    // Remove leading zeros
    let mut k = 0;
    while k < r.len() - 1 && r[k] == 0 {
        k += 1;
    }
    r[k..].to_vec()
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<usize> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
        let n = parts[0];
        let m = parts[1];
        let mut a = vec![0i32; n];
        for i in 0..n {
            a[i] = leitor.entrada().unwrap().trim().parse().unwrap();
        }
        let mut b = vec![0i32; m];
        for i in 0..m {
            b[i] = leitor.entrada().unwrap().trim().parse().unwrap();
        }
        let r = subtrai(&a, n, &b, m);
        let s: Vec<String> = r.iter().map(|d| d.to_string()).collect();
        println!("{}", s.join(" "));
    }
}
