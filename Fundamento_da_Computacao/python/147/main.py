import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'exercicios', '147'))
from Bib1 import entrada

def escrever_seq_fibonacci(N):
    fibs = []
    a, b = 1, 1
    for i in range(N):
        fibs.append(a)
        a, b = b, a + b
    # Ascending part: 1 to N-1
    for k in range(1, N):
        line = ' '.join(str(fibs[i]) for i in range(k))
        print(line)
    # Full line
    print(' '.join(str(f) for f in fibs))
    # Descending part: N-1 to 1
    for k in range(N-1, 0, -1):
        line = ' '.join(str(fibs[i]) for i in range(k))
        print(line)

while True:
    try:
        N = int(entrada())
    except:
        break
    escrever_seq_fibonacci(N)
