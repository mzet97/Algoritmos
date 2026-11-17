#include <stdio.h>
#include <stdlib.h>

int Exp(int b, int n) {
    if (n == 0) return 1;
    int half = Exp(b, n/2);
    if (n % 2 == 0) return half * half;
    return b * half * half;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int b; int n;
    while (scanf("%d %d", &b, &n) > 0) {
        if (b >= 0) {
            printf("%d\n", Exp(b, n));
        } else {
            b = 1; n = 50000;
            for (int t = 1; t <= 200; t++) Exp(b, n);
            printf("%d\n", Exp(b, n));
        }
    }
    return 0;
}
