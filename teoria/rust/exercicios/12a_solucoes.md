# Soluções comentadas da aula 12a

## 1. Prova do invariante

- **Início:** `ini = 0`, `fim = len`; a fatia é inteira e a promessa vale.
- **Preservação:** com `ini < fim`, `meio` está dentro da fatia (indexação verificada nunca dispara se o invariante vale). Igualdade entrega a pós-condição; os ramos descartam metades que a ordenação prova estarem do lado errado.
- **Término:** `fim - ini` (`usize`, sem sinal) diminui estritamente; ao sair, `ini == fim` e `None` é correto.

## 2. BFS com ciclo e destino inalcançável

```rust
use std::collections::{HashMap, HashSet, VecDeque};

fn bfs_contando(grafo: &HashMap<&str, Vec<&str>>, origem: &str) -> usize {
    let mut visitados = HashSet::from([origem]);
    let mut fila = VecDeque::from([origem]);
    let mut removidos = 0;
    while let Some(atual) = fila.pop_front() {
        removidos += 1;
        if let Some(vizinhos) = grafo.get(atual) {
            for &v in vizinhos {
                if visitados.insert(v) {
                    fila.push_back(v);
                }
            }
        }
    }
    removidos
}
```

Cada vértice entra em `visitados` uma vez; `removidos <= |V|` e o laço termina com ou sem destino. `insert` devolve `false` para repetidos, o que impede o ciclo de reinscrever vértices.

## 3. Sementes

As três sementes devem passar com a mesma conclusão (a medição não depende delas). A semente fixa torna a sequência de casos reproduzível: uma falha aponta o caso exato. Com tempo real como semente, cada execução testa casos diferentes e uma falha pode não se repetir — inútil como regressão.

## 4. Soma iterativa e aborto da recursão

```rust
fn soma_ate(n: u64) -> u64 {
    let mut acumulado = 0u64; // invariante: soma de 1..(i-1)
    for i in 1..=n {
        acumulado += i; // em debug, overflow entra em panico: declare o teto
    }
    acumulado
}
```

Com `n` da ordem de centenas de milhões, a versão recursiva aborta por estouro de pilha (sem `Result`, sem `catch_unwind` confiável em overflow de stack). A iterativa usa `O(1)` espaço. Em debug, `acumulado += i` acima do teto de `u64` entra em pânico; em release com overflow-checks desligados, embrulha em silêncio — o contrato precisa declarar o `n` máximo.
