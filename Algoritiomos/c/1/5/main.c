#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double ContaMisteriosa(int n) {
    long long limite = 1;
    for (int i = 0; i < n; i++) limite *= 10;
    double soma = 0.0;
    for (long long k = 1; k <= limite; k++) {
        double sinal = ((k + 1) % 2 == 0) ? 1.0 : -1.0;
        soma += sinal / (2.0 * k - 1.0);
    }
    return 4.0 * soma;
}

int main() {
    int n;
    while (scanf("%d", &n) > 0) {
        printf("%.8f\n", ContaMisteriosa(n));
    }
}
