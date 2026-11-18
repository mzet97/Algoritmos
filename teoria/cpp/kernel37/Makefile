# Kernel didatico RISC-V em C++ freestanding (sem excecoes/RTTI/libc).
CROSS ?= riscv64-linux-gnu-
CXX := $(CROSS)g++
CXXFLAGS := -march=rv64gc -mabi=lp64 -nostdlib -nostartfiles -ffreestanding \
            -fno-builtin -fno-exceptions -fno-rtti -Wall -Wextra -Werror -O2 \
            -mcmodel=medany -std=c++20
ASFLAGS := -march=rv64gc -mabi=lp64

kernel.elf: boot.S trap.S kernel.cpp link.ld
	$(CXX) $(CXXFLAGS) -T link.ld boot.S trap.S kernel.cpp -o $@

test: kernel.elf
	python3 test_kernel.py kernel.elf

clean:
	rm -f kernel.elf

.PHONY: test clean
