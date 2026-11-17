---
layout: default
---

# Aula 22: solucoes comentadas

Compare apos tentar os [exercicios](../22.md#21-exercicios-obrigatorios).

1 e 4. Pai e filho conversando por `pipe`, com protocolo de linha:

```c
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int fds[2];
    if (pipe(fds) != 0) {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        close(fds[0]); /* filho so escreve */
        const char *mensagens[] = {"ola\n", "mundo\n"};
        for (size_t i = 0; i < 2; i++) {
            const char *m = mensagens[i];
            size_t n = strlen(m), enviado = 0;
            while (enviado < n) {
                ssize_t e = write(fds[1], m + enviado, n - enviado);
                if (e <= 0) {
                    _exit(1);
                }
                enviado += (size_t)e;
            }
        }
        close(fds[1]);
        _exit(0);
    }
    close(fds[1]); /* pai so le; sem isto, read nunca devolve 0 */
    char buf[128];
    size_t usadas = 0;
    for (;;) {
        ssize_t lidos = read(fds[0], buf + usadas, sizeof buf - 1 - usadas);
        if (lidos < 0) {
            perror("read");
            close(fds[0]);
            return 1;
        }
        if (lidos == 0) {
            break; /* todas as pontas de escrita fechadas */
        }
        usadas += (size_t)lidos;
    }
    buf[usadas] = '\0';
    close(fds[0]);
    int status = 0;
    waitpid(pid, &status, 0);
    printf("recebido:\n%s", buf);
    printf("filho terminou ok: %s\n", WIFEXITED(status) ? "sim" : "nao");
    return 0;
}
```

O protocolo e o mesmo das aulas de rede — linhas terminadas em `\n` acumuladas ate o fim — porque pipe tambem e fluxo de bytes sem fronteiras de mensagem. Os dois fechamentos cruzados sao o contrato: se o pai mantiver a ponta de escrita aberta, o `read` bloqueia para sempre esperando um escritor que e ele mesmo.

2. Unix domain socket vence TCP local quando os dois lados estao na mesma maquina e voce quer: caminho de arquivo como endereco (com permissoes do sistema de arquivos como controle de acesso), passagem de descritores (`SCM_RIGHTS`, impossivel em TCP) e menos custo que a pilha de rede completa. TCP local vence quando o servico pode amanha mudar para outra maquina sem trocar o transporte.

3. Memoria compartilhada elimina a copia dos dados entre processos, mas entrega em troca o problema inteiro de sincronizacao: sem coordenacao, um processo le o objeto no meio da escrita do outro (leitura rasgada), e um crash no meio da atualizacao deixa o estado inconsistente para sempre — nao ha kernel mediando cada acesso como numa syscall.

5. Para coordenar: mutex compativel com processos (`pthread_mutex` com `PTHREAD_PROCESS_SHARED`), semaforos POSIX e campos de sequencia/versao para detectar leitura durante escrita. E defina a recuperacao: mutex robusto (`PTHREAD_MUTEX_ROBUST`) ou um dono que reconstrói o estado, porque "o outro processo nunca morre no meio" nao e premissa disponivel.

**Autoverificacao:** saida ordenada `ola`/`mundo`, pai termina sem travar (sinal de que fechou a ponta certa) e `waitpid` recolhe o filho sem zumbi.
