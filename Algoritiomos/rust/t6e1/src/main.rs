use bib1::Leitor;

fn pares_fora_ordem(l: &[i32]) -> i64 {
    fn merge_count(l: &[i32], aux: &mut [i32]) -> i64 {
        let n = l.len();
        if n <= 1 {
            return 0;
        }
        let mid = n / 2;
        let mut count = merge_count(&l[..mid], &mut aux[..mid]);
        count += merge_count(&l[mid..], &mut aux[mid..]);
        let mut i = 0;
        let mut j = mid;
        let mut k = 0;
        while i < mid && j < n {
            if l[i] <= l[j] {
                aux[k] = l[i];
                i += 1;
            } else {
                aux[k] = l[j];
                count += (mid - i) as i64;
                j += 1;
            }
            k += 1;
        }
        while i < mid {
            aux[k] = l[i];
            i += 1;
            k += 1;
        }
        while j < n {
            aux[k] = l[j];
            j += 1;
            k += 1;
        }
        // copy back is not needed since we work with slices
        count
    }
    let mut aux = vec![0i32; l.len()];
    let l_mut = l.to_vec();
    merge_count(&l_mut, &mut aux)
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            println!("{}", pares_fora_ordem(&l));
        } else {
            let max_n = 50000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            for i in (1..=max_n).rev() {
                l.push(i as i32);
            }
            println!("{}", pares_fora_ordem(&l));
        }
    }
}
