use bib1::Leitor;

fn remove_maiores(l: &mut Vec<i32>, k: i32) {
    let mut j = 0;
    for i in 0..l.len() {
        if l[i] <= k {
            l[j] = l[i];
            j += 1;
        }
    }
    l.truncate(j);
}

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
            let k: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
            remove_maiores(&mut l, k);
            l.sort();
            print!("[");
            for (i, v) in l.iter().enumerate() {
                if i > 0 {
                    // no separator in original C++ output
                }
                print!("{}", v);
            }
            println!("]");
        } else {
            let max_n = 1000000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                if i % 2 == 0 {
                    l.push(2);
                } else {
                    l.push(3);
                }
            }
            remove_maiores(&mut l, 2);
            l.sort();
            print!("[");
            for v in &l {
                print!("{}", v);
            }
            println!("]");
        }
    }
}
