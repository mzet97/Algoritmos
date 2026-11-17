import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '145'))
from Bib1 import entrada

def serie(N):
    s = 0.0
    for i in range(1, N+1):
        s += i / (N - i + 1)
    return s

while True:
    try:
        N = int(entrada())
    except:
        break
    print(serie(N))
