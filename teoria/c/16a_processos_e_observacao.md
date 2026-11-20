---
layout: default
---

# Aula 16a — Processos, syscalls e observação do sistema operacional

Um processo não é o seu programa: é o programa mais tudo que o kernel mantém sobre ele. Aprender a observar processos, syscalls e estado do sistema é aprender a ler o sistema operacional como quem lê um painel de instrumentos.

Você vai ao terminar, você deverá criar um processo filho e colher seu código de saída, medir o custo de uma syscall simples, ler o mapa de memória do próprio processo em `/proc/self/maps` e explicar faltas de página menores e maiores com `getrusage`. Tudo no espaço de usuário, em Linux ou WSL.

os programas até aqui rodaram *dentro* de um processo sem examiná-lo de fora. Sistemas operacionais isolam processos, traduzem endereços e cobram cada travessia usuário→kernel. Esta aula torna esses mecanismos observáveis com ferramentas do próprio espaço de usuário. O laboratório de kernel em QEMU (etapa seguinte do módulo C4) implementa esses mecanismos; aqui você aprende a medi-los antes de implementá-los.

**Limite explícito:** nada aqui prova como o kernel implementa nada — só que chamá-lo custa, que processos são isolados e que memória é mapeada por páginas. Afirmações sobre o interior do kernel ficam para o laboratório emulado.

## 1. Processo é isolamento com contabilidade

Um **processo** é uma instância em execução com espaço de endereços próprio, descritores e estado administrado pelo SO. `fork()` duplica o processo chamador (o filho recebe uma cópia); `waitpid()` colhe o código de saída e libera o estado de zumbi; `_exit()` termina sem descarregar buffers de `stdio` (use-o no filho após `fork` quando o pai ainda vai usar os buffers).

Trecho conceitual (fragmento didático; o programa completo está na seção 3):

```c
pid_t filho = fork();
if (filho == 0) {
    _exit(7); /* filho: termina com codigo 7 */
}
int estado = 0;
waitpid(filho, &estado, 0);
assert(WIFEXITED(estado) && WEXITSTATUS(estado) == 7);
```

Esquecer `waitpid` deixa um **zumbi**: terminado, mas com estado retido até o pai colher. Zumbis não executam, mas ocupam entrada na tabela de processos — vazamento de recurso do SO, não de heap.

## 2. Syscall custa uma travessia

`getpid()` quase não faz nada — e é exatamente por isso que serve de régua: mede o custo mínimo de chamar o kernel. O exemplo desta aula cronometra um milhão de chamadas e divide pelo total.

O número absoluto varia por máquina, mitigação de Spectre/Meltdown, vDSO (o kernel atende `getpid` sem travessia real em muitos sistemas via página compartilhada — então a medição pode refletir chamada de função, não troca de modo!) e ruído de outros processos. A lição honesta: o valor medido é um *teto inferior* do custo de uma syscall real com travessia; `read`/`write` em descritor custam mais. Nunca apresente o número sem o ambiente.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/16a_processo.c) contém exatamente o programa abaixo. Requer Linux ou WSL (usa `fork`, `/proc` e `getrusage`):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L teoria/c/exemplos/16a_processo.c -o processo
./processo
```

```c
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Cronometra um milhao de getpid: régua do custo minimo de syscall. */
static double medir_getpid(int repeticoes) {
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < repeticoes; i++) {
        volatile pid_t p = getpid();
        (void)p;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ini = (double)t0.tv_sec + (double)t0.tv_nsec / 1e9;
    double fim = (double)t1.tv_sec + (double)t1.tv_nsec / 1e9;
    return (fim - ini) / repeticoes;
}

/* Conta quantas entradas do mapa sao legiveis e executaveis. */
static void resumir_mapas(int *total, int *exec) {
    FILE *f = fopen("/proc/self/maps", "r");
    assert(f != NULL);
    char linha[512];
    *total = 0;
    *exec = 0;
    while (fgets(linha, sizeof linha, f) != NULL) {
        (*total)++;
        if (strstr(linha, "r-xp") != NULL) {
            (*exec)++;
        }
    }
    fclose(f);
}

int main(void) {
    /* 1. Processo filho com codigo de saida conhecido. */
    pid_t filho = fork();
    assert(filho >= 0);
    if (filho == 0) {
        _exit(7);
    }
    int estado = 0;
    assert(waitpid(filho, &estado, 0) == filho);
    assert(WIFEXITED(estado) && WEXITSTATUS(estado) == 7);
    printf("filho colhido: codigo=%d\n", WEXITSTATUS(estado));

    /* 2. Régua de syscall. */
    const int rep = 1000000;
    printf("getpid: %.1f ns/chamada (%d repeticoes)\n",
           medir_getpid(rep) * 1e9, rep);

    /* 3. Mapa de memoria do proprio processo. */
    int total = 0, exec = 0;
    resumir_mapas(&total, &exec);
    printf("regioes mapeadas: %d, executaveis: %d\n", total, exec);
    assert(total > 0 && exec > 0);

    /* 4. Faltas de pagina: menores (reclamadas sem I/O) x maiores (com I/O). */
    struct rusage antes, depois;
    assert(getrusage(RUSAGE_SELF, &antes) == 0);
    const size_t n = 8u * 1024u * 1024u;
    unsigned char *bloco = malloc(n);
    assert(bloco != NULL);
    for (size_t i = 0; i < n; i += 4096) {
        bloco[i] = 1; /* toca uma pagina por vez: falta menor por pagina */
    }
    assert(getrusage(RUSAGE_SELF, &depois) == 0);
    printf("faltas menores: %ld -> %ld\n",
           antes.ru_minflt, depois.ru_minflt);
    printf("faltas maiores: %ld -> %ld\n",
           antes.ru_majflt, depois.ru_majflt);
    assert(depois.ru_minflt > antes.ru_minflt);
    free(bloco);

    /* 5. Entrada nao confiavel: fork de comando inexistente falha no filho. */
    pid_t f2 = fork();
    assert(f2 >= 0);
    if (f2 == 0) {
        execl("/nao/existe/de/jeito/nenhum", "x", (char *)NULL);
        _exit(127); /* execl so retorna em erro */
    }
    int e2 = 0;
    assert(waitpid(f2, &e2, 0) == f2);
    assert(WIFEXITED(e2) && WEXITSTATUS(e2) == 127);
    printf("exec falho contido: codigo=%d\n", WEXITSTATUS(e2));

    return EXIT_SUCCESS;
}
```

Saída esperada (números variam por máquina e carga):

```text
filho colhido: codigo=7
getpid: ~ns/chamada (1000000 repeticoes)
regioes mapeadas: N, executaveis: M (ambos > 0)
faltas menores: A -> B (B > A)
faltas maiores: C -> D (D costuma igualar C: tocar memoria anonima nao le disco)
exec falho contido: codigo=127
```

Leia o resultado com o modelo certo: faltas *menores* sobem porque cada página tocada pela primeira vez precisa ser materializada (sob demanda); faltas *maiores* exigem I/O de disco e normalmente não sobem aqui, porque memória anônima não vem de arquivo. Se `D > C`, houve paginação sob pressão de memória — anote a carga da máquina em vez de ignorar.

O passo 5 é segurança defensiva em miniatura: o filho tenta `exec` de um caminho inexistente, o `execl` falha, e o filho devolve `127` sem afetar o pai. Fronteira de confiança explícita: nunca execute caminho vindo de entrada externa sem validar.

### O que a memória faz aqui?

O bloco de 8 MiB é memória anônima: reservada no `malloc`, materializada página a página no primeiro toque. `/proc/self/maps` mostra as faixas virtuais com permissões `rwxp/s`; `regioes executaveis > 0` confirma código mapeado com `x`. Nada disso revela endereços físicos — ver a [aula 10a](10a_arquitetura_e_memoria_virtual.md).

## 4. Erros comuns e implicações defensivas

1. **Esquecer `waitpid`:** zumbis acumulam na tabela de processos. Colha todo filho.
2. **`exit` no filho após `fork`:** descarrega buffers duplicados e pode duplicar saída. Use `_exit` no filho.
3. **Medir syscall sem declarar o ambiente:** vDSO, mitigações e carga mudam o número. Registre máquina, kernel e carga.
4. **Confundir falta menor com erro:** falta menor é funcionamento normal (página sob demanda), não falha.
5. **`execl` com caminho externo:** valide antes; trate retorno como erro, nunca como sucesso parcial.
6. **Ler `/proc` como API estável:** o formato é do Linux e pode variar; consulte `proc_pid_maps(5)` na versão do alvo.

## 6. Exercícios obrigatórios

1. Desenhe o caminho de um `getpid` do programa até o kernel e de volta, marcando onde o modo troca e onde o vDSO pode evitar a troca.
2. Provoque um zumbi de propósito (pai dorme 5 s sem `waitpid`, filho sai na hora) e observe-o com `ps` em outro terminal. Depois corrija com `waitpid`.
3. Compare `ru_minflt` tocando 8 MiB de uma vez (`memset`) contra tocando uma página por vez. Explique a diferença no número de faltas.
4. Faça o filho do passo 5 tentar `execl` de `/bin/true` e de um caminho inexistente; documente os dois códigos de saída.

Use [pistas graduais](exercicios/16a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/16a_solucoes.md) depois de registrar sua própria explicação.

## 7. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- criar um filho, colher o código de saída e explicar o que é um zumbi;
- medir uma syscall e listar três fatores que mudam o número;
- interpretar `/proc/self/maps` sem inferir endereços físicos;
- distinguir falta menor de falta maior e dizer quando cada uma sobe;
- explicar por que o filho usa `_exit` e o que o passo 5 contém;
- executar o exemplo e confrontar cada número com o modelo, não com o chute.


**Referências primárias:** *The Linux Programming Interface* (Kerrisk), capítulos 24–25, para processos; `man fork`, `man waitpid`, `man getrusage` para os contratos.

**Próxima aula:** performance tuning. As réguas de medição desta aula (tempo monotônico, contadores, ambiente anotado) são o método do capítulo de desempenho.

## 5. Ponte com Python e JavaScript

Python: `subprocess.run` colhe códigos de saída; `resource.getrusage` expõe as mesmas faltas; `os.getpid` é a mesma régua. JavaScript (Node): `child_process` com `status`, sem acesso direto a faltas de página. Nos três, a moral é igual: processo é isolamento com contabilidade, e toda travessia ao kernel tem custo mensurável.

---

### Para se aprofundar

1. *Operating Systems: Three Easy Pieces*, Remzi e Andrea Arpaci-Dusseau (versao gratuita em [pages.cs.wisc.edu/~remzi/OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/)); 2. W. Richard Stevens, *Advanced Programming in the UNIX Environment* (Addison-Wesley, 3. ed., ISBN 978-0321637734); 3. Manual do programador Linux (`man 2 syscall`, `man 2 mmap`).
