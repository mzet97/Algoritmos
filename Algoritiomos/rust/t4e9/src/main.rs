use bib1::Leitor;

fn ultimo(l: &Option<Box<No>>) -> i32 {
    match l {
        None => -1,
        Some(node) => {
            if node.prox.is_none() {
                node.e
            } else {
                ultimo(&node.prox)
            }
        }
    }
}

struct No {
    e: i32,
    prox: Option<Box<No>>,
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
                println!("{}", ultimo(&l));
            }
        } else {
            for i in 1..=50000 {
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
            for _ in 1..=100 { ultimo(&l); }
            println!("{}", ultimo(&l));
        }
    }
}
