import sys

sys.setrecursionlimit(100000)

def EhPalindromo(C, n):
    def check(i, j):
        if i >= j:
            return 1
        if C[i] != C[j]:
            return 0
        return check(i + 1, j - 1)
    return check(0, n - 1)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual: n <cadeia de n caracteres>
            C = tokens[i]; i += 1
            print(EhPalindromo(C, n))
        else:
            # teste automatico
            nlin = n
            n = 50000
            if nlin == -1:
                C = ''.join(chr(ord('a') + (j % 26)) for j in range(n))
                print(EhPalindromo(C, n))
            else:
                C_arr = [''] * n
                for j in range((n + 1) // 2):
                    ch = chr(ord('a') + (j % 26))
                    C_arr[j] = ch
                    C_arr[n - j - 1] = ch
                C = ''.join(C_arr)
                print(EhPalindromo(C, n))

if __name__ == "__main__":
    main()
