---
layout: default
---

# Pistas da aula 14a

1. As fontes são: `pthread_create`/`pthread_join`, `mutex_lock`/`mutex_unlock`, espera em condvar com o mutex, `pthread_once`. Para cada uma, escreva o que foi escrito antes e o que passa a ser visível depois.
2. Sem mutex, 8 threads somam no mesmo `long`: preveja menos que 800.000. O TSan denuncia as duas linhas em corrida — transcreva pilhas e endereços onde ele funcionar.
3. Com `if`, um acordo espúrio (permitido pelo padrão!) sai do `wait` com `pronto == 0` e lê `valor` ainda zero. O `while` recheca: quem garante não é o sinal, é o predicado.
4. O `signal` antes da espera acorda ninguém — mas `pronto` já vale 1. O consumidor checa o predicado antes de dormir: estado, não evento.
