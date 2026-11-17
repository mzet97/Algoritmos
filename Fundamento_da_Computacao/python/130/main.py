import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '130'))
from Bib1 import entrada

def sub_string(L, S):
    return S in L

while True:
    try:
        L = str(entrada())
        if L == "@entradagrande":
            L = "ab" * 2000
            S = "ab" * 1000 + "c"
        else:
            S = str(entrada())
    except:
        break
    print(sub_string(L, S))
