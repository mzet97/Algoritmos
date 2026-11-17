# Kernel didatico RISC-V em Rust no_std: staticlib (core) + link gcc cruzado.
CROSS ?= riscv64-linux-gnu-

all: kernel.elf

libkernel.a: main.rs
	rustc --edition 2024 --target riscv64gc-unknown-none-elf -C panic=abort \
	      -C opt-level=2 --crate-type staticlib -o $@ $<

kernel.elf: boot.S trap.S libkernel.a link.ld
	$(CROSS)gcc -march=rv64gc -mabi=lp64d -nostdlib -nostartfiles -T link.ld \
	      boot.S trap.S libkernel.a -o $@

test: kernel.elf
	python3 test_kernel.py kernel.elf

clean:
	rm -f kernel.elf libkernel.a

.PHONY: all test clean
