"""Aula 23b — relogios e particao bilateral sobre a 23a (simulacao)."""
from dataclasses import dataclass, field


@dataclass
class Evento:
    lamport: int
    vetor: tuple[int, ...]
    autor: int
    seq: int
    chave: str
    valor: int


@dataclass
class No:
    autor: int
    lamport: int = 0
    vetor: list[int] = field(default_factory=lambda: [0, 0])
    log: list[Evento] = field(default_factory=list)
    estado: dict[str, int] = field(default_factory=dict)
    seq: int = 0

    def escrever(self, chave: str, valor: int) -> Evento:
        self.lamport += 1
        self.vetor[self.autor] += 1
        self.seq += 1
        ev = Evento(self.lamport, tuple(self.vetor), self.autor, self.seq, chave, valor)
        self.log.append(ev)
        self.estado[chave] = valor
        return ev

    def receber(self, ev: Evento) -> bool:
        if any(e.autor == ev.autor and e.seq == ev.seq for e in self.log):
            return False
        self.lamport = max(self.lamport, ev.lamport) + 1
        self.vetor = [max(a, b) for a, b in zip(self.vetor, ev.vetor)]
        self.log.append(ev)
        return True


def antes(a: Evento, b: Evento) -> bool:
    return all(x <= y for x, y in zip(a.vetor, b.vetor)) and a.vetor != b.vetor


def sincronizar(a: No, b: No) -> None:
    la, lb = list(a.log), list(b.log)
    for ev in lb:
        a.receber(ev)
    for ev in la:
        b.receber(ev)
    for no in (a, b):
        ordenado = sorted(no.log, key=lambda e: (e.lamport, e.autor, e.seq))
        vistos: set[tuple[int, int]] = set()
        no.log = []
        for ev in ordenado:
            if (ev.autor, ev.seq) not in vistos:
                vistos.add((ev.autor, ev.seq))
                no.log.append(ev)
        no.estado = {}
        for ev in no.log:
            no.estado[ev.chave] = ev.valor


def main() -> int:
    a, b = No(0), No(1)
    e1 = a.escrever("x", 10)
    assert b.receber(e1) and not b.receber(e1)
    e2 = b.escrever("y", 20)
    assert antes(e1, e2)
    print(f"lamport: e1={e1.lamport} e2={e2.lamport} causalidade preservada")

    a2, b2 = No(0), No(1)
    p1 = a2.escrever("k", 111)
    p2 = b2.escrever("k", 222)
    assert not antes(p1, p2) and not antes(p2, p1), "particao deveria ser concorrente"
    assert a2.estado["k"] == 111 and b2.estado["k"] == 222
    print("particao: A ve k=111, B ve k=222, vetores detectam concorrencia")

    sincronizar(a2, b2)
    assert a2.estado["k"] == b2.estado["k"] == 222
    print("reconciliacao: ambos k=222; k=111 descartado pela ordem (lamport,autor)")

    e3 = b2.escrever("k", 333)
    assert antes(p1, e3) and antes(p2, e3)
    sincronizar(a2, b2)
    assert a2.estado["k"] == b2.estado["k"] == 333
    print("pos-sincronizacao: escrita causal k=333 vence nos dois nos")
    print("23b ok: ordem, concorrencia e reconciliacao com perda declarada")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
