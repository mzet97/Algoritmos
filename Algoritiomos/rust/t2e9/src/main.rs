use bib1::Leitor;

// Soma2a2 - pairwise sum of all pairs from list
// Input: n elements (inserted at head, so reversed), output sorted pairwise sums
// Ex: if list is 1->5->4->10, pairs sum: 1+5=6, 1+4=5, 1+10=11, 5+4=9, 5+10=15, 4+10=14
// Output sorted: [ 5 6 9 11 14 15 ]

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut elems: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                elems.push(e); // inserted at head in C++, so order is reversed
            }
            // In C++, insert at head reverses order. To match:
            // We inserted at head, so the linked list order is reverse of insertion order
            // elems is in insertion order; the linked list is reversed
            let mut sums: Vec<i32> = Vec::new();
            for i in 0..elems.len() {
                for j in (i + 1)..elems.len() {
                    // In linked list order (reversed), i and j represent
                    // positions in the list traversal
                    sums.push(elems[i] + elems[j]);
                }
            }
            sums.sort();
            print!("[ ");
            for v in &sums {
                print!("{} ", v);
            }
            println!("]");
        } else {
            let n_test = 1000;
            let mut elems: Vec<i32> = Vec::new();
            for i in 0..n_test {
                elems.push(i);
            }
            let mut sums: Vec<i32> = Vec::new();
            for i in 0..elems.len() {
                for j in (i + 1)..elems.len() {
                    sums.push(elems[i] + elems[j]);
                }
            }
            sums.sort();
            print!("[ ");
            for v in &sums {
                print!("{} ", v);
            }
            println!("]");
        }
    }
}
