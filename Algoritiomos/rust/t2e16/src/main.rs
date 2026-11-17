use bib1::Leitor;

// ConsultaLote - batch search
// Input: n elements for LS (sequential), then n*n elements for LE (linked)
// Count how many LE elements are in LS
fn quantos_contidos(ls: &[i32], le_elems: &[i32]) -> i32 {
    use std::collections::HashSet;
    let set: HashSet<i32> = ls.iter().cloned().collect();
    let mut count = 0;
    for e in le_elems {
        if set.contains(e) {
            count += 1;
        }
    }
    count
}

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n > -1 {
            let mut ls: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let x: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                ls.push(x);
            }
            let nn = (n as i64) * (n as i64);
            let mut le: Vec<i32> = Vec::with_capacity(nn as usize);
            for _ in 0..nn {
                let x: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                le.push(x);
            }
            println!("{}", quantos_contidos(&ls, &le));
        } else {
            let n_test: usize = 3000;
            let ls: Vec<i32> = vec![1; n_test];
            let le: Vec<i32> = vec![2; n_test * n_test];
            println!("{}", quantos_contidos(&ls, &le));
        }
    }
}
