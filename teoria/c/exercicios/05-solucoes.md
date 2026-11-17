---
layout: default
---

# Aula 05: solucoes comentadas

Compare apos tentar os [exercicios](../05.md#17-exercicios-obrigatorios).

1 e 2. Modelagem basica:

```c
#include <stdio.h>

enum StatusPedido {
    PEDIDO_RECEBIDO,
    PEDIDO_PAGO,
    PEDIDO_ENVIADO,
    PEDIDO_ENTREGUE,
    PEDIDO_CANCELADO
};

struct Produto {
    char nome[32];
    double preco;
    int estoque;
};

int main(void)
{
    struct Produto p = {"teclado", 199.90, 12};
    enum StatusPedido s = PEDIDO_PAGO;
    printf("%s %.2f %d status=%d\n", p.nome, p.preco, p.estoque, (int)s);
    return 0;
}
```

3. A ordem dos campos muda o preenchimento (padding) e, portanto, o tamanho:

```c
#include <stdio.h>

struct Espalhada {
    char a;
    double d;
    char b;
};

struct Compacta {
    double d;
    char a;
    char b;
};

int main(void)
{
    printf("espalhada: %zu\n", sizeof(struct Espalhada));
    printf("compacta:  %zu\n", sizeof(struct Compacta));
    return 0;
}
```

Na plataforma de referencia (x86-64, `double` de 8 bytes com alinhamento 8): 24 e 16. O padrao C garante o mecanismo (alinhamento e padding), nao esses numeros — por isso o programa **imprime** os valores em vez de assumi-los.

4. Na `union`, os membros compartilham o mesmo armazenamento:

```c
#include <stdio.h>

union Representacao {
    int inteiro;
    float flutuante;
    unsigned char bytes[4];
};

int main(void)
{
    union Representacao u;
    u.inteiro = 0x3F800000;
    printf("como float: %f\n", (double)u.flutuante);
    printf("primeiro byte: %02X\n", u.bytes[0]);
    return 0;
}
```

Em plataformas IEEE 754 (a de referencia), `0x3F800000` lido como `float` e `1.0`. Ler um membro diferente do ultimo escrito e permitido em C, mas o resultado depende da representacao da plataforma — e um laboratorio de bits, nao uma conversao portatil.

5. Gravar a `struct` crua (`fwrite(&p, sizeof p, 1, f)`) escreve tambem padding com conteudo indeterminado e congela no arquivo: tamanhos de tipo, alinhamento, endianness e a versao exata do layout. Outro compilador, outra arquitetura ou uma mudanca na ordem dos campos pode tornar o arquivo ilegivel ou — pior — legivel com valores errados. Serializacao seria define um formato campo a campo (texto, ou binario com tipos de largura fixa e ordem de bytes declarada).

**Autoverificacao:** tamanhos impressos com `%zu`, leitura da union descrita como representacao e nenhum `sizeof` presumido no texto da sua resposta.
