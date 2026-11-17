use bib1::Leitor;

struct No {
    col: i32,
    e: i32,
    prox: Option<Box<No>>,
}

struct MatrizEsparsa {
    n: usize,
    linhas: Vec<Option<Box<No>>>,
}

fn new_linhas(n: usize) -> Vec<Option<Box<No>>> {
    (0..n).map(|_| None).collect()
}

fn ao_quadrado(m: &MatrizEsparsa) -> MatrizEsparsa {
    let n = m.n;
    let mut res: Vec<Vec<i64>> = vec![vec![0i64; n]; n];

    for i in 0..n {
        let mut pk = &m.linhas[i];
        while let Some(ref node_k) = pk {
            let k = node_k.col as usize;
            let m_ik = node_k.e as i64;
            let mut pj = &m.linhas[k];
            while let Some(ref node_j) = pj {
                let j = node_j.col as usize;
                let m_kj = node_j.e as i64;
                res[i][j] += m_ik * m_kj;
                pj = &node_j.prox;
            }
            pk = &node_k.prox;
        }
    }

    let mut linhas = new_linhas(n);
    for i in 0..n {
        for j in 0..n {
            if res[i][j] != 0 {
                let novo = Box::new(No {
                    col: j as i32,
                    e: res[i][j] as i32,
                    prox: linhas[i].take(),
                });
                linhas[i] = Some(novo);
            }
        }
        linhas[i] = reverse_list(linhas[i].take());
    }

    MatrizEsparsa { n, linhas }
}

fn reverse_list(mut head: Option<Box<No>>) -> Option<Box<No>> {
    let mut prev: Option<Box<No>> = None;
    while let Some(mut curr) = head {
        head = curr.prox.take();
        curr.prox = prev;
        prev = Some(curr);
    }
    prev
}

fn escreve(m: &MatrizEsparsa) {
    print!("[");
    for i in 0..m.n {
        print!("[");
        let mut p = &m.linhas[i];
        while let Some(ref node) = p {
            print!("c={};e={} ", node.col, node.e);
            p = &node.prox;
        }
        print!("]");
    }
    println!("]");
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let line = match leitor.entrada() {
            Some(l) => l,
            None => break,
        };
        let parts: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
        let n_val = parts[0];

        if n_val >= 0 {
            let k = parts[1] as usize;
            let n = n_val as usize;
            let mut linhas = new_linhas(n);
            let mut tails: Vec<Option<*mut No>> = vec![None; n];

            for _ in 0..k {
                let line = leitor.entrada().unwrap();
                let vals: Vec<i32> = line.split_whitespace().map(|x| x.parse().unwrap()).collect();
                let l = vals[0] as usize;
                let c = vals[1];
                let e = vals[2];

                let novo = Box::new(No { col: c, e, prox: None });
                let novo_ptr: *mut No = &*novo as *const No as *mut No;
                unsafe {
                    if let Some(tail_ptr) = tails[l] {
                        (*tail_ptr).prox = Some(novo);
                    } else {
                        linhas[l] = Some(novo);
                    }
                }
                tails[l] = Some(novo_ptr);
            }

            let m = MatrizEsparsa { n, linhas };
            let m2 = ao_quadrado(&m);
            escreve(&m2);
        } else {
            // automatic test: 1000x1000 with 10x10 block of 1s
            let n = 1000usize;
            let mut linhas = new_linhas(n);
            let mut tails: Vec<Option<*mut No>> = vec![None; n];
            for l in 0..10usize {
                for c in 0..10i32 {
                    let novo = Box::new(No { col: c, e: 1, prox: None });
                    let novo_ptr: *mut No = &*novo as *const No as *mut No;
                    unsafe {
                        if let Some(tail_ptr) = tails[l] {
                            (*tail_ptr).prox = Some(novo);
                        } else {
                            linhas[l] = Some(novo);
                        }
                    }
                    tails[l] = Some(novo_ptr);
                }
            }
            let m = MatrizEsparsa { n, linhas };
            let m2 = ao_quadrado(&m);
            escreve(&m2);
        }
    }
}
