OUTPUT_ARCH(riscv)
ENTRY(_start)
MEMORY { RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M }
SECTIONS {
  . = ORIGIN(RAM);
  .text : { *(.text.start) *(.text*) } > RAM
  .rodata : { *(.rodata*) } > RAM
  .data : { *(.data*) } > RAM
  .bss : {
    _bss_ini = .;
    *(.bss*) *(COMMON)
    . = ALIGN(16);
    _pilhas = .;
    . += 2 * 16384;
    _pilha_trap0 = .;
    . += 16384;
    _bss_fim = .;
  } > RAM
}
