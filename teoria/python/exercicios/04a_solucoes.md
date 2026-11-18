---
layout: default
---

# Soluções comentadas da aula 04a

## 1. Prova do invariante

- **Início:** antes do laço, `ini = 0` e `fim = len(valores)`; a fatia é a lista inteira, então a promessa vale trivialmente.
- **Preservação:** seja `meio = (ini + fim) // 2`. Se `valores[meio] == alvo`, a pós-condição é entregue. Se `alvo < valores[meio]`, a ordenação garante que todo elemento em `valores[meio:fim]` é maior ou igual a `valores[meio]` e portanto diferente de `alvo`; descartar com `fim = meio` preserva a promessa. O ramo `ini = meio + 1` é simétrico.
- **Término:** `fim - ini` diminui estritamente a cada iteração sem acerto e nunca é negativo; logo o laço termina. Ao sair, `ini == fim`, a fatia está vazia e `-1` é a resposta correta.

## 2. BFS com ciclo e destino inalcançável

```python
from collections import deque


def bfs_caminho(grafo: dict[str, list[str]], origem: str, destino: str):
    visitados = {origem}
    fila = deque([(origem, [origem])])
    visitados_contagem = 0
    while fila:
        atual, caminho = fila.popleft()
        visitados_contagem += 1
        if atual == destino:
            return caminho, visitados_contagem
        for vizinho in grafo.get(atual, []):
            if vizinho not in visitados:
                visitados.add(vizinho)
                fila.append((vizinho, caminho + [vizinho]))
    return None, visitados_contagem
```

Cada vértice entra em `visitados` uma única vez, então no máximo `|V|` iterações removem da fila: o término não depende de alcançar o destino. Marcar antes de enfileirar (e não ao desenfileirar) impede que um ciclo reinsira o mesmo vértice.

## 3. Referência alternativa e custo do teste

```python
def busca_in(valores: list[int], alvo: int) -> int:
    try:
        return valores.index(alvo)
    except ValueError:
        return -1
```

`valores.index` percorre em C: cada tentativa custa menos tempo de interpretador que o laço Python de `busca_linear`, mas ambas são `O(n)`. Para 2000 tentativas pequenas, a diferença absoluta é ruído; o ganho aparece em listas grandes. Conclusão: para o teste de propriedade, a referência mais barata de escrever e ler vence, não a mais rápida.

## 4. Entrada para razão 100x

Exemplo de protocolo (números variam por máquina):

```python
import importlib.util
import time

caminho = "../exemplos/04a_busca.py"  # a partir de teoria/python/exercicios/
spec = importlib.util.spec_from_file_location("aula04a", caminho)
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)  # o bloco if __main__ não executa aqui
busca_binaria, busca_linear = mod.busca_binaria, mod.busca_linear

for n in (10_000, 50_000, 100_000, 500_000):
    valores = list(range(n))
    alvo = -1  # ausente: força o pior caso das duas
    t0 = time.perf_counter(); busca_linear(valores, alvo); t_lin = time.perf_counter() - t0
    t0 = time.perf_counter(); busca_binaria(valores, alvo); t_bin = time.perf_counter() - t0
    print(n, t_lin / t_bin)
```

Registre: modelo da CPU, versão do Python (`python3 --version`), sistema operacional, `n`, repetições e os dois tempos. A razão cresce aproximadamente com `n / log2(n)`: em `n = 100_000`, espera-se algo da ordem de centenas, então 100x é alcançável bem antes disso na maioria das máquinas.
