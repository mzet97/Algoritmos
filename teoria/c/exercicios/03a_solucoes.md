---
layout: default
---

# Soluções comentadas da aula 3A

Leia depois de escrever e testar sua própria solução.

## 1. Prova do algoritmo de Euclides

**Início:** o par atual é exatamente o par de entrada, então tem o mesmo MDC. **Preservação:** quando `b > 0`, os divisores comuns de `(a,b)` e `(b,a%b)` coincidem, logo a atribuição preserva o MDC. **Término:** o novo `b` é um resto menor que o `b` anterior e não negativo; o laço acaba em `b == 0`. Nesse ponto, o MDC é `a` para entradas não ambas nulas. Para `(0,0)`, a API retorna 0 por convenção.

## 2. Versão por subtração

```c
/* Pré-condição: a > 0 e b > 0. */
unsigned int mdc_subtracao(unsigned int a, unsigned int b) {
    while (a != b) {
        if (a > b) {
            a -= b;
        } else {
            b -= a;
        }
    }
    return a;
}
```

O invariante é o mesmo: subtrair um dos números do outro não altera os divisores comuns. A grandeza matemática `a+b` diminui estritamente em cada passo para entradas positivas, provando término. Para `987,986`, a versão por subtração faz uma subtração para obter `(1,986)` e mais 985 para chegar a `(1,1)`; a versão por módulo faz duas iterações. Para `987,610`, o algoritmo por módulo percorre uma sequência próxima do seu pior caso, ligada a números de Fibonacci, e a diferença fica menor. Compare contadores no seu programa. O trecho acima é uma função, não um programa completo.

## 3. Soma e overflow

No início da iteração `i`, a variável `soma` deve ser `1 + ... + (i-1)`. Depois de adicionar `i`, passa a ser `1 + ... + i`, preservando o invariante para a iteração seguinte. A pós-condição é obtida quando o contador ultrapassa `n`. Antes de implementar a soma para valores grandes, verifique se `n(n+1)/2` cabe no tipo escolhido **sem calcular esse produto no mesmo tipo se ele puder transbordar**. A análise matemática não remove o limite físico da representação.

## 4. Classes de teste

Casos: `mdc(7,7)=7`, `mdc(8,15)=1`, `mdc(6,18)=6`, `mdc(0,9)=9`, `mdc(9,0)=9` e `mdc(0,0)=0` pela convenção publicada. Testar essas classes aumenta confiança; a prova da questão 1 cobre o domínio inteiro sob o contrato.
