import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '127'))
from Bib1 import entrada

vogais = set('aeiouAEIOU')

def duplica(L):
    return ''.join(c * 2 if c in vogais else c for c in L)

while True:
    try:
        L = entrada()
        if L == "@entradagrande":
            L = "a" * 500000
    except:
        break
    print(duplica(L))
