import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '137'))
from Bib1 import entrada

def transposta(M, n, m):
    return [[M[i][j] for i in range(n)] for j in range(m)]

while True:
    try:
        n = int(entrada())
        if n > -1:
            m = int(entrada())
            M = []
            for i in range(0, n):
                linha = []
                for j in range(0, m):
                    x = int(entrada())
                    linha.append(x)
                M.append(linha)
        else:
            n = 1000; m = 1000
            M = [[i*n+j for j in range(m)] for i in range(n)]
    except:
        break
    print(transposta(M, n, m))
