import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, op_char=None, e_val=0):
        self.op = op_char
        self.E = e_val
        self.Esq = None
        self.Dir = None

def ExpressaoParaArvore(C, ini, fim):
    # C is a string, parse parenthesized expression
    # Format: (op left right) or number
    # Actually looking at the C++ more carefully:
    # The expression is parenthesized like "(+12)" meaning + with 1 and 2
    # Let's parse: if C[ini] == '(' then operator, else digit
    i = ini
    if C[i] == '(':
        # operator
        i += 1  # skip '('
        op = C[i]
        i += 1  # skip operator
        T = No(op)
        T.Esq, i = _parse(C, i)
        T.Dir, i = _parse(C, i)
        i += 1  # skip ')'
        return T
    else:
        # digit
        T = No('@', int(C[i]))
        return T

def _parse(C, i):
    if C[i] == '(':
        i += 1
        op = C[i]
        i += 1
        T = No(op)
        T.Esq, i = _parse(C, i)
        T.Dir, i = _parse(C, i)
        i += 1  # skip ')'
        return T, i
    else:
        T = No('@', int(C[i]))
        return T, i + 1

def Escrever(T):
    if T is not None:
        Escrever(T.Esq)
        Escrever(T.Dir)
        if T.op != '@':
            sys.stdout.write(T.op + " ")
        else:
            sys.stdout.write(str(T.E) + " ")

def main():
    tokens = sys.stdin.read().split()
    for expr in tokens:
        T = ExpressaoParaArvore(expr, 0, len(expr) - 1)
        Escrever(T)
        print()

if __name__ == "__main__":
    main()
