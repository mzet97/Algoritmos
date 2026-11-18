---
layout: default
---

# Aula 12 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../12.md#19-exercicios-obrigatorios).

```python
from dataclasses import dataclass
from enum import Enum


class NivelAcesso(Enum):
    LEITURA = "leitura"
    ESCRITA = "escrita"
    ADMIN = "admin"


@dataclass
class Aluno:
    nome: str
    nota: float
    ativo: bool = True

    def __post_init__(self) -> None:
        if self.nota < 0 or self.nota > 10:
            raise ValueError("nota fora de 0..10")


@dataclass
class Usuario:
    nome: str
    nivel: NivelAcesso = NivelAcesso.LEITURA


class AlunoManual:
    def __init__(self, nome: str, nota: float, ativo: bool = True) -> None:
        self.nome, self.nota, self.ativo = nome, nota, ativo

    def __repr__(self) -> str:
        return f"AlunoManual(nome={self.nome!r}, nota={self.nota!r}, ativo={self.ativo!r})"

    def __eq__(self, outro: object) -> bool:
        return (isinstance(outro, AlunoManual)
                and (self.nome, self.nota, self.ativo) == (outro.nome, outro.nota, outro.ativo))


def main() -> None:
    aluno = Aluno("Ada", 9.5)
    assert aluno.ativo and aluno == Aluno("Ada", 9.5, True)
    try:
        Aluno("Beto", -1)
    except ValueError:
        print("post_init barrou nota negativa")
    else:
        raise AssertionError("deveria ter barrado")

    usuario = Usuario("Cris", NivelAcesso.ADMIN)
    assert usuario.nivel is NivelAcesso.ADMIN and Usuario("Davi").nivel is NivelAcesso.LEITURA
    assert len(NivelAcesso) == 3

    manual = AlunoManual("Ada", 9.5)
    assert repr(manual).startswith("AlunoManual(") and manual == AlunoManual("Ada", 9.5)
    print("dataclass: init/repr/eq gerados; manual: escritos a mao", repr(aluno))
    print("testes ok")


if __name__ == "__main__":
    main()
```

2. Validar no `__post_init__` faz o objeto **nascer valido ou nao nascer** — o mesmo "parse, nao valide depois" das fronteiras, aplicado a objetos.

**Autoverificacao:** igualdade gerada funcionando, nota invalida barrada no nascimento, `Enum` com padrao e a versao manual provando o boilerplate que sumiu.
