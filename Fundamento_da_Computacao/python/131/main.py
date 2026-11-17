import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '131'))
from Bib1 import entrada

def acima_media(L):
    if not L: return 0
    media = sum(L) / len(L)
    return sum(1 for x in L if x > media)

while True:
    try:
        n = entrada()
        if n == "@entradagrande":
            n = 1000000
            L = []
            for i in range(n):
                L.append(i)
        else:
            n = int(n)
            L = []
            for i in range(0, n):
                x = int(entrada())
                L.append(x)
    except:
        break
    print(acima_media(L))
