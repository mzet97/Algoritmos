import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '133'))
from Bib1 import entrada

def troco(M, Q):
    total = sum(m * q for m, q in zip(M, Q))
    return total / 100

while True:
    try:
        n = int(entrada())
        M = []
        Q = []
        for i in range(0, n):
            x = int(entrada())
            M.append(x)
        for i in range(0, n):
            x = int(entrada())
            Q.append(x)
    except:
        break
    print(troco(M, Q))
