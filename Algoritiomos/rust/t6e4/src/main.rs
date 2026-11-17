use bib1::Leitor;

fn procura_soma(l: &[i32], k: i32) -> bool {
    let mut l_sorted = l.to_vec();
    l_sorted.sort();
    let n = l_sorted.len();
    let mut i = 0;
    let mut j = n as i32 - 1;
    while i < j {
        let sum = l_sorted[i as usize] + l_sorted[j as usize];
        if sum == k {
            return true;
        } else if sum < k {
            i += 1;
        } else {
            j -= 1;
        }
    }
    false
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
            println!("{}", if procura_soma(&l, k) { 1 } else { 0 });
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                l.push(i as i32);
            }
            l[200000] = 2000000;
            let k = 2600000;
            println!("{}", if procura_soma(&l, k) { 1 } else { 0 });
        }
    }
}
