use bib1::Leitor;

struct No {
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn ordenar(a: &mut [i32], n: usize) {
    // Build a BST and do in-order traversal
    if n == 0 { return; }
    let mut t: Option<Box<No>> = None;
    for i in 0..n {
        insere_bst(&mut t, a[i]);
    }
    let mut idx = 0;
    in_order(&t, a, &mut idx);
}

fn insere_bst(t: &mut Option<Box<No>>, x: i32) {
    match t {
        None => { *t = Some(Box::new(No { e: x, esq: None, dir: None })); }
        Some(ref mut node) => {
            if x < node.e {
                insere_bst(&mut node.esq, x);
            } else {
                insere_bst(&mut node.dir, x);
            }
        }
    }
}

fn in_order(t: &Option<Box<No>>, a: &mut [i32], idx: &mut usize) {
    if let Some(ref node) = *t {
        in_order(&node.esq, a, idx);
        a[*idx] = node.e;
        *idx += 1;
        in_order(&node.dir, a, idx);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut e: Vec<i32> = Vec::new();
    while let Some(line) = leitor.entrada() {
        let v: i32 = line.trim().parse().unwrap();
        if v > -1 {
            e.clear();
            e.push(v);
            loop {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                if x == -1 { break; }
                e.push(x);
            }
        } else {
            e.clear();
            let mut n = 0;
            insere_bal_vec(&mut e, &mut n, 1, 60000);
        }
        let n = e.len();
        let mut arr = e.clone();
        ordenar(&mut arr, n);
        for i in 0..n {
            print!("{} ", arr[i]);
        }
        println!();
    }
}

fn insere_bal_vec(e: &mut Vec<i32>, n: &mut usize, ini: i32, fim: i32) {
    if ini <= fim {
        let m = (ini + fim) / 2;
        e.push(m);
        *n += 1;
        insere_bal_vec(e, n, ini, m - 1);
        insere_bal_vec(e, n, m + 1, fim);
    }
}
