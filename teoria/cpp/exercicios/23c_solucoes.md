---
layout: default
---

# Aula 23c: soluções comentadas

Compare após tentar os [exercícios](../23c.md#7-exercícios-obrigatórios). Experimentos sobre `teoria/cpp/exemplos/23c_relogios.cpp` com `g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: não quebra. O retorno antecipado só distingue tamanhos — e tamanho de token não é segredo (viaja no cabeçalho de todo pacote). A propriedade "não vazar *qual* byte difere" vale porque, com tamanhos iguais, o laço sempre percorre tudo e acumula em `diff`. Tempo constante no conteúdo, variável no tamanho: a distinção é o ponto, não a falha.

2. Diagnóstico: a fase 1 **cai** no `assert(!l1.ausente ...)` (verificado). Com `QUORUM = 3` e `replicas[2]` isolada, só 2 respostas chegam — `respostas < QUORUM` devolve `Leitura{}` (ausente). Uma réplica caída veta toda leitura: quorum total na leitura trava como unanimidade na escrita. É o mesmo teorema da 23d, do outro lado do protocolo.

3. Projeto: com a regra "vence `replicas[2]`", a fase 4 converge para **k=333** (verificado) em vez de 222 — sem conflito, `VFS_OK` da cura. Nenhum vetor preferia 333; a regra preferiu. Detecção (vetor) e decisão (regra) são peças separadas: o vetor diz "há conflito", a regra publicada diz "e fica este". Trocar a regra troca a história sem quebrar o protocolo — e é por isso que a regra precisa ser publicada antes da partição, não inventada na cura.

4. Previsão: **conflito** (verificado: `ausente=0 conflito=1`). Três escritas isoladas produzem `(1,0,0)`, `(0,1,0)`, `(0,0,1)` — incomparáveis dois a dois, nenhuma domina. O quorum tem 3 respostas (quorum ok) mas declara em vez de servir: `valor=111` vem acompanhado de `conflito=1`, e cliente honesto trata o par, não o valor. Primário sozinho serviria 111 calado; quorum com vetor expõe a concorrência em vez de escondê-la.
