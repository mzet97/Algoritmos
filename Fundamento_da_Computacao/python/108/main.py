import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '108'))
from Bib1 import entrada

def multiplica_lista_lista(x, y):
    resultado = [0] * (len(x) + len(y))
    for i in range(len(x)):
        carry = 0
        for j in range(len(y)):
            p = resultado[i + j] + x[i] * y[j] + carry
            resultado[i + j] = p % 10
            carry = p // 10
        k = i + len(y)
        while carry > 0:
            p = resultado[k] + carry
            resultado[k] = p % 10
            carry = p // 10
            k += 1
    while len(resultado) > 1 and resultado[-1] == 0:
        resultado.pop()
    return resultado

while True:
    try:
        x = str(entrada())
        if x != "@entradagrande":
            y = str(entrada())
        else:
            x = "9"*1000
            y = "9"*1000
        x_lista = []
        y_lista = []
        for i in reversed(x):
            x_lista.append(int(i))
        for i in reversed(y):
            y_lista.append(int(i))
    except:
        break
    r = multiplica_lista_lista(x_lista, y_lista); r.reverse()
    r = ''.join([str(c) for c in r])
    print(r)
