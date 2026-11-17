import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '106'))
from Bib1 import entrada

def subtrai_lista(x, y):
    resultado = []
    borrow = 0
    for i in range(len(x)):
        a = x[i]
        b = y[i] if i < len(y) else 0
        a -= borrow
        if a < b:
            a += 10
            borrow = 1
        else:
            borrow = 0
        resultado.append(a - b)
    while len(resultado) > 1 and resultado[-1] == 0:
        resultado.pop()
    return resultado

while True:
    try:
        x = str(entrada())
        if x == '@entradagrande1':
            x = '1'*1000000
            y = '9'*(1000000-1)
        elif x == '@entradagrande2':
            x = '1'*1000000
            y = '1'*1000000
        else:
            y = str(entrada())
        x_lista = []
        y_lista = []
        for i in x:
            x_lista.append(int(i))
        for i in y:
            y_lista.append(int(i))
    except:
        break
    x_lista.reverse(); y_lista.reverse()
    r = subtrai_lista(x_lista, y_lista); r.reverse()
    r = ''.join([str(c) for c in r])
    print(r)
