import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '139'))
from Bib1 import entrada

def PG(a, q, n):
    resultado = []
    val = a
    for i in range(n):
        resultado.append(val)
        val *= q
    return resultado

while True:
    try:
        a = int(entrada())
        q = int(entrada())
        n = int(entrada())
    except:
        break
    print(PG(a, q, n))
