use bib1::Leitor;

fn soma(a: &[i32], n: usize, b: &[i32], m: usize) -> Vec<i32> {
    let mut r = Vec::new();
    let mut carry = 0i32;
    let mut i = n as i32 - 1;
    let mut j = m as i32 - 1;
    while i >= 0 || j >= 0 || carry > 0 {
        let mut sum = carry;
        if i >= 0 {
            sum += a[i as usize];
            i -= 1;
        }
        if j >= 0 {
            sum += b[j as usize];
            j -= 1;
        }
        r.push(sum % 10);
        carry = sum / 10;
    }
    r.reverse();
    r
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
        let r = soma(&a, n, &b, m);
        let s: Vec<String> = r.iter().map(|d| d.to_string()).collect();
        println!("{}", s.join(" "));
    }
}
