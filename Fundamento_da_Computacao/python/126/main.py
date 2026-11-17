import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '126'))
from Bib1 import entrada

def remove(L, c):
    return L.replace(c, '')

while True:
    try:
        L = entrada()
        if L == "@entradagrande":
            L = "ca" * 500000
        c = entrada()
    except:
        break
    print(remove(L, c))
