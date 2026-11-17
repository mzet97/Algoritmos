import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '101'))
from Bib1 import entrada

def operacao(x, y, op):
    if op == '+': return x + y
    elif op == '-': return x - y
    elif op == 'x': return x * y
    elif op == '/': return x / y

while True:
    try:
        x = int(entrada())
        y = int(entrada())
        op = entrada()
    except:
        break
    print(operacao(x, y, op))
