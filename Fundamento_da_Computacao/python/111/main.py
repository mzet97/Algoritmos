import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '111'))
from Bib1 import entrada

def calc_dist(v, a, t):
    return v * t + 0.5 * a * t * t

while True:
    try:
        v = float(entrada())
        a = float(entrada())
        t = float(entrada())
    except:
        break
    print(calc_dist(v, a, t))
