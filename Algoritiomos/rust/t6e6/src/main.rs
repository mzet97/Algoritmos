use bib1::Leitor;

fn maior_diferenca(l: &[i32]) -> i32 {
    if l.is_empty() {
        return 0;
    }
    let mut min_val = l[0];
    let mut max_diff = 0;
    for i in 1..l.len() {
        if l[i] - min_val > max_diff {
            max_diff = l[i] - min_val;
        }
        if l[i] < min_val {
            min_val = l[i];
        }
    }
    max_diff
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            println!("{}", maior_diferenca(&l));
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in (1..=max_n).rev() {
                l.push(i as i32);
            }
            l[500000] = -1;
            l[70000] = 3000000;
            println!("{}", maior_diferenca(&l));
        }
    }
}
