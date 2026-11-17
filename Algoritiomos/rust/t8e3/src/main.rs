use bib1::Leitor;

fn heapify(l: &mut [i32], n: usize, i: usize) {
    let mut largest = i;
    let left = 2 * i + 1;
    let right = 2 * i + 2;
    if left < n && l[left] > l[largest] {
        largest = left;
    }
    if right < n && l[right] > l[largest] {
        largest = right;
    }
    if largest != i {
        l.swap(i, largest);
        heapify(l, n, largest);
    }
}

fn ordenar(l: &mut [i32]) {
    let n = l.len();
    // Build max heap
    for i in (0..n / 2).rev() {
        heapify(l, n, i);
    }
    // Extract elements one by one
    for i in (1..n).rev() {
        l.swap(0, i);
        heapify(l, i, 0);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            ordenar(&mut l);
            for v in &l {
                print!("{} ", v);
            }
            println!();
        } else {
            let n = 100000;
            let mut l: Vec<i32> = Vec::with_capacity(n);
            for i in 0..n {
                l.push((n - i) as i32);
            }
            ordenar(&mut l);
            for v in &l {
                print!("{} ", v);
            }
            println!();
        }
    }
}
