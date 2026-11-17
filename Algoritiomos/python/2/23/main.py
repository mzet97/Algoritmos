import sys

class NoEnc:
    def __init__(self, Chave, E):
        self.Chave = Chave
        self.E = E
        self.Prox = None

class NoSeq:
    def __init__(self, Chave, E):
        self.Chave = Chave
        self.E = E

def ConverterSeqParaEnc(L, N):
    head = None; tail = None
    for i in range(N):
                novo = NoEnc(L[i].Chave, L[i].E)
                if head is None:
                    head = novo
                    tail = novo
                else:
                    tail.Prox = novo
                    tail = novo
    return head

def ConverterEncParaSeq(L):
    result = []
    p = L
    while p is not None:
        result.append(NoSeq(p.Chave, p.E))
        p = p.Prox
    return result

def EscreveEnc(L):
    sys.stdout.write("[")
    while L is not None:
        sys.stdout.write("c=%d;e=%d " % (L.Chave, L.E))
        L = L.Prox
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        L = None; u = None
        if n != -1:
            for i in range(n):
                c = int(tokens[idx]); idx += 1
                e = int(tokens[idx]); idx += 1
                novo = NoEnc(c, e)
                if L is None:
                    L = novo
                    u = novo
                else:
                    u.Prox = novo
                    u = novo
            Lconv1 = ConverterEncParaSeq(L)
            N = len(Lconv1)
            # Build NoSeq array
            seq_arr = Lconv1
            Lconv2 = ConverterSeqParaEnc(seq_arr, N)
            EscreveEnc(Lconv2)
        else:
            n = 500000
            for i in range(n):
                c = i; e = n + i
                novo = NoEnc(c, e)
                if L is None:
                    L = novo
                    u = novo
                else:
                    u.Prox = novo
                    u = novo
            Lconv1 = ConverterEncParaSeq(L)
            N = len(Lconv1)
            seq_arr = Lconv1
            Lconv2 = ConverterSeqParaEnc(seq_arr, N)
            EscreveEnc(Lconv2)

if __name__ == "__main__":
    main()
