---
layout: default
---

# Aula 18 (C++): pistas graduais

Volte aos [exercicios](../18.md#14-exercicios-obrigatorios) antes de abrir as solucoes.

1. `release()` entrega o fd e deixa o wrapper vazio (-1) sem fechar: quem recebe vira o dono.
2. Copia deletada porque dois donos do mesmo fd fechariam duas vezes — e o segundo `close` pode fechar **outro** arquivo.
3. `send`/`recv` devolvem parcial, zero ou -1; cada retorno pede uma decisao diferente.
4. Framing: bytes prontos nao significam mensagem completa, como na [aula 15 de C](../../c/exercicios/15-solucoes.md).
5. Separe o que e linguagem (`unique_ptr`-like, RAII) do que e POSIX (`socket`, `close`, `ssize_t`).
