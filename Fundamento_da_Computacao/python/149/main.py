import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '149'))
from Bib1 import entrada

def polinomio(N, a, k):
    resultado = 0
    for i in range(N, -1, -1):
        resultado = resultado * k + a[i]
    return resultado

while True:
    try:
        N = int(entrada())
        if N >= 0:
            a = []
            for i in range(0, N+1):
                x = int(entrada())
                a.append(x)
            k = int(entrada())
        else:
            N = 100000
            a = [1] * (N+1)
            k = 1
    except:
        break
    print(polinomio(N, a, k))
