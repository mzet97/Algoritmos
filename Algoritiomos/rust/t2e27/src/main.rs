use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

impl Drop for No {
    fn drop(&mut self) {
        let mut curr = self.prox.take();
        while let Some(mut node) = curr {
            curr = node.prox.take();
        }
    }
}

fn produto_polinomio_escalar(p: &Option<Box<No>>, a: i32) -> Option<Box<No>> {
    let mut head: Option<Box<No>> = None;
    let mut tail: Option<*mut No> = None;
    let mut curr = p;

    while let Some(ref node) = curr {
        let novo = Box::new(No { e: node.e * a, prox: None });
        let ptr: *mut No = &*novo as *const No as *mut No;
        unsafe {
            if let Some(t) = tail {
                (*t).prox = Some(novo);
            } else {
                head = Some(novo);
            }
        }
        tail = Some(ptr);
        curr = &node.prox;
    }

    head
}

fn escreve(l: &Option<Box<No>>) {
    print!("[");
    let mut p = l;
    while let Some(ref node) = p {
        print!("{} ", node.e);
        p = &node.prox;
    }
    println!("]");
}

fn build_list(vals: &[i32]) -> Option<Box<No>> {
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
            let coeffs: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
            let line = leitor.entrada().unwrap();
            let a: i32 = line.trim().parse().unwrap();

            let p = build_list(&coeffs);
            let r = produto_polinomio_escalar(&p, a);
            escreve(&r);
        } else {
            let n = 500_000usize;
            let coeffs: Vec<i32> = vec![1; n + 1];
            let p = build_list(&coeffs);
            let r = produto_polinomio_escalar(&p, 2);
            escreve(&r);
        }
    }
}
