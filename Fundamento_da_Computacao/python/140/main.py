import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '140'))
from Bib1 import entrada

def perfeito(n):
    if n < 2: return False
    soma = 1
    i = 2
    while i * i <= n:
        if n % i == 0:
            soma += i
            if i != n // i:
                soma += n // i
        i += 1
    return soma == n

while True:
    try:
        n = int(entrada())
    except:
        break
    print(perfeito(n))
