def entrada():
    import sys
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def CalculaSoma(n):
    return n * (n + 1) // 2

for line in entrada():
    n = int(line)
    print(CalculaSoma(n))
