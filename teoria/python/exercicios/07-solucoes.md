---
layout: default
---

# Aula 07 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../07.md#15-exercicios-obrigatorios).

```python
class Retangulo:
    def __init__(self, largura: float, altura: float) -> None:
        self.largura, self.altura = largura, altura

    def area(self) -> float:
        return self.largura * self.altura

    def perimetro(self) -> float:
        return 2 * (self.largura + self.altura)


class Conta:
    banco = "Banco Aula"  # atributo de classe: compartilhado

    def __init__(self, titular: str) -> None:
        self.titular, self.saldo = titular, 0.0

    def depositar(self, valor: float) -> None:
        if valor <= 0:
            raise ValueError("deposito deve ser positivo")
        self.saldo += valor

    def sacar(self, valor: float) -> bool:
        if valor > self.saldo:
            return False
        self.saldo -= valor
        return True


class Animal:
    def falar(self) -> str:
        return "..."


class Cachorro(Animal):
    def falar(self) -> str:
        return "au"


class Gato(Animal):
    def falar(self) -> str:
        return "miau"


class Motor:
    def ligar(self) -> str:
        return "motor ligado"


class Carro:  # composicao: tem um Motor, nao e um Motor
    def __init__(self) -> None:
        self.motor = Motor()

    def ligar(self) -> str:
        return self.motor.ligar()


def main() -> None:
    r = Retangulo(3, 4)
    assert (r.area(), r.perimetro()) == (12, 14)

    conta = Conta("Ada")
    conta.depositar(100)
    assert conta.sacar(30) and conta.saldo == 70
    assert not conta.sacar(1000)
    outra = Conta("Beto")
    print("atributo de classe igual nas duas:", conta.banco == outra.banco == Conta.banco)
    conta.banco = "Outro"  # sombra de instancia: nao toca a classe nem a outra conta
    assert outra.banco == "Banco Aula" and Conta.banco == "Banco Aula"

    falas = [a.falar() for a in (Cachorro(), Gato(), Animal())]
    print("polimorfismo:", falas)
    assert falas == ["au", "miau", "..."]
    assert Carro().ligar() == "motor ligado"
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. Heranca diz "Cachorro **e** um Animal" e herda o que nao usa junto; composicao diz "Carro **tem** um Motor" e troca a peca sem tocar na hierarquia. Trocar comportamento em composicao e trocar o objeto interno; em heranca, e criar subtipo novo.

**Autoverificacao:** area/perimetro exatos, saque recusado sem saldo, sombra de instancia isolada e a mesma chamada `falar()` com tres respostas.
