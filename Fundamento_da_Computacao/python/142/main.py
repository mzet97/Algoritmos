import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '142'))
from Bib1 import entrada

def ponto_reta(a, b, ponto):
    x, y = ponto
    diff = y - (a * x + b)
    if abs(diff) <= 0.00001:
        return '='
    elif diff > 0:
        return '>'
    else:
        return '<'

while True:
    try:
        a = float(entrada())
        b = float(entrada())
        x = float(entrada())
        y = float(entrada())
    except:
        break
    ponto = (x, y)
    print(ponto_reta(a, b, ponto))
