import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '117'))
from Bib1 import entrada

def cientifico(n):
    if n == 0: return "0.0x10^0"
    exp = 0
    val = abs(n)
    while val >= 10:
        val /= 10
        exp += 1
    while val < 1:
        val *= 10
        exp -= 1
    sinal = "-" if n < 0 else ""
    return f"{sinal}{val:.1f}x10^{exp}"

while True:
    try:
        n = float(entrada())
    except:
        break
    print(cientifico(n))
