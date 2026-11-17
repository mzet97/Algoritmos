#include "../bib1.h"
#include <stdlib.h>

#define BUFSZ 2000002

static char P[BUFSZ];

int conta_unicos(const char *P) {
    int count[256] = {0};
    for (int i = 0; P[i] != '\0'; i++) {
        count[(unsigned char)P[i]]++;
    }
    int unicos = 0;
    for (int i = 0; i < 256; i++) {
        if (count[i] == 1)
            unicos++;
    }
    return unicos;
}

int main(void) {
    char buf[64];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int N = atoi(buf);
        if (N >= 0) {
            if (!entrada(P, BUFSZ)) break;
        } else {
            /* P = "abcde" * 10000 */
            int pos = 0;
            const char *pat = "abcde";
            for (int i = 0; i < 10000; i++) {
                for (int j = 0; j < 5; j++) {
                    P[pos++] = pat[j];
                }
            }
            P[pos] = '\0';
        }
        printf("%d\n", conta_unicos(P));
    }
    return 0;
}
