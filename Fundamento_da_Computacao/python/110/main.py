import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '110'))
from Bib1 import entrada
from math import sqrt

def eh_primo(x):
    if x < 2: return False
    if x == 2: return True
    if x % 2 == 0: return False
    for i in range(3, int(sqrt(x)) + 1, 2):
        if x % i == 0: return False
    return True

while True:
    try:
        x = int(entrada())
    except:
        break
    print(eh_primo(x))
