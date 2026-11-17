use bib1::Leitor;

// SemRepeticoes - returns new list without duplicates
// Input: n elements, output: sorted unique elements in [x y z] format

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n > -1 {
            let mut elems: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                elems.push(e);
            }
            // Remove duplicates: use seen set
            let mut seen = std::collections::HashSet::new();
            let mut unique: Vec<i32> = Vec::new();
            for e in &elems {
                if seen.insert(*e) {
                    unique.push(*e);
                }
            }
            unique.sort();
            print!("[");
            for v in &unique {
                print!("{}", v);
            }
            println!("]");
        } else {
            // automatic test
            let nlin = n;
            let n_test = 1000;
            let mut elems: Vec<i32> = Vec::new();
            for i in 0..n_test {
                if i % 2 == 0 {
                    elems.push(2);
                } else {
                    elems.push(3);
                }
            }
            if nlin != -1 {
                for i in 0..n_test {
                    elems.push(i);
                }
            }
            let mut seen = std::collections::HashSet::new();
            let mut unique: Vec<i32> = Vec::new();
            for e in &elems {
                if seen.insert(*e) {
                    unique.push(*e);
                }
            }
            unique.sort();
            print!("[");
            for v in &unique {
                print!("{}", v);
            }
            println!("]");
        }
    }
}
