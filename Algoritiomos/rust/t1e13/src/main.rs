use std::io;

struct E1 {
    nome: Vec<char>,
    idade: i32,
}

struct E2 {
    turma: i32,
    aluno: *const E1,
    prox: Option<Box<E2>>,
}

fn testar_memoria(v: &[E1], l: &E2, t: i32) {
    match t {
        1 => println!("{} {} {}", v[0].idade, v[1].idade, v[2].idade),
        2 => println!("{} {} {} {}", v[0].nome[0], v[0].nome[1], v[0].nome[2], v[0].nome[3]),
        3 => println!("{} {} {}", v[1].nome[0], v[1].nome[1], v[1].nome[2]),
        4 => println!("{} {} {}", v[2].nome[0], v[2].nome[1], v[2].nome[2]),
        5 => {
            let t1 = l.turma;
            let t2 = l.prox.as_ref().unwrap().turma;
            let t3 = l.prox.as_ref().unwrap().prox.as_ref().unwrap().turma;
            let t4 = l.prox.as_ref().unwrap().prox.as_ref().unwrap().prox.as_ref().unwrap().turma;
            println!("{} {} {} {}", t1, t2, t3, t4);
        }
        6 => {
            let n = l.prox.as_ref().unwrap()
                .prox.as_ref().unwrap()
                .prox.as_ref().unwrap()
                .prox.is_none();
            println!("{}", if n { 1 } else { 0 });
        }
        7 => {
            unsafe {
                let a0 = (*l.aluno).idade;
                let a1 = (*l.prox.as_ref().unwrap().aluno).idade;
                let a2 = (*l.prox.as_ref().unwrap().prox.as_ref().unwrap().aluno).idade;
                let a3 = (*l.prox.as_ref().unwrap().prox.as_ref().unwrap().prox.as_ref().unwrap().aluno).idade;
                println!("{} {} {} {}", a0, a1, a2, a3);
            }
        }
        _ => {}
    }
}

fn main() {
    let v: Vec<E1> = vec![
        E1 { nome: vec!['J', 'O', 'A', 'O'], idade: 5 },
        E1 { nome: vec!['L', 'E', 'O'], idade: 8 },
        E1 { nome: vec!['L', 'I', 'A'], idade: 9 },
    ];

    let n4 = Box::new(E2 { turma: 2, aluno: &v[2] as *const E1, prox: None });
    let n3 = Box::new(E2 { turma: 2, aluno: &v[1] as *const E1, prox: Some(n4) });
    let n2 = Box::new(E2 { turma: 1, aluno: &v[1] as *const E1, prox: Some(n3) });
    let l = E2 { turma: 1, aluno: &v[0] as *const E1, prox: Some(n2) };

    let stdin = io::stdin();
    for line in stdin.lock().lines() {
        let line = line.unwrap();
        let line = line.trim();
        if line.is_empty() { continue; }
        let t: i32 = line.parse().unwrap();
        testar_memoria(&v, &l, t);
    }
}

use std::io::BufRead;
