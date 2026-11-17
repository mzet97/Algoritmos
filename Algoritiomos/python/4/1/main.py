import sys

sys.setrecursionlimit(100000)

def Exp(b, n):
    # assume b > 0, n >= 0
    # retorna b^n
    if n == 0:
        return 1
    return b * Exp(b, n - 1)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        b = int(tokens[i]); i += 1
        n = int(tokens[i]); i += 1
        if b >= 0:
            # teste manual
            print(Exp(b, n))
        else:
            # teste automatico
            b = 1; n = 50000
            for _ in range(200):
                Exp(b, n)
            print(Exp(b, n))

if __name__ == "__main__":
    main()
