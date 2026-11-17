import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '116'))
from Bib1 import entrada

def raiz(n):
    if n == 0: return 0
    i = 1
    while i * i <= n:
        i += 1
    return i - 1

while True:
    try:
        n = int(entrada())
    except:
        break
    print(raiz(n))
