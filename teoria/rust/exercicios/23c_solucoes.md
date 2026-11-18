---
layout: default
---

# Solucoes comentadas da aula 23c

Kahan ganha em erro (perto do f64) e perde em velocidade: a compensacao serializa. 8 acumuladores saturam porque o gargalo migra da cadeia para a banda/throughput das unidades. O desenho first-touch exige 2 nos: inicializar onde vai computar e a politica inteira; sem o segundo no, qualquer numero seria teatro. No asm, instrucoes packed confirmam vetorizacao; sem elas e com razao 3.48x, o ganho e paralelismo de instrucao escalar — a aula aceita os dois mecanismos e nomeia o que encontrar.
