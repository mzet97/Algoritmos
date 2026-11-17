#include "../bib1.h"

int contar(const char *L, char c) {
    int count = 0;
    for (int i = 0; L[i] != '\0'; i++) {
        if (L[i] == c)
            count++;
    }
    return count;
}

int main(void) {
    char L[2048];
    char cbuf[64];
    while (1) {
        if (!entrada(L, sizeof(L))) break;
        if (!entrada(cbuf, sizeof(cbuf))) break;
        printf("%d\n", contar(L, cbuf[0]));
    }
    return 0;
}
