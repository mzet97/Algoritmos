use bib1::Leitor;

fn histograma(l: &[i32], k: i32) -> Vec<i32> {
    let mut h = vec![0i32; k as usize];
    for &x in l {
        if x >= 0 && x < k {
            h[x as usize] += 1;
        }
    }
    h
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
            let Some(lin) = leitor.entrada() else { break };
            let k: i32 = lin.trim().parse().unwrap();
            let h = histograma(&l, k);
            for i in 0..h.len() {
                print!("{}: {}; ", i, h[i]);
            }
            println!();
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in (1..=max_n).rev() {
                l.push(i as i32);
            }
            let k = 100000;
            let h = histograma(&l, k);
            for i in 0..h.len() {
                print!("{}: {}; ", i, h[i]);
            }
            println!();
        }
    }
}
