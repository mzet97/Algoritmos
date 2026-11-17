import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '153'))
from Bib1 import entrada

def raiz(n, inf, sup):
    while sup - inf > 1:
        mid = (inf + sup) // 2
        if mid * mid <= n:
            inf = mid
        else:
            sup = mid
    return inf

while True:
    try:
        n = int(entrada())
    except:
        break
    print(raiz(n, 0, n+1))
