use bib1::Leitor;
use std::collections::VecDeque;

fn alternar_pos_neg(num: &mut [i32]) {
    let mut pos: VecDeque<i32> = VecDeque::new();
    let mut neg: VecDeque<i32> = VecDeque::new();
    for &x in num.iter() {
        if x >= 0 {
            pos.push_back(x);
        } else {
            neg.push_back(x);
        }
    }
    let mut i = 0;
    while !pos.is_empty() || !neg.is_empty() {
        if let Some(v) = pos.pop_front() {
            num[i] = v;
            i += 1;
        }
        if let Some(v) = neg.pop_front() {
            num[i] = v;
            i += 1;
        }
    }
    for j in 0..num.len() {
        print!("{} ", num[j]);
    }
    println!();
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n > 0 {
            let mut nums: Vec<i32> = Vec::new();
            for _ in 0..n {
                let Some(l) = leitor.entrada() else { break };
                let x: i32 = l.trim().parse().unwrap();
                nums.push(x);
            }
            alternar_pos_neg(&mut nums);
        } else if n == -1 {
            let n = 100000;
            let mut nums: Vec<i32> = Vec::new();
            let mut s = 1;
            for i in 0..n {
                nums.push(s * (i + 1));
                s = -s;
            }
            alternar_pos_neg(&mut nums);
        } else if n == -2 {
            let n = 100000;
            let mut nums: Vec<i32> = Vec::new();
            for i in 0..n / 2 {
                nums.push((i + 1) as i32);
            }
            for i in n / 2..n {
                nums.push(-((i + 1) as i32));
            }
            alternar_pos_neg(&mut nums);
        } else {
            let n = 100000;
            let mut nums: Vec<i32> = Vec::new();
            for i in 0..n / 2 {
                nums.push(-((i + 1) as i32));
            }
            for i in n / 2..n {
                nums.push((i + 1) as i32);
            }
            alternar_pos_neg(&mut nums);
        }
    }
}
