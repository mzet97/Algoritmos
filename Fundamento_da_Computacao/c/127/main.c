#include "../bib1.h"

#define BUFSZ 2000002

static char L[BUFSZ];
static char result[BUFSZ];

static int is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

void duplica(const char *L, char *out) {
    int j = 0;
    for (int i = 0; L[i] != '\0'; i++) {
        out[j++] = L[i];
        if (is_vowel(L[i]))
            out[j++] = L[i];
    }
    out[j] = '\0';
}

int main(void) {
    while (1) {
        if (!entrada(L, BUFSZ)) break;
        if (strcmp(L, "@entradagrande") == 0) {
            /* L = "a" * 500000 */
            memset(L, 'a', 500000);
            L[500000] = '\0';
        }
        duplica(L, result);
        printf("%s\n", result);
    }
    return 0;
}
