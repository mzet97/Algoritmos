---
layout: default
---

# Aula 10: solucoes comentadas

Compare apos tentar os [exercicios](../10.md#16-exercicios-obrigatorios).

1. Tamanhos diferentes pela ordem dos campos — ver tambem a [solucao da aula 05](05-solucoes.md); na plataforma de referencia, 24 contra 16 bytes pelos mesmos tres campos.

2. Inspecionar a representacao de um objeto com `memcpy`:

```c
#include <stdio.h>
#include <string.h>

int main(void)
{
    int valor = 0x01020304;
    unsigned char bytes[sizeof valor];
    memcpy(bytes, &valor, sizeof valor);
    for (size_t i = 0; i < sizeof bytes; i++) {
        printf("%02X ", bytes[i]);
    }
    putchar('\n');
    return 0;
}
```

Copiar para `unsigned char[]` com `memcpy` e o caminho definido pelo padrao para examinar bytes. A ordem impressa revela a endianness da plataforma (`04 03 02 01` na x86-64 de referencia, little-endian); o programa imprime, nao presume.

3. Cinco UB ja vistos: ler/escrever fora dos limites de um array; usar ponteiro apos `free` (use-after-free) ou liberar duas vezes; overflow de inteiro com sinal; divisao inteira por zero; passar a funcoes de string um buffer sem `\0` dentro dos limites. Em todos, o padrao nao define resultado algum — "funcionou aqui" nao e evidencia.

4. `memcmp` entre structs compara **todos** os bytes, incluindo padding, cujo conteudo e indeterminado. Duas structs com campos iguais podem diferir no padding (valores residuais da pilha, por exemplo) e `memcmp` dizer "diferentes"; e o contrario tambem nao e garantido entre representacoes. Comparacao correta e campo a campo — ou uma serializacao definida, se a comparacao for entre representacoes externas.

5. Roteiro com sanitizers (executado nesta rodada com GCC 16):

```bash
gcc -std=c11 -g -fsanitize=address,undefined -o defeito defeito.c
./defeito   # AddressSanitizer/UBSan nomeiam arquivo, linha e o objeto
gcc -std=c11 -g -fsanitize=address,undefined -o correto correto.c
./correto   # mesmo binario de verificacao, nenhum relatorio: saida limpa
```

O que eles detectam: acessos fora de limites e apos liberacao (ASan), overflow com sinal, deslocamentos invalidos e outras operacoes indefinidas instrumentadas (UBSan). O que eles **nao** provam: ausencia de UB nao instrumentado, de condicoes de corrida (isso e do ThreadSanitizer) ou de erros logicos com memoria valida.

**Autoverificacao:** bytes impressos sem acesso direto por ponteiro de outro tipo, lista de UB com um exemplo de codigo para cada item e um relatorio real de sanitizer colado na sua resposta.
