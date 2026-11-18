---
layout: default
---

# Aula 12a — Raciocínio discreto e algoritmos em Rust

---

## Objetivo da Aula

Escrever a pré-condição e a pós-condição de uma função, declarar o invariante de um laço, argumentar terminação por uma grandeza que diminui e provar um caso recursivo por indução — tudo em Rust seguro, sem `unsafe`. Pré-requisitos: aulas 01 a 12. Não precisa saber threads, `async`, FFI nem `no_std`.

Esta aula é a ponte R3 do mapa: quem domina invariantes aqui carrega o raciocínio para concorrência (13), `unsafe` (15) e kernel (R8), onde a invariante violada não dá erro gentil — dá condição de corrida ou falha de isolamento.

Ao final, você deverá: enunciar invariantes de laço, provar correção de um algoritmo iterativo simples por invariante, e distinguir a prova do invariante da rede de segurança da linguagem (Rust, Python ou C).
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir invariante, pré-condição e pós-condição`;
- **Nível 2 — Aplicar:** `prever a violação de um invariante em um exemplo e corrigir o laço`;
- **Nível 3 — Projetar:** `projetar um algoritmo com invariante declarado e prova de correção`.

---

## 1. Contrato antes do código

Tome a busca binária sobre uma fatia ordenada: devolver o índice do valor ou sinalizar ausência.

- **Pré-condição:** a fatia está ordenada em ordem não decrescente.
- **Pós-condição:** se o valor existe, o índice devolvido aponta para ele; se não existe, o resultado é `None`.
- **Caso especial:** fatia vazia devolve `None` sem comparar nada. Repetidos admitem qualquer índice válido; documente em vez de fingir unicidade.

Em Rust, o tipo de retorno `Option<usize>` torna a ausência parte do contrato verificada pelo compilador: quem chama é obrigado a tratar `None`. Compare com C, onde `-1` como "não achou" é convenção que o tipo `int` não impõe. O sistema de tipos elimina uma classe inteira de esquecimentos — mas não prova que o índice devolvido é o correto. Essa parte continua sendo um argumento sobre o laço.

Registre hipóteses de desenvolvimento com `debug_assert!`: verificam em debug e somem em release, como o `assert` de C e C++. Validação de entrada externa usa `if` e `Result`, nunca `assert` nem `panic!` como controle de fluxo normal.

## 2. O invariante que sobrevivem às iterações

Mantenha `ini` e `fim` com a promessa:

```text
se o valor está na fatia, então ele está em valores[ini..fim]
```

No início, a fatia é inteira e a promessa vale. A cada passo, compare com o meio: igualdade entrega a pós-condição; `alvo < meio` descarta a direita (`fim = meio`); maior descarta a esquerda (`ini = meio + 1`). Cada descarte preserva a promessa pela ordenação. Quando `ini == fim`, a fatia está vazia: devolva `None`.

**Terminação:** `fim - ini` é um `usize` que diminui estritamente a cada iteração sem acerto. Não existe sequência infinita assim.

Duas diferenças em relação a C e C++ merecem atenção explícita:

1. **Aritmética de índices não estoura em silêncio.** `meio = (ini + fim) / 2` com `usize` não tem o clássico bug de overflow de `(ini+fim)` em 32 bits para fatias reais (o comprimento máximo já limita a soma), e em debug o overflow entraria em pânico em vez de corromper. Em release, o overflow com `+` também entra em pânico por padrão (verificação de overflow ativada no perfil debug; no release, depende do projeto — confira o perfil antes de alegar). A conta `(ini + fim) / 2` continua correta aqui, mas o hábito defensivo é `ini + (fim - ini) / 2`, que nunca soma dois grandes.
2. **O borrow checker é um invariante mecânico.** `valores: &[i32]` é um empréstimo compartilhado: enquanto a busca roda, ninguém pode mutar a fatia pelo mesmo caminho. A pré-condição "ordenada" não pode ser invalidada no meio da busca por outro código com acesso à mesma referência. Em C, a mesma garantia exigiria disciplina manual.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/12a_busca.rs) contém exatamente o programa abaixo. Compile direto com `rustc`, sem `cargo`:

```bash
rustc --edition 2024 teoria/rust/exemplos/12a_busca.rs -o busca12a
./busca12a
```

```rust
/// Devolve o indice de `alvo` em `valores` ou `None` se ausente.
///
/// Pre-condicao: `valores` ordenada em ordem nao decrescente.
/// Invariante: se `alvo` esta na fatia, esta em `valores[ini..fim]`.
fn busca_binaria(valores: &[i32], alvo: i32) -> Option<usize> {
    let mut ini = 0usize;
    let mut fim = valores.len();
    while ini < fim {
        let meio = ini + (fim - ini) / 2;
        if valores[meio] == alvo {
            return Some(meio);
        }
        if alvo < valores[meio] {
            fim = meio;
        } else {
            ini = meio + 1;
        }
    }
    None
}

fn busca_linear(valores: &[i32], alvo: i32) -> Option<usize> {
    valores.iter().position(|&v| v == alvo)
}

/// Gerador deterministico simples (LCG): sem crates externas, semente fixa.
struct Lcg(u64);

impl Lcg {
    fn proximo(&mut self, limite: i32) -> i32 {
        self.0 = self.0.wrapping_mul(6364136223846793005).wrapping_add(1442695040888963407);
        ((self.0 >> 33) as i32).rem_euclid(limite)
    }
}

fn checar_propriedade(tentativas: u32) {
    let mut rng = Lcg(42); // semente fixa: a falha e reproduzivel
    for _ in 0..tentativas {
        let tamanho = rng.proximo(51) as usize;
        let mut valores: Vec<i32> = (0..tamanho as u32).map(|_| rng.proximo(201) - 100).collect();
        valores.sort();
        let alvo = rng.proximo(221) - 110;
        match busca_binaria(&valores, alvo) {
            Some(i) => assert_eq!(valores[i], alvo, "indice invalido"),
            None => assert!(!valores.contains(&alvo), "falso negativo"),
        }
    }
}

/// Oraculo: a busca da biblioteca padrao, exercitada sobre os mesmos casos.
fn checar_contra_std() {
    let mut rng = Lcg(7);
    for _ in 0..500 {
        let tamanho = rng.proximo(51) as usize;
        let mut valores: Vec<i32> = (0..tamanho as u32).map(|_| rng.proximo(201) - 100).collect();
        valores.sort();
        let alvo = rng.proximo(221) - 110;
        assert_eq!(
            busca_binaria(&valores, alvo).is_some(),
            valores.binary_search(&alvo).is_ok()
        );
    }
}

fn main() {
    assert_eq!(busca_binaria(&[], 5), None);
    assert_eq!(busca_binaria(&[5], 5), Some(0));
    assert_eq!(busca_binaria(&[5], 4), None);
    assert_eq!(busca_binaria(&[1, 3, 5, 7], 1), Some(0));
    assert_eq!(busca_binaria(&[1, 3, 5, 7], 7), Some(3));
    assert_eq!(busca_binaria(&[1, 3, 5, 7], 4), None);
    assert!(busca_binaria(&[2, 2, 2], 2).is_some_and(|i| [0, 1, 2].contains(&i)));

    checar_propriedade(2000);
    checar_contra_std();

    let grande: Vec<i32> = (0..1_000_000).map(|x| x * 2).collect();
    for f in [busca_linear, busca_binaria] {
        assert_eq!(f(&grande, 1_999_998), Some(999_999));
        assert_eq!(f(&grande, 1_999_999), None);
    }

    let rep_lin = 5;
    let inicio = std::time::Instant::now();
    for _ in 0..rep_lin {
        busca_linear(&grande, 1_999_999);
    }
    let t_lin = inicio.elapsed().as_secs_f64() / rep_lin as f64;

    let rep_bin = 200;
    let inicio = std::time::Instant::now();
    for _ in 0..rep_bin {
        busca_binaria(&grande, 1_999_999);
    }
    let t_bin = inicio.elapsed().as_secs_f64() / rep_bin as f64;

    println!("linear: {t_lin:.6}s/media ({rep_lin}x)");
    println!("binaria: {t_bin:.9}s/media ({rep_bin}x)");
}
```

Saída esperada (tempos variam por máquina; a razão importa, não o dígito):

```text
linear: ~0.0Xs/media (5x)
binaria: ~0.000000Xs/media (200x)
```

A busca linear percorre um milhão de elementos e a binária faz cerca de 20 comparações. Note o desenho do teste de propriedade: ele confere o *contrato* (índice válido ou ausência real), não a igualdade com a busca linear — com repetidos, dois índices diferentes podem ser ambos corretos, e exigir igualdade transformaria resposta válida em falha falsa. O segundo teste usa a biblioteca padrão como oráculo independente da *presença*, que é o que `binary_search` também garante.

### O que a memória faz aqui?

`grande` possui um milhão de `i32` no heap (`Vec`); as buscas tomam `&[i32]` e usam `O(1)` espaço adicional. O borrow checker garante que ninguém muta a fatia durante a busca pelo mesmo caminho. `valores[meio]` usa indexação verificada: fora da faixa, o programa entra em pânico em vez de ler memória inválida — o invariante `ini <= meio < fim` é o que prova que isso nunca acontece, e a prova continua necessária mesmo com a rede de segurança.

## 4. Indução: recursão com argumento, não com esperança

```rust
fn soma_ate(n: u64) -> u64 {
    if n == 0 {
        0
    } else {
        n + soma_ate(n - 1)
    }
}
```

Base: `soma_ate(0) == 0`. Passo: supondo `soma_ate(n-1)` correto, `n + soma_ate(n-1)` é a soma de `1..=n`. A prova vale para inteiros matemáticos; na máquina, `u64` tem teto (`n + soma` entra em pânico por overflow em debug) e cada chamada pendente consome stack — recursão profunda estoura a pilha e aborta o processo, sem `Result` para capturar. Prova de correção não remove limite de máquina: declare a profundidade máxima no contrato ou use laço.

## 5. Conjuntos, relações e o contrato de `Hash`

Relações aparecem sempre que um dado aponta para outro: dependências, rotas, referências. Um grafo é um conjunto de vértices mais pares (arestas). A BFS encontra o caminho mais curto em arestas, com o invariante: a fila só contém vértices alcançáveis, e cada vértice entra no conjunto `visitados` antes de entrar na fila — logo sai da fila no máximo uma vez e o laço termina mesmo sem alcançar o destino.

```rust
use std::collections::{HashMap, HashSet, VecDeque};

fn bfs_caminho(grafo: &HashMap<&str, Vec<&str>>, origem: &str, destino: &str) -> Option<Vec<&str>> {
    let mut visitados = HashSet::from([origem]);
    let mut fila: VecDeque<(&str, Vec<&str>)> = VecDeque::from([(origem, vec![origem])]);
    while let Some((atual, caminho)) = fila.pop_front() {
        if atual == destino {
            return Some(caminho);
        }
        if let Some(vizinhos) = grafo.get(atual) {
            for &vizinho in vizinhos {
                if visitados.insert(vizinho) {
                    let mut novo = caminho.clone();
                    novo.push(vizinho);
                    fila.push_back((vizinho, novo));
                }
            }
        }
    }
    None
}
```

E o contrato de hash que sustenta `HashSet` e `HashMap`: se `k1 == k2`, então `hash(k1) == hash(k2)`. Implementar `Hash` manual que quebre isso corrompe a estrutura em silêncio — o compilador não verifica; derive (`#[derive(Hash)]`) ou prove a consistência. `HashMap` usa ordem aleatória por padrão (DoS-resistência): não dependa da ordem de iteração.

## 6. Erros comuns e implicações defensivas

1. **Buscar sem garantir ordenação:** a binária devolve resposta errada em silêncio — e o tipo `Option` não a protege disso. Valide ou documente a pré-condição.
2. **Confundir média com pior caso:** `HashMap::get` é `O(1)` em média; entrada adversária degrada. Limite tamanho de entrada não confiável.
3. **Recursão sem limite declarado:** estouro de pilha aborta o processo. Prefira laço ou limite a profundidade no contrato.
4. **Medir antes de provar:** benchmark de algoritmo errado mede a velocidade do erro.
5. **Semente aleatória solta:** teste de propriedade sem semente fixa gera falha irreproduzível.
6. **`panic!` como controle de fluxo:** pânico é para bug, não para entrada inválida. Entrada externa usa `Result`.

## 8. Exercícios obrigatórios

1. Reescreva a prova do invariante da busca binária (início, preservação, término) sem consultar a seção 2.
2. Implemente `bfs_caminho` para um grafo com ciclo e prove que ela termina mesmo com destino inalcançável. Conte vértices visitados.
3. Troque a semente do `Lcg` para `7` e `1234`; o resultado muda? Explique por que a semente fixa importa e o que `std::time` no lugar dela quebraria.
4. Escreva a soma `1..=n` iterativa com invariante declarado e mostre um `n` em que a versão recursiva aborta na sua máquina.

Use [pistas graduais](exercicios/12a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/12a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- declarar pré-condição, pós-condição e convenção de repetidos e de fatia vazia;
- explicar por que o invariante vale antes e depois de cada iteração;
- indicar a grandeza que diminui e provar o término;
- compilar com `--edition 2024` sem warnings e confrontar `O(log n)` com a medição;
- explicar por que o teste de propriedade confere o contrato e não a igualdade com a referência;
- apontar o que o borrow checker garante na busca e o que ele não garante.

**Próxima aula:** concorrência e paralelismo. O raciocínio por invariantes desta aula sustenta invariantes de estado compartilhado: o que cada thread pode assumir e o que cada lock preserva.

## 7. Ponte com Python e JavaScript

Python tem `int` arbitrário e `in` linear legível; JavaScript usa `Number` de 64 bits, onde nem todo inteiro grande é exato. A identidade do invariante é a mesma nas três linguagens; o que muda é a rede de segurança: Rust verifica limites em tempo de execução e impõe o empréstimo exclusivo via tipos, Python verifica limites com exceção, C não verifica. A prova do invariante continua obrigatória nos três — a rede só muda o preço do erro.

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

1. *Introduction to Algorithms*, Cormen et al. (MIT Press, 4. ed., ISBN 978-0262046305); 2. Steve Klabnik e Carol Nichols, *The Rust Programming Language* (No Starch Press, ISBN 978-1718503106).
