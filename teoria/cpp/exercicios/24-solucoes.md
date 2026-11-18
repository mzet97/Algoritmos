---
layout: default
---

# Aula 24 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../24.md#21-exercicios-obrigatorios).

1 e 2. Parsing por request sobre arena monotonica:

```cpp
#include <array>
#include <cstddef>
#include <iostream>
#include <memory_resource>
#include <string>
#include <vector>

int main()
{
    std::array<std::byte, 4096> buffer{};
    std::pmr::monotonic_buffer_resource arena{
        buffer.data(), buffer.size(), std::pmr::null_memory_resource()};

    std::pmr::vector<std::pmr::string> campos{&arena};
    campos.emplace_back("GET");
    campos.emplace_back("/indice");
    campos.emplace_back("HTTP/1.1");
    std::cout << "campos: " << campos.size() << ' ' << campos[1] << '\n';
    std::cout << "resource vivo durante o uso: sim\n";
    // 'arena' e 'buffer' morrem aqui, juntos, no fim do "request".
    return 0;
}
```

`null_memory_resource` como upstream transforma estouro do buffer em excecao `bad_alloc` em vez de cair silenciosamente para o heap — um limite de laboratorio declarado, nao um detalhe. Vetores e strings `pmr` alocam **do resource passado**, nao do alocador global.

2. `monotonic_buffer_resource` combina com parsing por request porque o padrao de vida e exatamente o da arena de C da [aula 25](../../c/exercicios/25-solucoes.md): milhares de alocacoes pequenas durante a fase, zero liberacoes individuais e um descarte unico no fim. Alocar vira avancar um ponteiro.

3. Pool resource (`synchronized_pool_resource`/`unsynchronized_pool_resource`) faz mais sentido quando os blocos tem tamanhos parecidos, sao devolvidos em ordem qualquer e **reutilizados** muitas vezes — por exemplo, nos de mensagens de um servidor de vida longa. Arena monotonica nesse papel so cresce.

4. Um objeto `pmr` nao carrega o resource consigo como dono: ele guarda um **ponteiro** para ele. Devolver um `pmr::string` alimentado por um resource local e o use-after-free da [aula 13](13-solucoes.md) com roupa nova: o resource morre, a memoria volta a ser usada e a string aponta para o meio de outra coisa.

5. Antes de introduzir pmr: a medicao mostra alocacao entre os gargalos? Cada resource tem dono e tempo de vida documentados, maiores que todos os objetos que alimenta? O ganho esperado paga a contaminacao dos tipos (`pmr::` em todas as assinaturas)? Sem as tres respostas, `std::vector` comum e a escolha.

**Autoverificacao:** campos lidos antes do fim do escopo, upstream declarado e um cenario seu em que pool vence arena (e outro em que perde).
