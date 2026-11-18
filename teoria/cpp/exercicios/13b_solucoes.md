---
layout: default
---

# Aula 13b: soluções comentadas

Compare após tentar os [exercícios](../13b_ffi_e_ameacas.md#8-exercícios-obrigatórios). Experimentos sobre `teoria/cpp/exemplos/13b_ffi.cpp` com `g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror` (e `clang++` onde indicado).

1. Previsão: retorna **0** (verificado: `criar(0)+guardar(25)=0`). `teto == 0 ? 1024 : teto` troca o zero por 1024 silenciosamente. O risco: cliente que pediu teto 0 querendo dizer "só leitura" ganha um armazém de 1024 — default generoso numa fronteira que cobra limite é permissão disfarçada. Alternativa honesta seria recusar teto 0 (`-1` no criar) ou documentar o default no contrato — o exemplo escolhe documentar; o exercício cobra notar o preço.

2. Diagnóstico: sem a guarda, `armazem_ler(nullptr, ...)` morre com **falha de segmentação (exit 139)** (verificado, sem sanitizer). `static_cast` não checa nada; a chamada acessa `this->itens` no endereço zero. Dentro de C++ puro, "ponteiro válido" é pré-condição documentada; na FFI, `void*` nulo é o valor mais comum que atravessa — de cliente descuidado, de `malloc` falhado, de handle zerado. A fronteira checa porque o sistema de tipos do C++ termina no `extern "C"`.

3. Projeto: a extensão espelha o padrão — `noexcept`, nulo primeiro, `try/catch`:

```cpp
int armazem_tamanho(Handle h) noexcept
{
    if (h == nullptr) {
        return -1;
    }
    try {
        return static_cast<int>(static_cast<Armazem*>(h)->itens.size());
    } catch (...) {
        return -3;
    }
}
```

Teste: armazém novo → 0; após dois `guardar` válidos → 2; `nullptr` → −1. (`itens` precisaria de acessor ou amizade — detalhe de encapsulamento que o exercício revela: fronteira nova mexe no interior, e é por isso que o padrão se copia em vez de se reinventar.)

4. Previsão: o ASan relata **`heap-use-after-free`** (verificado, primeira linha do relatório transcrita acima). O `delete` liberou o `vector`; o handle pendurado continua apontando para a arena. A fronteira recusa nulo porque comparar com zero é grátis; rastrear destruição exigiria registrar cada handle vivo — estado global com sincronização, outro protocolo inteiro. O contrato divide o dever: a fronteira nega o barato, o cliente zera o que destruiu (`h = nullptr` não é estilo, é a outra metade da invariante de dono único).
