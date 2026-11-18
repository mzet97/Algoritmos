---
layout: default
---

# Aula 10 (Rust): pistas graduais

Volte aos [exercicios](../10.md#15-exercícios-obrigatórios) antes de abrir as solucoes.

1. Dois lifetimes independentes: o retorno so depende de `'a`.
2. `Palavra` guarda fatia e posicao; o texto original precisa viver mais que a struct.
3. No tokenizer, `pos` avanca em **bytes**; tokens sao fatias do input original.
4. Versao owned troca `&str` por `String`: ganha independencia, paga alocacao.
5. Prefixo comum compara caractere a caractere sem sair das fronteiras UTF-8.
6. `Cache` guarda `&'a T` — o compilador vai recusar qualquer uso apos o escopo do valor.
