import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '109'))
from Bib1 import entrada

def divide_lista(x, y):
    def lista_para_int(lst):
        val = 0
        for i in range(len(lst)-1, -1, -1):
            val = val * 10 + lst[i]
        return val
    def int_para_lista(val):
        if val == 0: return [0]
        lst = []
        while val > 0:
            lst.append(val % 10)
            val //= 10
        return lst
    dx = lista_para_int(x)
    dy = lista_para_int(y)
    q = dx // dy
    r = dx % dy
    return (int_para_lista(q), int_para_lista(r))

while True:
    try:
        x = str(entrada())
        y = str(entrada())
        x_lista = []
        y_lista = []
        for i in reversed(x):
            x_lista.append(int(i))
        for i in reversed(y):
            y_lista.append(int(i))
    except:
        break
    (q, r) = divide_lista(x_lista, y_lista)
    q.reverse(); r.reverse()
    q = ''.join([str(c) for c in q])
    r = ''.join([str(c) for c in r])
    print(f'({q},{r})')
