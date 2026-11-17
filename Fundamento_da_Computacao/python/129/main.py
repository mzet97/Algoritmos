import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '129'))
from Bib1 import entrada

def reverte(L):
    return L[::-1]

while True:
    try:
        L = entrada()
        if L == "@entradagrande":
            L = "ab" * 500000
    except:
        break
    print(reverte(L))
