import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '112'))
from Bib1 import entrada

dist = {'km': 1000, 'hm': 100, 'dam': 10, 'm': 1, 'dm': 0.1, 'cm': 0.01, 'mm': 0.001}
mass = {'kg': 1000, 'hg': 100, 'dag': 10, 'g': 1, 'dg': 0.1, 'cg': 0.01, 'mg': 0.001}
vol = {'kl': 1000, 'hl': 100, 'dal': 10, 'l': 1, 'dl': 0.1, 'cl': 0.01, 'ml': 0.001}

def converte(x, unidade1, unidade2):
    todas = {}
    todas.update(dist)
    todas.update(mass)
    todas.update(vol)
    if unidade1 not in todas or unidade2 not in todas:
        return 0
    # check same category
    cats = [dist, mass, vol]
    ok = False
    for cat in cats:
        if unidade1 in cat and unidade2 in cat:
            ok = True
            break
    if not ok:
        return 0
    return int(x * todas[unidade1] / todas[unidade2])

while True:
    try:
        x = int(entrada())
        unidade1 = entrada()
        unidade2 = entrada()
    except:
        break
    print(converte(x, unidade1, unidade2))
