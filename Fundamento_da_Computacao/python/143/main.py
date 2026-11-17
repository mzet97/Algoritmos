import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '143'))
from Bib1 import entrada

def saque(S):
    notas = [200, 100, 50, 20, 10, 5]
    resultado = []
    for n in notas:
        if S >= n:
            q = S // n
            resultado.append((q, n))
            S -= q * n
    if S != 0:
        return []
    return resultado

while True:
    try:
        S = int(entrada())
    except:
        break
    print(saque(S))
