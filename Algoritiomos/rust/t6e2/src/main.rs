use bib1::Leitor;

fn soma_sequencia_operacoes(s: &[i32]) -> i32 {
    let mut stack: Vec<i32> = Vec::new();
    let mut total: i32 = 0;
    for &x in s {
        if x == 0 {
            if let Some(v) = stack.pop() {
                total -= v;
            }
        } else {
            stack.push(x);
            total += x;
        }
    }
    total
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut s: Vec<i32> = Vec::with_capacity(n as usize);
            let Some(lin) = leitor.entrada() else { break };
            let vals: Vec<i32> = lin.split_whitespace()
                .map(|x| x.parse().unwrap())
                .collect();
            s.extend(vals);
            println!("{}", soma_sequencia_operacoes(&s));
        } else {
            let n = 1000000;
            let mut s: Vec<i32> = Vec::with_capacity(n);
            for _i in 0..n / 2 {
                s.push(1);
                s.push(0);
            }
            println!("{}", soma_sequencia_operacoes(&s));
        }
    }
}
