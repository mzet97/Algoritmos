---
layout: default
---

# Aula 32c: soluções comentadas

Compare após tentar os [exercícios](../32c_modelo_de_ameacas.md#9-exercícios-obrigatórios). Experimentos sobre `teoria/c/exemplos/32c_ameacas.c` com `gcc -std=c11 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: `n = 0` retorna **−1** (primeira guarda: `n == 0 || n > MAX_LINHA`). Linha vazia não é comando curto — aceitar seria responder ao nada, e servidor que responde ao nada amplifica qualquer lixo em tráfego. Rejeitar vazio é fechar a porta do "comando sem verbo".

2. Diagnóstico: sem a varredura, `"AB\0CD"` com `n = 5` deixa de retornar −1 — o próprio `assert(aceitar_linha("AB\0CD", 5) == -1)` do `main` cai (verificado). `strlen` diria 2; o framing (comprimento declarado 5) diz 5; os 3 bytes após o NUL viajariam escondidos para o próximo parse, que os leria como início de comando. Na fronteira, manda o comprimento declarado — string C é conveniência interna, nunca contrato de rede.

3. Projeto: a quarta linha segue o formato — ator ("cliente que envia chave longa"), entrada (`char[300]`), limite (`MAX_CHAVE = 256`), teste (par 256/257). Implementação espelha `aceitar_replay` sem a lista de vistos:

```c
static int aceitar_chave(const char *chave, size_t n)
{
    if (n == 0 || n > MAX_CHAVE)
        return -1;
    for (size_t i = 0; i < n; i++)
        if (chave[i] == '\0')
            return -1;
    return 0;
}
```

E o par que prende o off-by-one em `main`: `aceitar_chave(k256, 256) == 0`, `aceitar_chave(k257, 257) == -1`. Sem os dois lados, o teto é número mágico; com os dois, é contrato.

4. Previsão: **256 passa (0), 257 é recusado (−1)** — verificado lado a lado. `strlen(id) > MAX_CHAVE` rejeita a partir de 257; 256 cabe em `chave[MAX_CHAVE + 1]` com o NUL. O `+1` compra exatamente a chave máxima válida: sem ele, ou o teto real seria 255 (mentira documentada como 256), ou a chave-limite estouraria o buffer. Off-by-one se prende pelos dois lados, sempre.
