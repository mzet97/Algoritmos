---
layout: default
---

# Soluções comentadas da aula 03a

## 1. Prova do invariante

- **Início:** antes do laço, o par é o das entradas; o MDC é trivialmente o das entradas.
- **Preservação:** com `b > 0`, `resto = a % b` e o novo par é `(b, resto)`. Pela identidade da seção 3, `mdc(a,b) == mdc(b,resto)`.
- **Término:** `b` diminui estritamente e nunca é negativo; ao sair, `b == 0` e `mdc(a,0) == a`. O caso `0,0` usa a convenção documentada.

## 2. Medição e explicação

`(987,986)`: a versão por módulo faz poucas iterações; a por subtração executa cerca de 987 subtrações (986 delas subtraindo `1`). `(987,610)`: ambas fazem mais iterações que o caso trivial, mas a diferença relativa é menor porque o módulo também trabalha. Conclusão: subtração repetida é `O(a/b)` iterações no pior caso para entradas positivas, contra `O(log min(a,b))` do módulo.

## 3. Sementes

As três sementes devem passar com a mesma saída de medição (a medição não depende da semente). A semente fixa importa porque `mt19937(42)` gera sempre a mesma sequência de 2000 pares: uma falha aponta para o par exato. Com `std::random_device`, cada execução testa pares diferentes e uma falha pode não se repetir — teste que falha às vezes e passa outras não serve como regressão.

## 4. Soma iterativa e quebra da recursão

```cpp
#include <cassert>

unsigned long long soma_ate(unsigned int n) {
    unsigned long long acumulado = 0u;  // invariante: soma de 1..(i-1)
    for (unsigned int i = 1u; i <= n; ++i) {
        acumulado += i;
    }
    return acumulado;
}
```

Com `n = 100000000u`, a recursão equivalente empilha cem milhões de quadros e o processo morre por estouro de pilha (sem exceção capturável em C++ padrão), enquanto a versão iterativa usa `O(1)` espaço. Note ainda o tipo de retorno: `unsigned long long` comporta a soma, mas a prova precisa dizer em que tipo vale — em `unsigned int` de 32 bits, a soma estouraria por volta de `n = 92682` por wrap modular, silencioso e legal, porém provavelmente não desejado.
