#include "../bib1.h"
#include <stdlib.h>
#include <math.h>

double pi_approx(int N) {
    double S = 0.0;
    for (int i = 0; i < N; i++) {
        double denom = (2 * i + 1);
        double term = 1.0 / (denom * denom * denom);
        if (i % 2 == 0)
            S += term;
        else
            S -= term;
    }
    return cbrt(32.0 * S);
}

int main(void) {
    char buf[256];
    while (entrada(buf, sizeof(buf))) {
        int N = atoi(buf);
        printf("%.10f\n", pi_approx(N));
    }
    return 0;
}
