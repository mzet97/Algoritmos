use bib1::Leitor;

struct No {
    e: i32,
    q: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn quantidade_intervalo(t: &Option<Box<No>>, a: i32, b: i32) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.e < a {
                quantidade_intervalo(&node.dir, a, b)
            } else if node.e > b {
                quantidade_intervalo(&node.esq, a, b)
            } else {
                let left_count = match &node.esq {
                    Some(n) => n.q,
                    None => 0,
                };
                let right_count = match &node.dir {
                    Some(n) => n.q,
                    None => 0,
                };
                1 + left_less_than_a(&node.esq, a)
                    + right_greater_than_b(&node.dir, b)
                    + full_count(&node.esq) - left_less_than_a(&node.esq, a) - left_greater_than_b(&node.esq, b)
                    + full_count(&node.dir) - right_less_than_a(&node.dir, a) - right_greater_than_b(&node.dir, b)
            }
        }
    }
}

fn full_count(t: &Option<Box<No>>) -> i32 {
    match t {
        None => 0,
        Some(ref node) => node.q,
    }
}

fn left_less_than_a(t: &Option<Box<No>>, a: i32) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.e < a {
                let left = full_count(&node.esq);
                left + 1 + left_less_than_a(&node.dir, a)
            } else {
                left_less_than_a(&node.esq, a)
            }
        }
    }
}

fn right_greater_than_b(t: &Option<Box<No>>, b: i32) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.e > b {
                let right = full_count(&node.dir);
                right + 1 + right_greater_than_b(&node.esq, b)
            } else {
                right_greater_than_b(&node.dir, b)
            }
        }
    }
}

fn left_greater_than_b(t: &Option<Box<No>>, b: i32) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.e > b {
                let right = full_count(&node.dir);
                right + 1 + left_greater_than_b(&node.esq, b)
            } else {
                left_greater_than_b(&node.dir, b)
            }
        }
    }
}

fn right_less_than_a(t: &Option<Box<No>>, a: i32) -> i32 {
    match t {
        None => 0,
        Some(ref node) => {
            if node.e < a {
                let left = full_count(&node.esq);
                left + 1 + right_less_than_a(&node.dir, a)
            } else {
                right_less_than_a(&node.esq, a)
            }
        }
    }
}

fn insere(t: &mut Option<Box<No>>, x: i32) {
    match t {
        None => {
            *t = Some(Box::new(No { e: x, q: 1, esq: None, dir: None }));
        }
        Some(ref mut node) => {
            if x < node.e {
                insere(&mut node.esq, x);
            } else {
                insere(&mut node.dir, x);
            }
            node.q += 1;
        }
    }
}

fn insere_bal(t: &mut Option<Box<No>>, i: i32, j: i32) {
    if i <= j {
        let m = (i + j) / 2;
        insere(t, m);
        insere_bal(t, i, m - 1);
        insere_bal(t, m + 1, j);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut t: Option<Box<No>> = None;
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        t = None;
        if n > 0 {
            for _ in 0..n {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                insere(&mut t, x);
            }
            let Some(lab) = leitor.entrada() else { break };
            let parts: Vec<i32> = lab.split_whitespace()
                .map(|x| x.parse().unwrap())
                .collect();
            let a = parts[0];
            let b = parts[1];
            println!("{}", quantidade_intervalo(&t, a, b));
        } else {
            insere_bal(&mut t, 1, 500000);
            let a = 122111;
            let b = 422111;
            println!("{}", quantidade_intervalo(&t, a, b));
        }
    }
}
