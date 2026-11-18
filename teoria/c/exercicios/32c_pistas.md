---
layout: default
---

# Aula 32c: pistas graduais

Volte aos [exercícios](../32c_modelo_de_ameacas.md#9-exercícios-obrigatórios) antes de abrir as soluções.

1. `n == 0` cai na primeira guarda. Linha vazia não é comando curto — é ausência de comando; aceitar seria responder a nada, e "responder a nada" em protocolo vira amplificação (o servidor fala sozinho a cada lixo).
2. Sem a varredura, `"AB\0CD"` com `n = 5` retorna 0. Tudo que usa `strlen` a partir daí vê `"AB"`; os 3 bytes depois do NUL continuam no buffer — e no próximo parse. Framing decide pelo comprimento declarado; string C decide pelo primeiro NUL. Na fronteira, manda o framing.
3. A quarta linha segue o formato das três: ator, entrada, limite, teste. O par válido/inválido é o que prende o off-by-one: chave de 256 passa, de 257 é recusada. Sem o par, o teto é número mágico; com o par, é contrato testado.
4. `strlen > 256` rejeita: 256 passa (retorna 0), 257 cai (retorna −1) — verificado. O `+1` no `chave[MAX_CHAVE + 1]` é o NUL: sem ele, a chave máxima válida não caberia na saída. Off-by-one se prende pelos dois lados, sempre: o maior válido e o menor inválido, lado a lado.
