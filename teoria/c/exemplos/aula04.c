#include <stdio.h>
#include <string.h>

static void imprime_array(const int *valores, size_t quantidade) {
    for (size_t i = 0; i < quantidade; i++) {
        printf("valores[%zu] = %d\n", i, valores[i]);
    }
}

int main(void) {
    int valores[] = {10, 20, 30, 40};
    size_t quantidade = sizeof valores / sizeof valores[0];
    int *p = valores;

    printf("valores[0] = %d\n", valores[0]);
    printf("*(p + 2) = %d\n", *(p + 2));

    p[1] = 99;
    imprime_array(valores, quantidade);

    char nome[] = "Ana";
    printf("nome = %s\n", nome);
    printf("strlen(nome) = %zu\n", strlen(nome));

    return 0;
}
