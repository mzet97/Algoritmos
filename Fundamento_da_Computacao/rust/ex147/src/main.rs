use bib1::Leitor;

fn escrever_seq_fibonacci(n: usize) {
    let mut fibs: Vec<i64> = Vec::with_capacity(n);
    let mut a: i64 = 1;
    let mut b: i64 = 1;
    for _ in 0..n {
        fibs.push(a);
        let temp = a + b;
        a = b;
        b = temp;
    }
    // Ascending part
    for k in 1..n {
        let line: Vec<String> = (0..k).map(|i| fibs[i].to_string()).collect();
        println!("{}", line.join(" "));
    }
    // Full line
    let line: Vec<String> = fibs.iter().map(|f| f.to_string()).collect();
    println!("{}", line.join(" "));
    // Descending part
    for k in (1..n).rev() {
        let line: Vec<String> = (0..k).map(|i| fibs[i].to_string()).collect();
        println!("{}", line.join(" "));
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: usize = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        escrever_seq_fibonacci(n);
    }
}
