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

fn soma_polinomios(p: &Option<Box<No>>, q: &Option<Box<No>>) -> Option<Box<No>> {
    let mut head: Option<Box<No>> = None;
    let mut tail: Option<*mut No> = None;
    let mut pp = p;
    let mut qq = q;

    while pp.is_some() || qq.is_some() {
        let val = match (pp, qq) {
            (Some(ref pn), Some(ref qn)) => {
                let v = pn.e + qn.e;
                pp = &pn.prox;
                qq = &qn.prox;
                v
            }
            (Some(ref pn), None) => {
                let v = pn.e;
                pp = &pn.prox;
                v
            }
            (None, Some(ref qn)) => {
                let v = qn.e;
                qq = &qn.prox;
                v
            }
            _ => unreachable!(),
        };

        let novo = Box::new(No { e: val, prox: None });
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
        let parts: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
        let n1 = parts[0];

        if n1 >= 0 {
            let _n2 = parts[1];
            let line = leitor.entrada().unwrap();
            let coeffs_p: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
            let line = leitor.entrada().unwrap();
            let coeffs_q: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();

            let p = build_list(&coeffs_p);
            let q = build_list(&coeffs_q);
            let r = soma_polinomios(&p, &q);
            escreve(&r);
        } else {
            let mut coeffs_p: Vec<i32> = vec![0];
            coeffs_p.extend(vec![1; 1_000_000]);
            let mut coeffs_q: Vec<i32> = vec![1];
            coeffs_q.extend(vec![-1; 1_000_000]);

            let p = build_list(&coeffs_p);
            let q = build_list(&coeffs_q);
            let r = soma_polinomios(&p, &q);
            escreve(&r);
        }
    }
}
