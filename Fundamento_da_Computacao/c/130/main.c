#include "../bib1.h"

#define BUFSZ 2000002

static char L[BUFSZ];
static char S[BUFSZ];

int sub_string(const char *L, const char *S) {
    int lenL = strlen(L);
    int lenS = strlen(S);
    if (lenS > lenL) return 0;
    for (int i = 0; i <= lenL - lenS; i++) {
        int match = 1;
        for (int j = 0; j < lenS; j++) {
            if (L[i + j] != S[j]) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

int main(void) {
    while (1) {
        if (!entrada(L, BUFSZ)) break;
        if (strcmp(L, "@entradagrande") == 0) {
            /* L = "ab" * 2000 */
            int pos = 0;
            for (int i = 0; i < 2000; i++) {
                L[pos++] = 'a';
                L[pos++] = 'b';
            }
            L[pos] = '\0';
            /* S = "ab" * 1000 + "c" */
            pos = 0;
            for (int i = 0; i < 1000; i++) {
                S[pos++] = 'a';
                S[pos++] = 'b';
            }
            S[pos++] = 'c';
            S[pos] = '\0';
        } else {
            if (!entrada(S, BUFSZ)) break;
        }
        printf("%s\n", sub_string(L, S) ? "True" : "False");
    }
    return 0;
}
