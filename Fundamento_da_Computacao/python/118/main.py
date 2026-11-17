import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '118'))
from Bib1 import entrada

def mdc(x, y):
    while y:
        x, y = y, x % y
    return x

while True:
    try:
        x = int(entrada())
        y = int(entrada())
    except:
        break
    print(mdc(x, y))
