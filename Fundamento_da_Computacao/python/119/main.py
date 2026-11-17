import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '119'))
from Bib1 import entrada

def mdc(x, y):
    while y:
        x, y = y, x % y
    return x

def mmc(x, y):
    return (x * y) // mdc(x, y)

while True:
    try:
        x = int(entrada())
        y = int(entrada())
    except:
        break
    print(mmc(x, y))
