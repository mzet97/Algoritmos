import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '107'))
from Bib1 import entrada

def multiplica_lista(x, y):
    resultado = []
    carry = 0
    for d in x:
        p = d * y + carry
        resultado.append(p % 10)
        carry = p // 10
    while carry > 0:
        resultado.append(carry % 10)
        carry //= 10
    if not resultado:
        resultado = [0]
    return resultado

while True:
    try:
        x = entrada()
        y = int(entrada())
        x_lista = []
        for i in reversed(x):
            x_lista.append(int(i))
    except:
        break
    r = multiplica_lista(x_lista, y); r.reverse()
    r = ''.join([str(c) for c in r])
    print(r)
