use bib1::Leitor;

struct No {
    e: i64,
    prox: Option<Box<No>>,
}

// Iterative drop to avoid stack overflow on deep lists
impl Drop for No {
    fn drop(&mut self) {
        let mut curr = self.prox.take();
        while let Some(mut node) = curr {
            curr = node.prox.take();
        }
    }
}

fn avalia_polinomio(p: &Option<Box<No>>, x: i64) -> i64 {
    let mut result: i64 = 0;
    let mut curr = p;
    while let Some(ref node) = curr {
        result = result * x + node.e;
        curr = &node.prox;
    }
    result
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let line = match leitor.entrada() {
            Some(l) => l,
            None => break,
        };
        let n: i32 = line.trim().parse().unwrap();

        if n >= 0 {
            let line = leitor.entrada().unwrap();
            let coeffs: Vec<i64> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
            let x_line = leitor.entrada().unwrap();
            let x: i64 = x_line.trim().parse().unwrap();

            let head = build_list(&coeffs);
            println!("{}", avalia_polinomio(&head, x));
        } else {
            // automatic test: n=10000000, all coeffs=1, x=1
            let n = 10_000_000usize;
            let coeffs: Vec<i64> = vec![1; n + 1];
            let head = build_list(&coeffs);
            println!("{}", avalia_polinomio(&head, 1));
        }
    }
}

fn build_list(vals: &[i64]) -> Option<Box<No>> {
    let mut head: Option<Box<No>> = None;
    let mut tail: Option<*mut No> = None;
    for &e in vals {
        let novo = Box::new(No { e, prox: None });
        let ptr: *mut No = &*novo as *const No as *mut No;
        unsafe {
            if let Some(t) = tail {
                (*t).prox = Some(novo);
            } else {
                head = Some(novo);
            }
        }
        tail = Some(ptr);
    }
    head
}
