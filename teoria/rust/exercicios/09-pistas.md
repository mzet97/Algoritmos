---
layout: default
---

# Aula 09 (Rust): pistas graduais

Volte aos [exercicios](../09.md#11-exercícios-obrigatórios) antes de abrir as solucoes.

1. Min/max manual: um passe so guardando as duas referencias; slice vazio devolve `None`.
2. Serializar e um metodo por tipo; a funcao generica so conhece o trait.
3. `Pilha` delega para o `Vec`: `peek` devolve referencia, nao copia.
4. Fracoes pedem simplificacao por MDC e denominador positivo — decida isso no construtor/`From`.
5. `Box<dyn Desenhavel>` apaga o tipo concreto; cada elemento sabe se desenhar.
6. Tipo associado fixa o dado por implementacao: barras leem `f64`, linha le pares.
7. Sem metodos nativos: `match` sobre `Option`/`Result` e um laco para particionar.
