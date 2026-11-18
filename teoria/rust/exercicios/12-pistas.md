---
layout: default
---

# Aula 12 (Rust): pistas graduais

Volte aos [exercicios](../12.md#12-exercícios-obrigatórios) antes de abrir as solucoes.

1. Lista com `Box`: empilhar prepende; desempilhar devolve a cabeca.
2. `RefCell` move a checagem de emprestimo para a execucao — o contador muda atras de `&self`.
3. No grafo, dois nos apontam para o mesmo `Rc<RefCell<No>>`: compare `Rc::strong_count`.
4. `Arc<Mutex<u32>>`: 10 threads × 1000 incrementos com trava por incremento ou por lote — justifique.
5. BST com pai `Weak` evita o ciclo que reteria a arvore inteira.
6. `MeuBox` com `Deref` deixa metodos do conteudo aparecerem como seus.
7. Compartilhar `Arc<Vec>` le a mesma alocacao; copiar quadruplica a memoria — meça com um quarto por thread.
