import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '148'))
from Bib1 import entrada

def fatorial(N):
    r = 1
    for i in range(2, N+1):
        r *= i
    return r

while True:
    try:
        N = int(entrada())
    except:
        break
    print(fatorial(N))
