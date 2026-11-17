---
layout: default
---

# Aula 11: solucoes comentadas

Compare apos tentar os [exercicios](../11.md#16-exercicios-obrigatorios). Os roteiros abaixo foram executados nesta rodada (gdb 17.2, valgrind 3.27, GCC 16) sobre este programa com defeito plantado:

```c
#include <stdio.h>
#include <stdlib.h>

static int soma_vizinhos(const int *v, size_t i)
{
    return v[i] + v[i + 1];
}

int main(void)
{
    int v[4] = {10, 20, 30, 40};
    int total = soma_vizinhos(v, 3); /* le v[4]: fora dos limites */
    printf("total=%d\n", total);
    free(NULL);
    return 0;
}
```

1. Sessao minima de gdb — compilar com simbolos, parar na funcao, inspecionar e avancar uma linha:

```bash
gcc -std=c11 -g -Og -Wall -Wextra -o depura depura.c
gdb -batch -ex 'break soma_vizinhos' -ex run -ex 'print i' -ex 'print v[3]' -ex next -ex 'print total' ./depura
```

Trechos observados: `Breakpoint 1, soma_vizinhos (v=..., i=3)`, `$1 = 3`, `$2 = 40` e, apos o `next`, o programa segue sem sintoma — o gdb mostra **onde** voce esta e o valor das variaveis, nao se o acesso era valido. Enderecos variam por execucao; compile com `-Og` ou `-O0` para os passos seguirem o fonte.

2. O mesmo programa com sanitizers nomeia o defeito que o gdb nao viu:

```bash
gcc -std=c11 -g -fsanitize=address,undefined -o depura_asan depura.c
./depura_asan
```

Relatorio observado: `AddressSanitizer: stack-buffer-overflow ... READ of size 4 ... in soma_vizinhos depura.c:6`. O sanitizer instrumenta cada acesso e responde **qual** leitura saiu do objeto, com pilha.

3. Valgrind no binario comum:

```bash
valgrind --leak-check=full ./depura
```

Observado: `ERROR SUMMARY: 7 errors from 5 contexts`, com leitura invalida (`Invalid read`) no mesmo ponto. O valgrind roda o binario sem recompilar, mais lento, e tambem e a ferramenta desta lista que audita vazamentos.

4. Crash e pilha: com um programa que desreferencia `NULL` em cadeia (`main -> nivel1 -> nivel2`), o gdb interrompe no sinal e `bt` mostra o caminho:

```text
Program received signal SIGSEGV
nivel2 (p=0x0) at crash.c:5
#0 nivel2 at crash.c:5
#1 nivel1 at crash.c:10
#2 main at crash.c:16
```

Leia de `#0` (onde caiu) para cima (quem chamou); a correcao quase sempre esta no quadro que passou o ponteiro invalido, nao no que desreferenciou.

5. Depurador e sanitizer respondem perguntas diferentes. O gdb e interativo e retrocede no tempo da execucao: estado, variaveis, pilha e "como cheguei aqui", inclusive em programas que nao falham. Sanitizers sao verificadores de acessos: rodam sem interacao, detectam a **primeira** operacao invalida instrumentada e explicam o objeto violado — mas so enxergam o caminho executado e o que instrumentam. Fluxo pratico: sanitizer para achar a classe e o local do defeito; gdb para entender o estado que levou ate ele.

**Autoverificacao:** uma sessao gdb com `break/run/print/next` registrada, um relatorio ASan ou valgrind colado e um `bt` lido em voz alta do quadro interno ate o `main`.
