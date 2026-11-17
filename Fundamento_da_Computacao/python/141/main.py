import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '141'))
from Bib1 import entrada

def notas(M, N):
    max_n = max(N)
    min_n = min(N)
    maiores = [m for m, n in zip(M, N) if n == max_n]
    menores = [m for m, n in zip(M, N) if n == min_n]
    return (maiores, menores)

while True:
    try:
        n = int(entrada())
        if n >= 0:
            M = []
            N = []
            for i in range(0, n):
                x = int(entrada())
                M.append(x)
            for i in range(0, n):
                x = float(entrada())
                N.append(x)
        else:
            M = [i for i in range(1000000)]
            N = [2 for i in range(1000000-2)] + [1, 3]
    except:
        break
    print(notas(M, N))
