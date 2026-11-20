---
layout: default
---

# Aula 14a — Do interpretador ao kernel

## Nesta aula

Explicar o caminho de uma operação Python entre código-fonte, bytecode, implementação CPython, bibliotecas nativas, sistema operacional e hardware — e investigar cada camada com ferramentas do espaço de usuário. Pré-requisitos: aulas 01 a 14. Não precisa saber C, Assembly, QEMU nem kernel.

Você vai aprender a traçar o caminho do bytecode ao kernel (interpretador, syscalls), distinguir tempo de CPU de I/O, e apontar o que Python observa.

## 1. Linguagem, implementação e máquina são três contratos

Python, a linguagem, promete semântica: nomes, objetos, exceções. CPython, a implementação, escolhe *como*: bytecode, contagem de referências, GIL. A máquina impõe o resto: cache, páginas, syscalls, disco. Confundir as camadas gera dois erros clássicos: culpar a linguagem pelo que é detalhe da implementação (GIL), e culpar a implementação pelo que é física (disco lento).

## 2. Bytecode: o programa que o Python realmente executa

`dis.dis(f)` mostra as instruções que o interpretador avalia em loop. Não é Assembly: é um formato interno do CPython, instável entre versões — nunca trate bytecode como API. Serve para responder "quantas operações este laço faz?" antes de culpar o hardware.

## 3. A fronteira nativa e a ABI

Quando Python chama `numpy`, `hashlib` ou `mmap`, a execução cruza para código compilado pela ABI da plataforma. A ABI é contrato binário: como argumentos viajam, quem limpa a pilha, como structs se alinham. Por isso extensão nativa é específica de versão e SO — e por isso `tracemalloc` não vê memória alocada do lado nativo: ele rastreia objetos Python, não `malloc` alheio.

## 4. Memória virtual, RSS e os dois medidores

O processo Python enxerga endereços virtuais; o SO mapeia páginas sob demanda. Duas réguas diferentes:

- `tracemalloc`: rastreia alocações de objetos Python (útil para achar vazamento de lista/dict).
- RSS (`resource.getrusage().ru_maxrss` no Linux/macOS): pico real do processo, incluindo interpretador, bibliotecas nativas e páginas.

RSS >= tracemalloc, sempre. Se o RSS cresce e o `tracemalloc` não, o vazamento está do lado nativo — e nenhum teste de objeto Python vai achá-lo.

## 5. Syscalls, descritores e bloqueio

Abrir, ler e escrever arquivos são syscalls (`open`, `read`, `write`): cada uma atravessa ao kernel. Ler em blocos (`f.read(65536)`) faz menos travessias que ler byte a byte — a mesma lógica da aula 13 sobre I/O, agora medida do lado Python. Descritor esgotado, permissão negada e arquivo ausente são respostas normais do SO, não bugs do interpretador: o programa deve tratá-las (`FileNotFoundError`, `PermissionError`) em vez de presumir sucesso.

## 6. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/14a_sistema.py) contém exatamente o programa abaixo. Usa só a biblioteca padrão; `resource` e `os.sysconf` existem em Linux/macOS (no Windows, o programa explica e pula essas partes):

```bash
python3 teoria/python/exemplos/14a_sistema.py
```

```python
"""Do interpretador ao kernel: bytecode, arquivo, processo e memoria."""

import dis
import hashlib
import os
import subprocess
import sys
import time
import tracemalloc


def soma_quadrados(n: int) -> int:
    total = 0
    for i in range(n):
        total += i * i
    return total


def bytecode_demo() -> None:
    print("== bytecode de soma_quadrados ==")
    dis.dis(soma_quadrados)


def ler_em_blocos(caminho: str, bloco: int = 65536) -> tuple[int, float]:
    inicio = time.perf_counter()
    total = 0
    with open(caminho, "rb") as f:
        while True:
            parte = f.read(bloco)
            if not parte:
                break
            total += len(parte)
    return total, time.perf_counter() - inicio


def memoria_processo() -> dict:
    info = {}
    try:
        import resource
        uso = resource.getrusage(resource.RUSAGE_SELF)
        info["maxrss_kb"] = uso.ru_maxrss
        info["minflt"] = uso.ru_minflt
        info["majflt"] = uso.ru_majflt
    except ImportError:
        info["resource"] = "indisponivel (nao e Linux/macOS)"
    atual, pico = tracemalloc.get_traced_memory()
    info["tracemalloc_atual_b"] = atual
    info["tracemalloc_pico_b"] = pico
    try:
        info["pagina_b"] = os.sysconf("SC_PAGE_SIZE")
    except (AttributeError, ValueError, OSError):
        info["pagina_b"] = "desconhecida"
    return info


def main() -> int:
    print(f"implementacao={sys.implementation.name} versao={sys.version.split()[0]}")
    bytecode_demo()

    tracemalloc.start()
    dados_teste = bytes(range(256)) * 4096  # 1 MiB deterministico
    assert hashlib.sha256(dados_teste).hexdigest() == hashlib.sha256(bytes(range(256)) * 4096).hexdigest()
    caminho = "/tmp/14a_dados.bin"
    with open(caminho, "wb") as f:
        f.write(dados_teste)
    total, dur = ler_em_blocos(caminho)
    assert total == len(dados_teste)
    print(f"lidos {total} bytes em {dur * 1e3:.2f} ms")

    # Processo filho: a mesma contabilidade do C 16a, vista do Python.
    r = subprocess.run([sys.executable, "--version"], capture_output=True, text=True, timeout=10)
    assert r.returncode == 0, r.stderr
    print("filho:", r.stdout.strip() or r.stderr.strip())

    # Falhas provocadas: arquivo ausente e diretorio como arquivo.
    for ruim in ("/tmp/14a_nao_existe.bin", "/tmp"):
        try:
            ler_em_blocos(ruim)
            print(f"INESPERADO: {ruim} abriu")
            return 2
        except (FileNotFoundError, PermissionError, IsADirectoryError, OSError) as e:
            print(f"falha contida ({type(e).__name__}): {ruim}")

    for k, v in memoria_processo().items():
        print(f"{k}={v}")
    tracemalloc.stop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```

Saída esperada (números variam por máquina e SO):

```text
implementacao=cpython versao=3.12.3
== bytecode de soma_quadrados ==
... (instrucoes do interpretador; variam por versao)
lidos 1048576 bytes em ~ms
filho: Python 3.12.3
falha contida (...): /tmp/14a_nao_existe.bin
falha contida (...): /tmp
maxrss_kb=...
```

Leia com o modelo certo: RSS inclui interpretador e páginas; `tracemalloc` conta só objetos Python. Se o RSS cresce e o `tracemalloc` não, o vazamento está do lado nativo.

### O que a memória faz aqui?

O arquivo de 1 MiB passa pelo page cache do SO antes de chegar ao Python; a segunda leitura do mesmo arquivo costuma ser mais rápida — cache quente, não código mais rápido. O `bytes(range(256)) * 4096` vive no heap do processo como objeto imutável. Nada disso revela endereços físicos: ver a aula 10a da trilha de C.

## 7. Erros comuns e implicações defensivas

1. **Tratar bytecode como API:** muda por versão do CPython. Use para investigar, nunca como contrato.
2. **Confundir RSS com `tracemalloc`:** medidores diferentes, vazamentos diferentes.
3. **Ler byte a byte:** cada `read(1)` é uma travessia. Leia em blocos.
4. **Presumir arquivo:** existência, permissão e tipo mudam entre a checagem e o uso. Trate a exceção, não presuma.
5. **Atribuir ao GIL o que é I/O:** programa lento com disco não acelera com mais threads. Meça antes de paralelizar.
6. **Alegar controle do kernel:** o processo Python observa e pede; quem decide páginas, escalonamento e cache é o SO.

## 8. Exercícios obrigatórios

1. Desenhe o caminho de `f.read(65536)` do programa até o disco e de volta, marcando bytecode, syscall, page cache e cópia para o objeto `bytes`.
2. Leia o mesmo arquivo duas vezes seguidas e compare os tempos. Explique a diferença sem usar a palavra "otimização".
3. Provoque `PermissionError` de verdade (arquivo sem leitura para seu usuário) e registre a exceção exata.
4. Aloque 100 MB em lista e observe RSS contra `tracemalloc`; depois `del` e observe de novo. Explique o que volta e o que não volta.

Use [pistas graduais](exercicios/14a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/14a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- distinguir linguagem, implementação e máquina em um comportamento observado;
- explicar o que `dis` mostra e por que não é contrato;
- dizer por que RSS e `tracemalloc` divergem e onde procurar cada vazamento;
- medir leitura em blocos e interpretar cache quente sem alegar código mais rápido;
- tratar arquivo ausente, sem permissão e diretório como respostas normais do SO;
- executar o exemplo e confrontar cada número com o modelo.

**Próxima aula:** concorrência com `threading` e `multiprocessing`. O raciocínio desta aula sustenta a escolha: o GIL, o I/O e o custo de IPC decidem entre threads e processos.

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

1. *High Performance Python*, Gorelick e Ozsvald (O'Reilly, ISBN 978-1492055020); 2. Documentação de `threading`/`asyncio` em [docs.python.org/3/library](https://docs.python.org/3/library/); 3. Luciano Ramalho, *Python Fluente*, caps. sobre concorrência.
