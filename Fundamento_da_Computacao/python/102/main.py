import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '102'))
from Bib1 import entrada

def doishorarios(ponteiros):
    h, m = ponteiros
    real_min = m * 5 if m != 12 else 0
    h24_1 = h if h != 12 else 0
    h24_2 = h + 12 if h != 12 else 12
    t1 = f"{h24_1:02d}:{real_min:02d}"
    t2 = f"{h24_2:02d}:{real_min:02d}"
    return (t1, t2)

while True:
    try:
        h = int(entrada())
        m = int(entrada())
    except:
        break
    ponteiro = (h, m)
    horarios = doishorarios(ponteiro)
    print(f"{horarios[0]} / {horarios[1]}")
