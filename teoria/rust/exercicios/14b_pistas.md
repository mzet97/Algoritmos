---
layout: default
---

# Pistas da aula 14b

1. Em x86 (TSO forte), loads nao sao reordenados com loads nem stores com stores, entao o Relaxed costuma funcionar por acidente de hardware. O modelo Rust nao promete: em ARM o mesmo codigo pode ler DADO velho com PRONTO novo. O teste prova comportamento naquela maquina; a prova do programa e a ordenacao escrita.
2. Teto 0 (sync_channel(0)) e rendezvous: cada send espera um recv. 1024 folga o produtor ate encher. Meça mensagens/segundo nos tres.
3. recv_timeout devolve Timeout como erro distinguivel: o worker trata como "acordar e checar a flag", nao como falha.
4. O join final e uma aresta happens-before: tudo que a thread fez antes de terminar e visivel para quem da join.
