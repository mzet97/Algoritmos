import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '120'))
from Bib1 import entrada

def multiplos(a, b, n):
    m = 1
    l = a // mdc(a, b) * b
    return (n - 1) // l

def mdc(x, y):
    while y:
        x, y = y, x % y
    return x

while True:
    try:
        a = int(entrada())
        b = int(entrada())
        n = int(entrada())
    except:
        break
    print(multiplos(a, b, n))
