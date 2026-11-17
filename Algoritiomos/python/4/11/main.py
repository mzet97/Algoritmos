import sys

sys.setrecursionlimit(100000)

def Maximo(A, n):
    if n == 1:
        return A[0]
    m = Maximo(A, n - 1)
    if A[n - 1] > m:
        return A[n - 1]
    return m

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual
            A = []
            for j in range(n):
                A.append(int(tokens[i])); i += 1
            print(Maximo(A, n))
        else:
            # teste automatico
            n = 50000
            A = [0] * n
            for j in range(n // 2):
                A[j] = j
            for j in range(n // 2 + 1, n):
                A[j] = n - j
            for _ in range(20):
                Maximo(A, n)
            print(Maximo(A, n))

if __name__ == "__main__":
    main()
