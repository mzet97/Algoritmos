#include "../bib1.h"
#include <math.h>

char ponto_reta(double a, double b, double x, double y) {
    double y_reta = a * x + b;
    double diff = y - y_reta;
    if (fabs(diff) <= 0.00001) return '=';
    if (diff > 0) return '>';
    return '<';
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        double a = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double b = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double x = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double y = atof(buf);
        printf("%c\n", ponto_reta(a, b, x, y));
    }
    return 0;
}
