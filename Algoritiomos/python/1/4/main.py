import sys
import math

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def QtdePrimos(n):
    if n < 2:
        return 0
    count = 0
    for i in range(2, n + 1):
        primo = True
        j = 2
        while j * j <= i:
            if i % j == 0:
                primo = False
                break
            j += 1
        if primo:
            count += 1
    return count

for line in entrada():
    n = int(line)
    print(QtdePrimos(n))
