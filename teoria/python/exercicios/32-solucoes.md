---
layout: default
---

# Aula 32 (Python): solucoes comentadas

Compare apos tentar os [exercicios](../32.md#11-exercicios-obrigatorios). Extensao executada do [projeto 32](../projetos/32_pipeline/main.py) no ambiente dos projetos:

```python
import tempfile
from io import StringIO
from pathlib import Path

import pandas as pd

RAW_CSV = chr(10).join([
    "user_id,team,score",
    "1,blue,10",
    "2,red,18",
    "3,blue,25",
    "4,red,invalid",
    "5,blue,30",
    "",
])


def main() -> None:
    bruto = pd.read_csv(StringIO(RAW_CSV))
    linhas_brutas = len(bruto)
    df = bruto.copy()
    df["score"] = pd.to_numeric(df["score"], errors="coerce")
    invalidos = int(df["score"].isna().sum())
    df = df.dropna(subset=["score"])
    df["approved"] = df["score"] >= 20
    df["score_x2"] = df["score"] * 2

    agregado = (
        df.groupby("team", as_index=False)
        .agg(total_score=("score", "sum"), approved_count=("approved", "sum"))
        .sort_values("total_score", ascending=False)
    )
    print(agregado)
    print(f"linhas: brutas={linhas_brutas} validas={len(df)} invalidos_contados={invalidos}")

    with tempfile.TemporaryDirectory() as tmp:
        destino = Path(tmp) / "agregado.csv"
        agregado.to_csv(destino, index=False)
        relido = pd.read_csv(destino)
        print("csv exportado e relido:", relido["team"].tolist())
        assert relido["team"].tolist() == ["blue", "red"]
        topo = relido.iloc[0]
        assert topo["total_score"] == 65.0 and topo["approved_count"] == 2

    assert linhas_brutas == 5 and len(df) == 4 and invalidos == 1
    assert df["score_x2"].tolist() == [20.0, 36.0, 50.0, 60.0]
    print("testes ok")


if __name__ == "__main__":
    main()
```

4. `errors="coerce"` e util porque transforma "invalid" em `NaN` **contavel**: o pipeline acima sabe que exatamente 1 linha caiu e pode alarmar sobre ela. Sem coercao, a leitura morre na primeira linha ruim; com descarte silencioso, ela some da conta — coercao mais contagem e a terceira opcao, a honesta. E o mesmo `blue=65.0` que o verificador de projetos cobra do projeto 32.

5. Sinais de saida do Pandas puro: arquivo maior que a memoria confortavel, agregacao que o banco faria com indice sem carregar nada, ou pipeline que roda o dia inteiro em laco Python disfarçado de `apply`. A saida natural e consulta onde o dado mora (SQL/DuckDB) ou processamento em pedacos/distribuido — decidido pela medicao, nao pela escala imaginada.

**Autoverificacao:** derivada criada, CSV exportado e relido com o mesmo agregado, linhas contadas antes e depois e o invalido contado antes de cair.
