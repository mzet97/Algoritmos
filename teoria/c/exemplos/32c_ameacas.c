/* Aula 32c — modelo de ameacas executavel dos projetos de C.
 *
 * Tres projetos, tres fronteiras: servidor 31 (rede hostil), distribuido 23a
 * (par que pode mentir) e kernel 32a (usuario que pode pedir qualquer coisa).
 * Este programa executa a tabela de abuso: cada linha e uma decisao que o
 * projeto precisa tomar, com o limite cobrado em codigo.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/32c_ameacas.c -o /tmp/32c
 * Executar: /tmp/32c
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINHA 1024
#define MAX_CONEXOES 64
#define MAX_CHAVE 256

/* Fronteira 1 (servidor 31): entrada da rede nunca e confiavel. */
static int aceitar_linha(const char *entrada, size_t n)
{
    if (n == 0 || n > MAX_LINHA) {
        return -1; /* vazia ou acima do teto: rejeita */
    }
    for (size_t i = 0; i < n; i++) {
        if (entrada[i] == '\0') {
            return -1; /* NUL embutido: framing, nao string C */
        }
    }
    return 0;
}

static int aceitar_conexao(int abertas)
{
    return abertas < MAX_CONEXOES ? 0 : -1;
}

/* Fronteira 2 (distribuido 23a): o par pode repetir, reordenar e atrasar. */
static int aceitar_replay(const char *id, const char **vistos, int n_vistos,
                           char *chave_out, size_t cap)
{
    if (strlen(id) == 0 || strlen(id) > MAX_CHAVE) {
        return -1;
    }
    for (int i = 0; i < n_vistos; i++) {
        if (strcmp(vistos[i], id) == 0) {
            return 1; /* repeticao: idempotente, nao reexecuta */
        }
    }
    snprintf(chave_out, cap, "%s", id);
    return 0;
}

/* Fronteira 3 (kernel 32a): syscall com argumento do usuario. */
static int validar_syscall_ptr(const void *ptr, size_t len, int usuario)
{
    if (!usuario) {
        return 0; /* kernel confia no kernel */
    }
    if (ptr == NULL || len == 0 || len > MAX_LINHA) {
        return -1; /* ponteiro nulo ou tamanho absurdo: nega */
    }
    return 0;
}

int main(void)
{
    char grande[MAX_LINHA + 10];
    memset(grande, 'A', sizeof grande);
    assert(aceitar_linha(grande, sizeof grande) == -1);
    assert(aceitar_linha("GET k\n", 6) == 0);
    assert(aceitar_linha("AB\0CD", 5) == -1);
    printf("servidor: teto de linha, NUL embutido e teto de conexao cobrados\n");
    assert(aceitar_conexao(64) == -1 && aceitar_conexao(63) == 0);

    const char *vistos[] = { "op-1", "op-2" };
    char chave[MAX_CHAVE + 1];
    assert(aceitar_replay("op-1", vistos, 2, chave, sizeof chave) == 1);
    assert(aceitar_replay("op-3", vistos, 2, chave, sizeof chave) == 0);
    assert(aceitar_replay("", vistos, 2, chave, sizeof chave) == -1);
    printf("distribuido: replay idempotente, id vazio rejeitado\n");

    assert(validar_syscall_ptr(NULL, 10, 1) == -1);
    assert(validar_syscall_ptr("x", 1 << 20, 1) == -1);
    assert(validar_syscall_ptr("x", 1, 1) == 0);
    printf("kernel: ponteiro nulo e tamanho absurdo negados no modo usuario\n");

    /* Segredos e logs: o teste que falta e o grep que passa. */
    const char *log_ok = "conexao recusada limite=1024 cliente=10.0.0.5";
    assert(strstr(log_ok, "senha") == NULL && strstr(log_ok, "token") == NULL);
    printf("logs: sem segredo, com limite e origem para auditar\n");

    printf("32c ok: ameacas nomeadas, limites executados, abusos recusados\n");
    return 0;
}
