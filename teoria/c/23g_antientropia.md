---
layout: default
---

# Aula 23g — Anti-entropia por resumos: sincronizar pelo que mudou (C6)

**Competências SDD:** DST, SEC. **Pré-requisitos:** aulas [23c](23c_relogios_e_particao.md) (vetores, concorrência detectada, anti-entropia com ordem total) e [23d](23d_consenso_e_quorum.md) (versão vence por termo). O consenso decidiu o que vale; o resumo decide o que precisa viajar para todos saberem.

**Objetivo observável:** ao terminar, você deverá executar o exemplo, mostrar os três cenários (0, 1 e 3 faixas) com o número exato de entradas movidas, explicar por que a versão decide a direção da cópia — e dizer em que caso o resumo mente (colisão), com a probabilidade e a correção.

**Motivação:** a 23c sincroniza mandando os logs inteiros: funciona, mas o custo é proporcional ao tamanho da loja, não ao tamanho da divergência. Quando dois nós passam 99% do tempo iguais, mandar 100% para conferir 1% é desperdício — e em enlace caro (satélite, inter-região, bateria), o desperdício impede a sincronização frequente. O resultado é o pior dos mundos: nós que divergem porque sincronizar custa caro, e sincronizar custa caro porque mandam tudo. Esta aula quebra o ciclo: 8 resumos FNV-1a (um por faixa de 8 chaves) decidem o que viaja; faixa igual nem entra no fio. A versão por entrada decide a direção; o resumo decide o escopo.

**Como ler esta aula (três níveis):** *fundamental* — resumo que evita viajar dado igual; *intermediário* — grão da faixa e `memcpy` sem padding; *aprofundamento* — Merkle hierárquico e tumbas (fora do contrato).

## 1. Contrato antes do código

O [arquivo-fonte](exemplos/23g_antientropia.c) contém a simulação determinística (dois nós em um processo, sem rede, sem tempo — o objeto é a regra de sincronização, e a simulação a isola do transporte, como a 23d isola o commit):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23g_antientropia.c -o sins23g
./sins23g
# iguais: 0 faixas, 0 entradas (resumo decide, dado nem viaja)
# 1 chave a mais em b: 1 faixa diverge, 0 entradas voltam (versao de b vence)
# reverso: 1 faixa, 1 entrada, lojas iguais (sincroniza o que diverge)
# 3 faixas divergentes: 3 faixas, 3 entradas, resto nem viajou
# 23g ok: resumo decide, faixa igual nao viaja, divergente sincroniza
```

- **Faixa:** 8 chaves contíguas com um resumo FNV-1a de 32 bits sobre `(presente, valor, versao)` de cada entrada. 64 chaves, 8 faixas, 8 resumos por rodada.
- **Direção:** só viaja a entrada cuja versão no destino é menor. Resumo igual pula a faixa sem tocar nos dados; resumo diferente entra e copia entrada a entrada, cada uma na direção da versão maior.
- **Casos:** lojas iguais (0 faixas), 1 chave divergente (1 faixa, direção conforme a versão), 3 faixas espalhadas (3 faixas, resto intocado).
- **Fora do contrato:** colisão tratada como impossível (FNV-1a de 32 bits colide em teoria; a aula declara a probabilidade em vez de fingir impossibilidade), remoção com lixeira (só `presente`, sem GC de deletados), resumo hierárquico (Merkle de verdade, com prova por caminho — trabalho futuro listado no checklist). E sem rede: bytes de fio seriam invenção sem socket — o exemplo conta faixas e entradas, não bytes.

## 2. Onde cada decisão mora

**O resumo decide o escopo.** `resumo_faixa` condensa 8 entradas em 32 bits; faixas iguais são puladas sem tocar nos dados. O ganho é medido em faixas, não em bytes de fio: 64 chaves, 1 divergente → 1/8 do custo da sincronização total (mais 8 resumos, desprezível). A honestidade da medida: o programa conta faixas e entradas, porque contar "bytes economizados" sem socket seria inventar o fio para provar a economia. Em produção com rede, a conta fecha assim: 8 resumos (32 bytes) + faixas divergentes × entradas — contra 64 entradas sempre.

**A versão decide a direção.** Divergência não significa "copia tudo": só viaja a entrada cuja versão no destino é menor (`b->e[i].versao < a->e[i].versao`). O cenário 2 mostra os dois lados na mesma chave: `b` à frente (versão 2) sincronizando de `a` (versão 1) move 0 entradas — copiar cegamente traria o valor velho por cima do novo, e "sincronizar" viraria "regredir". É a mesma regra de "termo vence" da 23d, aplicada por entrada em vez de por log: a granularidade mudou, a autoridade da versão não.

**O conteúdo do resumo é campo a campo.** O buffer é montado com `memcpy` de `(presente, valor, versao)` por entrada — 12 bytes por entrada, 96 por faixa. Por que não `fnv1a(&loja, sizeof)` direto? Porque struct tem padding não-inicializado: os mesmos valores lógicos gerariam resumos diferentes entre compilações, arquiteturas ou até execuções (lixo de pilha no padding). Resumo sobre padding é loteria; resumo sobre campos é função do dado. Essa é a mesma disciplina do `TrapFrame` com `repr(C)`: serializar significa escolher bytes, nunca despejar struct.

## 3. O programa, por inteiro

O programa abaixo é o [arquivo-fonte](exemplos/23g_antientropia.c), reproduzido integralmente:

```c
/* Aula 23g — anti-entropia por resumos: sincronizar pelo que mudou.
 *
 * Dois nos com 64 chaves versionadas cada. Em vez de trocar os logs
 * inteiros (a 23c manda tudo), trocam 8 resumos FNV-1a (um por faixa de
 * 8 chaves): faixas iguais nem viajam; faixas divergentes trocam as 8
 * entradas. Tres cenarios deterministicos: lojas iguais (0 faixas),
 * uma chave divergente (1 faixa) e faixas espalhadas (sincroniza tudo
 * que diverge e nada que coincide).
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23g_antientropia.c -o /tmp/23g
 * Executar: /tmp/23g
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum { N_CHAVES = 64, FAIXAS = 8, POR_FAIXA = N_CHAVES / FAIXAS };

typedef struct {
    int presente;
    int valor;
    uint32_t versao;
} Entrada;

typedef struct {
    Entrada e[N_CHAVES];
} Loja;

static uint32_t fnv1a(const void *dados, size_t n)
{
    const unsigned char *p = dados;
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < n; i++) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

/* Resumo da faixa: FNV sobre (presente, valor, versao) das 8 entradas. */
static uint32_t resumo_faixa(const Loja *l, int f)
{
    unsigned char buf[POR_FAIXA * 12];
    size_t n = 0;
    for (int i = f * POR_FAIXA; i < (f + 1) * POR_FAIXA; i++) {
        memcpy(buf + n, &l->e[i].presente, 4);
        n += 4;
        memcpy(buf + n, &l->e[i].valor, 4);
        n += 4;
        memcpy(buf + n, &l->e[i].versao, 4);
        n += 4;
    }
    return fnv1a(buf, n);
}

static void loja_init(Loja *l)
{
    for (int i = 0; i < N_CHAVES; i++) {
        l->e[i].presente = 1;
        l->e[i].valor = i * 10;
        l->e[i].versao = 1;
    }
}

/* Sincroniza b a partir de a. Volta faixas trocadas e entradas movidas. */
static void antientropia(const Loja *a, Loja *b, int *faixas, int *entradas)
{
    *faixas = 0;
    *entradas = 0;
    for (int f = 0; f < FAIXAS; f++) {
        if (resumo_faixa(a, f) == resumo_faixa(b, f))
            continue; /* faixa igual: nem viaja */
        (*faixas)++;
        for (int i = f * POR_FAIXA; i < (f + 1) * POR_FAIXA; i++) {
            if (b->e[i].versao < a->e[i].versao) {
                b->e[i] = a->e[i];
                (*entradas)++;
            }
        }
    }
}

static int iguais(const Loja *a, const Loja *b)
{
    return memcmp(a, b, sizeof *a) == 0;
}

int main(void)
{
    Loja a, b;
    int faixas, entradas;

    /* Cenario 1: lojas iguais — zero faixas viajam. */
    loja_init(&a);
    loja_init(&b);
    antientropia(&a, &b, &faixas, &entradas);
    assert(faixas == 0 && entradas == 0 && iguais(&a, &b));
    printf("iguais: 0 faixas, 0 entradas (resumo decide, dado nem viaja)\n");

    /* Cenario 2: uma chave divergente — exatamente 1 faixa viaja. */
    b.e[40].valor = 9999;
    b.e[40].versao = 2;
    antientropia(&a, &b, &faixas, &entradas);
    assert(faixas == 1 && entradas == 0 && !iguais(&a, &b));
    printf("1 chave a mais em b: 1 faixa diverge, 0 entradas voltam (versao de b vence)\n");
    loja_init(&b);
    b.e[40].valor = 9999;
    b.e[40].versao = 2;
    antientropia(&b, &a, &faixas, &entradas);
    assert(faixas == 1 && entradas == 1 && iguais(&a, &b));
    assert(a.e[40].valor == 9999 && a.e[40].versao == 2);
    printf("reverso: 1 faixa, 1 entrada, lojas iguais (sincroniza o que diverge)\n");

    /* Cenario 3: divergencias espalhadas em 3 faixas — o resto nem viaja. */
    loja_init(&a);
    loja_init(&b);
    a.e[3].valor = 1;
    a.e[3].versao = 5; /* faixa 0 */
    a.e[30].valor = 2;
    a.e[30].versao = 5; /* faixa 3 */
    a.e[63].valor = 3;
    a.e[63].versao = 5; /* faixa 7 */
    antientropia(&a, &b, &faixas, &entradas);
    assert(faixas == 3 && entradas == 3 && iguais(&a, &b));
    printf("3 faixas divergentes: 3 faixas, 3 entradas, resto nem viajou\n");

    printf("23g ok: resumo decide, faixa igual nao viaja, divergente sincroniza\n");
    return 0;
}
```

Com sanitizers (exigido para ponte: ASan/UBSan limpos):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/c/exemplos/23g_antientropia.c -o sins23g_asan
./sins23g_asan   # identico
```

## 4. Leitura guiada dos cenários

**Cenário 1 (0 faixas):** o caso comum em produção — nós que já convergiam. Custo: 8 comparações de 32 bits. Sem resumos, o mesmo caso custaria 64 entradas comparadas (ou pior, transmitidas). A economia do caso comum é o que paga a sincronização frequente — e sincronização frequente é o que impede divergências grandes. O resumo não otimiza o caso raro; ele barateia o caso comum até a anti-entropia poder rodar sempre.

**Cenário 2 (1 faixa, 0 entradas num sentido / 1 no outro):** a mesma divergência física, duas direções lógicas. De `a` para `b`: faixa diverge (resumo difere), mas nenhuma entrada volta — porque `b` está à frente em versão. O `!iguais` final confirma: as lojas *continuam* diferentes, e corretamente — `a` está desatualizado e a sincronização nesta direção não o atualiza (a direção certa seria a oposta). De `b` para `a`: 1 faixa, 1 entrada, `iguais` — e os dois `assert` finais conferem valor E versão (copiar valor sem versão quebraria a próxima rodada: a versão velha no destino faria a entrada "divergir" para sempre).

**Cenário 3 (3 faixas, 3 entradas):** divergências nas faixas 0, 3 e 7 (chaves 3, 30, 63 — espalhadas de propósito para provar que o resumo não depende de localidade). 5 faixas intactas nem são tocadas: o `iguais` final com 3 entradas movidas prova convergência total com custo parcial. Note que cada faixa divergente moveu exatamente 1 entrada — as outras 7 da faixa viajaram (custo do grão de 8) mas não foram copiadas (versões iguais). Grão maior = menos resumos, mais desperdício por faixa; grão menor = mais resumos, menos desperdício. 8 é o meio didático; produção ajusta pelo tamanho da divergência típica.

## 5. O resumo pode mentir (e a aula diz quando)

FNV-1a de 32 bits tem colisão: duas faixas diferentes com o mesmo resumo sincronizam "nada" e divergem para sempre — silenciosamente, porque o protocolo confia no resumo. Com 8 faixas por rodada, a probabilidade por rodada é desprezível (~8²/2³³), mas "desprezível" não é "zero": em produção com milhões de faixas, 32 bits colidem de verdade (aniversário em ~77 mil faixas para 50%). A correção é subir para 64+ bits (FNV-1a 64, xxHash64) ou Merkle com re-check: se a faixa diverge mas nenhuma entrada difere após a cópia, desconfie do resumo, não dos dados. A aula declara a colisão no contrato em vez de calar — resumo didático promete detecção "quase sempre" e lista o upgrade; promessa falsa prometeria "sempre".

Há um segundo modo de mentira, mais sutil: resumo sobre dado *parcialmente atualizado*. Se uma escrita concorrente muda a entrada no meio do cálculo do resumo (sem lock, sem versão congelada), o resumo mistura velho com novo e não corresponde a nenhum estado real — nem o de antes nem o de depois. Nesta simulação não há concorrência (um thread, sem preempção no cálculo), mas em produção o resumo se calcula sobre snapshot (cópia ou versão congelada), nunca sobre dado vivo. Resumir dado que muda durante o resumo é fotografar corredor em movimento: a foto existe, mas ninguém passou por ela.

## 6. Erros comuns e implicações defensivas

1. **Sincronizar sem versão:** copiar a faixa inteira na direção errada apaga escrita mais nova. Resumo sem versão é detecção sem resolução — detecta onde diverge, mas resolve errado. Versão decide direção; resumo decide escopo; um sem o outro é metade do protocolo.
2. **Resumir o ponteiro em vez do conteúdo:** FNV sobre endereço, ou sobre struct com padding não-inicializado, varia entre processos, boots e compilações. O `memcpy` campo a campo existe para o resumo ser função do *dado* — teste rápido: rode o exemplo duas vezes e compare; se o resumo mudasse, o programa seria não-determinístico.
3. **Tratar colisão como impossível:** 32 bits colidem; teste que nunca viu colidir não prova ausência (prova só 3 cenários). O contrato promete "quase sempre" e lista o upgrade (64 bits, Merkle) em vez de calar — honestidade editorial sobre probabilidade.
4. **Mandar tudo "porque é simples":** simplicidade que custa 64× o necessário em cada rodada não é simples — é o motivo pelo qual sincronização frequente morre em enlace caro. O resumo é a diferença entre "sincroniza sempre" (barato no caso comum) e "sincroniza quando dá" (caro sempre).
5. **Grão errado para a divergência típica:** faixa de 8 com divergências de 1 entrada desperdiça 7/8 do que viaja na faixa; faixa de 1024 com divergências espalhadas viaja tudo de novo. O grão se ajusta pela divergência observada — e se mede, não se chuta.
6. **Resumir dado vivo sob concorrência:** sem snapshot, o resumo não corresponde a nenhum estado real. Em produção: congela (versão, cópia, lock curto) antes de resumir — a ordem é congelar, resumir, comparar, copiar.
7. **Chamar de "Merkle":** Merkle de verdade é hierárquico (resumo de resumos, com prova logarítmica por caminho). Isto é uma lista plana de resumos — o degrau anterior, honesto e suficiente para 64 chaves. O nome certo evita que o checklist cobre prova por caminho de quem nunca a implementou.

## 7. Ponte com as demais trilhas

C++ resolve a mesma divergência por cura com termo novo na [23f](../cpp/23f_eleicao_commit_rpc.md) (descarte nomeado, sem resumo — quando o termo decide, o dado obedece); Go repara na leitura com vetor na 23c (paga o custo total por leitura, sem rodada de fundo); Rust declara o conflito e repara em duas passadas na 23d (o resumo como pré-filtro antes do vetor). Em Python, o análogo honesto é comparar `hashlib` por partição antes de copiar arquivos (rsync didático: resumo decide, cópia obedece). Em JavaScript, o par é ETag/If-None-Match no HTTP: resumo (hash do corpo) decide se o corpo viaja — a mesma divisão escopo/direção, com o servidor no papel do outro nó. O padrão "resumo decide escopo, versão decide direção" vale nas cinco trilhas — muda a função de hash, o tamanho da faixa e quem paga a rodada.

## 8. Critério de aprovação e checklist

- [ ] Rodo o exemplo e digo, por cenário, quantas faixas viajaram, quantas entradas se moveram — e por quê (resumo + versão).
- [ ] Explico por que o cenário 2 copia 0 entradas num sentido e 1 no outro (direção pela versão).
- [ ] Descrevo um caso em que o resumo mente (colisão 32 bits; resumo de dado vivo) e o que muda com 64 bits + snapshot.
- [ ] Justifico o `memcpy` campo a campo contra padding não-inicializado (resumo de layout ≠ resumo de dado).
- [ ] Dimensiono o grão: quando faixa maior ajuda, quando atrapalha, e como medir para decidir.
- [ ] Distingo lista plana de resumos de Merkle hierárquico — e digo o que a hierarquia compraria (prova logarítmica).
- [ ] Nomeio o que falta: Merkle com prova por caminho, tumbas com GC, resumo de 64 bits, transporte real com contagem de bytes.

**Referências primárias:** o algoritmo de *anti-entropy* com Merkle trees do Dynamo (DeCandia et al., SOSP 2007) como origem do padrão resumo-escopo/versão-direção (aqui, versão em vez de timestamp); especificação da [FNV-1a](https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function) para a função usada. Nenhum texto foi copiado — a implementação é própria, em um processo, sem rede; a simulação isola a regra como a 23d isola o commit.

## 9. Exercícios obrigatórios

1. (Previsão) Dobre o grão (`FAIXAS = 4`, 16 entradas por faixa) e preveja: no cenário 3, quantas faixas e quantas entradas o contador mostra? Rode e explique o que o contador *esconde* — quantas entradas intactas viajaram por faixa divergente.
2. (Diagnóstico) Apague a guarda de versão (copie sempre que o resumo difere): preveja o que o cenário 2 (a→b) faz com a escrita mais nova de `b`. Rode e explique por que "resumo sem versão detecta onde diverge, mas resolve errado".
3. (Projeto) Suba o resumo para FNV-1a de 64 bits (offset `14695981039346656037`, primo `1099511628211`) e rode os três cenários: passam iguais? Explique o que mudou (espaço de colisão 2³²→2⁶⁴, resumo de 4→8 bytes por faixa) e o que *não* mudou (a mentira continua possível, só mais rara).
4. (Previsão) Re-rode a anti-entropia logo após o cenário 3, sem mudar nada: preveja faixas e entradas. Rode e explique por que esse zero é o que permite rodar a rodada sempre — barata no caso comum.

Use [pistas graduais](exercicios/23g_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23g_solucoes.md) depois de registrar sua própria explicação.

---

### Pesos da avaliacao

| Criterio | Peso |
|---|---|
| Compreensao conceitual (previsao, diagnostico, explicacao) | 40% |
| Analise de compilacao e ferramentas | 25% |
| Funcionamento (compila sem avisos e roda) | 20% |
| Documentacao (comandos, versao, saidas) | 15% |

A execucao bem-sucedida sozinha nao aprova: a maior parcela esta em demonstrar que voce entendeu o porque.

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1* (Addison-Wesley, 3. ed., ISBN 978-0131411555).
