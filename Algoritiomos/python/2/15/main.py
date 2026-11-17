import sys

def Busca(B, n, x):
    i = 0
    j = n - 1
    while i < n and j >= 0:
        if B[i][j] == x:
            return True
        elif B[i][j] < x:
            i += 1
        else:
            j -= 1
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
                x = n if nlin == -1 else n + 2
            elif nlin in (-3, -4):
                for i in range(n):
                    for j in range(n):
                        B[i][j] = j
                B[n-1][n-2] = n
                B[n-1][n-1] = n + 1
                x = n if nlin == -3 else n + 2
            else:
                for i in range(n):
                    for j in range(n):
                        B[i][j] = 3
                B[0][1] = 2
                B[0][0] = 1
                x = 2 if nlin == -5 else 0
            r = Busca(B, n, x)
            print(1 if r else 0)

if __name__ == "__main__":
    main()
