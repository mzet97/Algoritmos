---
layout: default
---

# Aula 16b: soluções comentadas

Compare após tentar os [exercícios](../16b_sistema_de_arquivos.md#10-exercícios-obrigatórios).

1. Previsão: sim, a fase 1 continua verde — e é exatamente esse o ponto. O teste lê de volta pelo nome final no mesmo boot, quando o page cache ainda contém tudo. A garantia perdida é contra o leitor pós-queda: com `rename` antes do `fsync`, existe uma janela em que o nome novo aponta para conteúdo que mora só em memória. Teste verde aqui mede "o código fez as chamadas", não "o disco recebeu os bytes". A ordem `fsync`-antes-`rename` existe para fechar essa janela, não para agradar o teste.

2. Diagnóstico: sem o loop, `write` num pipe com leitor dormindo retorna parcial. Programa mínimo que prova:

```c
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int fds[2];
    assert(pipe(fds) == 0);
    /* Sem leitor e sem bloqueio: o buffer (64 KiB) enche e o
     * write retorna o que coube. Pipe bloqueante com leitor
     * dormindo apenas bloquearia ate drenar — o parcial honesto
     * aparece no modo nao-bloqueante, como em sockets reais. */
    assert(fcntl(fds[1], F_SETFL, O_NONBLOCK) == 0);
    char *m = malloc(1 << 20);
    memset(m, 'x', 1 << 20);
    ssize_t w = write(fds[1], m, 1 << 20); /* UMA chamada, sem loop */
    printf("write unico retornou: %zd de %d (parcial = normal)\n", w, 1 << 20);
    assert(w > 0 && w < (1 << 20));
    /* O loop que falta: drena e continua de onde parou. */
    size_t feito = (size_t)w;
    char b[65536];
    while (feito < (1 << 20)) {
        ssize_t r = read(fds[0], b, sizeof b);
        assert(r > 0);
        ssize_t w2 = write(fds[1], m + feito, (1 << 20) - feito);
        if (w2 < 0) {
            assert(errno == EAGAIN);
            continue;
        }
        feito += (size_t)w2;
    }
    printf("com loop: %zu de %d (tudo entregue)\n", feito, 1 << 20);
    return 0;
}
```

Saída típica: `write unico retornou: 65536 de 1048576`, depois `com loop: 1048576 de 1048576`. O retorno parcial não é erro — é o contrato. O loop de `escreve_tudo` mora em todo caminho de produção que escreve mais do que cabe num buffer do kernel.

3. Projeto: a função retorna `0` ou `-1` com `errno` preservado, e limpa o temporário em qualquer falha:

```c
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int escreve_tudo(int fd, const char *dados, size_t n)
{
    size_t feito = 0;
    while (feito < n) {
        ssize_t r = write(fd, dados + feito, n - feito);
        if (r < 0)
            return -1;
        if (r == 0)
            break;
        feito += (size_t)r;
    }
    return feito == n ? 0 : -1;
}

int publica_atomica(const char *dir, const char *nome,
                    const char *dados, size_t n)
{
    char tmp[512], caminho[512], rc = 0;
    snprintf(tmp, sizeof tmp, "%s/%s.tmp", dir, nome);
    snprintf(caminho, sizeof caminho, "%s/%s", dir, nome);
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -1;
    if (escreve_tudo(fd, dados, n) != 0 || fsync(fd) != 0)
        rc = -1;
    if (close(fd) != 0)
        rc = -1;
    if (rc != 0 || rename(tmp, caminho) != 0) {
        unlink(tmp); /* nunca deixar rascunho para tras */
        return -1;
    }
    /* fsync do diretorio: garante a propria entrada do nome. */
    int dd = open(dir, O_RDONLY | O_DIRECTORY);
    if (dd >= 0) {
        if (fsync(dd) != 0)
            rc = -1;
        close(dd);
    }
    return rc;
}
```

Teste da queda: publique `"v1"`, depois escreva metade de `"v2..."` direto no `.tmp` sem `rename`; releia o nome final e confirme `"v1"` intacta. O publicado nunca é escrito no lugar — por isso sobrevive.

4. Medição: esqueça o número absoluto, fique com a razão. Esqueleto:

```c
struct timespec a, b;
clock_gettime(CLOCK_MONOTONIC, &a);
for (int i = 0; i < 100; i++)
    publica_atomica(dir, "bench", payload, sizeof payload - 1);
clock_gettime(CLOCK_MONOTONIC, &b);
```

Na máquina de referência, com `fsync`: dezenas de milissegundos por publicação; sem `fsync`: microssegundos. A razão (3 a 4 ordens de grandeza) é a decisão: cada escrita isolada com durabilidade paga o disco cheio — acumular N registros num lote paga uma vez. O número exato varia por disco; a conclusão não.
