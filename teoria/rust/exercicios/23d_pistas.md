---
layout: default
---

# Aula 23d: pistas graduais

Volte aos [exercícios](../23d_quorum_e_reparo.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Com quorum 3 e uma réplica morta, `escrever` soma 2 acks e devolve `false` (verificado: o `assert` da fase 2 cai). "Todos" pune a escrita saudável pela queda de um — o mesmo teorema das 23d em C/C++, agora devolvido como `bool` em vez de abortar o processo.
2. Sem o `continue`, as fases 1–3 passam (os relógios ainda diferem — só o coordenador incrementa o próprio contador). A 4ª leitura, com todas idênticas pós-reparo, declara `Conflito` falso (verificado). Igualdade não é "nem domina": é "já converge". Sem o `==`, o teste declara guerra civil em província pacificada.
3. A isolada em r0 ([1,0,0]/A) e B isolada em r1 ([0,1,0]/B): o quorum lê as duas, nenhuma domina (`0 < 1` de um lado, `0 < 1` do outro) → `Conflito` (verificado). Concorrência real não tem "melhor" — e o reparo, que copia o melhor, corretamente não roda: reparar conflito seria eleger vencedor sem regra.
4. Chave nunca escrita dá `SemQuorum` (verificado). Zero respostas < 2: "ninguém tem" cai no mesmo balde de "não deu para perguntar a bastantes". Ausência não é valor — e é por isso que `ler` tem três variantes: `Limpa` (há resposta), `Conflito` (há discordância), `SemQuorum` (não há base para decidir).
