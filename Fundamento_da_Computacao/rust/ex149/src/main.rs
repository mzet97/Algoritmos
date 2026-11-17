use bib1::Leitor;

fn polinomio(n: usize, a: &[i64], k: i64) -> i64 {
    // f(x) = a[N]*x^N + a[N-1]*x^(N-1) + ... + a[1]*x + a[0]
    // Use Horner's method
    let mut result: i64 = 0;
    for i in (0..=n).rev() {
        result = result * k + a[i];
    }
    result
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let n: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        if n >= 0 {
            let n = n as usize;
            let mut a: Vec<i64> = Vec::with_capacity(n + 1);
            let mut ok = true;
            for _ in 0..=n {
                match leitor.entrada() {
                    Some(s) => match s.parse::<i64>() {
                        Ok(v) => a.push(v),
                        Err(_) => {
                            ok = false;
                            break;
                        }
                    },
                    None => {
                        ok = false;
                        break;
                    }
                }
            }
            if !ok {
                break;
            }
            let k: i64 = match leitor.entrada() {
                Some(s) => match s.parse() {
                    Ok(v) => v,
                    Err(_) => break,
                },
                None => break,
            };
            println!("{}", polinomio(n, &a, k));
        } else {
            // N < 0: special case from Python code: N=100000, a=[1]*(N+1), k=1
            let big_n: usize = 100000;
            let a: Vec<i64> = vec![1; big_n + 1];
            let k: i64 = 1;
            println!("{}", polinomio(big_n, &a, k));
        }
    }
}
