#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t capacidade = 4;
    int *valores = malloc(capacidade * sizeof *valores);
    if (valores == NULL) {
        fprintf(stderr, "falha em malloc\n");
        return 1;
    }

    for (size_t i = 0; i < capacidade; i++) {
        valores[i] = (int)(i * 10);
    }

    size_t nova_capacidade = 8;
    int *tmp = realloc(valores, nova_capacidade * sizeof *valores);
    if (tmp == NULL) {
        free(valores);
        fprintf(stderr, "falha em realloc\n");
        return 1;
    }
    valores = tmp;

    for (size_t i = capacidade; i < nova_capacidade; i++) {
        valores[i] = (int)(i * 10);
    }

    for (size_t i = 0; i < nova_capacidade; i++) {
        printf("valores[%zu] = %d\n", i, valores[i]);
    }

    free(valores);
    valores = NULL;
    return 0;
}
