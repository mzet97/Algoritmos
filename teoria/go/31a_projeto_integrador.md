---
layout: default
---

# Aula 31a — Projeto integrador de Go: serviço, ferramenta e benchmark sob a mesma rubrica

## Objetivos verificáveis

Ao final, você deverá: integrar serviço, ferramenta e benchmark sob a mesma rubrica, confrontar resultados entre frentes, e declarar o que foi provado.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir a rubrica integradora e seus critérios`;
- **Nível 2 — Aplicar:** `diagnosticar divergência entre frentes e atribuir a causa`;
- **Nível 3 — Projetar:** `projetar a integração justificando cobertura e evidências`.

**Competências SDD:** FND–SEC (G8). **Pré-requisitos:** o livro até a 31, com as pontes [23a](23a_protocolos_distribuidos.md), [23b](23b_relogios_e_consistencia.md), [08d](08d_ferramenta_so.md) e [28a](28a_hpc.md)/[28b](28b_vetorizacao_e_numa.md) executadas.

**Objetivo observável:** montar, a partir das peças do livro, um repositório reproduzível com os três entregáveis do G8 funcionando juntos: (1) o serviço distribuído de dois nós da 23a com a política de consistência da 23b documentada; (2) a ferramenta de observação da 08d apontada para o serviço em execução; (3) um benchmark da 28a/28b sobre a carga do serviço. A defesa é escrita, usando a rubrica.

## Contrato de entrega

- **Ambiente limpo:** `bash teoria/regress.sh` verde cobre as três peças (serviço com `-race`, ferramenta, benchmarks) sem passos manuais. Qualquer passo manual entra no README do projeto com o comando exato, ou o projeto não é reproduzível.
- **Roteiro de falhas do serviço:** atraso, queda com `kill`, duplicação de requisição, reinício com replay e partição simulada, cada um com o comportamento esperado **antes** de executar e o observado depois. Divergência entre esperado e observado é defeito do projeto ou da documentação — os dois são entregáveis.
- **Medição com a ferramenta:** com o serviço sob carga, registrar goroutines, threads do SO, faltas de página e heap, ligando cada número ao caminho runtime → kernel → hardware (a leitura da 08d). Número sem interpretação não pontua.
- **Modelo de ameaças (uma página):** superfície (portas, protocolo, limites de linha/conexão/tempo), quem pode abusar do quê em loopback e fora dele, e o risco residual aceito. Sem TLS/autenticação, a frase "apenas loopback" precisa estar no README, não só na defesa.
- **Limites de produção declarados:** o que este serviço **não** é (sem consenso, sem eleição, sem persistência além do log, sem segurança de rede) — a lista de lacunas da 23a/23b vira a seção de limitações, sem maquiagem.

## Rubrica de aprovação (resumo)

| Critério | Evidência mínima |
|---|---|
| Corretude distribuída | roteiro de falhas verde com idempotência demonstrada e descarte de partição nomeado |
| Observabilidade | relatório da ferramenta com interpretação por número |
| Desempenho | benchmark com baseline, corretude antes de tempo e caso em que a otimização perde |
| Segurança | modelo de ameaças com limites de recurso e risco residual |
| Reprodutibilidade | `regress.sh` verde em clone limpo + README executável |
| Defesa | escolhas justificadas contra alternativas (consenso, fila ilimitada, threads sem teto) |

## Como as três peças se observam (o que a integração prova)

O serviço (23a/23b) sob carga do benchmark (28a/28b) observado pela ferramenta (08d): cada peça que o livro verificou isolada agora tem de conviver. A ferramenta apontada para o serviço mostra goroutines subindo com a carga (escalonador do runtime absorvendo), threads do SO paradas (multiplexação segurando), faltas menores crescendo no warm-up (páginas materializadas) e `HeapAlloc` acompanhando o log em memória — e cada número do relatório final precisa dessa amarração, porque "número sem interpretação não pontua". O roteiro de falhas é o segundo eixo da integração: atraso e queda com `kill` exercitam o deadline da 25 e o replay da 23a; duplicação exercita a idempotência; partição simulada exercita a regra de descarte da 23b — cada falha com esperado escrito antes e observado depois, e divergência é defeito (do projeto ou da doc, ambos entregáveis). A defesa, por fim, é o terceiro entregável avaliado: justificar por escrito cada escolha contra a alternativa (por que sem consenso? por que fila limitada? por que threads com teto?) usando a evidência das peças — integração sem defesa é montagem; com defesa, é engenharia.

## Exercícios, pistas e solução

O projeto é o exercício; [pistas](exercicios/31a_pistas.md) organizam a montagem e a [solução comentada](exercicios/31a_solucoes.md) discute decisões-modelo e armadilhas de defesa. Não há atalho de código novo: as peças já existem e foram verificadas aula a aula — o trabalho avaliado é integração, roteiro e defesa.

**Checklist de submissão:** os três entregáveis rodando juntos; roteiro de falhas com esperado × observado; relatório de observação interpretado; modelo de ameaças de uma página; limitações declaradas; clone limpo verde.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124).
