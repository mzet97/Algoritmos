import sys

sys.setrecursionlimit(100000)

def Divisores(n, k):
    # Conta divisores de n que sao <= k
    if k == 0:
        return 0
    d = Divisores(n, k - 1)
    if n % k == 0:
        return d + 1
    return d

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        k = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual
            print(Divisores(n, k))
        else:
            # teste automatico
            n = 50000; k = 50000
            for _ in range(200):
                Divisores(n, k)
            print(Divisores(n, k))

if __name__ == "__main__":
    main()
