// Aula 23d Rust — quorum de escrita e leitura com reparo.
//
// Sobre a base da 23b (idempotencia, log duravel): aqui a escrita exige
// maioria antes de existir para o cliente, a leitura consulta a maioria e
// a atrasada e reparada no caminho feliz. Simulacao deterministica em um
// processo, so biblioteca padrao.
//
// Compilar: rustc --edition 2024 -O -D warnings teoria/rust/exemplos/23d_consenso.rs -o /tmp/23d_rs
// Executar: /tmp/23d_rs
use std::collections::HashMap;

const NOS: usize = 3;
const QUORUM: usize = 2;

#[derive(Clone, Debug, PartialEq)]
struct Entrada {
    valor: String,
    relogio: [u64; NOS],
}

struct Replica {
    viva: bool,
    dados: HashMap<String, Entrada>,
}

// a domina b se e >= em tudo e > em algo.
fn domina(a: &[u64; NOS], b: &[u64; NOS]) -> bool {
    let mut maior = false;
    for i in 0..NOS {
        if a[i] < b[i] {
            return false;
        }
        maior = maior || a[i] > b[i];
    }
    maior
}

fn escrever(replicas: &mut [Replica; NOS], coord: usize, chave: &str, valor: &str) -> bool {
    let mut acks = 0;
    for (i, r) in replicas.iter_mut().enumerate() {
        if !r.viva {
            continue;
        }
        let mut e = r.dados.get(chave).cloned().unwrap_or(Entrada {
            valor: String::new(),
            relogio: [0; NOS],
        });
        if i == coord {
            e.relogio[i] += 1;
        }
        e.valor = valor.to_string();
        r.dados.insert(chave.to_string(), e);
        acks += 1;
    }
    acks >= QUORUM
}

enum Leitura {
    Limpa(Entrada),
    Conflito,
    SemQuorum,
}

fn ler(replicas: &mut [Replica; NOS], chave: &str) -> Leitura {
    // Primeira passada: so leitura (o borrow checker proibe reparar aqui).
    let mut respostas: Vec<Entrada> = Vec::new();
    for r in replicas.iter() {
        if !r.viva {
            continue;
        }
        if let Some(e) = r.dados.get(chave) {
            respostas.push(e.clone());
        }
    }
    if respostas.len() < QUORUM {
        return Leitura::SemQuorum;
    }
    let mut melhor = respostas[0].clone();
    let mut conflito = false;
    for e in respostas.iter().skip(1) {
        if e.relogio == melhor.relogio {
            continue; // mesma versao replicada: nem domina, nem conflita
        } else if domina(&e.relogio, &melhor.relogio) {
            melhor = e.clone();
        } else if !domina(&melhor.relogio, &e.relogio) {
            conflito = true;
        }
    }
    if conflito {
        return Leitura::Conflito;
    }
    // Segunda passada: reparo — agora sim com &mut, sem borrow ativo.
    for r in replicas.iter_mut() {
        if !r.viva {
            continue;
        }
        let precisa = match r.dados.get(chave) {
            Some(e) => e.relogio != melhor.relogio,
            None => true,
        };
        if precisa {
            r.dados.insert(chave.to_string(), melhor.clone());
        }
    }
    Leitura::Limpa(melhor)
}

fn main() {
    let mut replicas: [Replica; NOS] = [
        Replica { viva: true, dados: HashMap::new() },
        Replica { viva: true, dados: HashMap::new() },
        Replica { viva: true, dados: HashMap::new() },
    ];

    assert!(escrever(&mut replicas, 0, "k", "111"));
    match ler(&mut replicas, "k") {
        Leitura::Limpa(e) => assert_eq!(e.valor, "111"),
        _ => panic!("fase1: leitura deveria ser limpa"),
    }
    println!("fase1 quorum limpo: k=111");

    replicas[2].viva = false;
    assert!(escrever(&mut replicas, 0, "k", "222"));
    replicas[0].viva = false;
    match ler(&mut replicas, "k") {
        Leitura::SemQuorum => println!("fase2 minoria sem quorum: escrita e leitura recusadas"),
        _ => panic!("fase2: minoria de 1 nao pode ler por quorum"),
    }
    replicas[0].viva = true;

    replicas[2].viva = true;
    match ler(&mut replicas, "k") {
        Leitura::Limpa(e) => assert_eq!(e.valor, "222"),
        _ => panic!("fase3: cura deveria convergir em 222"),
    }
    assert_eq!(replicas[2].dados["k"].valor, "222");
    println!("fase3 pos-cura: k=222, atrasada reparada na leitura");
    println!("23d ok: escrita exige maioria, leitura encontra, reparo converge");
}
