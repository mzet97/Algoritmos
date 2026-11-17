import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '125'))
from Bib1 import entrada

def contar(L, c):
    return L.count(c)

while True:
    try:
        L = entrada()
        c = entrada()
    except:
        break
    print(contar(L, c))
