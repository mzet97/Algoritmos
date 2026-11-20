---
layout: default
---

# Aula 16a — Máquina e sistema operacional vistos de Rust seguro

---

## Nesta aula

Explicar o caminho de um programa Rust entre código-fonte, borrow checker, chamadas de sistema e hardware — e investigar cada camada com ferramentas do espaço de usuário, em Rust 100% seguro e só com a biblioteca padrão. Pré-requisitos: aulas 01 a 16. Não precisa saber `unsafe`, `no_std`, Assembly, QEMU nem kernel.

Ao terminar, você deverá criar um processo filho e colher sua saída, medir o custo de uma syscall de leitura, inspecionar o mapa de memória do próprio processo no Linux, comparar heap gerenciado com RSS e medir localidade de acesso — e explicar por que os mesmos números mudam entre máquinas e SOs.

Esta aula é o módulo R4 do mapa: quem domina a observação aqui carrega o modelo para desempenho (17–18), `unsafe` auditável e o kernel didático (R8), onde cada travessia e cada página passam a ter nome, dono e custo.

Você vai aprender a interpretar `/proc/self/maps` e `VmRSS` sem inferir endereços físicos, distinguir medição de tempo de alegação de causa microarquitetural, e apontar o que a `std` observa e não observa.

---

## 1. Processo é isolamento com contabilidade

`std::process::Command` cria um processo filho; `status()` e `output()` colhem saída e código. O `Drop` de `Child` sem `wait` não colhe automaticamente da mesma forma que um `waitpid` explícito — chame `wait`/`status` e documente. Código de saída é `Option<i32>`: sinal, não número, quando o filho morre por sinal — o tipo conta a história que C conta com macros.

## 2. Syscall custa uma travessia — meça uma de verdade

Sem crates externas não há `getpid` direto na biblioteca padrão. A régua honesta desta aula é `read` de 1 byte em `/dev/null`: cada chamada atravessa ao kernel de verdade (sem vDSO que a esconda). Cronometre centenas de milhares e divida: o número inclui a travessia, o trabalho mínimo e o ruído — um *teto* do custo, complementar ao *piso* do `getpid` das trilhas C/C++.

## 3. Memória virtual, RSS e os dois medidores

Como nas outras trilhas: o processo enxerga endereços virtuais; o SO mapeia páginas sob demanda. Sem crates, o RSS vem de `/proc/self/status` (campo `VmRSS`, Linux): leia o arquivo como texto e extraia o número — com recuo honesto fora do Linux. O heap gerenciado aparece indiretamente: aloque um `Vec<u8>` de 8 MiB, toque uma página por vez e observe o RSS subir.

## 4. Localidade: hipótese contra medição

Percorra um vetor de `u64` sequencialmente e depois com passo grande (uma palavra por linha de cache de 64 bytes, na prática passo 8 em `u64`), com o trabalho igualado: 8 passadas de N/8 somas de um lado, N somas do outro. Mesma quantidade de somas, tempos diferentes: a hipótese (localidade espacial) prevê, a medição decide. Sem contadores de hardware, não alegue "faltas de cache" — alegue "padrão de acesso X é N vezes mais lento", que é o que foi medido.

## 5. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/16a_sistema.rs) contém exatamente o programa abaixo. Compile direto com `rustc`, sem `cargo`:

```bash
rustc --edition 2024 teoria/rust/exemplos/16a_sistema.rs -o sistema16a
./sistema16a
```

```rust
use std::fs;
use std::io::Read;
use std::process::Command;
use std::time::Instant;

/// Conta regioes e executaveis em /proc/self/maps (Linux).
/// Fora do Linux, devolve zeros e diz por que.
fn resumir_mapas() -> (usize, usize, &'static str) {
    let texto = fs::read_to_string("/proc/self/maps");
    let texto = match texto {
        Ok(t) => t,
        Err(_) => return (0, 0, "indisponivel fora do Linux"),
    };
    let mut total = 0;
    let mut exec = 0;
    for linha in texto.lines() {
        if linha.is_empty() {
            continue;
        }
        total += 1;
        if linha.contains("r-xp") {
            exec += 1;
        }
    }
    (total, exec, "/proc/self/maps")
}

/// Extrai VmRSS em KiB de /proc/self/status (Linux).
fn rss_kb() -> Option<u64> {
    let texto = fs::read_to_string("/proc/self/status").ok()?;
    for linha in texto.lines() {
        if let Some(resto) = linha.strip_prefix("VmRSS:") {
            return resto.split_whitespace().next()?.parse().ok();
        }
    }
    None
}

/// Regua honesta de syscall: read em /dev/null, N repeticoes.
/// Nota: read devolve Ok(0) (EOF imediato) — e o `read` (nao `read_exact`),
/// pois `read_exact` exigiria 1 byte e falharia com UnexpectedEof.
fn regua_read(repeticoes: u32) -> f64 {
    let mut f = fs::File::open("/dev/null").expect("/dev/null deve abrir");
    let mut b = [0u8; 1];
    let inicio = Instant::now();
    for _ in 0..repeticoes {
        // /dev/null sempre devolve Ok(0): conta a syscall, nao bytes.
        // O assert consome o retorno e cala o clippy::unused_io_amount com razao.
        let n = f.read(&mut b).expect("read em /dev/null nao falha");
        assert_eq!(n, 0, "dev/null mudou de comportamento?");
    }
    inicio.elapsed().as_secs_f64() / f64::from(repeticoes)
}

fn main() {
    let cpus = std::thread::available_parallelism()
        .map(|n| n.get())
        .unwrap_or(1);
    println!("cpus(paralelismo)={cpus} pid={}", std::process::id());

    // 1. Processo filho com codigo de saida conhecido.
    let estado = Command::new("/bin/true")
        .status()
        .expect("falha ao criar /bin/true (fora do Linux, ajuste)");
    assert!(estado.success());
    println!("filho /bin/true: ok");

    // 2. Falha contida: comando inexistente e argumento invalido.
    let ruim = Command::new("/nao/existe/de/jeito/nenhum").output();
    assert!(ruim.is_err(), "comando inexistente deveria falhar no spawn");
    println!("exec falho contido: {}", ruim.unwrap_err());

    // 3. Regua de syscall.
    let rep = 200_000u32;
    let t = regua_read(rep);
    println!("read(/dev/null): {:.1} ns/chamada ({rep} repeticoes)", t * 1e9);

    // 4. Mapa de memoria do proprio processo.
    let (total, exec, origem) = resumir_mapas();
    println!("regioes={total} executaveis={exec} origem={origem}");
    if origem == "/proc/self/maps" {
        assert!(total > 0 && exec > 0, "mapa vazio: invariante violada");
    }

    // 5. RSS antes e depois de 8 MiB tocados pagina a pagina.
    let antes = rss_kb();
    let mut bloco = vec![0u8; 8 << 20];
    for i in (0..bloco.len()).step_by(4096) {
        bloco[i] = 1; // uma pagina nova por iteracao
    }
    let depois = rss_kb();
    println!("rss_kb: {antes:?} -> {depois:?}");
    if let (Some(a), Some(d)) = (antes, depois) {
        assert!(d > a, "RSS deveria crescer com 8 MiB tocados");
    }
    drop(bloco);

    // 6. Localidade: sequencial contra passo 8 (uma palavra por linha de 64 B).
    // Trabalho igualado: 8 passadas de N/8 somas = N somas por repeticao.
    let grande: Vec<u64> = (0..2_000_000u64).collect();
    let n = grande.len();
    let rep = 10;
    let inicio = Instant::now();
    let mut soma = 0u64;
    for _ in 0..rep {
        for &v in &grande {
            soma = soma.wrapping_add(v);
        }
    }
    let t_seq = inicio.elapsed().as_secs_f64() / f64::from(rep);
    let inicio = Instant::now();
    let mut soma2 = 0u64;
    for _ in 0..rep {
        for _passada in 0..8 {
            let mut i = 0;
            while i < n {
                soma2 = soma2.wrapping_add(grande[i]);
                i += 8;
            }
        }
    }
    let t_salto = inicio.elapsed().as_secs_f64() / f64::from(rep);
    println!("soma_seq: {t_seq:.6}s/media soma={soma}");
    println!("soma_salto8: {t_salto:.6}s/media soma={soma2} razao={:.2}x", t_salto / t_seq);
}
```

Saída esperada (números variam por máquina, SO e carga):

```text
cpus(paralelismo)=N pid=PID
filho /bin/true: ok
exec falho contido: ...
read(/dev/null): ~ns/chamada (200000 repeticoes)
regioes=N executaveis=M origem=/proc/self/maps (no Linux)
rss_kb=Some(A) -> Some(B) (B > A; espere ~8 MiB a mais)
soma_seq: ~s/media soma=... (10 repeticoes de N somas)
soma_salto8: ~s/media soma=... razao=~x (10 repeticoes de 8 passadas de N/8)
```

Leia com o modelo certo: a razão sequencial/salto mede *tempo de acesso*, não faltas de cache contadas — sem contadores de hardware, a hipótese de localidade explica, a medição decide. Fora do Linux, mapa e RSS declaram a lacuna em vez de fingir número.

### O que a memória faz aqui?

`bloco` vive no heap gerenciado; `drop` libera para o runtime, não promete devolver páginas ao SO. O vetor de `u64` soma 16 MB sequenciais contra 2 MB espaçados — o que muda é o aproveitamento de cada linha de cache trazida, não a quantidade de somas. Nada revela quadros físicos — ver a aula C 10a.

## 6. Erros comuns e implicações defensivas

1. **Presumir CPUs:** contêiner e afinidade mudam o paralelismo real. Leia o ambiente.
2. **Confundir medição com explicação:** tempo maior no salto sugere localidade, não prova falta de cache. Sem contador, a alegação para na razão.
3. **Esquecer `wait`:** filho não colhido retém estado. `Command::status`/`output` já colhem; `Child` solto exige `wait` explícito.
4. **`/proc` como API portátil:** é Linux. Em outro SO, declare a lacuna.
5. **Medir sem ambiente:** mitigações, carga e governador de CPU mudam a régua. Anote tudo.
6. **`unwrap` em caminho quente:** aqui, só em invariantes do experimento. Entrada externa usa `match`/`Result`.

## 7. Ponte com Python e JavaScript

Python: `os.getpid`, `resource.getrusage`, `subprocess.run` cobrem o mesmo terreno (aula 14a); `time.perf_counter` é o `Instant`. Node: `process.pid`, `os.cpus()`, `child_process`. Nos três, processo é isolamento com contabilidade e toda travessia custa.

## 8. Exercícios obrigatórios

1. Desenhe o caminho de `read` em `/dev/null` do programa até o kernel e de volta, marcando modo usuário/kernel.
2. Varie o passo (1, 8, 64) na soma e explique a curva com linhas de cache de 64 bytes e prefetch.
3. Substitua `/bin/true` por um binário que sai com código 3 e verifique `code() == Some(3)`.
4. Some 100 MB e compare RSS antes/depois/`drop`: o que volta ao processo e o que volta ao SO?

Use [pistas graduais](exercicios/16a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/16a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar o que `Command` cria e o que colher a saída garante;
- medir uma syscall real e listar o que o número inclui;
- interpretar `/proc/self/maps` e `VmRSS` sem inferir endereços físicos;
- distinguir medição de tempo de alegação de causa microarquitetural;
- executar o exemplo sem warnings e confrontar cada número com o modelo;
- apontar o que a biblioteca padrão observa e o que ela não observa.

**Próxima aula:** desempenho, profiling e otimização. As réguas desta aula (tempo monotônico, RSS, razão de padrões de acesso) viram método de profiling.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que compila.

**Para se aprofundar:**

1. *The Rustonomicon* ([doc.rust-lang.org/nomicon](https://doc.rust-lang.org/nomicon)); 2. Documentação do Tokio ([tokio.rs](https://tokio.rs)); 3. *The Rust Performance Book* ([nnethercote.github.io/perf-book](https://nnethercote.github.io/perf-book)).
