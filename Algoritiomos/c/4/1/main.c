#include <stdio.h>
#include <stdlib.h>

int Impar(int n) {
    if (n == 0) return 1;
    return Impar(n-1) + 2;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n;
    while (scanf("%d", &n) > 0) {
        if (n >= 0) {
            printf("%d\n", Impar(n));
        } else {
            n = 50000;
            for (int t = 1; t <= 200; t++) Impar(n);
            printf("%d\n", Impar(n));
        }
    }
    return 0;
}
