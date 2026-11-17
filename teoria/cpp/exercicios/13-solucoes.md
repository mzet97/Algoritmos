---
layout: default
---

# Aula 13 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../13.md#16-exercicios-obrigatorios). Roteiros executados nesta rodada (g++ 16, gdb 17.2, valgrind 3.27) sobre este programa com defeitos plantados:

```cpp
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v{10, 20, 30};
    int soma = v.data()[0] + v.data()[3]; // fora de faixa (o mesmo que v[3])
    std::cout << "soma=" << soma << '\n';
    int* vazado = new int{42};
    std::cout << "vazado=" << *vazado << '\n';
    return 0; // sem delete: leak
}
```

1 e 2. Sanitizer e valgrind nomeiam defeitos diferentes do mesmo programa:

```bash
g++ -std=c++20 -g -fsanitize=address,undefined -o defeito defeito.cpp
./defeito
# AddressSanitizer: heap-buffer-overflow ... READ of size 4

g++ -std=c++20 -g -o defeito_puro defeito.cpp
valgrind --leak-check=full ./defeito_puro
# LEAK SUMMARY: definitely lost: 4 bytes in 1 blocks
```

Nota desta maquina: com `v[3]`, a libstdc++ desta instalacao aborta antes, com a assercao `__n < this->size()` de `operator[]` — uma STL com checagens tambem e um instrumento, de outra camada. O acesso via `v.data()[3]` pula essa checagem e expoe o endereco invalido ao ASan, como no relatorio acima. Sem nenhum dos dois, o programa poderia imprimir uma soma qualquer e terminar "bem": e exatamente por isso que o defeito precisa de instrumento, nao de inspecao da saida.

3. Sessao gdb minima:

```bash
g++ -std=c++20 -g -Og -o depura defeito.cpp
gdb -batch -ex 'break main' -ex run -ex 'info locals' -ex next ./depura
```

Observado: `Breakpoint 1, main () at defeito.cpp:5` e a execucao passo a passo sob seu controle. O gdb mostra onde voce esta e o estado; ele nao declara o acesso invalido — quem declara e o ASan. A divisao de trabalho completa esta na [aula 11 de C](../../c/exercicios/11-solucoes.md).

4. **Crash** e um sintoma observavel (o processo morreu). **UB** e a causa contratual: o padrao nao define o resultado, e crash e apenas um dos desfechos possiveis — o mesmo codigo pode "funcionar" com outro compilador ou nivel de otimizacao. **Leak** e diferente dos dois: o programa esta dentro do contrato, so nunca devolve um recurso; o custo aparece em processos longos, nao na corretude da saida.

5. Dangling pointer aponta para um objeto cujo tempo de vida acabou. Ele e perigoso porque a memoria antiga frequentemente **ainda contem** os bytes esperados ate a proxima alocacao reutiliza-la: o programa parece funcionar em todos os testes curtos e falha sob carga, ordem de alocacao diferente ou outro alocador. Nenhuma leitura bem-sucedida prova que o ponteiro estava valido.

**Autoverificacao:** um relatorio ASan, um `definitely lost` do valgrind e uma sessao gdb registrados; na sua explicacao, crash nao e sinonimo de UB.
