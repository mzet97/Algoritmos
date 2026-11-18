# Kernel didatico RISC-V — build e teste em QEMU (virt, -bios none).
CROSS ?= riscv64-linux-gnu-
CC := $(CROSS)gcc
CFLAGS := -march=rv64gc -mabi=lp64 -nostdlib -nostartfiles -ffreestanding \
          -fno-builtin -Wall -Wextra -Werror -O2 -mcmodel=medany

kernel.elf: boot.S trap.S kernel.c link.ld
	$(CC) $(CFLAGS) -T link.ld boot.S trap.S kernel.c -o $@

test: kernel.elf
	python3 test_kernel.py kernel.elf

clean:
	rm -f kernel.elf

.PHONY: test clean
