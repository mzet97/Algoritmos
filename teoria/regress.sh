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

if make -C teoria/c/kernel-x86 kernel-x86.elf >"$OUT/log" 2>&1; then
    run "c/kernel-x86-qemu" python3 teoria/c/kernel-x86/test_kernel_x86.py teoria/c/kernel-x86/kernel-x86.elf
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED c/kernel-x86(compila)"
    printf 'FALHA(compila) - c/kernel-x86\n'; tail -20 "$OUT/log"
fi

gcc $CFLAGS -pthread teoria/c/exemplos/aula31.c -o "$OUT/aula31" 2>"$OUT/log" \
    && run "c/aula31+test" python3 teoria/c/exemplos/test_aula31.py "$OUT/aula31" \
    || { FAIL=$((FAIL + 1)); FAILED="$FAILED c/aula31(compila)"; printf 'FALHA(compila) - c/aula31\n'; tail -20 "$OUT/log"; }

for f in 03a_invariante 08a_algoritmos 12a_sistema 17a_concorrencia 23b_distribuido 30a_hpc 30b_gpu_modelo; do
    if g++ $CXXFLAGS -O2 -pthread "teoria/cpp/exemplos/$f.cpp" -o "$OUT/$f" 2>"$OUT/log"; then
        run "cpp/$f" "$OUT/$f"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED cpp/$f(compila)"
        printf 'FALHA(compila) - cpp/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done
if make -C teoria/cpp/kernel kernel.elf >"$OUT/log" 2>&1; then
    run "cpp/kernel-riscv-qemu" python3 teoria/cpp/kernel/test_kernel.py teoria/cpp/kernel/kernel.elf
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED cpp/kernel(compila)"
    printf 'FALHA(compila) - cpp/kernel\n'; tail -20 "$OUT/log"
fi
if make -C teoria/cpp/kernel-x86 kernel-x86.elf >"$OUT/log" 2>&1; then
    run "cpp/kernel-x86-qemu" python3 teoria/cpp/kernel-x86/test_kernel_x86.py teoria/cpp/kernel-x86/kernel-x86.elf
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED cpp/kernel-x86(compila)"
    printf 'FALHA(compila) - cpp/kernel-x86\n'; tail -20 "$OUT/log"
fi
for k in 34 35 36 37 38 39; do
if make -C "teoria/cpp/kernel$k" kernel.elf >"$OUT/log" 2>&1; then
    run "cpp/kernel$k-qemu" python3 "teoria/cpp/kernel$k/test_kernel.py" "teoria/cpp/kernel$k/kernel.elf"
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED cpp/kernel$k(compila)"
    printf 'FALHA(compila) - cpp/kernel%s\n' "$k"; tail -20 "$OUT/log"
fi
done
run "cpp/23a_endurecido+test" python3 teoria/cpp/exemplos/test_23a.py
run "cpp/13a_seguranca+test" python3 teoria/cpp/exemplos/test_13a.py

for f in 08a_busca 08b_sistema 08c_algoritmos 08d_ferramenta 11a_concorrencia 23a_distribuido 23b_relogios 28a_hpc 28b_vetorizacao; do
    if go vet "teoria/go/exemplos/$f.go" >"$OUT/log" 2>&1; then
        run "go/$f" go run -race "teoria/go/exemplos/$f.go"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED go/$f(vet)"
        printf 'FALHA(vet) - go/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done

if make -C teoria/rust/kernel kernel.elf >"$OUT/log" 2>&1; then
    run "rust/kernel-riscv-qemu" python3 teoria/rust/kernel/test_kernel.py teoria/rust/kernel/kernel.elf
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED rust/kernel(compila)"
    printf 'FALHA(compila) - rust/kernel\n'; tail -20 "$OUT/log"
fi
if make -C teoria/rust/kernel-x86 kernel-x86.elf >"$OUT/log" 2>&1; then
    run "rust/kernel-x86-qemu" python3 teoria/rust/kernel-x86/test_kernel_x86.py teoria/rust/kernel-x86/kernel-x86.elf
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED rust/kernel-x86(compila)"
    printf 'FALHA(compila) - rust/kernel-x86\n'; tail -20 "$OUT/log"
fi

for f in 12a_busca 12b_estruturas 14a_protocolo 14b_ordem 16a_sistema 16b_cache 23a_hpc 23b_distribuido 23c_vetorizacao; do
    if rustc --edition 2024 -O -D warnings "teoria/rust/exemplos/$f.rs" -o "$OUT/$f" 2>"$OUT/log"; then
        run "rust/$f" "$OUT/$f"
    else
        FAIL=$((FAIL + 1)); FAILED="$FAILED rust/$f(compila)"
        printf 'FALHA(compila) - rust/%s\n' "$f"; tail -20 "$OUT/log"
    fi
done

for f in 04a_busca 04b_arvores 14a_sistema 14b_cache 16a_concorrencia 16b_deadlock 19a_protocolo 23b_relogios 29a_hpc 29b_seguranca 29c_localidade; do
    run "python/$f" python3 "teoria/python/exemplos/$f.py"
done
run "python/23a_distribuido" python3 teoria/python/exemplos/23a_distribuido.py checar

if timeout 900 python3 teoria/verificar_projetos.py >"$OUT/log" 2>&1; then
    PASS=$((PASS + 1)); printf 'ok - projetos/rust30-32+python30-32\n'
else
    FAIL=$((FAIL + 1)); FAILED="$FAILED projetos"
    printf 'FALHA - projetos/rust30-32+python30-32\n'; tail -20 "$OUT/log"
fi

run "transversal/consistencia" python3 teoria/verificar_consistencia.py
run "fontes-unicas/c" python3 teoria/verificar_fontes_unicas.py c
run "fontes-unicas/cpp" python3 teoria/verificar_fontes_unicas.py cpp
run "fontes-unicas/rust" python3 teoria/verificar_fontes_unicas.py rust

echo "----------------------------------------"
echo "regress: $PASS passaram, $FAIL falharam"
[ "$FAIL" -eq 0 ] || echo "falhas:$FAILED"
[ "$FAIL" -eq 0 ]
