---
layout: default
---

# Pistas da aula 03a

1. Para a prova, escreva o MDC do par antes da primeira iteração. Depois substitua `(a,b)` por `(b,a%b)` e use a identidade da seção 3. No fim, descubra o valor de `b`.
2. Em `(987,986)`, a primeira subtração produz `(986,1)`; conte quantas subtrações de `1` restam. Em `(987,610)`, acompanhe quantas iterações cada versão precisa. O par que "trava" a subtração é o de números vizinhos.
3. Rode com as três sementes e compare as saídas. Se alguma semente falhasse, o número da semente diria exatamente como reproduzir a sequência. Pense no que aconteceria com `std::random_device` no lugar de `42`.
4. O invariante da soma é: antes da iteração com contador `i`, o acumulado vale `1+...+(i-1)`. Para quebrar a recursão, passe um `n` maior que o limite de quadros da sua pilha e observe o sinal de término (stack overflow, não exceção C++).
