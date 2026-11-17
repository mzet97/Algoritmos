#include <stdio.h>
#include <stdlib.h>

int EhPalindromo(char C[], int n) {
    if (n <= 1) return 1;
    if (C[0] != C[n-1]) return 0;
    return EhPalindromo(C+1, n-2);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; char * C;
    while (scanf("%d", &n) > 0) {
        if (n >= 0) {
            C = (char *) malloc(sizeof(char) * (n+1));
            scanf("%s", C);
            printf("%d\n", EhPalindromo(C, n));
            free(C);
        } else {
            int nlin = n;
            n = 50000;
            C = (char *) malloc(sizeof(char) * (n+1));
            if (nlin == -1) {
                for (int i = 0; i < n; i++) {
                    C[i] = 'a' + (i%26);
                }
                printf("%d\n", EhPalindromo(C, n));
            } else {
                for (int i = 0; i < (n+1)/2; i++) {
                    C[i] = 'a' + (i%26); C[n-i-1] = C[i];
                }
                printf("%d\n", EhPalindromo(C, n));
            }
            free(C);
        }
    }
    return 0;
}
