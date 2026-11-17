use bib1::Leitor;

// IntersecaoSequenciais - intersection of two sorted sequential lists
// Each list has distinct elements. Output sorted intersection.
// Input: n1 n2, then n1 elements, then n2 elements

fn intersecao(l1: &[i32], l2: &[i32]) -> Vec<i32> {
    let mut result = Vec::new();
    let mut i = 0;
    let mut j = 0;
    while i < l1.len() && j < l2.len() {
        if l1[i] == l2[j] {
            result.push(l1[i]);
            i += 1;
            j += 1;
        } else if l1[i] < l2[j] {
            i += 1;
        } else {
            j += 1;
        }
    }
    result
}

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let parts: Vec<i32> = line.split_whitespace()
            .map(|x| x.parse().unwrap())
            .collect();
        let n1 = parts[0] as usize;
        let n2 = parts[1] as usize;
        if parts[0] != -1 {
            let mut l1: Vec<i32> = Vec::with_capacity(n1);
            for _ in 0..n1 {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                l1.push(e);
            }
            let mut l2: Vec<i32> = Vec::with_capacity(n2);
            for _ in 0..n2 {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                l2.push(e);
            }
            let r = intersecao(&l1, &l2);
            print!("[");
            for v in &r {
                print!("{} ", v);
            }
            println!("]");
        } else {
            let n_test: usize = 9000;
            let mut l1: Vec<i32> = Vec::with_capacity(n_test);
            let mut l2: Vec<i32> = Vec::with_capacity(n_test);
            for i in 0..n_test {
                l1.push((i * 2) as i32);
                l2.push((i * 3) as i32);
            }
            let r = intersecao(&l1, &l2);
            print!("[");
            for v in &r {
                print!("{} ", v);
            }
            println!("]");
        }
    }
}
