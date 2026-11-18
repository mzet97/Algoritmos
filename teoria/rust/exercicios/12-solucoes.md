---
layout: default
---

# Aula 12 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../12.md#12-exercícios-obrigatórios).

```rust
use std::cell::RefCell;
use std::ops::Deref;
use std::rc::{Rc, Weak};
use std::sync::{Arc, Mutex};
use std::thread;

// ---- Exercicio 1: lista com Box
struct Node {
    valor: i32,
    proximo: Option<Box<Node>>,
}

struct Lista {
    cabeca: Option<Box<Node>>,
}

impl Lista {
    fn novo() -> Lista {
        Lista { cabeca: None }
    }
    fn empilhar(&mut self, valor: i32) {
        let novo = Box::new(Node { valor, proximo: self.cabeca.take() });
        self.cabeca = Some(novo);
    }
    fn desempilhar(&mut self) -> Option<i32> {
        self.cabeca.take().map(|node| {
            self.cabeca = node.proximo;
            node.valor
        })
    }
    fn comprimento(&self) -> usize {
        let mut total = 0;
        let mut atual = &self.cabeca;
        while let Some(node) = atual {
            total += 1;
            atual = &node.proximo;
        }
        total
    }
    fn para_vec(&self) -> Vec<i32> {
        let mut saida = Vec::new();
        let mut atual = &self.cabeca;
        while let Some(node) = atual {
            saida.push(node.valor);
            atual = &node.proximo;
        }
        saida
    }
}

// ---- Exercicio 2: cache com RefCell
struct Cache {
    contagem: RefCell<u32>,
    dados: Vec<String>,
}

impl Cache {
    fn buscar(&self, indice: usize) -> Option<&str> {
        let achou = self.dados.get(indice);
        if achou.is_some() {
            *self.contagem.borrow_mut() += 1;
        }
        achou.map(|s| s.as_str())
    }
    fn hits(&self) -> u32 {
        *self.contagem.borrow()
    }
}

// ---- Exercicio 3: grafo com Rc<RefCell>
struct No {
    id: i32,
    vizinhos: Vec<Rc<RefCell<No>>>,
}

impl No {
    fn novo(id: i32) -> Rc<RefCell<No>> {
        Rc::new(RefCell::new(No { id, vizinhos: Vec::new() }))
    }
}

fn adicionar_aresta(de: &Rc<RefCell<No>>, para: &Rc<RefCell<No>>) {
    de.borrow_mut().vizinhos.push(Rc::clone(para));
}

fn grau(no: &Rc<RefCell<No>>) -> usize {
    no.borrow().vizinhos.len()
}

fn vizinhos_ids(no: &Rc<RefCell<No>>) -> Vec<i32> {
    no.borrow().vizinhos.iter().map(|v| v.borrow().id).collect()
}

// ---- Exercicio 5: BST com Rc<RefCell> e pai Weak
#[derive(Default)]
struct NoBST {
    chave: i32,
    esquerda: Option<Rc<RefCell<NoBST>>>,
    direita: Option<Rc<RefCell<NoBST>>>,
    pai: Option<Weak<RefCell<NoBST>>>,
}

fn bst_inserir(raiz: &Rc<RefCell<NoBST>>, chave: i32) {
    let (vai_esquerda, ja_existe) = {
        let no = raiz.borrow();
        if chave == no.chave {
            (false, true)
        } else {
            (chave < no.chave, false)
        }
    };
    if ja_existe {
        return;
    }
    let lado = if vai_esquerda { "e" } else { "d" };
    let filho_existente = {
        let no = raiz.borrow();
        if lado == "e" { no.esquerda.clone() } else { no.direita.clone() }
    };
    match filho_existente {
        Some(filho) => bst_inserir(&filho, chave),
        None => {
            let novo = Rc::new(RefCell::new(NoBST {
                chave,
                esquerda: None,
                direita: None,
                pai: Some(Rc::downgrade(raiz)),
            }));
            let mut no = raiz.borrow_mut();
            if lado == "e" {
                no.esquerda = Some(novo);
            } else {
                no.direita = Some(novo);
            }
        }
    }
}

fn bst_contem(raiz: &Rc<RefCell<NoBST>>, chave: i32) -> bool {
    let (igual, filho) = {
        let no = raiz.borrow();
        if chave == no.chave {
            (true, None)
        } else if chave < no.chave {
            (false, no.esquerda.clone())
        } else {
            (false, no.direita.clone())
        }
    };
    if igual {
        return true;
    }
    match filho {
        Some(f) => bst_contem(&f, chave),
        None => false,
    }
}

fn bst_pai_de(raiz: &Rc<RefCell<NoBST>>, chave: i32) -> Option<i32> {
    // Ao achar a chave, devolve a chave do pai via Weak (sem reter).
    let filho = {
        let no = raiz.borrow();
        if chave == no.chave {
            return no
                .pai
                .as_ref()
                .and_then(|w| w.upgrade())
                .map(|p| p.borrow().chave);
        }
        if chave < no.chave { no.esquerda.clone() } else { no.direita.clone() }
    };
    match filho {
        Some(f) => bst_pai_de(&f, chave),
        None => None,
    }
}

fn bst_em_ordem(raiz: &Rc<RefCell<NoBST>>, saida: &mut Vec<i32>) {
    let (esquerda, direita, chave) = {
        let no = raiz.borrow();
        (no.esquerda.clone(), no.direita.clone(), no.chave)
    };
    if let Some(e) = esquerda {
        bst_em_ordem(&e, saida);
    }
    saida.push(chave);
    if let Some(d) = direita {
        bst_em_ordem(&d, saida);
    }
}

// ---- Exercicio 6: smart pointer proprio
struct MeuBox<T> {
    valor: T,
}

impl<T> MeuBox<T> {
    fn novo(valor: T) -> MeuBox<T> {
        MeuBox { valor }
    }
}

impl<T> Deref for MeuBox<T> {
    type Target = T;
    fn deref(&self) -> &T {
        &self.valor
    }
}

impl<T> Drop for MeuBox<T> {
    fn drop(&mut self) {
        println!("MeuBox<{}> destruido", std::any::type_name::<T>());
    }
}

fn main() {
    let mut lista = Lista::novo();
    lista.empilhar(1);
    lista.empilhar(2);
    lista.empilhar(3);
    println!("lista: {:?} comp={}", lista.para_vec(), lista.comprimento());
    println!("desempilha: {:?}", lista.desempilhar());
    assert_eq!(lista.desempilhar(), Some(2));

    let cache = Cache {
        contagem: RefCell::new(0),
        dados: vec![String::from("alfa"), String::from("beta")],
    };
    println!("cache: {:?} {:?}", cache.buscar(0), cache.buscar(5));
    println!("cache hits: {}", cache.hits());
    assert_eq!(cache.hits(), 1);

    let a = No::novo(1);
    let b = No::novo(2);
    let c = No::novo(3);
    adicionar_aresta(&a, &c);
    adicionar_aresta(&b, &c);
    println!("vizinhos de a: {:?}; grau a={} b={}", vizinhos_ids(&a), grau(&a), grau(&b));
    println!("fortes em c: {}", Rc::strong_count(&c));
    assert_eq!(Rc::strong_count(&c), 3); // a, b e a variavel c

    let contador = Arc::new(Mutex::new(0u32));
    let mut handles = Vec::new();
    for _ in 0..10 {
        let clonado = Arc::clone(&contador);
        handles.push(thread::spawn(move || {
            for _ in 0..1000 {
                *clonado.lock().unwrap() += 1;
            }
        }));
    }
    for h in handles {
        h.join().unwrap();
    }
    println!("contador: {}", *contador.lock().unwrap());
    assert_eq!(*contador.lock().unwrap(), 10000);

    let raiz = Rc::new(RefCell::new(NoBST { chave: 10, ..Default::default() }));
    for chave in [5, 15, 3, 7] {
        bst_inserir(&raiz, chave);
    }
    let mut ordem = Vec::new();
    bst_em_ordem(&raiz, &mut ordem);
    println!("bst em ordem: {:?}", ordem);
    assert_eq!(ordem, vec![3, 5, 7, 10, 15]);
    assert!(bst_contem(&raiz, 7));
    assert!(!bst_contem(&raiz, 8));
    println!("pai de 7: {:?} (raiz sem pai: {:?})", bst_pai_de(&raiz, 7), bst_pai_de(&raiz, 10));
    assert_eq!(bst_pai_de(&raiz, 7), Some(5));

    let caixa = MeuBox::novo(String::from("coercao"));
    println!("MeuBox len via Deref: {}", caixa.len());

    let dados = Arc::new((0..1_000_000i32).collect::<Vec<i32>>());
    let mut somas = Vec::new();
    for quarto in 0..4 {
        let clonado = Arc::clone(&dados);
        somas.push(thread::spawn(move || {
            let inicio = quarto * 250_000;
            clonado[inicio..inicio + 250_000].iter().map(|x| *x as i64).sum::<i64>()
        }));
    }
    let total: i64 = somas.into_iter().map(|h| h.join().unwrap()).sum();
    println!("soma compartilhada: {} (fortes={})", total, Arc::strong_count(&dados));
    assert_eq!(total, (0..1_000_000i64).sum::<i64>());
}
```

O pai `Weak` na BST e a diferenca entre a arvore viver e vaziar: pai forte criaria um ciclo `Rc` (filho segura pai, pai segura filho) que nunca chega a zero. `Weak` observa sem reter — em producao, muitos desenhos simplesmente nao guardam o pai.

**Autoverificacao:** contador 10000 sem `unsafe`, `strong_count` de `c` igual a 3 no grafo e a BST imprimindo em ordem crescente.
