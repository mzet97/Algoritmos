import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '136'))
from Bib1 import entrada

def imposto(L):
    total = 0.0
    for S in L:
        if S <= 3000:
            continue
        elif S <= 8000:
            total += (S - 3000) * 0.15
        else:
            total += 5000 * 0.15 + (S - 8000) * 0.27
    return total

while True:
    try:
        n = int(entrada())
        L = []
        for i in range(0, n):
            x = int(entrada())
            L.append(x)
    except:
        break
    print(imposto(L))
