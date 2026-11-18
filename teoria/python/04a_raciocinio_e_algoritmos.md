---
layout: default
---

# Aula 04a — Raciocínio discreto e algoritmos em Python

## Objetivos verificáveis

Ao final, você deverá: enunciar invariantes, provar correção por invariante, e distinguir a prova da rede de segurança da linguagem.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir invariante, pré/pós-condição`;
- **Nível 2 — Aplicar:** `prever violação de invariante e corrigir`;
- **Nível 3 — Projetar:** `projetar algoritmo com invariante e prova`.

**Competências SDD:** MAT, ALG e SEC. **Pré-requisitos:** aulas 01 a 04; saber criar listas, dicionários e conjuntos, escrever funções e laços, e executar um script. Não é preciso conhecer classes, decoradores ou bibliotecas externas.

**Objetivo observável:** ao terminar, você deverá escrever a pré-condição e a pós-condição de uma função, declarar o invariante de um laço, argumentar terminação por uma grandeza que diminui, provar um caso recursivo por indução e confrontar uma previsão assintótica com uma medição. Tudo com a biblioteca padrão, sem instalar nada.

**Motivação:** as coleções da aula 04 resolvem o caso comum, mas escondem duas perguntas que voltam em desempenho, redes e sistemas distribuídos: *esta busca sempre termina com a resposta certa?* e *quanto ela custa quando a entrada cresce?* Um teste mostra o comportamento nas entradas testadas. Um argumento de correção explica o comportamento em todas as entradas do contrato. Esta aula ensina os dois, e quando cada um basta.

## 1. Contrato antes do código

Tome a busca binária: dada uma lista ordenada, devolver o índice de um valor ou sinalizar ausência.

- **Pré-condição:** a lista está ordenada em ordem não decrescente e seus elementos são comparáveis com `<`.
- **Pós-condição:** se o valor existe na lista, o índice devolvido aponta para ele; se não existe, o resultado é `-1`.
- **Caso especial:** lista vazia devolve `-1` sem comparar nada. Elementos repetidos podem devolver qualquer índice válido; documente a escolha em vez de fingir unicidade.

Em Python, `x in lista` e `lista.index(x)` fazem busca linear: corretas, mas custam tempo proporcional ao tamanho. A busca binária troca uma exigência a mais (ordenação) por menos comparações. O contrato registra exatamente essa troca.

## 2. O invariante que sobrevivem às iterações

Mantenha dois limites, `ini` e `fim`, com a promessa:

```text
se o valor está na lista, então ele está em lista[ini:fim]
```

No início, `ini = 0` e `fim = len(lista)`: a promessa vale porque a fatia é a lista inteira. A cada passo, compare com o elemento do meio `meio`:

- se igual, devolva o índice (pós-condição satisfeita);
- se o valor é menor, descarte a metade direita (`fim = meio`);
- se é maior, descarte a metade esquerda (`ini = meio + 1`).

Cada descarte preserva a promessa, porque a metade eliminada só contém elementos que a ordenação prova estarem do lado errado. Quando `ini == fim`, a fatia está vazia e a promessa implica ausência: devolva `-1`.

**Terminação:** a grandeza `fim - ini` é um inteiro não negativo que diminui estritamente a cada iteração sem acerto. Não existe sequência infinita assim; o laço termina.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/04a_busca.py) contém exatamente o programa abaixo:

```python
"""Busca binaria com invariante documentado e medicao linear x binaria."""

import time


def busca_binaria(valores: list[int], alvo: int) -> int:
    """Devolve o indice de alvo em valores ou -1 se ausente.

    Pre-condicao: valores ordenada em ordem nao decrescente.
    Invariante: se alvo esta na lista, esta em valores[ini:fim].
    """
    ini, fim = 0, len(valores)
    while ini < fim:
        meio = (ini + fim) // 2
        if valores[meio] == alvo:
            return meio
        if alvo < valores[meio]:
            fim = meio
        else:
            ini = meio + 1
    return -1


def busca_linear(valores: list[int], alvo: int) -> int:
    """Versao de referencia: percorre tudo, sem exigir ordenacao."""
    for i, valor in enumerate(valores):
        if valor == alvo:
            return i
    return -1


def medir(funcao, valores: list[int], alvo: int, repeticoes: int) -> float:
    inicio = time.perf_counter()
    for _ in range(repeticoes):
        funcao(valores, alvo)
    return (time.perf_counter() - inicio) / repeticoes


def main() -> None:
    assert busca_binaria([], 5) == -1
    assert busca_binaria([5], 5) == 0
    assert busca_binaria([5], 4) == -1
    assert busca_binaria([1, 3, 5, 7], 1) == 0
    assert busca_binaria([1, 3, 5, 7], 7) == 3
    assert busca_binaria([1, 3, 5, 7], 4) == -1
    assert busca_binaria([2, 2, 2], 2) in (0, 1, 2)

    grande = list(range(0, 2_000_000, 2))
    alvo_presente, alvo_ausente = 1_999_998, 1_999_999
    for f in (busca_linear, busca_binaria):
        assert f(grande, alvo_presente) == alvo_presente // 2
        assert f(grande, alvo_ausente) == -1

    rep_lin, rep_bin = 5, 200
    t_lin = medir(busca_linear, grande, alvo_ausente, rep_lin)
    t_bin = medir(busca_binaria, grande, alvo_ausente, rep_bin)
    print(f"linear: {t_lin * 1e3:.3f} ms/media ({rep_lin}x)")
    print(f"binaria: {t_bin * 1e3:.6f} ms/media ({rep_bin}x)")


if __name__ == "__main__":
    main()
```

Execute a partir da raiz do repositório:

```bash
python3 teoria/python/exemplos/04a_busca.py
```

Saída esperada (tempos variam por máquina; a ordem de grandeza importa, não o dígito):

```text
linear: ~ms/media (5x)
binaria: ~µs/media (200x)
```

A busca linear percorre um milhão de elementos e a binária faz cerca de 20 comparações: `log2(1_000_000) ≈ 20`. A previsão assintótica (`O(n)` contra `O(log n)`) não fixa segundos, mas proíbe surpresa na razão entre os tempos.

### O que a memória faz aqui?

`grande` guarda um milhão de inteiros de uma vez: custo de espaço `O(n)` explícito, medido em objetos Python (cada `int` é um objeto, não 4 bytes). As buscas usam `O(1)` espaço adicional. Medir antes de verificar a correção inverte a ordem: primeiro os `assert`, depois o cronômetro.

## 4. Indução: recursão com argumento, não com esperança

A soma `1 + 2 + ... + n` em versão recursiva:

```python
def soma_ate(n: int) -> int:
    if n <= 0:
        return 0
    return n + soma_ate(n - 1)
```

A prova por indução tem o mesmo formato do invariante, deslocado para chamadas:

- **Base:** `soma_ate(0) == 0`, correto por definição.
- **Passo:** suponha `soma_ate(n-1)` correto; então `n + soma_ate(n-1)` soma `n` ao total de `1..n-1`, que é o total de `1..n`.

Duas advertências de engenharia acompanham a prova. Primeira: cada chamada pendente ocupa um quadro na pilha, e o limite padrão de recursão (`sys.getrecursionlimit()`, 1000) transforma `soma_ate(10**6)` em `RecursionError`. A prova matemática não remove o limite da máquina. Segunda: inteiros Python têm precisão arbitrária, então não há overflow aqui — mas a mesma função em C estouraria o tipo finito, como mostra a aula 3A da trilha de C. O contrato precisa dizer em que representação a prova vale.

## 5. Conjuntos, relações e o custo do `dict`

Relações aparecem sempre que um dado aponta para outro: amizades, dependências, rotas. Um grafo é um conjunto de vértices mais um conjunto de pares (arestas). A busca em largura (BFS) encontra o caminho mais curto em arestas, e seu invariante é direto: a fila só contém vértices alcançáveis, e todo vértice sai da fila uma única vez porque entra no conjunto `visitados` antes de entrar na fila.

```python
from collections import deque


def bfs_caminho(grafo: dict[str, list[str]], origem: str, destino: str) -> list[str] | None:
    visitados = {origem}
    fila: deque[tuple[str, list[str]]] = deque([(origem, [origem])])
    while fila:
        atual, caminho = fila.popleft()
        if atual == destino:
            return caminho
        for vizinho in grafo.get(atual, []):
            if vizinho not in visitados:
                visitados.add(vizinho)
                fila.append((vizinho, caminho + [vizinho]))
    return None
```

Note o par com `dict` e `set`: pertinência e inserção custam `O(1)` em média por causa do hash, e a prova de término usa o conjunto `visitados`, que cresce no máximo até o número de vértices. O alerta honesto: `O(1)` médio não é `O(1)` sempre. Com colisões adversárias de hash, operações degradam; nunca prometa latência de pior caso para `dict` sem medir a carga real. Em Python, `hash()` de `str` ainda varia por processo (randomização contra DoS): não persista valores de hash entre execuções.

## 6. Teste de propriedade sem biblioteca externa

Testes por exemplo conferem casos escolhidos. Testes de propriedade conferem uma afirmação para centenas de entradas geradas, comparando duas implementações independentes:

```python
import random


def checar_busca(tentativas: int = 500) -> None:
    rng = random.Random(42)  # semente fixa: o teste e reproduzivel
    for _ in range(tentativas):
        tamanho = rng.randint(0, 50)
        valores = sorted(rng.randint(-100, 100) for _ in range(tamanho))
        alvo = rng.randint(-110, 110)
        achado = busca_binaria(valores, alvo)
        if alvo in valores:
            # Repetidos admitem qualquer indice valido: confira o contrato.
            assert achado != -1 and valores[achado] == alvo
        else:
            assert achado == -1
```

A semente fixa torna a falha reproduzível. O teste confere o contrato (`in` mais acesso indexado), não a igualdade com a busca linear: com repetidos, dois índices diferentes podem ser ambos corretos, e exigir igualdade transformaria uma resposta válida em falha falsa. A busca linear continua útil como referência legível durante o desenvolvimento. Quando o teste de propriedade falha, ele entrega a entrada que quebra a afirmação — um contraexemplo, não um palpite.

## 7. Ponte com JavaScript

JavaScript usa `Number` (ponto flutuante de 64 bits) para a aritmética comum: nem todo inteiro grande é exato, e `2**53 + 1 === 2**53 + 2`. Python tem `int` arbitrário, então a prova da soma recursiva vale sem ressalva de overflow — mas a ressalva volta em `float`, `numpy` e nas fronteiras com C. `Array.prototype.sort` sem comparador ordena como texto (`[10, 9]` vira `[10, 9]`? não: vira `[10, 9]` ordenado como `"10" < "9"`); `sorted` do Python ordena pelo valor com algoritmo estável. Nos dois casos, a pré-condição "lista ordenada" só vale se você souber *por qual critério* ela foi ordenada.

## 8. Erros comuns e implicações defensivas

1. **Buscar sem garantir ordenação:** a binária devolve resposta errada em silêncio. Valide ou documente a exigência no contrato.
2. **Confundir média com pior caso:** `dict` é `O(1)` em média; entrada adversária degrada. Limite tamanho de entrada não confiável antes de indexar.
3. **Recursão sem limite declarado:** `RecursionError` em produção é queda. Prefira laço ou declare o limite máximo de profundidade.
4. **Medir antes de provar:** benchmark de algoritmo errado mede a velocidade do erro.
5. **Semente aleatória solta:** teste de propriedade sem semente fixa gera falha irreproduzível. Fixe a semente e registre-a.
6. **Assumir estabilidade de hash:** `hash(str)` muda por processo. Não use como identificador persistente nem como prova de igualdade entre execuções.

## 9. Exercícios obrigatórios

1. Reescreva a prova do invariante da busca binária (início, preservação, término) sem consultar a seção 2.
2. Implemente `bfs_caminho` para um grafo com ciclo e prove que ela termina mesmo que o destino seja inalcançável. Meça o número de vértices visitados.
3. Escreva `checar_busca` com uma segunda implementação de referência (por exemplo, `alvo in valores`) e compare o tempo das duas estratégias de teste para 2000 tentativas.
4. Construa uma entrada de 100 mil inteiros que torne a busca linear 100 vezes mais lenta que a binária na sua máquina; registre ambiente, tamanhos e tempos.

Use [pistas graduais](exercicios/04a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/04a_solucoes.md) depois de registrar sua própria explicação.

## 10. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- declarar pré-condição, pós-condição e convenção de repetidos e de lista vazia;
- explicar por que o invariante vale antes e depois de cada iteração;
- indicar a grandeza que diminui e provar o término;
- executar o exemplo e confrontar a previsão `O(log n)` com a medição;
- explicar base e passo de uma prova por indução e o limite de recursão da máquina;
- distinguir teste por exemplo de teste de propriedade reproduzível.

**Próxima aula:** dicionários e conjuntos em profundidade. O raciocínio por invariantes desta aula sustenta o uso seguro de `visitados` em qualquer travessia de grafo.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. Luciano Ramalho, *Python Fluente* (Novatec/O'Reilly, ISBN 978-1492056355); 2. Documentação oficial em [docs.python.org/3](https://docs.python.org/3/); 3. *Tutorial oficial* em [docs.python.org/3/tutorial](https://docs.python.org/3/tutorial/).
