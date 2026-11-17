#!/usr/bin/env bash
# Regression sweep da masterclass: compila e executa todos os exemplos
# completos de teoria/*/exemplos. Uso: bash teoria/regress.sh
# Sai 0 somente se todas as verificacoes passarem. Requer gcc/g++, rustc,
# go e python3 no PATH. Sanitizers ficam fora (compilacao separada por aula).
set -u
cd "$(dirname "$0")/.." || exit 2
OUT=$(mktemp -d /tmp/regress_XXXXXX)
trap 'rm -rf "$OUT"' EXIT
PASS=0
FAIL=0
FAILED=""

run() { # nome comando...
    local nome="$1"; shift
    if timeout 180 "$@" >"$OUT/log" 2>&1; then
        PASS=$((PASS + 1)); printf 'ok - %s\n' "$nome"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED $nome"
        printf 'FALHA - %s\n' "$nome"; sed 's/^/    /' "$OUT/log" | tail -20
    fi
}

CFLAGS="-std=c11 -Wall -Wextra -Wpedantic -Werror"
CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic -Werror"

for f in 03a_mdc 10a_memoria 13a_estruturas 13b_arvores 14a_threads 16a_processo 23b_fuzz 23c_relogios \
         28a_benchmark 30a_modelo_gpu aula01 aula04 aula07 aula14 aula23; do
    if gcc $CFLAGS -pthread "teoria/c/exemplos/$f.c" -o "$OUT/$f" 2>"$OUT/log"; then
        run "c/$f" "$OUT/$f"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED c/$f(compila)"
        printf 'FALHA(compila) - c/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done

gcc $CFLAGS -pthread teoria/c/exemplos/23a_distribuido.c -o "$OUT/c23a" 2>"$OUT/log" \
    && run "c/23a_distribuido" "$OUT/c23a" checar \
    || { FAIL=$((FAIL + 1)); FAILED="$FAILED c/23a(compila)"; printf 'FALHA(compila) - c/23a\n'; tail -20 "$OUT/log"; }

gcc $CFLAGS -O2 teoria/c/exemplos/30b_simd.c -o "$OUT/c30b" -lm 2>"$OUT/log" \
    && run "c/30b_simd" "$OUT/c30b" \
    || { FAIL=$((FAIL + 1)); FAILED="$FAILED c/30b(compila)"; printf 'FALHA(compila) - c/30b\n'; tail -20 "$OUT/log"; }

if make -C teoria/c/kernel kernel.elf >"$OUT/log" 2>&1; then
    run "c/kernel-riscv-qemu" python3 teoria/c/kernel/test_kernel.py teoria/c/kernel/kernel.elf
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED c/kernel(compila)"
    printf 'FALHA(compila) - c/kernel\n'; tail -20 "$OUT/log"
fi

gcc $CFLAGS -pthread teoria/c/exemplos/aula31.c -o "$OUT/aula31" 2>"$OUT/log" \
    && run "c/aula31+test" python3 teoria/c/exemplos/test_aula31.py "$OUT/aula31" \
    || { FAIL=$((FAIL + 1)); FAILED="$FAILED c/aula31(compila)"; printf 'FALHA(compila) - c/aula31\n'; tail -20 "$OUT/log"; }

for f in 03a_invariante 12a_sistema 17a_concorrencia 23b_distribuido; do
    if g++ $CXXFLAGS -pthread "teoria/cpp/exemplos/$f.cpp" -o "$OUT/$f" 2>"$OUT/log"; then
        run "cpp/$f" "$OUT/$f"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED cpp/$f(compila)"
        printf 'FALHA(compila) - cpp/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done
run "cpp/23a_endurecido+test" python3 teoria/cpp/exemplos/test_23a.py

for f in 08a_busca 08b_sistema 11a_concorrencia 23a_distribuido 28a_hpc; do
    if go vet "teoria/go/exemplos/$f.go" >"$OUT/log" 2>&1; then
        run "go/$f" go run -race "teoria/go/exemplos/$f.go"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED go/$f(vet)"
        printf 'FALHA(vet) - go/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done

for f in 12a_busca 14a_protocolo 16a_sistema 23a_hpc; do
    if rustc --edition 2024 -D warnings "teoria/rust/exemplos/$f.rs" -o "$OUT/$f" 2>"$OUT/log"; then
        run "rust/$f" "$OUT/$f"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED rust/$f(compila)"
        printf 'FALHA(compila) - rust/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done

for f in 04a_busca 14a_sistema 16a_concorrencia 29a_hpc; do
    run "python/$f" python3 "teoria/python/exemplos/$f.py"
done
run "python/23a_distribuido" python3 teoria/python/exemplos/23a_distribuido.py checar

echo "----------------------------------------"
echo "regress: $PASS passaram, $FAIL falharam"
[ "$FAIL" -eq 0 ] || echo "falhas:$FAILED"
[ "$FAIL" -eq 0 ]
