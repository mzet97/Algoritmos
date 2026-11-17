import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '113'))
from Bib1 import entrada

def conta_num(lista):
    qp = sum(1 for x in lista if x > 0)
    qn = sum(1 for x in lista if x < 0)
    return (qp, qn)

while True:
    try:
        n = int(entrada())
        lista = []
        for i in range(0, n):
            x = int(entrada())
            lista.append(x)
    except:
        break
    print(conta_num(lista))
