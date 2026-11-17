import sys

sys.setrecursionlimit(100000)

def RaizQuad(n):
    # assume n >= 0
    # retorna floor(sqrt(n)) usando busca binaria recursiva
    def busca(lo, hi):
        if lo > hi:
            return hi
        mid = (lo + hi) // 2
        if mid * mid == n:
            return mid
        elif mid * mid < n:
            return busca(mid + 1, hi)
        else:
            return busca(lo, mid - 1)
    if n == 0:
        return 0
    return busca(1, n)

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
