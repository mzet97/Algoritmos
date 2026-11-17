import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '114'))
from Bib1 import entrada

def raiz(a, b):
    if a == 0: return None
    return -b / a

while True:
    try:
        a = int(entrada())
        b = int(entrada())
    except:
        break
    print(raiz(a, b))
