use bib1::Leitor;

fn manobrar_vagoes(n: usize, v: &[i32]) {
    let mut stack: Vec<i32> = Vec::new();
    let mut queue: Vec<i32> = Vec::new();
    let mut j = 0;
    for i in 0..n {
        stack.push(v[i]);
        while !stack.is_empty() && j < n && *stack.last().unwrap() == (j as i32 + 1) {
            queue.push(stack.pop().unwrap());
            j += 1;
        }
    }
    for x in &queue {
        print!("{} ", x);
    }
    if !queue.is_empty() {
        println!();
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let n = n as usize;
            let mut v: Vec<i32> = Vec::new();
            for _ in 0..n {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                v.push(x);
            }
            manobrar_vagoes(n, &v);
        } else {
            let n = 1000;
            let mut v = vec![0i32; n];
            let mut m = n;
            let mut mm = 1;
            let mut i = n;
            while i >= 2 {
                i -= 1;
                v[i] = m as i32;
                i -= 1;
                v[i] = mm as i32;
                m -= 1;
                mm += 1;
            }
            manobrar_vagoes(n, &v);
        }
    }
}
