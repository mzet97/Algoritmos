---
layout: default
---

# Aula 29 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../29.md#13-exercicios-obrigatorios).

```python
import cProfile
import io
import pstats
import tracemalloc


def caro(n: int) -> int:
    return sum(i * i for i in range(n))


def barato(n: int) -> int:
    return n + 1


def carga() -> int:
    return caro(60000) + barato(3)


def main() -> None:
    perfil = cProfile.Profile()
    perfil.enable()
    carga()
    perfil.disable()
    saida = io.StringIO()
    stats = pstats.Stats(perfil, stream=saida)
    stats.sort_stats("tottime")
    stats.print_stats(6)
    texto = saida.getvalue()
    linhas_funcoes = [ln.strip().split()[-1] for ln in texto.splitlines() if "(" in ln and ":" in ln]
    print("funcoes no topo do perfil:", linhas_funcoes[:3])
    assert any("caro" in nome for nome in linhas_funcoes[:3])

    tracemalloc.start()
    antes = tracemalloc.take_snapshot()
    grande = [i * i for i in range(200000)]
    depois = tracemalloc.take_snapshot()
    pico = sum(s.size_diff for s in depois.compare_to(antes, "filename"))
    print(f"tracemalloc: crescimento aproximado {pico / 1e6:.1f} MB para {len(grande)} quadrados")
    assert pico > 1_000_000
    tracemalloc.stop()
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. NumPy ajuda mais que Numba quando a operacao ja e uma funcao vetorizada da biblioteca (soma, filtro, produto escalar sobre arrays grandes): voce nao escreve laco nenhum. Numba ajuda quando o laco e **seu** — logica sequencial com dependencia entre iteracoes que nao existe pronta no NumPy — e compilar o laco elimina o interpretador sem reescrever em C, o contraste da [aula 18](18-solucoes.md).

4. FFI faz sentido quando a biblioteca compilada e o padrao do dominio e nao tem equivalente Python maduro (o codec do fornecedor, a biblioteca cientifica de decadas) — a mesma fronteira das aulas [18 de Go](../../go/exercicios/18-solucoes.md) e 29 de Go, com os mesmos custos de deploy.

5. A heuristica reescrita: meça antes de otimizar; otimize o topo do perfil, nao o codigo feio; desça de nivel (NumPy, Numba, FFI) so onde o perfil aponta e so pelo tempo que o ganho paga o custo de fronteira.

**Autoverificacao:** a funcao cara no topo do perfil por tempo proprio e o pico de memoria medido entre as duas fotos.
