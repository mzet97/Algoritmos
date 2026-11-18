---
layout: default
---

# Aula 28 (Go): pistas graduais

Volte aos [exercicios](../28.md#25-exercicios-obrigatorios) antes de abrir as solucoes.

1. `os.ReadFile` traz o arquivo inteiro; `bufio` atravessa em pedacos — compare contando a mesma coisa.
2. Buffer maior ajuda ate o custo da syscall deixar de dominar; depois, nada muda.
3. `mmap` brilha em acesso aleatorio repetido sobre arquivo maior que a RAM util.
4. Riscos de `mmap`: erro de E/S vira sinal, nao `error`; e cada plataforma tem seu dialeto.
5. Contar palavras e trocar o que o laco faz com cada linha, nao a leitura.
6. Page cache faz a segunda leitura "do disco" sair da RAM: esquente antes de medir.
