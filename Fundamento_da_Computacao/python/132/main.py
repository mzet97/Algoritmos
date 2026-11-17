import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '132'))
from Bib1 import entrada
from math import sqrt

def tv(D, pL, pH):
    diag_cm = D * 2.54
    ratio = pL / pH
    H = diag_cm / sqrt(1 + ratio**2)
    L = H * ratio
    return (round(L, 1), round(H, 1))

while True:
    try:
        D = int(entrada())
        pL = int(entrada())
        pH = int(entrada())
    except:
        break
    print(tv(D, pL, pH))
