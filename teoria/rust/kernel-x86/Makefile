# Componente comparativo x86-64 (trilha C++) — Multiboot1 via QEMU -kernel.
kernel-x86.elf: boot.S link.ld
	gcc -m32 -nostdlib -ffreestanding -Wall -Wextra -Werror -T link.ld boot.S -o $@

test: kernel-x86.elf
	python3 test_kernel_x86.py kernel-x86.elf

clean:
	rm -f kernel-x86.elf

.PHONY: test clean
