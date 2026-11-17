import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '155'))
from Bib1 import entrada

def maiorigual_lista(x, y):
    if len(x) > len(y): return True
    if len(x) < len(y): return False
    for i in range(len(x)-1, -1, -1):
        if x[i] > y[i]: return True
        if x[i] < y[i]: return False
    return True

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
    if maiorigual_lista(x_lista, y_lista):
        print('1')
    else:
        print('0')
