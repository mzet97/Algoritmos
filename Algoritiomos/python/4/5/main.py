import sys

sys.setrecursionlimit(100000)

def RaizQuad(n):
    # assume n >= 0
    # retorna r tal que r^2 <= n < (r+1)^2
    if n == 0:
        return 0
    r = RaizQuad(n - 1)
    if (r + 1) * (r + 1) <= n:
        return r + 1
    return r

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual
            print(RaizQuad(n))
        else:
            # teste automatico
            n = 50000
            for _ in range(200):
                RaizQuad(n)
            print(RaizQuad(n))

if __name__ == "__main__":
    main()
