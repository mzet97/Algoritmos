#include "../bib1.h"
#include <stdlib.h>

double calc_dist(double v, double a, double t) {
    return v * t + 0.5 * a * t * t;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        double v = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double a = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double t = atof(buf);
        double r = calc_dist(v, a, t);
        print_python_float(r); printf("\n");
    }
    return 0;
}
