#include "../bib1.h"

#define BUFSZ 2000002

static char L[BUFSZ];

int eh_palindromo(const char *L) {
    int len = strlen(L);
    /* Build a version without spaces */
    /* Use two-pointer approach, skipping spaces */
    int left = 0;
    int right = len - 1;
    while (left < right) {
        while (left < right && L[left] == ' ')
            left++;
        while (left < right && L[right] == ' ')
            right--;
        if (left < right) {
            if (L[left] != L[right])
                return 0;
            left++;
            right--;
        }
    }
    return 1;
}

int main(void) {
    while (1) {
        if (!entrada(L, BUFSZ)) break;
        if (strcmp(L, "@entradagrande1") == 0) {
            /* L = "ab" * 500000 + "a" */
            int pos = 0;
            for (int i = 0; i < 500000; i++) {
                L[pos++] = 'a';
                L[pos++] = 'b';
            }
            L[pos++] = 'a';
            L[pos] = '\0';
        } else if (strcmp(L, "@entradagrande2") == 0) {
            /* L = "ab" * 500000 */
            int pos = 0;
            for (int i = 0; i < 500000; i++) {
                L[pos++] = 'a';
                L[pos++] = 'b';
            }
            L[pos] = '\0';
        }
        printf("%s\n", eh_palindromo(L) ? "True" : "False");
    }
    return 0;
}
