---
layout: default
---

# Aula 33 — Projeto integrador de Python: plataforma de experimentos distribuídos

**Competências SDD:** todas (PY10). **Pré-requisitos:** PY1–PY9, com as pontes [19a](19a_protocolos_tcp.md), [23a](23a_sistemas_distribuidos.md)/[23b](23b_relogios_e_particao.md), [29b](29b_seguranca_defensiva.md) e [29c](29c_localidade_e_amdahl.md) executadas.

**Objetivo observável:** entregar uma plataforma local e reproduzível que junta as peças do livro: (1) o serviço replicado de dois nós da 23a com a política de consistência da 23b escrita em uma frase e demonstrada; (2) uma CLI de observação do SO (processos filhos, RSS por `/proc`, tempo e estado/erro, na linha da 14a) apontada para o serviço; (3) uma carga numérica da 29c com baseline e Amdahl; (4) injeção de falhas (atraso, duplicação, queda com kill, reinício) com esperado × observado.

## Contrato de entrega

- **Ambiente limpo:** `bash teoria/regress.sh` verde cobre as peças sem passo manual; o README da entrega lista qualquer comando adicional exatamente como deve ser digitado.
- **Consistência em uma frase:** a política do serviço (eventual, ordem total publicada, perda declarada) aparece no README e é a mesma demonstrada no roteiro — frase diferente da demonstração é defeito de documentação.
- **Fronteira de segurança:** entrada só por JSON validado ou pelo protocolo com tetos (19a/29b); nada de `pickle`, `eval` ou `shell=True` no caminho de entrada; modelo de ameaças de uma página com risco residual.
- **Observação honesta:** RSS lido de `/proc` (residência) e não só de `tracemalloc` (que não mede memória nativa nem a de outros processos); pico (`ru_maxrss`) e atual (VmRSS) nunca trocados no relatório.
- **Limitações nomeadas:** sem consenso/eleição, sem TLS, log sem rotação, GPU apenas modelada. A seção de limitações é entregável, não confissão opcional.

## Como as quatro peças se cobram (o que a integração prova)

O serviço (23a/23b) sob carga numérica (29c) observado pela CLI (14a) com falhas injetadas: cada peça verificada isolada agora convive, e a convivência é o teste. A carga da 29c contra o serviço mostra o que o livro inteiro ensinou sobre transporte — pickle serializando blocos entre processos é a mesma "viagem" que a conta da 29c já precificou, agora atravessando o serviço em vez do pool. A CLI de observação lê RSS por `/proc` (residência real, incluindo o que `tracemalloc` não vê) e distingue pico de atual — e é por isso que a solução comentada avisa onde `tracemalloc` engana: instrumento de alocação Python não mede memória de outro processo, e o relatório que troca um pelo outro mente com número verdadeiro. O roteiro de falhas fecha o círculo: atraso exercita deadline, duplicação exercita idempotência com replay, queda com `kill` exercita o log com fsync da 23a, reinício exercita a reconciliação da 23b — cada falha com esperado escrito antes e observado depois, e divergência é defeito (do projeto ou da doc, ambos entregáveis). Sem consenso, sem TLS, log sem rotação: a seção de limitações vira o contrato do que o projeto não promete — e projeto sem limitações declaradas promete tudo, que é a forma mais comum de não entregar nada.

## Rubrica (resumo)

| Critério | Evidência |
|---|---|
| Distribuído | roteiro de falhas verde; idempotência e descarte de partição demonstrados |
| Protocolos | os cinco casos de framing da 19a contra o serviço |
| Segurança | fronteira validada, abusos recusados em teste, modelo de ameaças |
| Observação | CLI com RSS/tempo/estado interpretados, pico ≠ atual |
| Desempenho | baseline conferido, speedup com ambiente, Amdahl e o caso que perde |
| Reprodutibilidade | clone limpo verde + README executável |

## Exercícios, pistas e solução

O projeto é o exercício: [pistas](exercicios/33_pistas.md) ordenam a montagem (serviço → CLI → carga → falhas → defesa) e a [solução comentada](exercicios/33_solucoes.md) discute decisões-modelo, incluindo onde `tracemalloc` engana e por que o relatório usa `/proc`.

**Checklist de submissão:** as quatro peças rodando juntas; consistência em uma frase conferindo com a demonstração; abusos recusados; observação interpretada; limitações declaradas; clone limpo verde.
