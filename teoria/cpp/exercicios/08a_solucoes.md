# Soluções comentadas da aula 08a

## 1. Invariante e o `size_t`

Invariante: "se o alvo está no vetor, está em `v[ini..fim)`". Início: o vetor inteiro. Preservação: se `alvo < v[meio]`, ele não está em `[meio, fim)` pela ordenação, logo segue em `[ini, meio)`; simétrico no outro ramo. Término: `fim - ini` diminui estritamente a cada iteração. O ponto do sem-sinal: a prova exige `ini <= fim` sempre; os ramos usados (`fim = meio`, `ini = meio + 1`) mantêm isso porque `ini <= meio < fim`. Um ramo `fim = meio - 1` quebraria com `meio == 0` — em `size_t`, `0 - 1` é o maior valor representável, e a "fatia" viraria o vetor inteiro mais o infinito.

## 2. Estabilidade

Com registros `(chave, seq)` ordenados só por chave: inserção e mergesort mantêm `seq` crescente dentro de cada chave (estáveis); `std::sort` não promete e, em entradas maiores, de fato embaralha iguais. A correção não é "consertar" o `std::sort`, é escolher `std::stable_sort` (ou o mergesort) quando o contrato inclui a ordem original — e escrever o teste que distingue, porque com inteiros puros o defeito é invisível.

## 3. O cruzamento em n pequeno

Abaixo de algumas dezenas de elementos, a inserção vence: laço curto, sem recursão, sem buffer, e os dados cabem numa linha de cache. É por isso que `std::sort` (introsort) termina trechos pequenos com inserção. O número exato do cruzamento depende da máquina; o fenômeno, não — meça na sua e registre o ambiente junto.

## 4. Pré-condição é contrato, não cortesia

No vetor `{2, 1}` com alvo 2: `meio` aponta para um dos dois, o ramo descarta a metade que contém o 2, e a busca devolve `-1` para um elemento presente. A função não pode verificar a ordenação a cada chamada sem transformar busca `O(log n)` em `O(n)`. Logo: a pré-condição fica escrita no contrato, quem chama garante (ordenou antes, como faz o teste), e uso em entrada não confiável pede ordenação ou outra estrutura — a decisão é de quem conhece a origem dos dados.
