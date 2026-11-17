#include "../bib1.h"
#include <math.h>

void raiz(double a, double b, double c) {
    double delta = b*b - 4*a*c;
    if (delta < 0) {
        printf("None\n");
    } else {
        double r1 = (-b + sqrt(delta)) / (2*a);
        if (delta == 0) {
            printf("(");
            if (r1 == (long long)r1 && fabs(r1) < 1e15) printf("%lld", (long long)r1);
            else print_python_float(r1);
            printf(", None)\n");
        } else {
            double r2 = (-b - sqrt(delta)) / (2*a);
            printf("(");
            if (r1 == (long long)r1 && fabs(r1) < 1e15) printf("%lld", (long long)r1);
            else print_python_float(r1);
            printf(", ");
            if (r2 == (long long)r2 && fabs(r2) < 1e15) printf("%lld", (long long)r2);
            else print_python_float(r2);
            printf(")\n");
        }
    }
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        double a = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double b = atof(buf);
        if (!entrada(buf, sizeof(buf))) break;
        double c = atof(buf);
        raiz(a, b, c);
    }
    return 0;
}
