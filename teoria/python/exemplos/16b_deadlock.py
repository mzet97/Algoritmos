"""Aula 16b — deadlock reproduzido com deteccao, e backpressure de fila."""
import queue
import threading
import time

ESPERA = threading.Event()


def cenario_deadlock() -> bool:
    """Duas threads, dois locks, ordens opostas. Devolve True se travar."""
    a = threading.Lock()
    b = threading.Lock()
    pegou_primeiro = threading.Barrier(3)

    def t1():
        with a:
            pegou_primeiro.wait()
            ESPERA.wait(2)  # garante que t2 pegou b antes de tentarmos
            b.acquire()
            b.release()

    def t2():
        with b:
            pegou_primeiro.wait()
            a.acquire()
            a.release()

    x = threading.Thread(target=t1, daemon=True)
    y = threading.Thread(target=t2, daemon=True)
    x.start()
    y.start()
    pegou_primeiro.wait()
    x.join(timeout=1.0)
    y.join(timeout=1.0)
    return x.is_alive() or y.is_alive()


def cenario_ordem_unica() -> bool:
    """Mesma estrutura, ordem global de aquisicao: nao trava."""
    a = threading.Lock()
    b = threading.Lock()

    def trabalho(primeiro, segundo):
        with primeiro:
            with segundo:
                pass

    x = threading.Thread(target=trabalho, args=(a, b))
    y = threading.Thread(target=trabalho, args=(a, b))
    x.start()
    y.start()
    x.join(timeout=5)
    y.join(timeout=5)
    return not (x.is_alive() or y.is_alive())


def cenario_backpressure() -> float:
    """Fila limitada a 4, consumidor dormindo: o 5o put bloqueia."""
    fila: queue.Queue[int] = queue.Queue(maxsize=4)
    consumidos = []

    def consumidor():
        time.sleep(0.3)
        while True:
            item = fila.get()
            if item == -1:
                return
            consumidos.append(item)

    c = threading.Thread(target=consumidor)
    c.start()
    t0 = time.perf_counter()
    for i in range(5):
        fila.put(i)
    espera = time.perf_counter() - t0
    fila.put(-1)
    c.join(timeout=5)
    assert consumidos == [0, 1, 2, 3, 4]
    return espera


def main() -> int:
    ESPERA.set()
    travou = cenario_deadlock()
    print(f"deadlock por ordem oposta: {'TRAVADO (detectado por timeout)' if travou else 'nao travou'}")
    assert travou, "o cenario deveria travar para a deteccao significar algo"

    assert cenario_ordem_unica(), "ordem global deveria completar"
    print("ordem global de aquisicao: completa sem travar")

    espera = cenario_backpressure()
    print(f"backpressure: 5o put esperou {espera:.3f}s (fila de 4, consumidor dormindo)")
    assert espera >= 0.2
    print("16b ok: deadlock detectado, correcao por ordem global e backpressure medida")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
