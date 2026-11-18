#include <stdio.h>

int main(void) {
    int idade = 32;
    const int limite = 18;
    double nota = 8.5;

    printf("idade = %d\n", idade);
    printf("nota = %.1f\n", nota);

    if (idade >= limite && nota >= 7.0) {
        printf("aprovado\n");
    } else {
        printf("reprovado\n");
    }

    for (int i = 0; i < 3; i++) {
        printf("i = %d\n", i);
    }

    return 0;
}
