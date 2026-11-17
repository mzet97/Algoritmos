import sys

def Concatenar(L1, n1, L2, n2):
    R = [0] * (n1 + n2)
    for i in range(n1):
        R[i] = L1[i]
    for i in range(n2):
        R[n1 + i] = L2[i]
    return R

def Escreve(L, n):
    sys.stdout.write("[")
    for i in range(n):
        sys.stdout.write(str(L[i]) + " ")
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
    n1_n2 = tokens[idx] + " " + tokens[idx + 1]; idx += 2
    n1 = int(n1_n2.split()[0]) if ' ' in n1_n2 else int(tokens[idx - 2]);
    n2 = int(tokens[idx - 1]) if idx >= 2 else 0
    # Simpler approach
    n1 = int(tokens[idx - 2]) if idx >= 2 else 0
    n2 = int(tokens[idx - 1])
    if n1 != -1:
        L1 = []
        for i in range(n1):
            L1.append(int(tokens[idx])); idx += 1
        L2 = []
        for i in range(n2):
            L2.append(int(tokens[idx])); idx += 1
        R = Concatenar(L1, n1, L2, n2)
        Escreve(R, n1 + n2)
    else:
        n1 = n2 = 500000
        L1 = [1] * n1
        L2 = [2] * n2
        R = Concatenar(L1, n1, L2, n2)
        Escreve(R, n1 + n2)

if __name__ == "__main__":
    main()
