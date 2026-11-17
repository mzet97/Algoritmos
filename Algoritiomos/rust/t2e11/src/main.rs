use bib1::Leitor;

// Remove by pointer (singly linked list) - O(n)
// Input: n, e1..en, i (1-based index of element to remove)
// The C++ builds a pointer array V[] so V[i-1] points to the node to remove
// In Rust we use Vec and remove by index

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            // Read n elements (inserted at head -> reversed)
            let mut elems: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                elems.push(e);
            }
            let idx: usize = leitor.entrada().unwrap().trim().parse().unwrap();
            // In C++, elements are inserted at head: list order is reverse of input
            // V[i] points to the i-th inserted element (which is list position n-1-i)
            // Removing V[i-1] means removing the (i-1)-th inserted element
            // In list order that's position n-1-(i-1) = n-i
            // But we track by insertion order, so just remove elems[idx-1]
            // Actually: the C++ code does Insere(L,e); V[i] = L;
            // So V[0] = first inserted, V[1] = second inserted, etc.
            // The list in traversal order is: V[n-1] -> V[n-2] -> ... -> V[0]
            // Remove V[i-1] = remove insertion index (idx-1)
            // After removal, list order (reversed remaining) should be printed
            elems.remove(idx - 1);
            // Print in list traversal order (which is reverse of insertion order)
            print!("[ ");
            for i in (0..elems.len()).rev() {
                print!("{} ", elems[i]);
            }
            println!("]");
        } else {
            let n_test: usize = 100000;
            let mut elems: Vec<i32> = Vec::with_capacity(n_test);
            for i in 0..n_test {
                elems.push((i % 100) as i32);
            }
            // Remove 10 elements at positions 0, 9000, 18000, ...
            let mut to_remove: Vec<usize> = Vec::new();
            for i in 0..10 {
                to_remove.push(i * 9000);
            }
            // Remove from highest index to lowest to preserve indices
            to_remove.sort();
            to_remove.reverse();
            for &idx in &to_remove {
                if idx < elems.len() {
                    elems.remove(idx);
                }
            }
            print!("[ ");
            for i in (0..elems.len()).rev() {
                print!("{} ", elems[i]);
            }
            println!("]");
        }
    }
}
