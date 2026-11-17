use bib1::Leitor;

fn soma_mais_proxima_zero(l: &[i32]) -> i32 {
    let mut prefix_sums: Vec<i64> = Vec::with_capacity(l.len() + 1);
    prefix_sums.push(0);
    let mut sum: i64 = 0;
    for &x in l {
        sum += x as i64;
        prefix_sums.push(sum);
    }
    prefix_sums.sort();
    let mut best = i64::MAX;
    for i in 1..prefix_sums.len() {
        let diff = prefix_sums[i] - prefix_sums[i - 1];
        if diff.abs() < best.abs() {
            best = diff;
        }
    }
    best as i32
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
            println!("{}", soma_mais_proxima_zero(&l));
        } else {
            let max_n = 100000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 1..=max_n {
                l.push(i as i32);
            }
            l[50000] = -20000;
            println!("{}", soma_mais_proxima_zero(&l));
        }
    }
}
