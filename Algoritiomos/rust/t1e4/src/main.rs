use bib1::Leitor;

fn qtde_primos(n: i32) -> i32 {
    if n < 2 {
        return 0;
    }
    let mut count = 0;
    for i in 2..=n {
        let mut primo = true;
        let mut j = 2;
        while j * j <= i {
            if i % j == 0 {
                primo = false;
                break;
            }
            j += 1;
        }
        if primo {
            count += 1;
        }
    }
    count
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        println!("{}", qtde_primos(n));
    }
}
