---
layout: default
---

# Aula 30 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../30.md#9-exercicios-obrigatorios). Extensao executada sobre o desenho do [projeto 30](../projetos/30_backend/main.py), com o servico em modulo proprio neste arquivo unico de laboratorio:

```python
import os

from fastapi import FastAPI, HTTPException
from fastapi.testclient import TestClient
from pydantic import BaseModel, Field


class TaskService:
    def __init__(self) -> None:
        self._tasks: dict[int, dict] = {}
        self._next_id = 1

    def create_task(self, title: str) -> dict:
        task = {"task_id": self._next_id, "title": title, "done": False}
        self._tasks[self._next_id] = task
        self._next_id += 1
        return task

    def get_task(self, task_id: int) -> dict | None:
        return self._tasks.get(task_id)

    def complete_task(self, task_id: int) -> dict | None:
        task = self._tasks.get(task_id)
        if task is None:
            return None
        task["done"] = True
        return task


class TaskCreate(BaseModel):
    title: str = Field(min_length=1, max_length=120)


DEBUG = os.getenv("DEBUG", "false").lower() == "true"

service = TaskService()
app = FastAPI(title="todo-service", debug=DEBUG)


@app.get("/health")
def health() -> dict:
    return {"status": "ok", "debug": DEBUG}


@app.post("/tasks", status_code=201)
def create_task(payload: TaskCreate) -> dict:
    return service.create_task(payload.title)


@app.get("/tasks/{task_id}")
def get_task(task_id: int) -> dict:
    task = service.get_task(task_id)
    if task is None:
        raise HTTPException(status_code=404, detail="task not found")
    return task


@app.post("/tasks/{task_id}/complete")
def complete_task(task_id: int) -> dict:
    task = service.complete_task(task_id)
    if task is None:
        raise HTTPException(status_code=404, detail="task not found")
    return task


def main() -> None:
    cliente = TestClient(app, raise_server_exceptions=False)
    criada = cliente.post("/tasks", json={"title": "estudar"})
    concluida = cliente.post(f"/tasks/{criada.json()['task_id']}/complete")
    inexistente = cliente.get("/tasks/999")
    concluir_inexistente = cliente.post("/tasks/999/complete")
    print("criar:", criada.status_code, "| concluir:", concluida.status_code, concluida.json(),
          "| 404 buscar:", inexistente.status_code, "| 404 concluir:", concluir_inexistente.status_code,
          "| debug:", cliente.get("/health").json()["debug"])
    assert criada.status_code == 201 and concluida.json()["done"] is True
    assert inexistente.status_code == 404 and concluir_inexistente.status_code == 404
    print("testes ok")


if __name__ == "__main__":
    main()
```

5. Onde entrariam as proximas camadas no projeto real: **banco** no lugar do dicionario do servico quando restart nao puder esquecer tarefas (a interface do servico nao muda — e para isso que ele foi extraido); **fila** quando criar tarefa disparar trabalho lento (notificacao, relatorio) que nao deve segurar a resposta; **observabilidade** mais forte quando `/health` e log solto nao responderem "qual requisicao esta lenta" (a [aula 23](23-solucoes.md): `request_id`, metricas e spans nas fronteiras).

**Autoverificacao:** concluir muda o estado via servico, os dois 404 exigidos pelo status e `DEBUG` lido do ambiente na partida.
