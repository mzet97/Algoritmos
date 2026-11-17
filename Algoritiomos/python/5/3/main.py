import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, op_char=None, e_val=0):
        self.op = op_char  # '+'/'-'/'*'/'/'/'@'
        self.E = e_val     # if op=='@', this is the number
        self.Esq = None
        self.Dir = None

def AvaliarExpressao(T):
    if T is None:
        return 0
    if T.op == '@':
        return T.E
    esq = AvaliarExpressao(T.Esq)
    dir_ = AvaliarExpressao(T.Dir)
    if T.op == '+':
        return esq + dir_
    elif T.op == '-':
        return esq - dir_
    elif T.op == '*':
        return esq * dir_
    elif T.op == '/':
        return esq // dir_
    return 0

def InsereExpr(e, idx):
    i = idx[0]
    if e[i] == '.':
        i += 1
        idx[0] = i
        return None
    T = No()
    if '0' <= e[i] <= '9':
        T.E = int(e[i])
        T.op = '@'
    else:
        T.op = e[i]
    i += 1
    idx[0] = i
    T.Esq = InsereExpr(e, idx)
    T.Dir = InsereExpr(e, idx)
    return T

def main():
    tokens = sys.stdin.read().split()
    for expr in tokens:
        idx = [0]
        T = InsereExpr(list(expr), idx)
        print(AvaliarExpressao(T))

if __name__ == "__main__":
    main()
