use bib1::Leitor;

fn ordena(p: &mut Vec<i32>) {
    let mut q: Vec<i32> = Vec::new();
    while !p.is_empty() {
        let tmp = p.pop().unwrap();
        while !q.is_empty() && *q.last().unwrap() > tmp {
            p.push(q.pop().unwrap());
        }
        q.push(tmp);
    }
    *p = q;
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        let mut p: Vec<i32> = Vec::new();
        if n >= 0 {
            for _ in 0..n {
                let Some(l) = leitor.entrada() else { break };
                let e: i32 = l.trim().parse().unwrap();
                p.push(e);
            }
        } else if n == -1 {
            for i in 1..=1000 {
                p.push(i + 1001 * (i % 2));
            }
        } else {
            for i in 1..=1000 {
                p.push(i + 1001 * (if i % 5 == 0 { 0 } else { 1 }));
            }
        }
        ordena(&mut p);
        while !p.is_empty() {
            print!("{} ", p.pop().unwrap());
        }
        println!();
    }
}
