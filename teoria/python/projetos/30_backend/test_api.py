from fastapi.testclient import TestClient
from main import app


client = TestClient(app)


def test_create_and_get_task() -> None:
    response = client.post("/tasks", json={"title": "estudar"})
    assert response.status_code == 201
    task_id = response.json()["task_id"]

    response = client.get(f"/tasks/{task_id}")
    assert response.status_code == 200
    assert response.json()["title"] == "estudar"
