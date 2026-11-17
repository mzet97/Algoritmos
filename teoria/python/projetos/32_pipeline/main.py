from io import StringIO
import time
import tracemalloc

import pandas as pd


RAW_CSV = """user_id,team,score
1,blue,10
2,red,18
3,blue,25
4,red,invalid
5,blue,30
"""


def load_frame() -> pd.DataFrame:
    return pd.read_csv(StringIO(RAW_CSV))


def validate_schema(df: pd.DataFrame) -> None:
    required = {"user_id", "team", "score"}
    missing = required - set(df.columns)
    if missing:
        raise ValueError(f"missing columns: {sorted(missing)}")


def transform(df: pd.DataFrame) -> pd.DataFrame:
    df = df.copy()
    df["score"] = pd.to_numeric(df["score"], errors="coerce")
    df = df.dropna(subset=["score"])
    # Nao force int32 sem verificar faixa e integralidade em dados externos.
    df["approved"] = df["score"] >= 20
    return df


def aggregate(df: pd.DataFrame) -> pd.DataFrame:
    return (
        df.groupby("team", as_index=False)
        .agg(
            total_score=("score", "sum"),
            avg_score=("score", "mean"),
            approved_count=("approved", "sum"),
        )
        .sort_values("total_score", ascending=False)
    )


def main() -> None:
    tracemalloc.start()
    start = time.perf_counter()

    df = load_frame()
    validate_schema(df)
    clean = transform(df)
    result = aggregate(clean)

    elapsed_ms = (time.perf_counter() - start) * 1000.0
    current, peak = tracemalloc.get_traced_memory()

    print(clean)
    print(result)
    print(f"elapsed_ms={elapsed_ms:.2f}")
    print(f"current_traced_python_bytes={current}")
    print(f"peak_traced_python_bytes={peak}")


if __name__ == "__main__":
    main()
