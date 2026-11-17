import sys

def Busca(B, n, x):
    for i in range(n):
        lo, hi = 0, n - 1
        while lo <= hi:
            mid = (lo + hi) // 2
            if B[i][mid] == x:
                return True
            elif B[i][mid] < x:
                lo = mid + 1
            else:
                hi = mid - 1
    return False

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n > -1:
            B = []
            for i in range(n):
                row = []
                for j in range(n):
                    row.append(int(tokens[idx])); idx += 1
                B.append(row)
            x = int(tokens[idx]); idx += 1
            r = Busca(B, n, x)
            print(1 if r else 0)
        else:
            nlin = n
            n = 16000
            B = [[0] * n for _ in range(n)]
            if nlin in (-1, -2):
                for i in range(n):
                    for j in range(n):
                        B[i][j] = i
                B[n-1][n-2] = n
                B[n-1][n-1] = n + 1
                if nlin == -1:
                    x = n
                else:
                    x = n + 2
            elif nlin in (-3, -4):
                for i in range(n):
                    for j in range(n):
                        B[i][j] = j
                B[n-1][n-2] = n
                B[n-1][n-1] = n + 1
                if nlin == -3:
                    x = n
                else:
                    x = n + 2
            else:
                for i in range(n):
                    for j in range(n):
                        B[i][j] = 3
                B[0][1] = 2
                B[0][0] = 1
                if nlin == -5:
                    x = 2
                else:
                    x = 0
            r = Busca(B, n, x)
            print(1 if r else 0)

if __name__ == "__main__":
    main()
