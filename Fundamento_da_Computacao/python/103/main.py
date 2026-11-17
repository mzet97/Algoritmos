import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '103'))
from Bib1 import entrada

def calc_sec(tupla1, tupla2):
    h1, m1 = tupla1
    h2, m2 = tupla2
    sec1 = h1 * 3600 + m1 * 60
    sec2 = h2 * 3600 + m2 * 60
    return abs(sec2 - sec1)

while True:
    try:
        h1 = int(entrada())
        m1 = int(entrada())
        h2 = int(entrada())
        m2 = int(entrada())
    except:
        break
    print(calc_sec((h1, m1), (h2, m2)))
