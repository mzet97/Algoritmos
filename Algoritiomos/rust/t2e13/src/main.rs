use bib1::Leitor;

// ParesIguais - count pairs (x,y) where x in A, y in B, x==y
// Both arrays sorted. O(n).
fn pares_iguais(a: &[i32], b: &[i32]) -> i64 {
    let mut count: i64 = 0;
    let mut i = 0;
    let mut j = 0;
    while i < a.len() && j < b.len() {
        if a[i] == b[j] {
            // Count how many of each value
            let va = a[i];
            let vb = b[j];
            let mut ca: i64 = 0;
            let mut cb: i64 = 0;
            while i < a.len() && a[i] == va {
                ca += 1;
                i += 1;
            }
            while j < b.len() && b[j] == vb {
                cb += 1;
                j += 1;
            }
            count += ca * cb;
        } else if a[i] < b[j] {
            i += 1;
        } else {
            j += 1;
        }
    }
    count
}

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n > -1 {
            let mut a: Vec<i32> = Vec::with_capacity(max_n as usize);
            let mut b: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                a.push(e);
            }
            for _ in 0..max_n {
                let e: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
                b.push(e);
            }
            println!("{}", pares_iguais(&a, &b));
        } else {
            let nlin = max_n;
            let max_n_test = 200000;
            let mut a: Vec<i32> = Vec::with_capacity(max_n_test);
            let mut b: Vec<i32> = Vec::with_capacity(max_n_test);
            if nlin == -1 {
                for i in 0..max_n_test {
                    a.push(i as i32);
                    b.push(i as i32);
                }
            } else if nlin == -2 {
                for _ in 0..max_n_test {
                    a.push(1);
                    b.push(1);
                }
            } else {
                for i in 0..(max_n_test / 10000) {
                    for _ in 0..10000 {
                        a.push(i as i32);
                        b.push(i as i32);
                    }
                }
            }
            println!("{}", pares_iguais(&a, &b));
        }
    }
}
