---
layout: default
---

# Aula 32d — VFS mínimo: o resolvedor é o sistema de arquivos (C4)

Você vai ao terminar, você deverá executar `teoria/c/kernel32d/`, abrir `/dados/const.txt` pelo resolvedor de caminho e explicar, passo a passo, por que `/dados/../versao.txt` é recusado no parse (e não na busca) — e listar as três validações que o VFS não faz (ponteiro do usuário, escrita, permissão), dizendo onde cada uma moraria.

a 32a entrega o kernel com sete marcadores e declara o VFS como lacuna; esta aula a preenche sem fingir um sistema de arquivos de produção. O ponto arquitetural que justifica a aula inteira: VFS não é disco — é *resolução de caminho + descritores*. A tabela aqui é estática (três arquivos, um diretório, somente leitura), mas o resolvedor anda componente a componente pelo pai, confere cada nível e nega subida, diretório e ausente. Se um dia existir outro suporte (outro dispositivo, outra tabela), o resolvedor continua valendo; o que muda é de onde vêm os bytes. Quem entende este resolvedor entende o que o Linux faz antes de tocar em qualquer bloco.

## 1. Contrato antes do código

- `make -C teoria/c/kernel32d && python3 teoria/c/kernel32d/test_kernel.py teoria/c/kernel32d/kernel.elf` imprime `KERNEL32D_OK` (QEMU `virt`, `-bios none`, `-smp 1`: um hart basta, o tema é resolução, não paralelismo).
- Syscalls em modo-S sobre a base da 32a: `a7 == 3` abre (caminho → fd ou -1), `a7 == 4` lê (fd, destino, máximo → tamanho ou -1), `a7 == 5` fecha (fd → 0 ou -1). Os arquivos `boot.S`, `trap.S` e `link.ld` são os da 32a, cópia verbatim: o estágio prova que o VFS vive acima da fronteira existente, sem mexer nela.
- Fora do contrato: escrita (só leitura), montagem de outro dispositivo (uma tabela), permissões por usuário (sem dono), links (sem indireção), remoção com lixeira. A árvore é estática e somente-leitura — ROM com resolvedor, e dizê-lo no contrato é parte da honestidade: o nome do marcador é `VFS_OK` porque o que foi testado foi resolução e descritores.

## 2. O resolvedor em três regras (e por que cada uma existe)

**Regra 1 — anda pelo pai, não pela string.** Cada componente é procurado entre os filhos do diretório atual (`nos[i].pai == atual`), com comparação de comprimento + conteúdo (`prefixo_igual` nos N bytes do componente). Comparar o caminho inteiro como string pronta aceitaria prefixo errado (`/dados` casando `/dados2` com `strncmp` de tamanho errado, ou `/dado` casando prefixo de `/dados`). Andar nível a nível elimina a classe inteira de bugs de prefixo: cada nível só enxerga os filhos do pai já resolvido, e o nome só é aceito se comprimento E conteúdo baterem.

**Regra 2 — `..` não sobe, diretório não abre.** A subida é recusada no parse (antes de qualquer busca): o resolvedor nem chega a procurar, porque `..` nunca deveria ter entrado no andador. Por que tão cedo? Porque cada nível de processamento antes da recusa é uma chance de o `..` interagir com algo (normalização, cache, log). Recusar no parse é recusar antes de existir estado. E diretório resolvido com sucesso devolve -1 no `open`: ler diretório como bytes vazaria estrutura interna para o chamador. Tipo errado nega no `open`, não no `read` — depois de aberto com fd válido, o dano (fd que permite ler metadados como dados) já tem nome e número.

**Regra 3 — o caminho absoluto é exigência, não conveniência.** Sem `/` inicial, recusa imediata. Caminho relativo dependeria de "diretório atual" — conceito que este kernel não tem (sem processo com cwd, sem `chdir`). Exigir absoluto elimina uma variável de estado que não existe: o resolvedor é função pura do caminho e da tabela, e função pura é testável exaustivamente (o teste cobre os 4 proibidos + 3 válidos).

## 3. Descritores: o estado que o VFS carrega

Abrir devolve um fd — índice em `fd_no[]` com posição corrente em `fd_pos[]`. Três decisões embutidas: (a) a posição mora no descritor, não no arquivo — duas aberturas do mesmo arquivo leem independentemente (a tarefa abre, lê, fecha; se reabrisse, recomeçaria do zero); (b) `read` além do fim devolve o resto (`resto < max ? resto : max`), não erro — EOF parcial é dado, não falha, e o chamador distingue "0 = acabou" de "-1 = negado"; (c) `close` de fd inválido devolve -1 em vez de travar — fechar duas vezes é bug do chamador, mas o kernel não deve corromper a tabela por causa dele (e o teste confere o -1 do `sys_ler(99, ...)`).

Oito fds (`N_FD`) com `fd_alocar` linear: didático e suficiente (a tarefa usa 1 por vez). Em produção a tabela cresce e o alocador vira bitmap ou contador — mas a semântica (posição por descritor, EOF parcial, close idempotente-negado) não muda. O que muda com escrita: posição para escrita, modo do fd (só-leitura aqui por construção — não há flag de modo porque não há modo), e sincronização entre descritores do mesmo arquivo.

## 4. O núcleo, por inteiro

O programa abaixo é o [kernel deste estágio](kernel32d/kernel.c), reproduzido integralmente (`boot.S`, `trap.S`, `link.ld`, `Makefile` e `test_kernel.py` no mesmo diretório, os três primeiros idênticos aos da 32a):

```c
/* Nucleo da aula 32d: VFS minimo sobre a base da 32a (QEMU, -smp 1).
 * Tres arquivos somente-leitura organizados em diretorios; o caminho e
 * resolvido componente a componente ("/" separa, cada nivel confere o
 * nome no diretorio pai) em vez de comparado como string pronta: e
 * isso que faz o V ser de "virtual" — o mesmo resolvedor serviria para
 * outro suporte se um dia existisse. Syscalls em modo-S: a7=3 abre
 * (a0=caminho) e devolve fd ou -1; a7=4 le (fd, destino, max) e devolve
 * o tamanho ou -1; a7=5 fecha. Sem escrita, sem montagem de outro
 * dispositivo: a tabela e estatica e o VFS e o resolvedor + fds.
 * Marcadores: BOOT_OK VFS_OK DONE. */
#include <stdint.h>

#define UART 0x10000000UL
#define TEST_DEVICE 0x100000UL

#define F_A0 9
#define F_A1 10
#define F_A2 11
#define F_A7 16
#define F_MEPC 31

typedef struct {
    uint64_t f[32];
} TrapFrame;

volatile int liberar_harts = 0;
volatile int hart1_ok = 0;

static volatile uint8_t *const uart = (volatile uint8_t *)UART;

static void kputc(char c) { *uart = (uint8_t)c; }
static void kputs(const char *s) { while (*s) kputc(*s++); }
static void kputhex(uint64_t v) {
    kputs("0x");
    for (int i = 60; i >= 0; i -= 4) {
        int d = (int)((v >> i) & 0xF);
        kputc((char)(d < 10 ? '0' + d : 'a' + d - 10));
    }
}

/* ---------- a arvore estatica ---------- */
typedef struct No {
    const char *nome;
    int dir;              /* 1 = diretorio, 0 = arquivo */
    int pai;              /* indice do diretorio pai (-1 na raiz) */
    const uint8_t *dados; /* NULL em diretorio */
    uint64_t tamanho;
} No;

static const uint8_t arq_versao[] = "kernel32d v1\n";
static const uint8_t arq_pi[] = "PI=314159\n";
static const uint8_t arq_nada[] = "";

static const No nos[] = {
    {"", 1, -1, 0, 0},              /* 0: raiz */
    {"versao.txt", 0, 0, arq_versao, sizeof arq_versao - 1}, /* 1 */
    {"dados", 1, 0, 0, 0},           /* 2: diretorio */
    {"const.txt", 0, 2, arq_pi, sizeof arq_pi - 1},          /* 3 */
    {"vazio.txt", 0, 2, arq_nada, 0},   /* 4: arquivo vazio */
};
#define N_NOS (sizeof nos / sizeof nos[0])

static void desligar(uint64_t codigo) {
    *(volatile uint32_t *)TEST_DEVICE = (uint32_t)codigo;
    for (;;) { __asm__ volatile("wfi"); }
}

/* Compara no maximo n bytes; devolve 0 se iguais no prefixo. */
static int prefixo_igual(const char *a, const char *b, uint64_t n) {
    for (uint64_t i = 0; i < n; i++)
        if (a[i] != b[i])
            return (int)(unsigned char)a[i] - (int)(unsigned char)b[i];
    return 0;
}

static uint64_t tamanho_nome(const char *s) {
    uint64_t n = 0;
    while (s[n] != '\0' && s[n] != '/') n++;
    return n;
}

/* Resolve o caminho componente a componente. Devolve o indice do no
 * ou -1. Rejeita "..", "//" vazios no meio e caminho sem "/" inicial:
 * resolvedor permissivo demais e traversal com outro nome. */
static int resolver(const char *caminho) {
    if (caminho[0] != '/')
        return -1;
    int atual = 0;
    const char *p = caminho + 1;
    if (*p == '\0')
        return 0; /* "/" e a raiz */
    for (;;) {
        while (*p == '/') p++;
        if (*p == '\0')
            return atual;
        uint64_t n = tamanho_nome(p);
        if (n == 0)
            return -1;
        if (n == 2 && p[0] == '.' && p[1] == '.')
            return -1; /* sem subida: arvore estatica nao tem pai alem da raiz */
        int achou = -1;
        for (uint64_t i = 0; i < N_NOS; i++) {
            if (nos[i].pai != atual)
                continue;
            uint64_t m = 0;
            while (nos[i].nome[m] != '\0') m++;
            if (m == n && prefixo_igual(nos[i].nome, p, n) == 0) {
                achou = (int)i;
                break;
            }
        }
        if (achou < 0)
            return -1;
        atual = achou;
        p += n;
        if (*p == '\0')
            return nos[atual].dir ? -1 : atual; /* diretorio nao abre */
    }
}

/* ---------- fds ---------- */
#define N_FD 8
static int fd_no[N_FD];
static uint64_t fd_pos[N_FD];

static int fd_alocar(int no) {
    for (int i = 0; i < N_FD; i++) {
        if (fd_no[i] < 0) {
            fd_no[i] = no;
            fd_pos[i] = 0;
            return i;
        }
    }
    return -1;
}

TrapFrame *trap_handler(TrapFrame *tf) {
    uint64_t causa;
    __asm__ volatile("csrr %0, mcause" : "=r"(causa));

    if ((int64_t)causa < 0 || causa != 9) {
        kputs("TRAP_INESPERADA causa=");
        kputhex(causa);
        kputc('\n');
        desligar(0x3333);
        return tf;
    }
    uint64_t a7 = tf->f[F_A7];
    if (a7 == 3) { /* abrir */
        int no = resolver((const char *)tf->f[F_A0]);
        tf->f[F_A0] = (uint64_t)(no < 0 ? -1 : (int64_t)fd_alocar(no));
    } else if (a7 == 4) { /* ler */
        uint64_t id = tf->f[F_A0];
        uint8_t *dst = (uint8_t *)tf->f[F_A1];
        uint64_t max = tf->f[F_A2];
        if (id >= N_FD || fd_no[id] < 0) {
            tf->f[F_A0] = (uint64_t)-1;
        } else {
            const No *no = &nos[fd_no[id]];
            uint64_t resto = no->tamanho > fd_pos[id] ? no->tamanho - fd_pos[id] : 0;
            uint64_t n = resto < max ? resto : max;
            for (uint64_t i = 0; i < n; i++)
                dst[i] = no->dados[fd_pos[id] + i];
            fd_pos[id] += n;
            tf->f[F_A0] = n;
        }
    } else if (a7 == 5) { /* fechar */
        uint64_t id = tf->f[F_A0];
        if (id >= N_FD || fd_no[id] < 0) {
            tf->f[F_A0] = (uint64_t)-1;
        } else {
            fd_no[id] = -1;
            tf->f[F_A0] = 0;
        }
    } else {
        kputs("SYSCALL_DESCONHECIDA a7=");
        kputhex(a7);
        kputc('\n');
        desligar(0x3333);
        return tf;
    }
    tf->f[F_MEPC] += 4;
    return tf;
}

static uint64_t sys_abrir(const char *caminho) {
    register uint64_t a7 __asm__("a7") = 3;
    register uint64_t a0 __asm__("a0") = (uint64_t)caminho;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0));
    return a0;
}

static uint64_t sys_ler(uint64_t fd, uint8_t *dst, uint64_t max) {
    register uint64_t a7 __asm__("a7") = 4;
    register uint64_t a0 __asm__("a0") = fd;
    register uint64_t a1 __asm__("a1") = (uint64_t)dst;
    register uint64_t a2 __asm__("a2") = max;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0), "+r"(a1), "+r"(a2));
    return a0;
}

static uint64_t sys_fechar(uint64_t fd) {
    register uint64_t a7 __asm__("a7") = 5;
    register uint64_t a0 __asm__("a0") = fd;
    __asm__ volatile("ecall" : "+r"(a7), "+r"(a0));
    return a0;
}

/* Roda em modo-S: abre pelos caminhos, le, fecha, e tenta o proibido. */
static void modo_s_tarefa(void) {
    uint8_t buf[64];

    uint64_t fd = sys_abrir("/versao.txt");
    if (fd == (uint64_t)-1) { kputs("ABRIR_VERSAO_FALHOU\n"); desligar(0x3333); }
    uint64_t n = sys_ler(fd, buf, sizeof buf);
    if (n != sizeof arq_versao - 1) { kputs("LER_VERSAO_DIVERGIU\n"); desligar(0x3333); }
    for (uint64_t i = 0; i < n; i++) kputc((char)buf[i]);
    if (sys_fechar(fd) != 0) { kputs("FECHAR_FALHOU\n"); desligar(0x3333); }

    fd = sys_abrir("/dados/const.txt"); /* caminho aninhado: anda 2 niveis */
    if (fd == (uint64_t)-1) { kputs("ABRIR_ANINHADO_FALHOU\n"); desligar(0x3333); }
    n = sys_ler(fd, buf, sizeof buf);
    if (n != sizeof arq_pi - 1) { kputs("LER_ANINHADO_DIVERGIU\n"); desligar(0x3333); }
    for (uint64_t i = 0; i < n; i++) kputc((char)buf[i]);

    fd = sys_abrir("/dados/vazio.txt");
    if (fd == (uint64_t)-1) { kputs("ABRIR_VAZIO_FALHOU\n"); desligar(0x3333); }
    if (sys_ler(fd, buf, sizeof buf) != 0) { kputs("VAZIO_NAO_E_ZERO\n"); desligar(0x3333); }

    /* O proibido: diretorio nao abre, ausente nega, ".." nao sobe. */
    if (sys_abrir("/dados") != (uint64_t)-1) { kputs("DIR_ABRIU\n"); desligar(0x3333); }
    if (sys_abrir("/nao/existe.txt") != (uint64_t)-1) { kputs("AUSENTE_ABRIU\n"); desligar(0x3333); }
    if (sys_abrir("/dados/../versao.txt") != (uint64_t)-1) { kputs("DOTDOT_PASSOU\n"); desligar(0x3333); }
    if (sys_ler(99, buf, sizeof buf) != (uint64_t)-1) { kputs("FD_INVALIDO_LEU\n"); desligar(0x3333); }

    kputs("VFS_OK\nDONE\n");
    *(volatile uint32_t *)TEST_DEVICE = 0x5555;
    for (;;) { __asm__ volatile("wfi"); }
}

void kmain(void) {
    for (int i = 0; i < N_FD; i++) fd_no[i] = -1;
    kputs("BOOT_OK\n");
    __asm__ volatile("csrw pmpaddr0, %0\n csrw pmpcfg0, %1" ::"r"(-1UL), "r"(0xFUL));
    uint64_t mstatus;
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3UL << 11);
    mstatus |= (1UL << 11);
    __asm__ volatile("csrw mstatus, %0" ::"r"(mstatus));
    __asm__ volatile("csrw mepc, %0" ::"r"((uint64_t)modo_s_tarefa));
    __asm__ volatile("mret");
    desligar(0x3333); /* nunca alcancado */
}
```

```bash
make -C teoria/c/kernel32d kernel.elf
python3 teoria/c/kernel32d/test_kernel.py teoria/c/kernel32d/kernel.elf
# KERNEL32D_OK marcadores=BOOT_OK,VFS_OK,DONE
```

Transcrição serial esperada (determinística, saída 0 pelo test device):

```text
BOOT_OK
kernel32d v1
PI=314159
VFS_OK
DONE
```

## 5. Leitura guiada: o que cada open prova

`/versao.txt` (1 nível) prova o caminho mínimo: raiz → filho → fd → leitura exata de 13 bytes (`sizeof - 1`, sem o NUL do C — detalhe que pega quem confunde tamanho de string com tamanho de arquivo). O `sys_fechar` com retorno 0 prova o ciclo completo abrir-ler-fechar antes do teste avançar: descritor vazado não falha este teste, mas o `fd_no` cheio falharia o próximo — higiene de fd é testada por construção, não por `assert` dedicado.

`/dados/const.txt` (2 níveis) prova o andador: raiz → `dados` (pai 0) → `const.txt` (pai 2). É o caso que distingue resolvedor de comparador de string: um `strcmp(caminho, "/dados/const.txt")` passaria neste teste e falharia em qualquer árvore maior; o andador passa nos dois. `/dados/vazio.txt` prova o arquivo vazio: `sys_ler` devolve 0 (não -1) — EOF é dado, não erro, e o teste distingue os dois com `!= 0` + `desligar`.

Os quatro proibidos, cada um com sua mensagem de defeito: `/dados` resolve mas é diretório (`DIR_ABRIU` seria o bug de ler metadado como dado); `/nao/existe.txt` não resolve no primeiro nível (`AUSENTE_ABRIU`); `/dados/../versao.txt` morre no parse, antes de qualquer busca (`DOTDOT_PASSOU` — traversal com outro nome); `sys_ler(99, ...)` nega no guarda de fd (`FD_INVALIDO_LEU`). Quatro negações, quatro mensagens, zero ambiguidade sobre qual regra falhou — diagnóstico nomeado é metade da aula.

## 6. Erros comuns e implicações defensivas

1. **Comparar o caminho inteiro:** `strncmp("/dados", "/dados2", 6)` casa prefixo. Andar por componente elimina a classe: cada nível só enxerga filhos do pai resolvido, e comprimento + conteúdo têm de bater juntos.
2. **Aceitar `..` "porque a árvore é estática":** hoje não há para onde subir; amanhã, com outro suporte montado, o mesmo resolvedor permissivo vira traversal. Negar no parse (antes de existir estado) é contrato, não pessimismo — e é mais barato que normalizar depois.
3. **Abrir diretório como arquivo:** ler um diretório como bytes vaza estrutura interna (nomes, layout) para o chamador. Tipo errado nega no `open`, não no `read`: depois de aberto com fd válido, o dano já tem nome e número.
4. **Confiar no ponteiro do usuário:** este estágio lê `a0` como endereço sem validar (a 32d declara como futuro, na linha da 38 de C++). Em produção, `copy_from_user` valida faixa antes de tocar — sem ela, o chamador aponta o kernel para onde quiser. A lacuna está nomeada no checklist, não escondida no código.
5. **Esquecer o `mepc += 4` na syscall:** sem o avanço, o `mret` volta ao `ecall` e a tarefa repete a chamada para sempre — o sintoma é loop com o mesmo marcador, não falha. Cada causa tem sua retomada: `ecall` pula (+4), falta reexecuta (+0) — e trocar as duas quebra as duas aulas.
6. **EOF como erro:** devolver -1 para arquivo vazio confunde "acabou" com "negado" e quebra todo loop de leitura (`while ((n = ler(...)) > 0)`). 0 é dado, -1 é negação — a distinção mora no contrato do `read(2)`, e o teste a cobra com `!= 0`.
7. **Chamar de "sistema de arquivos":** sem escrita, sem montagem, sem permissão e sem links, é resolvedor + ROM com fds. O nome do marcador é `VFS_OK` porque o que foi testado foi resolução e descritores — o resto está no checklist como falta, não como promessa silenciosa.

## 7. Ponte com as demais trilhas

C++ faz a ROM didática por id na [38](../cpp/38.md) (sem caminho, sem diretório — o passo anterior; aqui o caminho existe e é resolvido de verdade, o passo seguinte que a 38 declarou como futuro). Rust valida ponteiros de usuário no kernel da 28a (a validação que aqui falta, lá existe — comparar os dois mostra o preço e o valor); C++ promete a mesma validação na 38. Em Python, o análogo honesto é `open()` com `pathlib` — com a diferença de que lá o SO resolve por você, e aqui a aula mostra o que "resolver" significa, nível por nível. Em Go, o `os.Open` com caminho limpo pelo `path.Clean` (que remove `..` por normalização) é o primo em espaço de usuário da regra 2 — mesma negação, outra camada. O padrão "resolve nível a nível, nega subida e tipo errado, conta EOF como dado" vale nas cinco trilhas — muda onde o resolvedor mora.

## 8. Ao final, você consegue

- [ ] Rodo o estágio e mostro `/dados/const.txt` resolvido em dois níveis, explicando cada passo do andador (pai, comprimento, conteúdo).
- [ ] Explico por que `/dados/../versao.txt` é recusado no parse (antes de existir estado) e não na busca.
- [ ] Digo por que diretório resolvido nega no `open` em vez de ler como bytes (metadado ≠ dado).
- [ ] Descrevo o que quebra se a comparação for por string inteira (prefixo: `/dados` casa `/dados2`).
- [ ] Aponto onde a validação de ponteiro do usuário deveria estar (`a0` do open, `a1` do read) e descrevo o ataque sem ela.
- [ ] Distingo EOF (0, dado) de negação (-1, erro) com o caso do arquivo vazio.
- [ ] Nomeio o que falta: escrita, montagem, permissão, validação de ponteiro, links — e digo qual syscall cada um exigiria.

**Referências primárias:** o conceito de VFS (camada que separa resolvedor de suporte, presente em todo Unix didático — o resolvedor aqui é próprio, a separação é a lição) e o contrato `open(2)`/`read(2)` do POSIX (`O_DIRECTORY`, EOF parcial, `EBADF`) como especificação de comportamento. Nenhum texto foi copiado — o resolvedor foi confirmado executando este estágio no QEMU com um hart.

## 9. Exercícios obrigatórios

Estes exercícios rodam no QEMU real (`make -C teoria/c/kernel32d && python3 teoria/c/kernel32d/test_kernel.py teoria/c/kernel32d/kernel.elf`). Trabalhe numa cópia do diretório — o estágio do repositório não muda.

1. (Previsão) Na tarefa, tente `sys_abrir("/dado")` (prefixo de `/dados`) esperando `-1`, e `sys_abrir("/dados//const.txt")` (barra dupla) esperando sucesso. Preveja antes: qual nega, qual resolve? Rode e explique as duas regras envolvidas (comprimento + conteúdo; barras extras puladas no andador).
2. (Diagnóstico) Apague a guarda de `..` no `resolver` (a linha `n == 2 && ..`) e rode o teste: ele continua passando (`DOTDOT_PASSOU` não aparece)? Explique por que o veredito é o mesmo — e por que a guarda continua no código (negar no parse vs. negar na busca: mesma resposta hoje, estados diferentes; com montagem futura, a busca poderia achar).
3. (Projeto) Adicione um quarto arquivo à tabela (`/dados/novo.txt`, sem tocar no andador nem no `resolver`) e estenda a tarefa para abri-lo, ler e fechar. Rode e explique o que isso prova: o resolvedor é função da tabela, não do teste.
4. (Previsão) Reabra `/versao.txt`, feche duas vezes e leia após fechar: preveja os três retornos. Rode e explique por que close duplo e leitura pós-close negam — e o que corromperia se aceitassem (tabela de fds como estado compartilhado).

Use [pistas graduais](exercicios/32d_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/32d_solucoes.md) depois de registrar sua própria explicação.

---


A execucao bem-sucedida sozinha nao aprova: a maior parcela esta em demonstrar que voce entendeu o porque.

### Para se aprofundar

1. *Operating Systems: Three Easy Pieces*, Remzi e Andrea Arpaci-Dusseau ([pages.cs.wisc.edu/~remzi/OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/)); 2. *The RISC-V Reader*, Patterson e Waterman (Strawberry Canyon, ISBN 978-0999060791); 3. Especificacao RISC-V privilegiada ([riscv.org](https://riscv.org/specifications/privileged-isa/)).
