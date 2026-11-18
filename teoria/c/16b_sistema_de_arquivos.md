---
layout: default
---

# Aula 16b — Sistema de arquivos: nome, inode e o crash no meio

**Competências SDD:** ARQ, OS e SEC. **Pré-requisitos:** aulas [08](08.md) (I/O e buffers), [16](16.md) (syscalls, `mmap`) e [16a](16a_processos_e_observacao.md) (processos e observação). O kernel [32a](32a_kernel_riscv.md) mostra o outro lado (falta sob demanda, syscall); esta aula mostra o lado que todo programa encontra primeiro: o arquivo que sobrevive — ou não — à queda de energia.

**Objetivo observável:** ao terminar, você deverá publicar um arquivo de forma atômica (tmp + `fsync` + `rename`), detectar uma escrita rasgada por checksum, explicar o que o diretório garante (nome → inode) e o que ele não garante (conteúdo pela metade) — e dizer, para cada uma das três reordenações possíveis da sequência, qual leitor ou qual crash ela expõe.

**Motivação:** quase todo bug de durabilidade tem a mesma forma: o programa escreveu "quase tudo" e caiu. Sem disciplina, o leitor seguinte encontra meio arquivo e confia nele. Esta aula estabelece a disciplina mínima: publique de uma vez, desconfie de tudo que não foi publicado, e prove a integridade antes de confiar.

**Como ler esta aula (três níveis):** *fundamental* — ordem de escrita e rasgo detectado; *intermediário* — `write` parcial e o loop; *aprofundamento* — `fsync` de diretório, journal e o VFS da 32d.

## 1. Contrato antes do código

- **Diretório é mapa, não cofre:** ele liga um nome a um inode. O `rename` troca a ligação de lugar atomicamente — quem abre o nome vê o velho ou o novo, nunca uma mistura.
- **Publicação atômica:** escreva o conteúdo completo num temporário, `fsync` o temporário (desça do page cache para o disco), e só então `rename` sobre o nome final. A ordem é a garantia.
- **Rasgo detectável:** todo registro lido fora do caminho publicado passa pelo checksum antes de ser confiado. Checksum divergente não é "dado alternativo" — é corrupção, e o programa recusa.
- **Fora do contrato:** journaling do ext4/xfs, `fsync` do diretório pai (necessário em produção após `rename`), e o VFS do kernel 32a. A aula mostra o mecanismo em espaço de usuário; o kernel mostra onde ele mora de verdade.

## 2. Nome, inode e conteúdo: três coisas diferentes

Todo arquivo Unix é três coisas amarradas por duas setas: o **nome** vive no diretório (um mapa de strings para números de inode); o **inode** guarda metadados e a lista de blocos; o **conteúdo** mora nos blocos. O `rename` velho→novo mexe só no mapa: desliga o nome novo do inode antigo e liga ao inode novo, numa operação que o sistema de arquivos promete indivisível. Nenhum byte de conteúdo se move — e é por isso que publicar 27 bytes ou 27 GiB custa o mesmo `rename`.

Três consequências que o exemplo prova sem dizer em voz alta. Primeira: dois nomes podem apontar para o mesmo inode (hard link) — e então "sobrescrever um" sem `rename` muda o outro junto; o temporário + `rename` evita essa armadilha porque o publicado nunca é escrito no lugar. Segunda: apagar (`unlink`) remove do mapa, não do disco — o leitor que já abriu continua lendo até fechar. Terceira: o tamanho no inode e os blocos no disco podem divergir no meio de uma escrita — e é exatamente essa divergência que a fase 2 chama de rasgo.

## 3. A ordem tmp → fsync → rename, passo a passo

Cada passo existe por um leitor ou um crash específico. O **temporário** existe pelo leitor concorrente: enquanto o conteúdo está incompleto, nenhum leitor o alcança pelo nome final — ele lê a versão anterior inteira ou espera a próxima. Escrever direto no nome final expõe cada prefixo: o crash sempre encontra *alguma* versão parcial válida o bastante para enganar.

O **`fsync`** existe pelo crash com o `rename` já feito. Sem ele, a sequência "escreveu tudo + renomeou" publica um nome cujo conteúdo ainda mora só no page cache — a queda de energia leva os dois, e o leitor seguinte encontra o nome novo com conteúdo velho ou zeros. `fsync` antes do `rename` transforma "o nome mudou" em "o conteúdo está no disco e depois o nome mudou". O preço é uma espera de milissegundos; o custo de não pagar é corrupção silenciosa.

O **`rename`** existe para fazer a troca ser indivisível. Sem ele — por exemplo, copiando por cima do destino — há um instante em que o nome aponta para conteúdo misturado. Com ele, qualquer abertura do nome final encontra o inode velho inteiro ou o inode novo inteiro. Note o que o `rename` *não* faz: não sincroniza o diretório pai. Em produção, um `fsync` no fd do diretório após o `rename` garante a própria entrada do nome — sem ele, o crash pode perder o `rename` em si (o conteúdo novo existe em blocos órfãos, mas sem nome ninguém o alcança).

As três reordenações erradas, cada uma com sua vítima: `rename` antes do `fsync` expõe conteúdo não sincronizado ao leitor pós-queda; `fsync` depois do `rename` é tarde demais para o crash no meio; escrever direto sem temporário expõe prefixos ao leitor concorrente. A ordem certa não é convenção — é a única das quatro que não tem vítima.

## 4. Page cache é memória, não disco

O `write` bem-sucedido não escreveu no disco: copiou para o page cache do kernel e voltou. Tudo que o programa "escreveu" pode evaporar na queda de energia até o `fsync` (ou o `close` + sync periódico + pressão de memória, nenhum deles sob controle do programa). Essa é a distinção que separa "o `write` retornou N" de "N bytes sobreviverão": a primeira é promessa da syscall, a segunda é promessa do armazenamento — e só o `fsync` converte uma na outra.

O exemplo marca a fronteira duas vezes. Na fase 1, `escreve_tudo` termina com `fsync` *dentro* da função — publicar sem sincronizar nem compila no vocabulário desta aula. Na fase 2, a ausência deliberada de `fsync` (comentada no código: "o SO ainda pode não ter persistido") é o crash simulado: fecha o fd, mas o conteúdo pode ainda estar só no cache. O teste não precisa puxar o cabo de verdade porque a distinção já está no código: com `fsync` é publicação, sem `fsync` é rascunho.

## 5. Exemplo completo e executável

O programa abaixo é o [arquivo-fonte desta aula](exemplos/16b_filesystem.c), reproduzido integralmente — o que executa é este texto, sem divergência:

```c
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
```

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/16b_filesystem.c -o /tmp/16b
/tmp/16b
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/16b_filesystem.c -o /tmp/16b_asan
/tmp/16b_asan   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística):

```text
fase1 atomico: 27 bytes publicados de uma vez: versao=2...
fase2 rasgo: 16 de 32 bytes, checksum 64cd001c != b64a96c9 -> detectado
fase3: publicado intacto (27 bytes), tmp descartado
16b ok: rename atomico publica, checksum denuncia rasgo
```

## 6. Leitura guiada da saída

1. **Publicação (27 bytes de uma vez):** o temporário recebe `"versao=2\nchecksum=incluido\n"` com `fsync` e vira `dados.txt` num `rename`. O número 27 é o tamanho exato da string — a fase prova atomicidade pelo tamanho (inteiro, não prefixo) e o leitor seguinte confirma os mesmos bytes. Leitores concorrentes nunca veem 13 bytes: o nome só aponta para o inode novo depois de tudo sincronizado.
2. **Rasgo (16 de 32, checksums diferentes):** metade de `"registro-0001: valor importante\n"` (32 bytes) é escrita sem `fsync` e sem `rename` — o "crash". A leitura de volta encontra 16 bytes e o checksum `64cd001c` contra `b64a96c9` do registro inteiro: metade soma diferente do todo, sempre (FNV-1a sobre prefixo ≠ FNV sobre o todo para qualquer conteúdo não-vazio). A recusa é o `assert` — em produção, seria o erro retornado ao chamador.
3. **Sobrevivência (27 bytes intactos, tmp descartado):** `stat` confirma que o publicado não mudou um byte com o crash do temporário — isolamento entre rascunho e publicado. Descartar temporário com `unlink` é rotina, não emergência: todo ciclo de publicação termina ou em `rename` (sucesso) ou em `unlink` (desistência).

### O que a memória faz aqui?

`write` com loop até completar (escrita parcial é normal, não erro — o kernel pode aceitar menos bytes que o pedido e ninguém deve presumir atomicidade de `write` em arquivo regular), `fsync` antes de publicar (page cache é memória, não disco), e leitura com teto de buffer (`sizeof buf - 1` + terminador: o programa nunca confia que o arquivo cabe no buffer). Nada de `mmap` aqui: o ponto é a ordem das operações, não a velocidade delas.

## 7. Erros comuns e implicações defensivas

1. **Publicar sem `fsync`:** o `rename` troca o nome, mas o conteúdo ainda pode estar só no page cache. Queda real perde os dois — e o nome novo aponta para lixo.
2. **Confiar em tamanho como integridade:** 16 de 32 bytes também é "um arquivo com tamanho". Só o checksum diz que está incompleto: tamanho é metadado, integridade é conteúdo.
3. **Reutilizar o temporário entre escritas:** resto da versão anterior vaza para a próxima (o `O_TRUNC` da fase 2 existe por isso — sem ele, um registro menor deixaria cauda do maior). Temporário é descartável por definição: cria, usa, renomeia ou apaga.
4. **Esquecer o diretório pai:** em produção, `fsync` no fd do diretório após o `rename` garante a própria entrada do nome. A aula declara a ausência; o checklist cobra. Sem ele, o crash perde o `rename` — o conteúdo existe em blocos órfãos que ninguém alcança.
5. **Escrever direto no destino "porque é mais rápido":** economiza um `rename` e um `fsync` e compra todas as janelas de rasgo. A economia é real e errada: durabilidade não tem atalho, tem protocolo.
6. **Checksum fraco ou ausente na leitura:** CRC392 sem sal detecta rasgo acidental, não sabotagem. Para ameaça (não queda), o checksum precisa ser MAC — a [32c](32c_modelo_de_ameacas.md) separa as duas classes.
7. **Chamar isto de banco de dados:** sem journal, sem transação e sem concorrência, isto é o degrau zero. O degrau um é o log com `fsync` da 23a; o degrau dois é o journal do sistema de arquivos — cada um move uma fronteira do "pode perder" para o "sobrevive".

## 8. Ponte com as demais trilhas

O mesmo trio aparece em toda a masterclass: Go publica config com tmp+rename na trilha de infraestrutura; Python faz checkpoint com fsync na trilha de ETL; Rust persiste o log antes de responder na 23a; C++ ordena durabilidade com `fsync` antes de confirmar commit. Em JavaScript, o análogo é o `localStorage` que só confirma após serializar inteiro — meio JSON gravado é o rasgo desta aula em outra roupa. E o VFS do kernel [32d](32d_vfs_minimo.md) mostra onde o `rename` mora de verdade: resolvedor que troca a ligação nome→inode sem tocar nos bytes — a mesma operação que esta aula usa sem ver.

## 9. Checklist do que esta aula garante

- [ ] Explico a diferença entre nome, inode e conteúdo — e qual dos três o `rename` move (o nome).
- [ ] Reproduzo a publicação atômica e digo onde o `fsync` mora na ordem e por quê (antes do `rename`: conteúdo no disco antes do nome mudar).
- [ ] Para cada reordenação (rename antes do fsync, fsync depois do rename, sem temporário), digo quem é a vítima.
- [ ] Demonstro um rasgo detectado por checksum e a recusa que se segue.
- [ ] Digo por que `write` parcial é normal e onde o loop mora.
- [ ] Listo o que falta para produção: `fsync` do diretório, journal e concorrência.
- [ ] Aponto onde este mecanismo reaparece: log da 23a, checkpoint, `rename` do VFS 32d.

**Referências primárias:** `man 2 rename` (atomicidade), `man 2 fsync` (durabilidade) e a documentação do [ext4](https://www.kernel.org/doc/html/latest/filesystems/ext4/index.html) sobre ordem de journal. Nada aqui substitui o capítulo de crash consistency de um livro de sistemas — ele diz onde esta aula termina.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
