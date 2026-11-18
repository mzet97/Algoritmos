#!/usr/bin/env python3
"""Sobe o estagio 38 (E-S e FS minimo) no QEMU RISC-V e exige os marcadores.

Uso: python3 test_kernel.py CAMINHO_DO_KERNEL_ELF
Sucesso: imprime KERNEL38_OK e sai 0.
"""
import subprocess
import sys

MARCADORES = ["BOOT_OK", "FS_OK", "DONE"]


def main() -> int:
    if len(sys.argv) != 2:
        print("uso: test_kernel.py CAMINHO_DO_KERNEL_ELF", file=sys.stderr)
        return 2
    cmd = ["qemu-system-riscv64", "-machine", "virt", "-cpu", "rv64",
           "-smp", "1", "-bios", "none", "-kernel", sys.argv[1], "-nographic"]
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
    except subprocess.TimeoutExpired:
        print("FALHA: timeout (kernel nao desligou pelo test device)")
        return 1
    saida = r.stdout + r.stderr
    faltando = [m for m in MARCADORES if m not in saida]
    if r.returncode != 0:
        print(f"FALHA: qemu saiu {r.returncode}\n{saida[-2000:]}")
        return 1
    if faltando:
        print(f"FALHA: marcadores ausentes: {faltando}\n{saida[-2000:]}")
        return 1
    print("KERNEL38_OK marcadores=" + ",".join(MARCADORES))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
