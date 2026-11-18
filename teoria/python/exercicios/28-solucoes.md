---
layout: default
---

# Aula 28 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../28.md#12-exercicios-obrigatorios). Executado com o Pandas do ambiente dos [projetos](../projetos/README.md):

```python
import pandas as pd


def bucket(score: int) -> str:
    if score >= 30:
        return "alto"
    if score >= 15:
        return "medio"
    return "baixo"


def main() -> None:
    df = pd.DataFrame({
        "nome": ["ada", "beto", "cris", "davi", "elisa"],
        "score": [10, 15, 22, 30, 41],
    })
    antes = len(df)
    df["approved"] = df["score"] >= 15
    df["bucket"] = df["score"].map(bucket)
    agregado = df.groupby("bucket")["score"].agg(["count", "mean"]).round(1)
    print(df)
    print(agregado)
    print("linhas antes/depois da limpeza:", antes, len(df))

    assert df["approved"].tolist() == [False, True, True, True, True]
    contagem = agregado["count"].to_dict()
    assert contagem == {"baixo": 1, "medio": 2, "alto": 2}

    leve = df.astype({"score": "int32"})
    print("memoria score int64 vs int32 (bytes):",
          df["score"].dtype.itemsize * len(df), leve["score"].dtype.itemsize * len(df))
    assert leve["score"].dtype.itemsize == 4
    print("testes ok")


if __name__ == "__main__":
    main()
```

3. `astype` importa para memoria porque `dtype` e o preco por linha de cada coluna: um `int64` onde o valor cabe em `int32` dobra a coluna, e colunas de texto lidas como `object` custam ponteiros e objetos onde `category` custaria codigos. Numa tabela de milhoes de linhas, e a diferenca entre caber e nao caber.

4. Etapas saudaveis de um ETL: extrair sem transformar (o bruto fica), validar na fronteira (o que nao presta e contado e separado), transformar com funcoes puras testaveis, carregar com idempotencia (a [aula 26](26-solucoes.md) e o checkpoint), e contagens lidas/escritas/ignoradas publicadas entre etapas.

5. Evite carregar tudo quando o dado nao cabe confortavel na memoria, quando o consumo e incremental (logs que nunca acabam) ou quando so uma fatia e usada — nesses casos, leitura em pedacos (`chunksize`) ou uma ferramenta de fluxo/consulta resolve sem o `DataFrame` inteiro, como o pipeline do [projeto 32](../projetos/README.md) mede.

**Autoverificacao:** `approved` derivado da regra, agregacao por bucket com contagem conferida e a conta de memoria da troca de `dtype`.
