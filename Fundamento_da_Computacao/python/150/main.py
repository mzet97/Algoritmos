import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '150'))
from Bib1 import entrada

def conta_unicos(P):
    from collections import Counter
    c = Counter(P)
    return sum(1 for v in c.values() if v == 1)

while True:
    try:
        N = int(entrada())
        if N >= 0:
            P = entrada()
        else:
            P = "abcde" * 10000
    except:
        break
    print(conta_unicos(P))
