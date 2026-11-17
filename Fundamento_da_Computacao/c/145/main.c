#include "../bib1.h"
#include <stdlib.h>

double serie(int N) {
    double s = 0.0;
    for (int i = 1; i <= N; i++) {
        s += (double)i / (double)(N - i + 1);
    }
    return s;
}

int main(void) {
    char buf[256];
    while (entrada(buf, sizeof(buf))) {
        int N = atoi(buf);
        double r = serie(N);
        print_python_float(r); printf("\n");
    }
    return 0;
}
