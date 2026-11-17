#include "../bib1.h"
#include <stdlib.h>

#define BUFSZ 2000002

static char L[BUFSZ];
static char result[BUFSZ];

void remove_char(const char *L, char c, char *out) {
    int j = 0;
    for (int i = 0; L[i] != '\0'; i++) {
        if (L[i] != c)
            out[j++] = L[i];
    }
    out[j] = '\0';
}

int main(void) {
    char cbuf[64];
    while (1) {
        if (!entrada(L, BUFSZ)) break;
        if (strcmp(L, "@entradagrande") == 0) {
            /* L = "ca" * 500000 */
            int pos = 0;
            for (int i = 0; i < 500000; i++) {
                L[pos++] = 'c';
                L[pos++] = 'a';
            }
            L[pos] = '\0';
        }
        if (!entrada(cbuf, sizeof(cbuf))) break;
        remove_char(L, cbuf[0], result);
        printf("%s\n", result);
    }
    return 0;
}
