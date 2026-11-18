---
layout: default
---

# Aula 29 (Rust): pistas graduais

Volte aos [exercicios](../29.md#exercícios-obrigatórios-teóricos-e-práticos) antes de abrir as solucoes.

1. Com `XDP_DROP` em `lo`, o ping morre na entrada do driver, antes da pilha IP — preveja o sintoma antes de testar.
2. XDP/tc vivem no caminho do pacote; Kprobe observa funcoes do kernel, nao pacotes.
3. Monitor de arquivos pede o ponto onde o kernel abre arquivos (syscalls), nao a placa de rede.
