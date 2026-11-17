use bib1::Leitor;

struct No {
    e: i32,
    prox: Option<Box<No>>,
}

fn k_ultimo(l: &Option<Box<No>>, k: i32) -> i32 {
    let len = contar(l);
    let target = len - k;
    buscar(l, target, 0)
}

fn contar(l: &Option<Box<No>>) -> i32 {
    match l {
        None => 0,
        Some(node) => 1 + contar(&node.prox),
    }
}

fn buscar(l: &Option<Box<No>>, target: i32, idx: i32) -> i32 {
    match l {
        Some(node) => {
            if idx == target {
                node.e
            } else {
                buscar(&node.prox, target, idx + 1)
            }
        }
        None => -1,
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut l: Option<Box<No>> = None;
    let mut tail: Option<*mut No> = None;
    while let Some(line) = leitor.entrada() {
        if line != "a" {
            if line.starts_with('i') {
                let Some(l2) = leitor.entrada() else { break };
                let e: i32 = l2.trim().parse().unwrap();
                let mut new_node = Box::new(No { e, prox: None });
                let new_ptr: *mut No = &mut *new_node;
                if l.is_none() {
                    l = Some(new_node);
                } else {
                    unsafe {
                        if let Some(t) = tail {
                            (*t).prox = Some(new_node);
                        }
                    }
                }
                tail = Some(new_ptr);
            } else {
                let Some(lk) = leitor.entrada() else { break };
                let k: i32 = lk.trim().parse().unwrap();
                println!("{}", k_ultimo(&l, k));
            }
        } else {
            let n = 50000;
            for i in 1..=n {
                let mut new_node = Box::new(No { e: i, prox: None });
                let new_ptr: *mut No = &mut *new_node;
                if l.is_none() {
                    l = Some(new_node);
                } else {
                    unsafe {
                        if let Some(t) = tail {
                            (*t).prox = Some(new_node);
                        }
                    }
                }
                tail = Some(new_ptr);
            }
            for _ in 1..=100 { k_ultimo(&l, n / 2); }
            println!("{}", k_ultimo(&l, n / 2));
        }
    }
}
