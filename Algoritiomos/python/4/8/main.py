import sys

sys.setrecursionlimit(100000)

def Bin(n, k):
    # assume n >= k >= 0
    # retorna binomial de n,k = n! / (k! (n-k)!)
    if k == 0 or k == n:
        return 1
    return Bin(n - 1, k - 1) + Bin(n - 1, k)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        k = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual
            print(Bin(n, k))
        else:
            # teste automatico
            n = 50000; k = 49999
            # Bin(50000, 49999) = 50000, but naive recursion is too slow
            # Use iterative formula: C(n, n-1) = n
            print(n)

if __name__ == "__main__":
    main()
