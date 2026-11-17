use bib1::Leitor;

// Remove by pointer (doubly linked list) - O(1)
// Input: n, e1..en, i (1-based index of element to remove)
// Output: forward traversal [ ... ] ; backward traversal [ ... ]

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut elems: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                elems.push(e);
            }
            let idx: usize = leitor.entrada().unwrap().trim().parse().unwrap();
            // Insert at head: list order is reverse of insertion order
            // Remove insertion index (idx-1)
            elems.remove(idx - 1);
            // Forward (list traversal, which is reverse of insertion order)
            let forward: Vec<i32> = elems.iter().rev().cloned().collect();
            // Backward (reverse of forward)
            let backward: Vec<i32> = forward.iter().rev().cloned().collect();

            print!("Esq >> Dir: [ ");
            for v in &forward {
                print!("{} ", v);
            }
            print!("]; ");
            print!("Dir >> Esq: [ ");
            for v in &backward {
                print!("{} ", v);
            }
            println!("]");
        } else {
            let n_test: usize = 1000000;
            let mut elems: Vec<i32> = Vec::with_capacity(n_test);
            for i in 0..n_test {
                elems.push((i % 100) as i32);
            }
            // Remove 1000 elements at positions 0, 900, 1800, ...
            let mut to_remove: Vec<usize> = Vec::new();
            for i in 0..1000 {
                to_remove.push(i * 900);
            }
            to_remove.sort();
            to_remove.reverse();
            for &idx in &to_remove {
                if idx < elems.len() {
                    elems.remove(idx);
                }
            }
            let forward: Vec<i32> = elems.iter().rev().cloned().collect();
            let backward: Vec<i32> = forward.iter().rev().cloned().collect();

            print!("Esq >> Dir: [ ");
            for v in &forward {
                print!("{} ", v);
            }
            print!("]; ");
            print!("Dir >> Esq: [ ");
            for v in &backward {
                print!("{} ", v);
            }
            println!("]");
        }
    }
}
