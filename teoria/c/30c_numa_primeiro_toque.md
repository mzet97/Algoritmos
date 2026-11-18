---
layout: default
---

# Aula 30c — NUMA: onde a memória mora, medida com honestidade

**Competências SDD:** HPC, ARQ e OS. **Pré-requisitos:** aulas [28a](28a_benchmark_honesto.md) (protocolo de medição) e [30b](30b_simd_e_afinidade.md) (SIMD, afinidade, 1 nó contado aqui). Esta aula fecha o que a 30b declarou em aberto: o multinó — ou melhor, declara com precisão o que fecha e o que fica como protocolo.

**Objetivo observável:** ao terminar, você deverá contar os nós NUMA da máquina, explicar first-touch e interleave com o mecanismo de ancoragem de páginas, medir a banda local — e declarar quando a comparação entre nós **não** foi feita, em vez de fingi-la. E deverá dizer, para um programa com acesso imprevisível, por que first-touch piora e interleave salva (e vice-versa para acesso partível).

**Motivação:** em máquina multinó, "memória" não é um lugar só: cada página mora num nó, e o núcleo que a toca de longe paga a viagem a cada cache line. Programa que ignora isso mede bem no laptop e sangra no servidor — 2× de banda perdida sem uma linha de código errada. Esta aula ensina a regra de moradia antes que o servidor cobre a aula: quem ancora, onde ancora, e como provar.

**Como ler esta aula (três níveis):** *fundamental* — quem toca primeiro decide o nó; *intermediário* — banda local medida e o passo de 64; *aprofundamento* — o protocolo multinó escrito sem placa para executar.

## 1. Contrato antes do código

- **Detecção:** contar `/sys/devices/system/node/node*` lendo o diretório, não presumindo. Nesta máquina, o resultado é 1 — e o programa imprime isso em vez de supor. Detecção que presume é configuração com outro nome.
- **First-touch:** a página é ancorada no nó de quem primeiro a toca (falta de página atendida com frame local ao faultor). `memset` inicial na thread que vai usar o buffer não é "aquecimento" — é escritura de moradia, e tem de acontecer no contexto certo.
- **Interleave:** espalhar páginas round-robin pelos nós (`numactl --interleave=all`). Ajuda quando o acesso é imprevisível (todo nó serve um pouco); atrapalha quando o acesso é local por construção (metade das linhas viaja à toa).
- **Medida local honesta:** varredura de 4×64 MiB com banda em GiB/s. O número vale como ordem de grandeza e como baseline do protocolo abaixo — com ambiente anotado, como manda a 28a.
- **Fora do contrato:** a comparação local × remoto — sem 2+ nós, ela seria ficção. O protocolo está escrito, passo a passo, com as três linhas de `numactl`; a medição, declaradamente pendente. Protocolo escrito não é medição feita, e a aula não confunde os dois.

## 2. Por que moradia decide banda?

Cache line de 64 bytes viaja inteira: tocar um byte remoto busca os 63 vizinhos pelo interconexo (QPI/UPI/Infinity Fabric, conforme o fabricante). Em loop sequencial sobre gigabytes, a diferença entre "todas as linhas locais" e "metade remota" não é ruído — é o teto do programa: cada linha remota paga latência de dezenas de nanossegundos a mais, e em streaming a banda cai na proporção das linhas que viajam. First-touch acerta a moradia quando o padrão é partível (cada thread ancora o seu pedaço antes de computar); interleave distribui o azar quando o padrão é imprevisível (ponteiro que pula,哈希 hash espalhado, grafo). Escolher sem medir o padrão é superstição com flag: a mesma flag que dobra a banda num padrão destrói no outro.

O mecanismo de ancoragem merece precisão: no Linux, a primeira falta de página num endereço anônimo aloca o frame no nó local ao CPU que faltou (política padrão `MPOL_DEFAULT` com modo `MPOL_PREFERRED` implícito de first-touch). "Tocar" significa ler ou escrever — o `memset` da linha 54 ancora porque escreve. Se a thread de inicialização roda no nó 0 e as trabalhadoras no nó 1, todas as páginas moram no 0 e as trabalhadoras pagam remoto para sempre: o bug clássico de "inicializei na main, paralelizei depois". A correção é ancorar onde se computa — inicializar dentro da thread trabalhadora, no seu pedaço.

## 3. Os três experimentos do protocolo (para a máquina com 2+ nós)

O programa imprime o protocolo porque executá-lo aqui provaria zero. Na máquina com 2+ nós, rode os três e anote banda + variância (5 repetições, ambiente anotado):

1. **Local** (`--cpunodebind=0 --membind=0`): computa e mora no mesmo nó. Este é o teto: nada viaja.
2. **Remoto** (`--cpunodebind=0 --membind=1`): computa no 0, mora no 1. A razão remoto/local é o preço da viagem no seu hardware — tipicamente 0.5×–0.8× da banda, e latência 1.3×–2×.
3. **Espalhado** (`--interleave=all`): cada página num nó, round-robin. Para varredura sequencial com threads em todos os nós, aproxima o local; para thread única, aproxima a média.

Só depois das três linhas, decida moradia — e documente qual padrão de acesso justificou a escolha. Sem isso, qualquer afirmação sobre ganho NUMA é palpite com unidade. E se a máquina tem 1 nó (como esta), as três linhas dão o mesmo número: publicar a "comparação" seria fabricar evidência, e a aula proíbe.

## 4. Exemplo completo e executável

O programa abaixo é o [arquivo-fonte desta aula](exemplos/30c_numa.c), reproduzido integralmente:

```c
/* Aula 30c — NUMA: onde a memoria mora e por que isso importa.
 *
 * Deteccao honesta + medicao local: conta os nos em
 * /sys/devices/system/node, mede banda de um buffer e explica first-touch
 * vs interleave — declarando que esta maquina tem 1 no, entao a comparacao
 * entre nos fica como protocolo, nao como medicao.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/30c_numa.c -o /tmp/30c
 * Executar: /tmp/30c
 */
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int conta_nos(void)
{
    int n = 0;
    DIR *d = opendir("/sys/devices/system/node");
    if (d == NULL) {
        return -1;
    }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (strncmp(e->d_name, "node", 4) == 0) {
            n++;
        }
    }
    closedir(d);
    return n;
}

static double agora_s(void)
{
    struct timespec t;
    assert(clock_gettime(CLOCK_MONOTONIC, &t) == 0);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

int main(void)
{
    int nos = conta_nos();
    printf("numa: nos detectados em /sys = %d\n", nos);
    assert(nos >= 1);

    /* Banda local: o numero que importa e a ordem de grandeza, nao o digito. */
    const size_t n = (size_t)64 << 20; /* 64 MiB */
    unsigned char *buf = malloc(n);
    assert(buf != NULL);
    memset(buf, 0x5a, n); /* first-touch: quem toca primeiro ancora a pagina */
    double ini = agora_s();
    unsigned long soma = 0;
    for (size_t passada = 0; passada < 4; passada++) {
        for (size_t i = 0; i < n; i += 64) {
            soma += buf[i];
        }
    }
    double fim = agora_s();
    double gbps = (4.0 * (double)n / (1 << 30)) / (fim - ini);
    printf("numa: varredura 4x64MiB em %.2fs = %.1f GiB/s (soma=%lu)\n",
           fim - ini, gbps, soma);
    free(buf);

    if (nos == 1) {
        printf("numa: 1 no nesta maquina — first-touch vs interleave sem efeito\n"
               "mensuravel aqui; o protocolo abaixo vale quando houver 2+ nos:\n"
               "  numactl --cpunodebind=0 --membind=0 ./prog   (local)\n"
               "  numactl --cpunodebind=0 --membind=1 ./prog (remoto)\n"
               "  numactl --interleave=all ./prog              (espalhado)\n"
               "30c ok: nos contados, banda local medida, limite multinó declarado\n");
    } else {
        printf("numa: %d nos — rode o protocolo acima e compare local x remoto\n"
               "30c ok: nos contados, banda local medida\n",
               nos);
    }
    return 0;
}
```

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/30c_numa.c -o /tmp/30c
/tmp/30c
```

Saída esperada (nesta máquina; a banda varia, a estrutura não):

```text
numa: nos detectados em /sys = 1
numa: varredura 4x64MiB em 0.06s = 4.2 GiB/s (soma=...)
numa: 1 no nesta maquina — first-touch vs interleave sem efeito
mensuravel aqui; o protocolo abaixo vale quando houver 2+ nos:
  numactl --cpunodebind=0 --membind=0 ./prog   (local)
  numactl --cpunodebind=0 --membind=1 ./prog (remoto)
  numactl --interleave=all ./prog              (espalhado)
30c ok: nos contados, banda local medida, limite multinó declarado
```

O protocolo pendente, para a máquina com 2+ nós: rode as três linhas, anote banda e variância, e só então decida moradia. Sem isso, qualquer afirmação sobre ganho NUMA é palpite com unidade.

## 5. Leitura guiada do programa

A detecção conta entradas `node*` no sysfs em vez de chamar `numa_available()` (evita dependência de `libnuma` e funciona em qualquer kernel com sysfs — o preço é não distinguir nó vazio de CPU de nó de memória, detalhe documentado no `numa(7)`). O `-1` de `opendir` falho vira `assert(nos >= 1)`: sem sysfs legível, não há o que declarar — falhar alto em vez de presumir 1.

A medição ancora antes de cronometrar: o `memset` toca todas as páginas (first-touch completo) para que a varredura meça banda, não faltas. Sem ele, a primeira passada pagaria falta por página e o número misturaria alocação com transferência. O passo de 64 bytes toca uma cache line por iteração — nem byte a byte (que mede latência de dependência, não banda) nem linha repetida (que mede cache, não RAM). E a `soma` impede o otimizador de apagar o trabalho: sem efeito observável, o loop some no `-O2` e a "banda" vira infinita — o mesmo `black_box` moral das pontes de benchmark, sem depender de API externa.

### O que a memória faz aqui?

`malloc` + `memset` imediato (ancoragem no contexto de uso), varredura com passo de cache line (banda, não latência), `free` no fim (higiene mesmo em programa curto — o ASan agradece e o hábito escala). Nada de `numa_alloc_onnode` aqui: a aula ensina a regra padrão do kernel antes da API de exceção — quem não entende first-touch usa `membind` como amuleto.

## 6. Erros comuns e implicações defensivas

1. **Afinar thread sem fixar memória:** `sched_setaffinity` no CPU errado com páginas no nó errado é o pior dos dois mundos — declare os dois juntos (`cpunodebind` + `membind`), nunca um só.
2. **Inicializar na main, computar nas threads:** o `memset` da main ancora tudo no nó da main. Cada thread deve ancorar seu pedaço (inicializar dentro da trabalhadora) ou a paralelização paga remoto para sempre.
3. **Medir uma vez e publicar:** banda varia com vizinhos (outros processos), frequência (turbo) e temperatura. Baseline exige repetição e ambiente anotado, como manda a 28a — um número sem contexto é anedota.
4. **Interleave como padrão:** espalhar o que era local transforma acerto em loteria. Interleave é remédio para acesso imprevisível (grafo, hash), não vitamina para todo programa.
5. **Fingir multinó com um nó:** `numactl` com um só nó sempre dá "local". Publicar comparação nessas condições é fabricar evidência — a aula declara o limite em vez de contorná-lo.
6. **Confundir banda com latência:** varredura sequencial mede banda (GiB/s); ponteiro que pula de 64 em 64 KiB mede latência (ns). O programa desta aula mede a primeira; a segunda é outro experimento, com outro loop e outra unidade.
7. **Contar `node*` e presumir CPUs:** entradas `node*` incluem nós só-memória (sem CPU). Para afinidade, confira `cpulist` de cada nó — contar diretório diz quantos nós, não onde computar.

## 7. Ponte com as demais trilhas

C++ repete o protocolo com `numactl` e afinidade de threads na 30b (mesmas três linhas, mesma declaração de limite); Rust ancora com thread dedicada por pedaço na 23e (first-touch programático em vez de flag); Go mede contenção granular na 23a e sofre o mesmo efeito sob o runtime (o GC também mora em algum nó — pausa longa pode ser página remota); Python declara o mesmo limite de 1 nó na 29c. Em JavaScript, o análogo é a localidade de `TypedArray` por worker — a viagem existe, só muda de nome (e de unidade: lá se mede em ms de frame, não em GiB/s).

## 8. Checklist do que esta aula garante

- [ ] Conto os nós da máquina e digo onde li (sysfs, sem presumir).
- [ ] Explico first-touch com o mecanismo de falta de página (quem falta primeiro, ancora) e aponto o `memset` que ancora.
- [ ] Digo quando interleave ajuda (acesso imprevisível) e quando atrapalha (local partível), com um exemplo de cada.
- [ ] Explico por que inicializar na main e computar nas threads ancora tudo no lugar errado.
- [ ] Meço a banda local, anoto ambiente e repetição, e digo por que o passo é 64 (cache line) e por que a soma existe (anti-DCE).
- [ ] Declaro sem rodeios: sem 2+ nós, comparação entre nós é protocolo escrito, não medição feita — e recito as três linhas do protocolo.

**Referências primárias:** `man 8 numactl`, `man 2 set_mempolicy` e a documentação de [NUMA no kernel](https://www.kernel.org/doc/html/latest/vm/numa.html) (política padrão de first-touch). Nenhuma afirmação de ganho multinó nesta aula — por decisão editorial, o limite é o conteúdo.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
