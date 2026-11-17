use bib1::Leitor;

fn ler_escrever_k_ultimos(k: i32, nums: &[i32]) {
    let start = if nums.len() > k as usize {
        nums.len() - k as usize
    } else {
        0
    };
    for i in start..nums.len() {
        print!("{} ", nums[i]);
    }
    println!();
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let k: i32 = line.trim().parse().unwrap();
        let mut nums: Vec<i32> = Vec::new();
        loop {
            let Some(l) = leitor.entrada() else { break };
            let x: i32 = l.trim().parse().unwrap();
            if x == 0 {
                break;
            }
            nums.push(x);
        }
        ler_escrever_k_ultimos(k, &nums);
    }
}
