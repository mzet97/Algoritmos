use bib1::Leitor;

struct NoSeq {
    chave: i32,
    e: i32,
}

struct NoEnc {
    chave: i32,
    e: i32,
    prox: Option<Box<NoEnc>>,
}

fn converter_enc_para_seq(l: &Option<Box<NoEnc>>) -> Vec<NoSeq> {
    let mut result = Vec::new();
    let mut p: Option<&NoEnc> = l.as_ref().map(|b| b.as_ref());
    while let Some(no) = p {
        result.push(NoSeq { chave: no.chave, e: no.e });
        p = no.prox.as_ref().map(|b| b.as_ref());
    }
    result
}

fn converter_seq_para_enc(seq: &[NoSeq]) -> Option<Box<NoEnc>> {
    let mut head: Option<Box<NoEnc>> = None;
    for ns in seq.iter().rev() {
        let novo = Box::new(NoEnc {
            chave: ns.chave,
            e: ns.e,
            prox: head,
        });
        head = Some(novo);
    }
    head
}

fn escreve_enc(l: &Option<Box<NoEnc>>) {
    print!("[");
    let mut p: Option<&NoEnc> = l.as_ref().map(|b| b.as_ref());
    while let Some(no) = p {
        print!("c={};e={} ", no.chave, no.e);
        p = no.prox.as_ref().map(|b| b.as_ref());
    }
    println!("]");
}

fn main() {
    let mut leitor = Leitor::new();

    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            // Read n pairs (chave, elemento)
            let mut elems: Vec<(i32, i32)> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let pair_line = leitor.entrada().unwrap();
                let parts: Vec<i32> = pair_line.split_whitespace()
                    .map(|x| x.parse().unwrap())
                    .collect();
                elems.push((parts[0], parts[1]));
            }
            // Build linked list using raw pointers for tail tracking
            let mut head: Option<Box<NoEnc>> = None;
            let mut tail_ptr: Option<*mut NoEnc> = None;
            for (c, e) in elems {
                let novo = Box::new(NoEnc { chave: c, e, prox: None });
                let ptr: *mut NoEnc = &*novo as *const NoEnc as *mut NoEnc;
                unsafe {
                    if let Some(t) = tail_ptr {
                        (*t).prox = Some(novo);
                    } else {
                        head = Some(novo);
                    }
                }
                tail_ptr = Some(ptr);
            }
            let seq = converter_enc_para_seq(&head);
            let enc2 = converter_seq_para_enc(&seq);
            escreve_enc(&enc2);
        } else {
            // automatic test
            let n_test: usize = 500000;
            let mut head: Option<Box<NoEnc>> = None;
            let mut tail_ptr: Option<*mut NoEnc> = None;
            for i in 0..n_test {
                let novo = Box::new(NoEnc { chave: i as i32, e: (n_test + i) as i32, prox: None });
                let ptr: *mut NoEnc = &*novo as *const NoEnc as *mut NoEnc;
                unsafe {
                    if let Some(t) = tail_ptr {
                        (*t).prox = Some(novo);
                    } else {
                        head = Some(novo);
                    }
                }
                tail_ptr = Some(ptr);
            }
            let seq = converter_enc_para_seq(&head);
            let enc2 = converter_seq_para_enc(&seq);
            escreve_enc(&enc2);
        }
    }
}
