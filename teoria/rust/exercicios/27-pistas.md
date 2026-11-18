---
layout: default
---

# Aula 27 (Rust): pistas graduais

Volte aos [exercicios](../27.md#exercícios-obrigatórios) antes de abrir as solucoes.

1. O endereco `0x1000_0000` no QEMU `virt` e o UART0: ler/volatil e o registro de status/dados do hardware, nao memoria comum.
2. Volatil impede o compilador de eliminar/mesclar acessos — sem ele, o loop infinito otimiza a leitura para fora.
3. PAC e o mapa de registradores gerado do SVD; HAL e a API segura e idiomatica construida sobre ele.
