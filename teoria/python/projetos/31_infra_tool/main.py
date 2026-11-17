import argparse
import json
import subprocess
import sys
from pathlib import Path


CHECKPOINT_PATH = Path("infra_checkpoint.json")


def load_checkpoint() -> set[str]:
    if not CHECKPOINT_PATH.exists():
        return set()
    data = json.loads(CHECKPOINT_PATH.read_text(encoding="utf-8"))
    return set(data.get("processed", []))


def save_checkpoint(processed: set[str]) -> None:
    CHECKPOINT_PATH.write_text(
        json.dumps({"processed": sorted(processed)}),
        encoding="utf-8",
    )


def run_probe(path: Path, timeout_s: float) -> int:
    # O processo filho calcula o SHA-256 do arquivo recebido, em streaming.
    code = (
        "import hashlib, sys\n"
        "with open(sys.argv[1], 'rb') as source:\n"
        "    print(hashlib.file_digest(source, 'sha256').hexdigest())\n"
    )
    try:
        completed = subprocess.run(
            [sys.executable, "-c", code, str(path)],
            capture_output=True,
            text=True,
            timeout=timeout_s,
            check=False,
        )
    except subprocess.TimeoutExpired:
        print(f"[error] timeout: {path}", file=sys.stderr)
        return 1
    except OSError as exc:
        print(f"[error] nao foi possivel iniciar subprocesso: {exc}", file=sys.stderr)
        return 1
    if completed.returncode != 0:
        print(completed.stderr.strip(), file=sys.stderr)
    else:
        print(f"sha256={completed.stdout.strip()} file={path}")
    return completed.returncode


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="infra-tool")
    parser.add_argument("paths", nargs="+")
    parser.add_argument("--timeout", type=float, default=2.0)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    if args.timeout <= 0:
        parser.error("--timeout deve ser positivo")
    processed = load_checkpoint()
    failures = 0

    for raw_path in args.paths:
        path = Path(raw_path)

        if raw_path in processed:
            print(f"[skip] already processed {raw_path}")
            continue

        if not path.is_file():
            print(f"[error] arquivo ausente ou nao regular: {raw_path}", file=sys.stderr)
            failures += 1
            continue

        code = run_probe(path, timeout_s=args.timeout)
        if code != 0:
            print(f"[error] probe failed for {raw_path}", file=sys.stderr)
            failures += 1
            continue

        processed.add(raw_path)
        save_checkpoint(processed)
        print(f"[ok] processed {raw_path}")

    return 0 if failures == 0 else 2


if __name__ == "__main__":
    raise SystemExit(main())
