import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '115'))
from Bib1 import entrada
import math

def raiz(a, b, c):
    delta = b**2 - 4*a*c
    if delta < 0:
        return (None, None)
    r1 = (-b + math.sqrt(delta)) / (2*a)
    if delta == 0:
        return (r1, None)
    r2 = (-b - math.sqrt(delta)) / (2*a)
    return (r1, r2)

while True:
    try:
        a = int(entrada())
        b = int(entrada())
        c = int(entrada())
    except:
        break
    print(raiz(a, b, c))
