use bib1::Leitor;

fn ordenacao_por_contagem(l: &mut [i32]) {
    if l.is_empty() {
        return;
    }
    let max_val = *l.iter().max().unwrap();
    let min_val = *l.iter().min().unwrap();
    let range = (max_val - min_val + 1) as usize;
    let mut count = vec![0i32; range];
    for &x in l.iter() {
        count[(x - min_val) as usize] += 1;
    }
    let mut idx = 0;
    for i in 0..range {
        for _ in 0..count[i] {
            l[idx] = (i as i32) + min_val;
            idx += 1;
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let max_n: i32 = line.trim().parse().unwrap();
        if max_n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(max_n as usize);
            for _ in 0..max_n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            ordenacao_por_contagem(&mut l);
            for (i, v) in l.iter().enumerate() {
                if i > 0 {
                    print!(",");
                }
                print!("{}", v);
            }
            println!();
        } else {
            let max_n = 500000;
            let mut l: Vec<i32> = Vec::with_capacity(max_n);
            let mut i = max_n;
            while i > 0 {
                l.push(i as i32);
                l.push(i as i32);
                i -= 2;
            }
            ordenacao_por_contagem(&mut l);
            for (i, v) in l.iter().enumerate() {
                if i > 0 {
                    print!(",");
                }
                print!("{}", v);
            }
            println!();
        }
    }
}
