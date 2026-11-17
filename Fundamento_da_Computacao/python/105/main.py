import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '105'))
from Bib1 import entrada

def soma_lista(x, y):
    resultado = []
    carry = 0
    for i in range(max(len(x), len(y))):
        a = x[i] if i < len(x) else 0
        b = y[i] if i < len(y) else 0
        s = a + b + carry
        resultado.append(s % 10)
        carry = s // 10
    if carry:
        resultado.append(carry)
    return resultado

while True:
    try:
        x = str(entrada())
        if x != '@entradagrande':
            y = str(entrada())
        else:
            x = '9'*1000000
            y = '9'*1000000
        x_lista = []
        y_lista = []
        for i in x:
            x_lista.append(int(i))
        for i in y:
            y_lista.append(int(i))
    except:
        break
    x_lista.reverse(); y_lista.reverse()
    r = soma_lista(x_lista, y_lista); r.reverse()
    r = ''.join([str(c) for c in r])
    print(r)
