#ifndef BIB1_H
#define BIB1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Lê uma linha de stdin ignorando linhas vazias.
// Retorna 1 em sucesso, 0 em EOF.
static int entrada(char *buf, int size) {
    while (1) {
        if (!fgets(buf, size, stdin))
            return 0;
        int len = strlen(buf);
        while (len > 0 && isspace((unsigned char)buf[len - 1]))
            buf[--len] = '\0';
        if (len > 0)
            return 1;
    }
}

// Imprime double no formato do Python (shortest round-trip representation).
// Python usa notação fixa para |x| < 1e16 e >= 1e-4 (ou x==0).
static void print_python_float(double x) {
    if (isinf(x)) { printf("%sinf", x < 0 ? "-" : ""); return; }
    if (isnan(x)) { printf("nan"); return; }
    char buf[64];
    double ax = fabs(x);
    if (ax == 0.0 || (ax >= 1e-4 && ax < 1e16)) {
        // Notação fixa: encontra o mínimo de casas decimais que faz round-trip
        for (int d = 0; d <= 20; d++) {
            snprintf(buf, sizeof(buf), "%.*f", d, x);
            if (strtod(buf, NULL) == x) {
                if (d == 0) { printf("%s.0", buf); return; }
                // Remove trailing zeros, mantendo pelo menos x.0
                int len = strlen(buf);
                while (len > 1 && buf[len-1] == '0' && buf[len-2] != '.')
                    len--;
                buf[len] = '\0';
                printf("%s", buf);
                return;
            }
        }
    }
    // Notação científica ou fallback
    for (int prec = 1; prec <= 17; prec++) {
        snprintf(buf, sizeof(buf), "%.*g", prec, x);
        if (strtod(buf, NULL) == x) { printf("%s", buf); return; }
    }
    printf("%.17g", x);
}

#endif
