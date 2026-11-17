import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

class E1:
    def __init__(self, nome, idade):
        self.Nome = nome
        self.Idade = idade

class E2:
    def __init__(self, turma, aluno, prox=None):
        self.Turma = turma
        self.Aluno = aluno
        self.Prox = prox

V = [
    E1(list("JOAO"), 5),
    E1(list("LEO"), 8),
    E1(list("LIA"), 9),
]

n4 = E2(2, V[2], None)
n3 = E2(2, V[1], n4)
n2 = E2(1, V[2], n3)
n1 = E2(1, V[0], n2)
L = n1

def TestarMemoria(t):
    if t == 1:
        print(f"{V[0].Idade} {V[1].Idade} {V[2].Idade}")
    if t == 2:
        print(f"{V[0].Nome[0]} {V[0].Nome[1]} {V[0].Nome[2]} {V[0].Nome[3]}")
    if t == 3:
        print(f"{V[1].Nome[0]} {V[1].Nome[1]} {V[1].Nome[2]}")
    if t == 4:
        print(f"{V[2].Nome[0]} {V[2].Nome[1]} {V[2].Nome[2]}")
    if t == 5:
        print(f"{L.Turma} {L.Prox.Turma} {L.Prox.Prox.Turma} {L.Prox.Prox.Prox.Turma}")
    if t == 6:
        print(f"{1 if L.Prox.Prox.Prox.Prox is None else 0}")
    if t == 7:
        print(f"{L.Aluno.Idade} {L.Prox.Aluno.Idade} {L.Prox.Prox.Aluno.Idade} {L.Prox.Prox.Prox.Aluno.Idade}")

for line in entrada():
    t = int(line)
    TestarMemoria(t)
