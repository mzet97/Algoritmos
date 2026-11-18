---
layout: default
---

# Pistas da aula 3A

1. Para a prova, escreva o MDC do par antes da primeira iteração. Depois substitua `(a,b)` por `(b,a%b)` e use a identidade apresentada na aula. No fim, descubra o valor de `b`.
2. Na versão por subtração, enquanto `a != b`, subtraia o menor do maior. Conte uma iteração a cada subtração. Restrinja o contrato a entradas positivas; zero exige regra adicional. Em `987,986`, observe quantas vezes o valor `1` precisa ser subtraído após a primeira iteração.
3. Antes de executar a iteração cujo contador vale `i`, qual deve ser o valor acumulado? Escreva a soma até `i-1`. Para o risco de overflow, compare o maior resultado possível ao limite do tipo escolhido antes de somar.
4. Pares iguais retornam o próprio valor. Pares coprimos retornam 1. Para múltiplos, o menor valor positivo é o MDC. O par `0,0` precisa da convenção da API.
