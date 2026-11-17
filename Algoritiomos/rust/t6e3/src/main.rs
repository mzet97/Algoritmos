use bib1::Leitor;

fn quantidade_intervalo(l: &[i32], a: i32, b: i32) -> i32 {
    let mut count = 0;
    for &x in l {
        if x >= a && x <= b {
            count += 1;
        }
    }
    count
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
            let parts: Vec<i32> = lin.split_whitespace()
                .map(|x| x.parse().unwrap())
                .collect();
            let a = parts[0];
            let b = parts[1];
            println!("{}", quantidade_intervalo(&l, a, b));
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                l.push(3 * i as i32);
            }
            let a = 3 * 200000;
            let b = 3 * 900000;
            println!("{}", quantidade_intervalo(&l, a, b));
        }
    }
}
