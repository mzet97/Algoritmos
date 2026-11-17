#include "../bib1.h"
#include <stdlib.h>
#include <math.h>

double menor_angulo(int H, int M) {
    double hour_angle = (H % 12) * 30.0 + M * 0.5;
    double min_angle = M * 6.0;
    double diff = fabs(hour_angle - min_angle);
    if (diff > 180.0) diff = 360.0 - diff;
    return diff;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int H = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int M = atoi(buf);
        double r = menor_angulo(H, M);
        print_python_float(r); printf("\n");
    }
    return 0;
}
