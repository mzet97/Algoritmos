import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '128'))
from Bib1 import entrada

def eh_palindromo(L):
    s = L.replace(' ', '')
    return s == s[::-1]

while True:
    try:
        L = entrada()
        if L == "@entradagrande1":
            L = "ab" * 500000 + "a"
        elif L == "@entradagrande2":
            L = "ab" * 500000
    except:
        break
    print(eh_palindromo(L))
