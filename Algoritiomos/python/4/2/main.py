import sys

sys.setrecursionlimit(100000)

def Impar(n):
    # assume n >= 0
    # retorna o n-esimo impar
    if n == 0:
        return 1
    return Impar(n - 1) + 2

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual
            print(Impar(n))
        else:
            # teste automatico
            n = 50000
            for _ in range(200):
                Impar(n)
            print(Impar(n))

if __name__ == "__main__":
    main()
