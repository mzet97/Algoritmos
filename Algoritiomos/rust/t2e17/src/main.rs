use bib1::Leitor;

// InversaoSequencial - reverse array in place
// Input: n, e1..en
// Output: [e_n e_{n-1} ... e_1 ]

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                l.push(e);
            }
            l.reverse();
            print!("[");
            for v in &l {
                print!("{} ", v);
            }
            println!("]");
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                l.push((i % 10) as i32);
            }
            l.reverse();
            print!("[");
            for v in &l {
                print!("{} ", v);
            }
            println!("]");
        }
    }
}
