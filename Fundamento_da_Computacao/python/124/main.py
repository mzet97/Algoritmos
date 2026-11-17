import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '124'))
from Bib1 import entrada

def mdc(x, y):
    while y:
        x, y = y, x % y
    return x

def reduz(p, q):
    g = mdc(abs(p), abs(q))
    return (p // g, q // g)

def divide(fracao1, fracao2):
    p1, q1 = fracao1
    p2, q2 = fracao2
    return reduz(p1 * q2, q1 * p2)

while True:
    try:
        p1 = int(entrada())
        q1 = int(entrada())
        p2 = int(entrada())
        q2 = int(entrada())
    except:
        break
    fracao1 = (p1, q1)
    fracao2 = (p2, q2)
    print(divide(fracao1, fracao2))
