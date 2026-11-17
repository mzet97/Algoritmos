use bib1::Leitor;

fn sao_anagramas(a: &[char], b: &[char]) -> bool {
    if a.len() != b.len() {
        return false;
    }
    let mut count_a = vec![0i32; 26];
    let mut count_b = vec![0i32; 26];
    for &c in a {
        count_a[(c as usize) - ('a' as usize)] += 1;
    }
    for &c in b {
        count_b[(c as usize) - ('a' as usize)] += 1;
    }
    count_a == count_b
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let Some(lin_a) = leitor.entrada() else { break };
            let a: Vec<char> = lin_a.chars().collect();
            let Some(lin_b) = leitor.entrada() else { break };
            let b: Vec<char> = lin_b.chars().collect();
            println!("{}", if sao_anagramas(&a, &b) { 1 } else { 0 });
        } else {
            let max_n = 1000000;
            let t = ('z' as usize) - ('a' as usize) + 1;
            let mut a: Vec<char> = Vec::with_capacity(max_n);
            for i in (0..max_n).rev() {
                a.push((b'a' as u8 + (i % t) as u8) as char);
            }
            let mut b: Vec<char> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                b.push((b'a' as u8 + (i % t) as u8) as char);
            }
            println!("{}", if sao_anagramas(&a, &b) { 1 } else { 0 });
        }
    }
}
