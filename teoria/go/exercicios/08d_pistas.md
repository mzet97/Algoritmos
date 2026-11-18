---
layout: default
---

# Pistas da aula 08d

1. runtime.GC() forca a coleta: HeapAlloc cai, HeapSys tende a ficar (o runtime segura as spans). Confira os dois.
2. Ler arquivo de 32 MiB: quem se move e o page cache do SO e as faltas de leitura; RSS do processo cresce pouco porque a copia e por buffer.
3. Trabalhadores dormindo quase nao criam threads: o escalonador estaciona goroutines. Threads sobem com trabalho bloqueante de verdade (syscalls), nao com sleep de goroutine.
4. A ferramenta ve o proprio processo: paginas compartilhadas de bibliotecas contam inteiras no RSS de cada processo; smaps_rollup/PSS reparte. Essa e a historia que falta.
