from dataclasses import dataclass
import logging
import os
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("todo-service")


@dataclass
class Settings:
    app_name: str


def load_settings() -> Settings:
    return Settings(app_name=os.getenv("APP_NAME", "todo-service"))


class TaskCreate(BaseModel):
    title: str = Field(min_length=1, max_length=120)


class TaskOut(BaseModel):
    task_id: int
    title: str
    done: bool


class TaskService:
    def __init__(self) -> None:
        self._tasks: dict[int, dict] = {}
        self._next_id = 1

    def create_task(self, title: str) -> dict:
        task = {"task_id": self._next_id, "title": title, "done": False}
        self._tasks[self._next_id] = task
        self._next_id += 1
        logger.info("task_created id=%s", task["task_id"])
        return task

    def get_task(self, task_id: int) -> dict | None:
        return self._tasks.get(task_id)


settings = load_settings()
service = TaskService()
app = FastAPI(title=settings.app_name)


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok", "app": settings.app_name}


@app.post("/tasks", response_model=TaskOut, status_code=201)
def create_task(payload: TaskCreate) -> TaskOut:
    task = service.create_task(payload.title)
    return TaskOut.model_validate(task)


@app.get("/tasks/{task_id}", response_model=TaskOut)
def get_task(task_id: int) -> TaskOut:
    task = service.get_task(task_id)
    if task is None:
        raise HTTPException(status_code=404, detail="task not found")
    return TaskOut.model_validate(task)
