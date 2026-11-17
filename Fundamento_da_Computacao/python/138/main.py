import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '138'))
from Bib1 import entrada

def PA(a, r, n):
    return [a + i * r for i in range(n)]

while True:
    try:
        a = int(entrada())
        r = int(entrada())
        n = int(entrada())
    except:
        break
    print(PA(a, r, n))
