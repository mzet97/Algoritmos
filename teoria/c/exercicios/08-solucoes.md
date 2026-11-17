---
layout: default
---

# Aula 08: solucoes comentadas

Compare apos tentar os [exercicios](../08.md#16-exercicios-obrigatorios).

1 e 4. Contar linhas com erro em `stderr`:

```c
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s ARQUIVO\n", argv[0]);
        return 2;
    }
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }
    char buf[256];
    long linhas = 0;
    int ultima_tem_nl = 1;
    while (fgets(buf, sizeof buf, f) != NULL) {
        for (const char *p = buf; *p != '\0'; p++) {
            if (*p == '\n') {
                linhas++;
                ultima_tem_nl = 1;
            } else {
                ultima_tem_nl = 0;
            }
        }
    }
    if (ferror(f)) {
        perror("leitura");
        fclose(f);
        return 1;
    }
    if (!ultima_tem_nl) {
        linhas++; /* ultima linha sem \n tambem e linha */
    }
    printf("linhas: %ld\n", linhas);
    fclose(f);
    return 0;
}
```

2. Copia texto com `fgets`/`fputs`:

```c
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "uso: %s ORIGEM DESTINO\n", argv[0]);
        return 2;
    }
    FILE *origem = fopen(argv[1], "r");
    if (origem == NULL) {
        perror(argv[1]);
        return 1;
    }
    FILE *destino = fopen(argv[2], "w");
    if (destino == NULL) {
        perror(argv[2]);
        fclose(origem);
        return 1;
    }
    char buf[256];
    while (fgets(buf, sizeof buf, origem) != NULL) {
        if (fputs(buf, destino) == EOF) {
            perror("escrita");
            fclose(origem);
            fclose(destino);
            return 1;
        }
    }
    fclose(origem);
    if (fclose(destino) != 0) {
        perror("fechamento");
        return 1;
    }
    return 0;
}
```

3. Copia com `fread`/`fwrite`: o retorno de `fread` e o numero de itens lidos; escreva so esses itens:

```c
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "uso: %s ORIGEM DESTINO\n", argv[0]);
        return 2;
    }
    FILE *origem = fopen(argv[1], "rb");
    if (origem == NULL) {
        perror(argv[1]);
        return 1;
    }
    FILE *destino = fopen(argv[2], "wb");
    if (destino == NULL) {
        perror(argv[2]);
        fclose(origem);
        return 1;
    }
    unsigned char buf[4096];
    size_t lidos;
    while ((lidos = fread(buf, 1, sizeof buf, origem)) > 0) {
        if (fwrite(buf, 1, lidos, destino) != lidos) {
            perror("escrita");
            fclose(origem);
            fclose(destino);
            return 1;
        }
    }
    if (ferror(origem)) {
        perror("leitura");
        fclose(origem);
        fclose(destino);
        return 1;
    }
    fclose(origem);
    if (fclose(destino) != 0) {
        perror("fechamento");
        return 1;
    }
    return 0;
}
```

A versao binaria tambem preserva arquivos sem `\n` e bytes zero, que a versao por linhas nao foi feita para tratar.

5. Sem buffering, cada caractere lido seria uma chamada ao sistema operacional — uma travessia cara para mover um byte. O buffer da biblioteca le um bloco grande de uma vez e serve os caracteres da memoria, amortizando o custo da syscall por milhares de operacoes. O preco: dados escritos podem ainda estar no buffer quando o programa cai, dai `fclose`/`fflush` fazerem parte do contrato de corretude, nao de etiqueta.

**Autoverificacao:** copia identica conferida com `cmp`, arquivo inexistente gera mensagem em `stderr` e codigo de saida diferente de zero, e arquivo sem `\n` final conta a ultima linha.
