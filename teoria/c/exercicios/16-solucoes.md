---
layout: default
---

# Aula 16: solucoes comentadas

Compare apos tentar os [exercicios](../16.md#15-exercicios-obrigatorios).

1, 2 e 3. Leitor com `open`/`read` e tamanho com `fstat`:

```c
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s ARQUIVO\n", argv[0]);
        return 2;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        return 1;
    }
    struct stat info;
    if (fstat(fd, &info) != 0) {
        perror("fstat");
        close(fd);
        return 1;
    }

    char buf[8192];
    long long total = 0;
    for (;;) {
        ssize_t lidos = read(fd, buf, sizeof buf);
        if (lidos < 0) {
            perror("read");
            close(fd);
            return 1;
        }
        if (lidos == 0) {
            break;
        }
        total += lidos;
    }
    close(fd);
    printf("bytes lidos: %lld\n", total);
    printf("tamanho fstat: %lld\n", (long long)info.st_size);
    return 0;
}
```

Sem `stdio`, nao ha buffer da biblioteca: cada `read` e uma syscall, e o laco trata os tres resultados possiveis (positivo, zero = fim, negativo = erro). Para arquivo regular, `bytes lidos` e `st_size` coincidem; para dispositivos e pipes, `st_size` pode ser 0 sem o arquivo estar "vazio" — o contador do laco e a medida que nao depende do tipo.

4. `mmap` ajuda quando o acesso e esparso e repetido sobre um arquivo grande: mapear e tocar so as paginas necessarias, deixando o kernel trazer e manter as paginas quentes, em vez de copiar o arquivo inteiro por `read` para um buffer do processo. Exemplo de desenho: um indice sobre um arquivo de registros fixos que consulta posicoes distantes muitas vezes. Para varredura unica e sequencial, `read` em blocos ja e o caminho simples e rapido — mapear nao acelera o que nao se revisita.

5. Na segunda leitura do mesmo arquivo, os bytes costumam vir do page cache — paginas que o kernel manteve na RAM apos a primeira leitura — e nao do disco. Um benchmark que mede "leitura de arquivo" sem descartar ou contornar o cache pode estar medindo memoria, nao armazenamento: numeros otimistas demais e que nao se repetem com arquivos maiores que a RAM. Declarar o estado do cache faz parte do protocolo de medicao honesto da [aula 28a](../28a_benchmark_honesto.md).

**Autoverificacao:** contador e `st_size` iguais para arquivo regular, arquivo inexistente com `perror` e saida diferente de zero, e nenhum `FILE*` no programa.
