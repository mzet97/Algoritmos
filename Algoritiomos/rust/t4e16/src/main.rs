use bib1::Leitor;

fn numero_caminhos(n: i32, m: usize, x: &[i32], y: &[i32]) -> i64 {
    // Build adjacency list
    let mut adj: Vec<Vec<i32>> = vec![vec![]; (n + 1) as usize];
    for i in 0..m {
        adj[x[i] as usize].push(y[i]);
    }
    let mut count: Vec<i64> = vec![0; (n + 1) as usize];
    count[1] = 1;
    for u in 1..=n {
        for &v in &adj[u as usize] {
            count[v as usize] += count[u as usize];
        }
    }
    count[n as usize]
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<i32> = line.split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        let n = parts[0];
        let m = parts[1] as usize;
        if n >= 0 {
            let Some(lx) = leitor.entrada() else { break };
            let x: Vec<i32> = lx.split_whitespace()
                .map(|v| v.parse().unwrap())
                .collect();
            let Some(ly) = leitor.entrada() else { break };
            let y: Vec<i32> = ly.split_whitespace()
                .map(|v| v.parse().unwrap())
                .collect();
            println!("{}", numero_caminhos(n, m, &x, &y));
        } else {
            let n = 2000;
            let mut x: Vec<i32> = Vec::new();
            let mut y: Vec<i32> = Vec::new();
            for i in 2..=n {
                x.push(i);
                y.push(i - 1);
                x.push(i - 1);
                y.push(i + 1);
            }
            println!("{}", numero_caminhos(n, x.len(), &x, &y));
        }
    }
}
