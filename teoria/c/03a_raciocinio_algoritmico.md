---
layout: default
---

# Aula 3A — Invariantes, prova de correção e custo de algoritmos

## Objetivo e pré-requisitos

Depois desta aula, você deverá conseguir escrever a pré-condição e a pós-condição de uma função, justificar que um laço termina, encontrar uma propriedade que permanece verdadeira em cada iteração e estimar tempo e memória sem confundir medição com prova. Você precisa das [aulas 1 a 3](indice.md): variáveis, `while`, funções e operações inteiras. Ainda não precisa saber ponteiros ou alocação dinâmica.

Se você vem de Python ou JavaScript, já escreveu laços. O passo novo é **explicar por que eles sempre entregam o resultado prometido**. Essa explicação continua válida quando o programa muda de linguagem, compilador ou computador.

**Como ler esta aula (três níveis):** *fundamental* — pré/pós-condição e invariante do MDC; *intermediário* — término por grandeza que diminui, `O(...)` vs medição; *aprofundamento* — `assert` vs validação de entrada externa (exercícios).

## 1. Um contrato antes do código

Considere uma função que calcula o máximo divisor comum, ou **MDC**, de dois inteiros não negativos.

- **Pré-condição:** as entradas pertencem ao domínio prometido, aqui `unsigned int`.
- **Pós-condição:** o resultado divide as duas entradas e nenhum divisor comum positivo é maior que ele.
- **Caso especial:** matematicamente, `mdc(0, 0)` não tem valor positivo único. Nossa API escolhe retornar `0` para esse caso. A escolha deve ser documentada; ela não é um teorema.

Um teste mostra que a função funcionou para as entradas testadas. Um argumento de correção explica por que funciona para *todas* as entradas do contrato. Ambos são necessários: a prova pode partir de uma hipótese errada sobre o código real, e os testes não conseguem enumerar todas as entradas.

## 2. A propriedade que sobrevive ao laço

O algoritmo de Euclides usa a identidade:

```text
mdc(a, b) = mdc(b, a mod b), quando b > 0
```

Por quê? Se um número divide `a` e `b`, ele também divide `a - q*b`, inclusive o resto de `a` por `b`. No sentido inverso, se divide `b` e o resto, divide `q*b + resto = a`. Assim, os divisores comuns dos dois pares são os mesmos.

Essa igualdade é o **invariante do laço**: antes e depois de cada iteração, o MDC do par atual continua igual ao MDC das entradas originais. Não é suficiente escrever “o laço parece certo”; identifique o que permanece verdadeiro e demonstre o passo que preserva a propriedade.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/03a_mdc.c) contém exatamente o programa abaixo:

```c
#include <assert.h>
#include <stdio.h>

static unsigned int mdc(unsigned int a, unsigned int b) {
    while (b != 0U) {
        unsigned int resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}

int main(void) {
    assert(mdc(54U, 24U) == 6U);
    assert(mdc(0U, 24U) == 24U);
    assert(mdc(24U, 0U) == 24U);
    assert(mdc(0U, 0U) == 0U);

    printf("mdc(54, 24) = %u\n", mdc(54U, 24U));
    return 0;
}
```

Em Linux/macOS ou em um ambiente GCC compatível no Windows:

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Werror -O0 -g teoria/c/exemplos/03a_mdc.c -o mdc
./mdc
```

Saída esperada:

```text
mdc(54, 24) = 6
```

No PowerShell, o executável gerado normalmente é chamado como `./mdc.exe` ou `.\mdc.exe`, conforme o compilador usado. Execute a partir da raiz do repositório para que o caminho do arquivo-fonte acima funcione.

### O que a memória faz aqui?

`a`, `b` e `resto` são valores locais de tipo inteiro. A função não pede memória dinâmica e usa uma quantidade constante de espaço adicional. O compilador pode manter valores em registradores; não assuma que cada variável corresponde obrigatoriamente a uma posição física na stack. O modelo de variáveis locais é útil para raciocinar sobre tempo de vida, mas não determina sozinho o código de máquina.

## 4. Por que o algoritmo termina?

Em cada iteração com `b > 0`, a operação `a % b` produz um resto `r` tal que `0 <= r < b`. O próximo valor de `b` é `r`: um inteiro não negativo estritamente menor. Uma sequência infinita de inteiros não negativos que diminui estritamente não existe. Portanto, o laço termina.

Quando termina, `b == 0`. O invariante diz que o MDC do par atual ainda é o das entradas originais; `mdc(a, 0) == a`. Logo, retornar `a` satisfaz a pós-condição, exceto pelo caso `0,0`, que já recebeu uma convenção explícita.

Observe o papel de `b != 0U`: ele também impede divisão por zero na expressão `a % b`. A condição não é apenas uma otimização do laço.

## 5. Custo: prova, estimativa e medição

Cada iteração executa uma quantidade constante de operações no modelo usual de inteiros de máquina. O algoritmo de Euclides faz `O(log min(a,b))` iterações no pior caso para entradas positivas; pares consecutivos de Fibonacci aproximam esse pior caso. Usa `O(1)` espaço adicional. Essas afirmações descrevem crescimento assintótico, não segundos de execução. Para comparar implementações reais, também se medem custo de `%`, arquitetura, otimizações e tamanho de entrada.

Quando uma entrada é zero, o laço faz zero iterações. Não tente calcular `log 0`; trate esse caso separadamente no enunciado da análise.

O `assert` verifica exemplos durante o desenvolvimento, mas pode desaparecer se o programa for compilado com `NDEBUG`. Não o use para validar entrada não confiável, nem como única prova de correção. Esta aula usa constantes; uma versão que lê dados externos precisará verificar formato, faixa e tratamento de erro antes de chamar `mdc`.

## 6. Ponte com Python e JavaScript

Python também possui `%` e inteiros de precisão arbitrária; o `unsigned int` de C tem largura finita. JavaScript usa `Number` para a aritmética comum, e nem todo inteiro matemático grande é representado exatamente. A prova do algoritmo usa inteiros matemáticos não negativos; ao implementá-la, precisamos conferir se a representação escolhida respeita essa premissa. O exemplo em C recebe valores já representáveis e não faz somas que possam transbordar.

## 7. Erros comuns e implicações defensivas

1. **Chamar `%` com divisor zero:** o teste do laço protege essa operação.
2. **Aceitar um domínio maior que o implementado:** converter um número negativo para `unsigned` altera seu valor; uma API que aceite negativos precisa definir essa conversão ou rejeitá-los.
3. **Confundir casos de teste com prova:** um milhão de testes não demonstra sozinho a propriedade para todas as entradas.
4. **Usar `assert` como validação de entrada:** builds de produção podem removê-lo.
5. **Medir antes de verificar:** um programa rápido que entrega resposta errada não satisfaz o contrato.

## 8. Exercícios obrigatórios

1. Escreva as três partes da prova (início, preservação, término) do invariante do exemplo sem consultar a seção 4.
2. Implemente uma versão do MDC que usa subtrações repetidas apenas para entradas positivas. Compare o número de iterações em `mdc(987, 986)` e em `mdc(987, 610)`. Explique por que o primeiro par expõe uma diferença grande entre as versões e o segundo exige mais iterações do algoritmo por módulo.
3. Escreva uma função que soma `1 + 2 + ... + n` para um `n` pequeno e declare seu invariante. Depois explique por que a mesma implementação pode falhar para `n` grande em um tipo inteiro finito.
4. Acrescente testes para pares iguais, coprimos e múltiplos; documente o resultado escolhido para `0,0`.

Use [pistas graduais](exercicios/03a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/03a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- declarar pré-condição, pós-condição e convenção do caso especial;
- explicar por que o invariante é verdadeiro antes e depois de cada iteração;
- indicar uma grandeza que diminui e provar o término;
- compilar e executar o exemplo com warnings sem erro;
- distinguir análise `O(...)` de uma medição em segundos;
- explicar a diferença entre teste, `assert` e validação de entrada externa.

### Pesos da avaliacao

| Critério | Peso |
|---|---|
| Contrato escrito (pré/pós) + invariante enunciada | 30% |
| Término provado + custo estimado sem confundir com medição | 30% |
| Diagnóstico (prever invariante quebrada, explicar `assert` vs validação) | 25% |
| Documentação (exemplo compilado, análise escrita) | 15% |

**Referências primárias:** *Introduction to Algorithms* (Cormen et al.), Parte I, para invariantes e análise; [N1570, seção 7.2 (`assert`)](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf) para o contrato interno.

**Próxima aula:** arrays e ponteiros. O mesmo raciocínio por invariantes ajudará a manter índices dentro dos limites de um buffer.

---

### Para se aprofundar

1. Brian Kernighan e Dennis Ritchie, *C Programming Language* (Prentice Hall, 2. ed., ISBN 978-0131103627); 2. *C11 standard draft N1570* ([open-std.org](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)); 3. *cppreference.com - C* ([en.cppreference.com/w/c](https://en.cppreference.com/w/c)).
