---
layout: default
---

# Aula 16: pistas graduais

Volte aos [exercicios](../16.md#15-exercicios-obrigatorios) antes de abrir as solucoes.

1. Troque `fopen/fread/fclose` por `open/read/close`: o descritor e `int`, e o fim de arquivo e `read` devolvendo 0.
2. Conte somando o retorno de cada `read` ate ele devolver 0; retorno negativo e erro, nao fim.
3. `fstat` sobre o descritor aberto preenche `struct stat`; o tamanho fica em `st_size` para arquivo regular.
4. Para o `mmap`: pense num arquivo grande com acessos esparsos e repetidos a trechos distantes, em vez de varredura unica.
5. Para a explicacao: meça a segunda leitura do mesmo arquivo e pergunte de onde os bytes vieram.
