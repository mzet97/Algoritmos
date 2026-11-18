/* Aula 16b — sistema de arquivos: nome, inode e o crash no meio.
 *
 * Demonstracao deterministica em espaco de usuario:
 * 1) troca atomica por tmp + fsync + rename;
 * 2) escrita rasgada detectada por checksum (o crash simulado);
 * 3) o que o diretorio e: nome -> inode, e o que sobrevive ao crash.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/16b_filesystem.c -o /tmp/16b
 * Executar: /tmp/16b
 */
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static uint32_t checksum(const char *dados, size_t n)
{
    uint32_t soma = 2166136261u;
    for (size_t i = 0; i < n; i++) {
        soma ^= (uint32_t)(unsigned char)dados[i];
        soma *= 16777619u;
    }
    return soma;
}

static void escreve_tudo(int fd, const char *dados, size_t n)
{
    size_t feito = 0;
    while (feito < n) {
        ssize_t r = write(fd, dados + feito, n - feito);
        assert(r > 0);
        feito += (size_t)r;
    }
    assert(fsync(fd) == 0);
}

int main(void)
{
    const char *dir = "/tmp/aula16b_fs";
    char caminho[256], tmp[256];
    snprintf(caminho, sizeof caminho, "%s/dados.txt", dir);
    snprintf(tmp, sizeof tmp, "%s/dados.txt.tmp", dir);
    if (mkdir(dir, 0755) != 0) {
        assert(errno == EEXIST);
    }

    /* Fase 1: publicacao atomica. Quem le "dados.txt" ve o velho ou o novo,
     * nunca meio arquivo: o rename troca o nome de lugar, nao os bytes. */
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(fd >= 0);
    const char *novo = "versao=2\nchecksum=incluido\n";
    escreve_tudo(fd, novo, strlen(novo));
    assert(close(fd) == 0);
    assert(rename(tmp, caminho) == 0);

    int lido = open(caminho, O_RDONLY);
    assert(lido >= 0);
    char buf[128];
    ssize_t n = read(lido, buf, sizeof buf - 1);
    assert(n > 0);
    buf[n] = '\0';
    assert(close(lido) == 0);
    printf("fase1 atomico: %zd bytes publicados de uma vez: %.12s...\n", n, buf);

    /* Fase 2: o crash simulado. Escreve metade, "cai a energia" (sem fsync,
     * sem rename), e a leitura seguinte detecta pelo checksum. */
    fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assert(fd >= 0);
    const char *grande = "registro-0001: valor importante\n";
    assert(write(fd, grande, strlen(grande) / 2) == (ssize_t)(strlen(grande) / 2));
    assert(close(fd) == 0); /* sem fsync: o SO ainda pode nao ter persistido */

    int rasgado = open(tmp, O_RDONLY);
    assert(rasgado >= 0);
    char metade[128];
    ssize_t m = read(rasgado, metade, sizeof metade - 1);
    assert(m > 0);
    metade[m] = '\0';
    assert(close(rasgado) == 0);
    uint32_t soma_parcial = checksum(metade, (size_t)m);
    uint32_t soma_total = checksum(grande, strlen(grande));
    printf("fase2 rasgo: %zd de %zu bytes, checksum %08x != %08x -> detectado\n",
           m, strlen(grande), soma_parcial, soma_total);
    assert(soma_parcial != soma_total);

    /* Fase 3: o arquivo publicado continua intacto; o tmp morre sem pena. */
    struct stat st;
    assert(stat(caminho, &st) == 0 && st.st_size == (off_t)strlen(novo));
    assert(unlink(tmp) == 0);
    printf("fase3: publicado intacto (%lld bytes), tmp descartado\n",
           (long long)st.st_size);
    printf("16b ok: rename atomico publica, checksum denuncia rasgo\n");
    return 0;
}
