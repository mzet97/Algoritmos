#include "../bib1.h"
#include <stdlib.h>
#include <string.h>

unsigned long long lista_para_int(const int *lst, int len) {
    unsigned long long val = 0;
    for (int i = len-1; i >= 0; i--)
        val = val * 10 + lst[i];
    return val;
}

void int_para_lista(unsigned long long val, int **out, int *outlen) {
    if (val == 0) { int *r = malloc(sizeof(int)); r[0] = 0; *out = r; *outlen = 1; return; }
    int *r = malloc(25 * sizeof(int));
    int ri = 0;
    while (val > 0) { r[ri++] = val % 10; val /= 10; }
    *out = r;
    *outlen = ri;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        char *xstr = strdup(buf);
        if (!entrada(buf, sizeof(buf))) { free(xstr); break; }
        char *ystr = strdup(buf);
        int xlen = strlen(xstr), ylen = strlen(ystr);
        int *xl = malloc(xlen * sizeof(int)), *yl = malloc(ylen * sizeof(int));
        for (int i = 0; i < xlen; i++) xl[xlen-1-i] = xstr[i] - '0';
        for (int i = 0; i < ylen; i++) yl[ylen-1-i] = ystr[i] - '0';
        unsigned long long dx = lista_para_int(xl, xlen);
        unsigned long long dy = lista_para_int(yl, ylen);
        unsigned long long q = dx / dy, rem = dx % dy;
        int *ql, *rl, qlen, rlen;
        int_para_lista(q, &ql, &qlen);
        int_para_lista(rem, &rl, &rlen);
        printf("(");
        for (int i = qlen-1; i >= 0; i--) putchar('0' + ql[i]);
        printf(",");
        for (int i = rlen-1; i >= 0; i--) putchar('0' + rl[i]);
        printf(")\n");
        free(xl); free(yl); free(ql); free(rl); free(xstr); free(ystr);
    }
    return 0;
}
