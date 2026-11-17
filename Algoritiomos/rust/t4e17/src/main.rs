use bib1::Leitor;

fn remove_maiores(l: &mut Vec<i32>, k: i32) {
    l.retain(|&x| x <= k);
}

fn escreve(l: &[i32]) {
    let mut v = l.to_vec();
    v.sort();
    print!("[");
    for x in &v {
        print!("{}", x);
    }
    println!("]");
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut l: Vec<i32> = Vec::new();
            for _ in 0..n {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                l.push(e);
            }
            let Some(lk) = leitor.entrada() else { break };
            let k: i32 = lk.trim().parse().unwrap();
            remove_maiores(&mut l, k);
            escreve(&l);
        } else {
            let nlin = 50000;
            let k = 2;
            for _ in 1..=20 {
                let mut l: Vec<i32> = Vec::new();
                for i in 0..nlin {
                    l.push(if i % 2 == 0 { 2 } else { 3 });
                }
                remove_maiores(&mut l, k);
            }
            let mut l: Vec<i32> = Vec::new();
            for i in 0..nlin {
                l.push(if i % 2 == 0 { 2 } else { 3 });
            }
            remove_maiores(&mut l, k);
            escreve(&l);
        }
    }
}
