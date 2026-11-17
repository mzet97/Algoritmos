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
