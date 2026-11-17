#include "../bib1.h"

int main(void) {
    printf("[");
    int first = 1;
    for (int n = 1000; n < 10000; n++) {
        int esq = n / 100;
        int dir = n % 100;
        if ((esq + dir) * (esq + dir) == n) {
            if (!first) printf(", ");
            printf("%d", n);
            first = 0;
        }
    }
    printf("]\n");
    return 0;
}
