use bib1::Leitor;

// RemoveMaiores for linked list - use Vec for simplicity
// C++ inserts at head, so list traversal = reverse of input order
// We collect elements, filter, sort, and output

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut elems: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                elems.push(e);
            }
            let k: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
            elems.retain(|&x| x <= k);
            elems.sort();
            print!("[");
            for v in &elems {
                print!("{}", v);
            }
            println!("]");
        } else {
            let max_n = 1000000;
            let mut elems: Vec<i32> = Vec::with_capacity(max_n);
            for i in 0..max_n {
                if i % 2 == 0 {
                    elems.push(2);
                } else {
                    elems.push(3);
                }
            }
            elems.retain(|&x| x <= 2);
            elems.sort();
            print!("[");
            for v in &elems {
                print!("{}", v);
            }
            println!("]");
        }
    }
}
