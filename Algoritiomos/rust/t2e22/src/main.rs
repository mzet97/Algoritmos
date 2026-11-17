use bib1::Leitor;

// CabecaCauda - sentinel head/tail list
// Input: n elements for sorted list (with head=3000 and tail=3000 sentinels), then x
// Operations: (i) insert x, (ii) check if 2*x exists, (iii) remove all 3*x
// Output: "0" or "1" (found double), then [elements between head and tail]

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        let mut elems: Vec<i32> = Vec::new();

        if n != -1 {
            // Read n elements (already sorted, inserted between head and tail)
            for _ in 0..n {
                let x: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                elems.push(x);
            }
            let x: i32 = leitor.entrada().unwrap().trim().parse().unwrap();

            // (i) Insert x
            elems.push(x);
            elems.sort();

            // (ii) Check if 2*x exists
            let double_x = x * 2;
            let found = elems.contains(&double_x);

            // (iii) Remove all elements equal to 3*x
            let triple_x = x * 3;
            elems.retain(|&v| v != triple_x);

            print!("{} ", found as i32);
            print!("[");
            for v in &elems {
                print!("{} ", v);
            }
            println!("]");
        } else {
            // automatic test
            let n_test: usize = 500000;
            for i in 0..n_test {
                elems.push(i as i32);
            }
            let x: i32 = 300000;

            elems.push(x);
            elems.sort();

            let double_x = x * 2;
            let found = elems.contains(&double_x);

            let triple_x = x * 3;
            elems.retain(|&v| v != triple_x);

            print!("{} ", found as i32);
            print!("[");
            for v in &elems {
                print!("{} ", v);
            }
            println!("]");
        }
    }
}
