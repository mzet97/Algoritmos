#include "../bib1.h"
#include <stdlib.h>
#include <math.h>

double calc_dist(double v, double a, double t) {
    /*
     * Total distance (path length) with general acceleration.
     * v >= 0. If a >= 0, object never stops: d = v*t + 0.5*a*t^2
     * If a < 0, object may stop at t_stop = -v/a.
     * If t <= t_stop: d = v*t + 0.5*a*t^2
     * If t > t_stop: distance up to stop + distance of backward travel
     *   d_stop = v*t_stop + 0.5*a*t_stop^2
     *   After t_stop, velocity = a*(t - t_stop), distance backward = 0.5*|a|*(t-t_stop)^2
     *   total = d_stop + 0.5*|a|*(t - t_stop)^2
     */
    if (a < 0) {
        double t_stop = -v / a;
        if (t > t_stop) {
            double d_stop = v * t_stop + 0.5 * a * t_stop * t_stop;
            double dt = t - t_stop;
            double d_back = 0.5 * fabs(a) * dt * dt;
            return d_stop + d_back;
        }
    }
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
