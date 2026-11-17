import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '104'))
from Bib1 import entrada

def contagem(x, y):
    s = str(abs(x))
    return s.count(str(y))

while True:
    try:
        x = int(entrada())
        y = int(entrada())
    except:
        break
    print(contagem(x, y))
