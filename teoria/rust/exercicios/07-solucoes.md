---
layout: default
---

# Aula 07 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../07.md#9-exercícios-obrigatórios).

```rust
use std::collections::{BTreeMap, HashMap, HashSet, VecDeque};

fn frequencia(texto: &str) -> Vec<(String, usize)> {
    let mut mapa: HashMap<String, usize> = HashMap::new();
    for palavra in texto.split_whitespace() {
        *mapa.entry(palavra.to_string()).or_insert(0) += 1;
    }
    let mut pares: Vec<(String, usize)> = mapa.into_iter().collect();
    pares.sort_by(|a, b| b.1.cmp(&a.1).then_with(|| a.0.cmp(&b.0)));
    pares
}

fn two_sum(nums: &[i32], alvo: i32) -> Option<(usize, usize)> {
    let mut vistos: HashMap<i32, usize> = HashMap::new();
    for (i, &n) in nums.iter().enumerate() {
        if let Some(&j) = vistos.get(&(alvo - n)) {
            return Some((j, i));
        }
        vistos.insert(n, i);
    }
    None
}

#[derive(Default)]
struct FilaDeTarefas {
    fila: VecDeque<String>,
}

impl FilaDeTarefas {
    fn adicionar(&mut self, tarefa: String) {
        self.fila.push_back(tarefa);
    }
    fn adicionar_urgente(&mut self, tarefa: String) {
        self.fila.push_front(tarefa);
    }
    fn proxima(&mut self) -> Option<String> {
        self.fila.pop_front()
    }
    fn tamanho(&self) -> usize {
        self.fila.len()
    }
    fn esta_vazia(&self) -> bool {
        self.fila.is_empty()
    }
}

fn intersecao(a: &[i32], b: &[i32]) -> Vec<i32> {
    let conjunto_b: HashSet<i32> = b.iter().copied().collect();
    let mut vistos: HashSet<i32> = HashSet::new();
    let mut saida: Vec<i32> = a
        .iter()
        .copied()
        .filter(|x| conjunto_b.contains(x) && vistos.insert(*x))
        .collect();
    saida.sort_unstable();
    saida
}

fn soma_por_categoria(pares: Vec<(String, i32)>) -> BTreeMap<String, i32> {
    let mut grupos: BTreeMap<String, Vec<i32>> = BTreeMap::new();
    for (categoria, valor) in pares {
        grupos.entry(categoria).or_default().push(valor);
    }
    grupos
        .into_iter()
        .map(|(categoria, valores)| {
            let soma: i32 = valores.iter().sum();
            (categoria, soma)
        })
        .collect()
}

fn merge_k_sorted(arrays: Vec<Vec<i32>>) -> Vec<i32> {
    let total: usize = arrays.iter().map(|a| a.len()).sum();
    let mut indices = vec![0usize; arrays.len()];
    let mut filas: VecDeque<usize> = (0..arrays.len()).collect();
    let mut saida = Vec::with_capacity(total);
    while !filas.is_empty() {
        // Retira o array cuja ponta atual e a menor; O(k) por elemento
        // neste laboratorio (a versao O(n log k) usa BinaryHeap, exercicio 6+).
        let mut melhor = filas[0];
        for &candidato in &filas {
            if arrays[candidato][indices[candidato]] < arrays[melhor][indices[melhor]] {
                melhor = candidato;
            }
        }
        filas.retain(|&i| i != melhor);
        saida.push(arrays[melhor][indices[melhor]]);
        indices[melhor] += 1;
        if indices[melhor] < arrays[melhor].len() {
            filas.push_back(melhor);
        }
    }
    saida
}

struct LRUCache {
    capacidade: usize,
    dados: VecDeque<(String, String)>,
}

impl LRUCache {
    fn novo(capacidade: usize) -> LRUCache {
        LRUCache { capacidade, dados: VecDeque::new() }
    }
    fn get(&mut self, chave: &str) -> Option<String> {
        let pos = self.dados.iter().position(|(k, _)| k == chave)?;
        let par = self.dados.remove(pos)?;
        let valor = par.1.clone();
        self.dados.push_back(par);
        Some(valor)
    }
    fn put(&mut self, chave: String, valor: String) {
        if let Some(pos) = self.dados.iter().position(|(k, _)| *k == chave) {
            self.dados.remove(pos);
        }
        self.dados.push_back((chave, valor));
        while self.dados.len() > self.capacidade {
            self.dados.pop_front();
        }
    }
}

fn main() {
    println!("frequencia: {:?}", frequencia("o rato roeu o queijo do rei o"));
    assert_eq!(two_sum(&[2, 7, 11, 15], 9), Some((0, 1)));

    let mut fila = FilaDeTarefas::default();
    fila.adicionar(String::from("a"));
    fila.adicionar(String::from("b"));
    fila.adicionar(String::from("c"));
    fila.adicionar_urgente(String::from("urgente"));
    assert_eq!(fila.tamanho(), 4);
    assert!(!fila.esta_vazia());
    let mut ordem = Vec::new();
    while let Some(t) = fila.proxima() {
        ordem.push(t);
    }
    println!("ordem da fila: {:?}", ordem);
    assert_eq!(ordem, vec!["urgente", "a", "b", "c"]);
    assert!(fila.esta_vazia());

    assert_eq!(intersecao(&[4, 9, 5, 9], &[9, 4, 9, 8]), vec![4, 9]);
    println!("categorias: {:?}", soma_por_categoria(vec![
        (String::from("fruta"), 3),
        (String::from("doce"), 5),
        (String::from("fruta"), 2),
    ]));
    assert_eq!(merge_k_sorted(vec![vec![1, 4], vec![2, 3], vec![5]]), vec![1, 2, 3, 4, 5]);

    let mut cache = LRUCache::novo(2);
    cache.put(String::from("a"), String::from("1"));
    cache.put(String::from("b"), String::from("2"));
    assert_eq!(cache.get("a"), Some(String::from("1")));
    cache.put(String::from("c"), String::from("3"));
    assert_eq!(cache.get("b"), None); // 'b' era o menos recente
    println!("lru ok");
}
```

Duas honestidades de complexidade: o merge acima escolhe a menor ponta em O(k) por elemento — correto, mas nao e o O(n log k) do enunciado; a versao com `BinaryHeap` (guardando `(Reverse(valor), array)`) e o passo seguinte e deve ser a sua entrega se a meta de complexidade for cobrada. E frequencia desempata alfabeticamente porque `HashMap` nao promete ordem: sem desempate, a saida mudaria entre execucoes e o teste ficaria intermitente.

**Autoverificacao:** fila processando `urgente` primeiro, LRU expulsando `b` depois do acesso a `a` e two_sum achando `(0, 1)` sem dois lacos.
