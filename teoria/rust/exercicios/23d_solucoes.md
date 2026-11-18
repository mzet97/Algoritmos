---
layout: default
---

# Aula 23d: soluções comentadas

Compare após tentar os [exercícios](../23d_quorum_e_reparo.md#7-exercícios-obrigatórios). Experimentos sobre `teoria/rust/exemplos/23d_consenso.rs` com `rustc --edition 2024 -O -D warnings`.

1. Previsão: `escrever` devolve **`false`** com quorum 3 e uma réplica morta (verificado: o `assert` da fase 2 cai). 2 acks < 3: a escrita saudável é vetada pela queda de um. O mesmo teorema das 23d em C/C++ — unanimidade trava na primeira falha — agora devolvido como `bool` em vez de abortar: em Rust o protocolo diz "não", e o chamador decide; em C didático, o `assert` decidia.

2. Diagnóstico: sem o `continue`, as fases 1–3 **passam** (verificado) — os relógios ainda diferem em todas (só o coordenador incrementa o próprio contador, então réplicas da mesma escrita têm relógios *diferentes*). A 4ª leitura, com todas idênticas após o reparo, declara **`Conflito` falso** (verificado). Sem o `==`, igualdade cai em "nem domina" dos dois lados (`domina` exige `>` em algo). A lição: o `==` não é otimização — é o que impede declarar guerra civil em província pacificada. Igualdade é convergência, não concorrência.

3. Projeto: "A" isolada em r0 ([1,0,0]) e "B" isolada em r1 ([0,1,0]) → **`Conflito`** no quorum (verificado). `[1,0,0]` vs `[0,1,0]`: `0 < 1` num índice de cada lado, ninguém domina. Concorrência real não tem "melhor" — e o reparo, que copia `melhor` para as atrasadas, corretamente não roda: não há melhor, há empate. Reparo converge atraso; conflito exige regra externa (a cura da 23c em C++ escolhe por soma — aqui, declarar é a resposta honesta).

4. Previsão: **`SemQuorum`** (verificado). Zero respostas < 2: "ninguém tem a chave" usa o mesmo caminho de "não consegui perguntar a bastantes". Ausência não é valor — e é por isso que `ler` tem três variantes em vez de `Option`: `Limpa` (há resposta convergente), `Conflito` (há discordância decidível por ninguém), `SemQuorum` (não há base). Cliente que trata `SemQuorum` como "vazio" confunde partição com deleção.
