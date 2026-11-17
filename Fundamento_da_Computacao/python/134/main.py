import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '134'))
from Bib1 import entrada

def menor_angulo(H, M):
    ang_h = (H % 12) * 30 + M * 0.5
    ang_m = M * 6
    diff = abs(ang_h - ang_m)
    return min(diff, 360 - diff)

while True:
    try:
        H = int(entrada())
        M = int(entrada())
    except:
        break
    print(menor_angulo(H, M))
