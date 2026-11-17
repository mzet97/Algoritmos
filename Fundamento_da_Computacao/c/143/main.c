#include "../bib1.h"

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int s = atoi(buf);
        int notas[] = {200, 100, 50, 20, 10, 5};
        printf("[");
        int first = 1, restante = s;
        for (int i = 0; i < 6; i++) {
            if (restante >= notas[i]) {
                int q = restante / notas[i];
                if (!first) printf(", ");
                printf("(%d, %d)", q, notas[i]);
                restante -= q * notas[i];
                first = 0;
            }
        }
        if (restante != 0) printf("]"); /* empty */
        printf("]\n");
    }
    return 0;
}
