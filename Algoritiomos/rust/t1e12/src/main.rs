use bib1::Leitor;

fn permutacao_circular(a: &mut [u8], n: usize, k: i32) {
    if n == 0 {
        return;
    }
    let k = (k as usize) % n;
    if k == 0 {
        return;
    }
    // Reverse entire, then reverse first k, then reverse remaining
    a[..n].reverse();
    a[..k].reverse();
    a[k..n].reverse();
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
        let n = parts[0] as usize;
        let k = parts[1];
        let mut a = leitor.entrada().unwrap().into_bytes();
        permutacao_circular(&mut a, n, k);
        // Only print first n characters
        let s: String = a[..n].iter().map(|&ch| ch as char).collect();
        println!("{}", s);
    }
}
