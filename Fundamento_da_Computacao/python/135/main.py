import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '135'))
from Bib1 import entrada

def propriedade():
    resultados = []
    for n in range(1000, 10000):
        esq = n // 100
        dir_ = n % 100
        if (esq + dir_) ** 2 == n:
            resultados.append(n)
    return resultados

print(propriedade())
