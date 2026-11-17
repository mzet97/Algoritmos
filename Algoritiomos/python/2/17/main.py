import sys

def Inverter(L, n):
    for i in range(n // 2):
        L[i], L[n - 1 - i] = L[n - 1 - i], L[i]

    sys.stdout.write("[")
    for i in range(n):
        sys.stdout.write(str(L[i]) + " ")
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
    n = int(tokens[idx]); idx += 1
    if n != -1:
        L = []
        for i in range(n):
        L.append(int(tokens[idx])); idx += 1
        Inverter(L, n)
    else:
        n = 1000000
        L = [0] * n
        for i in range(n):
            L[i] = str(i % 10)
        Inverter(L, n)

if __name__ == "__main__":
    main()
