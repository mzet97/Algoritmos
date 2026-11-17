#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int QtdePrimos(int n) {
    if (n < 2) return 0;
    int count = 0;
    for (int i = 2; i <= n; i++) {
        int primo = 1;
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                primo = 0;
                break;
            }
        }
        if (primo) count++;
    }
    return count;
}

int main() {
    int n;
    while (scanf("%d", &n) > 0) {
        printf("%d\n", QtdePrimos(n));
    }
}
