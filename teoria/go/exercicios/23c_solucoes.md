---
layout: default
---

# Solucoes comentadas da aula 23c

1. O `ler` da fase 2, ao convergir em `[2,0,0]/222`, repara a réplica 1 (o laço de reparo escreve `melhor` em todas as respostas, inclusive nela). A fase 3 compara então `[2,0,0]` com `[2,0,0]`: sem o `case` de igualdade, `domina` falha nos dois sentidos e `!domina(melhor, e)` é verdadeiro — conflito falso, `panic` na cura. O `==` existe porque relógios iguais após reparo são o caso comum, não a exceção.
2. A janela {1, 2} devolve `222, ok=true, conflito=false`: a réplica 1 carrega `[2,0,0]/222` (pertenceu à maioria de escrita da fase 2) e qualquer par de réplicas intersecta essa maioria — é o `W + R > N` em ação. O mesmo `ler` cura a réplica 2 para `[2,0,0]/222`. `111` nunca volta porque nenhuma maioria o contém mais; leitura obsoleta exigiria ler só réplicas que perderam a escrita, impossível com `R = 2, N = 3`.
3. Com só a réplica 0 viva, `escrever` retorna falso (1 confirmação < quorum 2) — mas o valor `A` já ficou gravado localmente em `[1,0,0]`, pois o modelo simplificado não faz rollback; o mesmo vale para `B` em `[0,1,0]` na réplica 1. Reunidas, `[1,0,0]` e `[0,1,0]` são incomparáveis: `domina` falha nos dois sentidos de verdade, `conflito=true`. Diferença para o exercício 1: lá os relógios eram iguais (convergência real, conflito falso); aqui são concorrentes (divergência real, conflito verdadeiro).
4. `ler("ausente")` retorna `ok=false, conflito=false`: zero respostas carregam a chave, abaixo do quorum 2. Ausência não é valor obsoleto nem conflito — é falta de quorum, e o chamador deve tratar como "não sei", não como zero. O `conflito` só existe quando há quorum com versões incomparáveis.
