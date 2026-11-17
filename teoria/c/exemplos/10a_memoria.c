#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int global_inicializada = 7;
static int global_zerada;

int main(void) {
    int local = 42;
    const size_t tamanho = 8u * 1024u * 1024u;
    long pagina_sistema = sysconf(_SC_PAGESIZE);
    if (pagina_sistema <= 0) {
        fputs("nao foi possivel consultar o tamanho da pagina\n", stderr);
        return EXIT_FAILURE;
    }

    size_t pagina = (size_t)pagina_sistema;
    unsigned char *bloco = malloc(tamanho);
    if (bloco == NULL) {
        fputs("falha ao alocar o bloco\n", stderr);
        return EXIT_FAILURE;
    }

    size_t paginas_tocadas = 0;
    unsigned long soma = 0;
    for (size_t deslocamento = 0; deslocamento < tamanho; deslocamento += pagina) {
        bloco[deslocamento] = (unsigned char)(paginas_tocadas % 251u);
        soma += bloco[deslocamento];
        paginas_tocadas++;
    }

    printf("PID: %ld\n", (long)getpid());
    printf("tamanho da pagina informado pelo SO: %zu bytes\n", pagina);
    printf("bloco solicitado: %zu bytes; posicoes tocadas: %zu; soma: %lu\n",
           tamanho, paginas_tocadas, soma);
    printf("global inicializada: %p\n", (void *)&global_inicializada);
    printf("global zerada:      %p\n", (void *)&global_zerada);
    printf("local automatica:   %p\n", (void *)&local);
    printf("bloco alocado:      %p\n", (void *)bloco);
    puts("Abra /proc/PID/maps em outro terminal e pressione Enter para terminar.");
    fflush(stdout);
    (void)getchar();

    free(bloco);
    return EXIT_SUCCESS;
}
