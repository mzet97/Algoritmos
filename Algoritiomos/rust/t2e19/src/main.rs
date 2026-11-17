use bib1::Leitor;

// ConcatenarEncadeadas - concatenate two linked lists
// Input: n1 n2, then n1 elements (append to end), then n2 elements (append to end)
// Output: [e1 e2 ... ]

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
            print!("[");
            for v in &l1 {
                print!("{} ", v);
            }
            for v in &l2 {
                print!("{} ", v);
            }
            println!("]");
        } else {
            // automatic test: max_n1 = max_n2 = 500000, reads from stdin
            let n1_test: usize = 500000;
            let n2_test: usize = 500000;
            let mut l1: Vec<i32> = Vec::with_capacity(n1_test);
            for _ in 0..n1_test {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                l1.push(e);
            }
            let mut l2: Vec<i32> = Vec::with_capacity(n2_test);
            for _ in 0..n2_test {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                l2.push(e);
            }
            print!("[");
            for v in &l1 {
                print!("{} ", v);
            }
            for v in &l2 {
                print!("{} ", v);
            }
            println!("]");
        }
    }
}
