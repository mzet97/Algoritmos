use bib1::Leitor;

// ElementoMatriz O(n log n) - search sorted matrix
// Each row and column sorted. Binary search each row: O(n log n)
fn busca(b: &Vec<Vec<i32>>, n: usize, x: i32) -> bool {
    for i in 0..n {
        // Binary search in row i
        let mut lo = 0usize;
        let mut hi = n;
        while lo < hi {
            let mid = (lo + hi) / 2;
            if b[i][mid] == x {
                return true;
            } else if b[i][mid] < x {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
    }
    false
}

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n > -1 {
            let mut b: Vec<Vec<i32>> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let row_line = leitor.entrada().unwrap();
                let row: Vec<i32> = row_line.split_whitespace()
                    .map(|x| x.parse().unwrap())
                    .collect();
                b.push(row);
            }
            let x: i32 = leitor.entrada().unwrap().trim().parse().unwrap();
            println!("{}", busca(&b, n as usize, x) as i32);
        } else {
            let nlin = n;
            let n_test: usize = 16000;
            let mut b = vec![vec![0i32; n_test]; n_test];
            if nlin == -1 || nlin == -2 {
                for i in 0..n_test {
                    for j in 0..n_test {
                        b[i][j] = i as i32;
                    }
                }
                b[n_test - 1][n_test - 2] = n_test as i32;
                b[n_test - 1][n_test - 1] = (n_test + 1) as i32;
                if nlin == -1 {
                    println!("{}", busca(&b, n_test, n_test as i32) as i32);
                } else {
                    println!("{}", busca(&b, n_test, (n_test + 2) as i32) as i32);
                }
            } else if nlin == -3 || nlin == -4 {
                for i in 0..n_test {
                    for j in 0..n_test {
                        b[i][j] = j as i32;
                    }
                }
                b[n_test - 1][n_test - 2] = n_test as i32;
                b[n_test - 1][n_test - 1] = (n_test + 1) as i32;
                if nlin == -3 {
                    println!("{}", busca(&b, n_test, n_test as i32) as i32);
                } else {
                    println!("{}", busca(&b, n_test, (n_test + 2) as i32) as i32);
                }
            } else {
                for i in 0..n_test {
                    for j in 0..n_test {
                        b[i][j] = 3;
                    }
                }
                b[0][1] = 2;
                b[0][0] = 1;
                if nlin == -5 {
                    println!("{}", busca(&b, n_test, 2) as i32);
                } else {
                    println!("{}", busca(&b, n_test, 0) as i32);
                    for _ in 0..10 {
                        busca(&b, n_test, 0);
                    }
                }
            }
        }
    }
}
