import sys

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n >= 0:
            # Manual test: n then the parenthesization string
            s = tokens[idx]; idx += 1
            E = []
            j = 0
            while j < len(s):
                desc = s[j]; j += 1
                tipo_str = ''
                while j < len(s) and s[j] != '(' and s[j] != ')':
                    tipo_str += s[j]
                    j += 1
                E.append((desc, int(tipo_str)))
            out.append('1' if verifica(E, n) else '0')
        else:
            nlin = n
            n = 1000000
            E = []
            if nlin == -1:
                for j in range(n // 2):
                    E.append(('(', 1))
                for j in range(n // 2, n):
                    E.append((')', 1))
            elif nlin == -2:
                for j in range(n // 2):
                    E.append(('(', j + 1))
                for j in range(n // 2, n):
                    E.append((')', n - j))
            elif nlin == -3:
                for j in range(0, n, 2):
                    E.append(('(', j + 1))
                    E.append((')', j + 1))
            elif nlin == -4:
                for j in range(n // 2 - 1):
                    E.append(('(', 1))
                for j in range(n // 2 - 1, n):
                    E.append((')', 1))
            elif nlin == -5:
                for j in range(n // 2 + 1):
                    E.append(('(', 1))
                for j in range(n // 2 + 1, n):
                    E.append((')', 1))
            out.append('1' if verifica(E, n) else '0')
    sys.stdout.write('\n'.join(out))

def verifica(E, n):
    stack = []
    for desc, tipo in E:
        if desc == '(':
            stack.append(tipo)
        else:  # ')'
            if not stack or stack[-1] != tipo:
                return False
            stack.pop()
    return len(stack) == 0

if __name__ == "__main__":
    main()
