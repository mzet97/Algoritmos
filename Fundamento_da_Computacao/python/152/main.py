import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '152'))
from Bib1 import entrada

def sistema(a, b, N):
    x = [0.0] * N
    for i in range(N):
        s = b[i]
        for j in range(i):
            s -= a[i][j] * x[j]
        x[i] = s / a[i][i]
    return x

while True:
    try:
        N = int(entrada())
        if N >= 1:
            a = []
            b = []
            for i in range(0, N):
                linha = entrada().split()
                a.append([int(e) for e in linha[:len(linha)-1]])
                b.append(int(linha[-1]))
        else:
            N = 1000
            a = [[1]*i for i in range(1, N+1)]
            b = [1] * N
    except:
        break
    x = sistema(a, b, N)
    print(' '.join([str("%.1f" % e) for e in x]))
