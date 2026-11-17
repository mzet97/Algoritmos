import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '121'))
from Bib1 import entrada

def reduz(fracao):
    p, q = fracao
    g = mdc(abs(p), abs(q))
    return (p // g, q // g)

def mdc(x, y):
    while y:
        x, y = y, x % y
    return x

while True:
    try:
        p = int(entrada())
        q = int(entrada())
    except:
        break
    fracao = (p, q)
    print(reduz(fracao))
