#!/usr/bin/env python3
"""Sobe o componente x86-64 no QEMU e exige os marcadores da transicao.

Uso: python3 test_kernel_x86.py CAMINHO_DO_ELF
O desligamento e pelo isa-debug-exit (porta 0xF4, valor 0x10): o QEMU sai
com (0x10 << 1) | 1 = 33, que aqui e o codigo de SUCESSO contratado.
"""
import subprocess
import sys

MARCADORES = ["X86_BOOT_OK", "X86_TRAP_OK", "X86_DONE"]
EXIT_SUCESSO = 33


def main() -> int:
    if len(sys.argv) != 2:
        print("uso: test_kernel_x86.py CAMINHO_DO_ELF", file=sys.stderr)
        return 2
    cmd = ["qemu-system-x86_64", "-machine", "q35", "-m", "64",
           "-kernel", sys.argv[1], "-serial", "stdio", "-display", "none",
           "-device", "isa-debug-exit,iobase=0xF4,iosize=0x04"]
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
    except subprocess.TimeoutExpired:
        print("FALHA: timeout (sem isa-debug-exit)")
        return 1
    saida = r.stdout + r.stderr
    faltando = [m for m in MARCADORES if m not in saida]
    if faltando or r.returncode != EXIT_SUCESSO:
        print(f"FALHA: exit={r.returncode} (esperado {EXIT_SUCESSO}) "
              f"faltando={faltando}\n{saida[-2000:]}")
        return 1
    print("KERNEL_X86_OK marcadores=" + ",".join(MARCADORES))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
