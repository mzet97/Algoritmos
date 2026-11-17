use bib1::Leitor;

fn pilhas_identicas(p1: &mut Vec<i32>, p2: &mut Vec<i32>) -> bool {
    if p1.len() != p2.len() {
        return false;
    }
    let len = p1.len();
    for i in 0..len {
        if p1[len - 1 - i] != p2[len - 1 - i] {
            return false;
        }
    }
    true
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        let mut p1: Vec<i32> = Vec::new();
        let mut p2: Vec<i32> = Vec::new();
        if n != -1 {
            let n1 = n as usize;
            for _ in 0..n1 {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                p1.push(e);
            }
            let Some(lin2) = leitor.entrada() else { break };
            let n2: i32 = lin2.trim().parse().unwrap();
            for _ in 0..n2 {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                p2.push(e);
            }
        } else {
            let n = 1000000;
            for i in 0..n {
                p1.push((i + 1) as i32);
                p2.push((i + 1) as i32);
            }
        }
        println!("{}", if pilhas_identicas(&mut p1, &mut p2) { 1 } else { 0 });
    }
}
