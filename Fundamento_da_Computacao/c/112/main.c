#include "../bib1.h"
#include <string.h>

double unidade_val(const char *u) {
    struct { const char *n; double v; } tab[] = {
        {"km",1000},{"hm",100},{"dam",10},{"m",1},{"dm",0.1},{"cm",0.01},{"mm",0.001},
        {"kg",1000},{"hg",100},{"dag",10},{"g",1},{"dg",0.1},{"cg",0.01},{"mg",0.001},
        {"kl",1000},{"hl",100},{"dal",10},{"l",1},{"dl",0.1},{"cl",0.01},{"ml",0.001},
    };
    for (int i = 0; i < (int)(sizeof(tab)/sizeof(tab[0])); i++)
        if (strcmp(u, tab[i].n) == 0) return tab[i].v;
    return 0;
}

int converte(int x, const char *u1, const char *u2) {
    return (int)(x * unidade_val(u1) / unidade_val(u2));
}

int main(void) {
    char buf[64], u1[16], u2[16];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int x = atoi(buf);
        if (!entrada(u1, sizeof(u1))) break;
        if (!entrada(u2, sizeof(u2))) break;
        printf("%d\n", converte(x, u1, u2));
    }
    return 0;
}
