import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '144'))
from Bib1 import entrada

def pi(N):
    s = 0.0
    for i in range(N):
        s += ((-1)**i) / (2*i + 1)**3
    return (s * 32) ** (1/3)

while True:
    try:
        N = int(entrada())
    except:
        break
    print("%.10f" % pi(N))
