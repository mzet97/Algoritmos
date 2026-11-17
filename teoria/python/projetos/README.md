# Projetos das aulas 30–32 de Python

Fontes extraídas dos blocos de código das aulas e executadas em ambiente
limpo, dentro do ambiente do próprio projeto (`.venv` gerenciado por uv):

```bash
cd teoria/python/projetos
uv sync
.venv/bin/python -m pytest 30_backend/test_api.py -q
.venv/bin/python 32_pipeline/main.py
```

| Projeto | Aula | O que é testado |
| --- | --- | --- |
| `30_backend` | [Aula 30](../30.md) | Teste da aula via `TestClient` (criar e ler tarefa) e servidor uvicorn ao vivo respondendo `/health` |
| `31_infra_tool` | [Aula 31](../31.md) | SHA-256 por subprocesso, checkpoint com `[skip]` na reexecução e código de saída 2 para arquivo ausente |
| `32_pipeline` | [Aula 32](../32.md) | Pipeline Pandas: linha inválida descartada, agregado `blue = 65.0` e medição de tempo/memória impressa |

A verificação completa dos seis projetos (Rust + Python) roda com
`python3 teoria/verificar_projetos.py` a partir da raiz do repositório.
