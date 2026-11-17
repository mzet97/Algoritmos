import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '146'))
from Bib1 import entrada

def fibonacci(N):
    if N <= 0: return 0
    if N <= 2: return 1
    a, b = 1, 1
    for i in range(2, N):
        a, b = b, a + b
    return b

while True:
    try:
        N = int(entrada())
    except:
        break
    print(fibonacci(N))
