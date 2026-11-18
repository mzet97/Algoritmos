---
layout: default
---

# Aula 20 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../20.md#14-exercicios-obrigatorios). Executado com o cliente de teste do FastAPI no ambiente dos [projetos](../projetos/README.md):

```python
from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from fastapi.testclient import TestClient
from pydantic import BaseModel


class ErroNegocio(Exception):
    def __init__(self, mensagem: str) -> None:
        super().__init__(mensagem)
        self.mensagem = mensagem


class InventoryService:
    def __init__(self) -> None:
        self._itens: dict[int, dict] = {}
        self._proximo = 1

    def criar(self, nome: str, quantidade: int) -> dict:
        if not nome.strip():
            raise ErroNegocio("nome em branco")
        if any(i["nome"] == nome for i in self._itens.values()):
            raise ErroNegocio("item duplicado")
        item = {"id": self._proximo, "nome": nome, "quantidade": quantidade}
        self._itens[self._proximo] = item
        self._proximo += 1
        return item

    def obter(self, item_id: int) -> dict:
        if item_id not in self._itens:
            raise ErroNegocio("item inexistente")
        return self._itens[item_id]


class ItemEntrada(BaseModel):
    nome: str
    quantidade: int


servico = InventoryService()
app = FastAPI()


@app.exception_handler(ErroNegocio)
async def tratar_negocio(request: Request, exc: ErroNegocio) -> JSONResponse:
    return JSONResponse(status_code=422, content={"erro": exc.mensagem})


@app.get("/health")
def health() -> dict:
    return {"status": "ok"}


@app.post("/itens", status_code=201)
def criar(entrada: ItemEntrada) -> dict:
    return servico.criar(entrada.nome, entrada.quantidade)


@app.get("/itens/{item_id}")
def obter(item_id: int) -> dict:
    return servico.obter(item_id)


def main() -> None:
    cliente = TestClient(app, raise_server_exceptions=False)
    saude = cliente.get("/health")
    criado = cliente.post("/itens", json={"nome": "livro", "quantidade": 3})
    branco = cliente.post("/itens", json={"nome": "  ", "quantidade": 1})
    duplicado = cliente.post("/itens", json={"nome": "livro", "quantidade": 1})
    forma_errada = cliente.post("/itens", json={"nome": "x"})
    inexistente = cliente.get("/itens/99")
    print("health:", saude.status_code, "| criar:", criado.status_code,
          "| branco:", branco.status_code, branco.json(),
          "| duplicado:", duplicado.status_code,
          "| sem campo:", forma_errada.status_code,
          "| inexistente:", inexistente.status_code, inexistente.json())
    assert saude.status_code == 200 and criado.status_code == 201
    assert branco.status_code == 422 and duplicado.status_code == 422
    assert forma_errada.status_code == 422 and inexistente.status_code == 422
    print("testes ok")


if __name__ == "__main__":
    main()
```

4 e 5. Repare nos dois 422 com donos diferentes: o de **forma** (campo faltando) vem do schema do Pydantic antes do servico acordar; o de **regra** (branco, duplicado, inexistente) vem do `ErroNegocio` do servico traduzido no handler unico. Schema nao sabe o que ja existe; regra nao deveria revalidar tipo. Misturar os dois faz a mesma pergunta ser respondida em dois lugares — e divergir.

2. Num projeto real, `InventoryService` mora em `servico.py` e as rotas em `main.py` importando-o — aqui ficam num arquivo so para o harness executar; a fronteira entre eles ja e a do exercicio.

**Autoverificacao:** health sem negocio, criacao 201, regra e forma rejeitadas por caminhos distinguiveis.
