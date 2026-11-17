import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '154'))
from Bib1 import entrada

def calc_dist(v, a, t):
    if a == 0:
        return v * t
    # Time when velocity reaches zero
    t_zero = -v / a
    if t_zero <= 0 or t_zero >= t:
        # No sign change in [0, t]
        return abs(v * t + 0.5 * a * t * t)
    # Split at t_zero
    d1 = abs(v * t_zero + 0.5 * a * t_zero * t_zero)
    d2 = abs(v * t + 0.5 * a * t * t - (v * t_zero + 0.5 * a * t_zero * t_zero))
    return d1 + d2

while True:
    try:
        v = float(entrada())
        a = float(entrada())
        t = float(entrada())
    except:
        break
    print(calc_dist(v, a, t))
