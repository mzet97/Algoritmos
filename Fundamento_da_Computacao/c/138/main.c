#include "../bib1.h"

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int a = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int r = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        printf("[");
        for (int i = 0; i < n; i++) {
            if (i > 0) printf(", ");
            printf("%d", a + i * r);
        }
        printf("]\n");
    }
    return 0;
}
