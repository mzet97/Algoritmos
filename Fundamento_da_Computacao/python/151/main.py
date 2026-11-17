import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '151'))
from Bib1 import entrada

def multi_matriz(r, s, t, M1, M2):
    MR = [[0]*t for _ in range(r)]
    for i in range(r):
        for j in range(t):
            for k in range(s):
                MR[i][j] += M1[i][k] * M2[k][j]
    return MR

while True:
    try:
        r = int(entrada())
        s = int(entrada())
        t = int(entrada())
        M1 = []
        M2 = []
        for i in range(0, r):
            linha = entrada().split()
            linha = [int(x) for x in linha]
            M1.append(linha)
        for i in range(0, s):
            linha = entrada().split()
            linha = [int(x) for x in linha]
            M2.append(linha)
    except:
        break
    MR = multi_matriz(r, s, t, M1, M2)
    for i in range(0, r):
        if i > 0:
            print(' | ', end='')
        for j in range(0, t):
            if j > 0:
                print(' ', end='')
            print(MR[i][j], end='')
    print()
