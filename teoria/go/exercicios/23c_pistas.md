---
layout: default
---

# Aula 23c: pistas graduais

Volte aos [exercícios](../23c_quorum_e_reparo.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. Sem o `case` de igualdade, dois relógios idênticos caem no último `case` — e `!domina(a, a)` é sempre verdadeiro. A pergunta é quais dois relógios chegam iguais à fase 3: releia o que o `ler` da fase 2 escreve nas réplicas além de retornar. (Verificado: `panic: fase3: cura deveria convergir em 222`.)
2. A réplica 1 participou da escrita de `222`; a réplica 2, não. Quorum lê 2 de 3 — pelo menos uma delas viu a última escrita. O valor que volta é o da maioria de escrita, e a obsoleta sai curada do mesmo `ler`. (Verificado: `janela {1,2}: 222 true false`.)
3. Escreva cada valor com apenas a sua réplica viva (a escrita isolada retorna falso por falta de quorum, mas o valor fica gravado — o modelo não faz rollback), depois reúna e leia. Compare os dois relógios finais coordenada por coordenada: nenhum domina o outro. (Verificado: `conflito=true` com `[1,0,0]/A` contra `[0,1,0]/B`.)
4. Nenhuma réplica tem a chave, então nenhuma resposta entra na lista — e o quorum conta respostas, não réplicas vivas. Distinga `ok=false` (sem quorum) de `conflito=true` (quorum com divergência). (Verificado: `ok=false, conflito=false`.)
