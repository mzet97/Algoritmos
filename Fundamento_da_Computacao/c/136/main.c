#include "../bib1.h"
#include <stdlib.h>

double imposto(const int *l, int n) {
    double total = 0.0;
    for (int i = 0; i < n; i++) {
        int s = l[i];
        if (s <= 3000) continue;
        else if (s <= 8000) total += (s - 3000) * 0.15;
        else total += 5000 * 0.15 + (s - 8000) * 0.27;
    }
    return total;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        int *l = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            if (!entrada(buf, sizeof(buf))) { free(l); return 0; }
            l[i] = atoi(buf);
        }
        print_python_float(imposto(l, n)); printf("\n");
        free(l);
    }
    return 0;
}
