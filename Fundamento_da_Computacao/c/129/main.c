#include "../bib1.h"

#define BUFSZ 2000002

static char L[BUFSZ];
static char result[BUFSZ];

void reverte(const char *L, char *out) {
    int len = strlen(L);
    for (int i = 0; i < len; i++) {
        out[i] = L[len - 1 - i];
    }
    out[len] = '\0';
}

int main(void) {
    while (1) {
        if (!entrada(L, BUFSZ)) break;
        if (strcmp(L, "@entradagrande") == 0) {
            /* L = "ab" * 500000 */
            int pos = 0;
            for (int i = 0; i < 500000; i++) {
                L[pos++] = 'a';
                L[pos++] = 'b';
            }
            L[pos] = '\0';
        }
        reverte(L, result);
        printf("%s\n", result);
    }
    return 0;
}
