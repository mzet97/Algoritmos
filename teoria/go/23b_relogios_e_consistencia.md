---
layout: default
---

# Aula 23b — Relógios, ordem causal e consistência declarada em Go

Sem relógio comum, 'antes' e 'depois' viram opinião. Relógios, ordem causal e consistência declarada são o vocabulário mínimo para prometer qualquer coisa entre máquinas.

## Nesta aula

Você vai aprender a distinguir relógio físico de relógio lógico, ordenar eventos por causalidade, declarar o nível de consistência, e prever anomalia de leitura.

Você vai ordenar eventos com Lamport, detectar concorrência com relógios vetoriais, reproduzir a partição bilateral (`k=111` contra `k=222`), reconciliar por anti-entropia com ordem total publicada `(lamport, autor, seq)` convergindo os dois nós — e nomear a escrita descartada.

**Contrato:** simulação determinística em processo (o transporte é a 23a). Reconciliação = trocar logs, reaplicar em ordem total crescente, o último por chave vence; a perda é declarada na saída. Consenso (eleição/quorum) continua fora — especificado como fronteira, não implementado. O [arquivo-fonte](exemplos/23b_relogios.go) é o programa completo:

```bash
go vet teoria/go/exemplos/23b_relogios.go
go run -race teoria/go/exemplos/23b_relogios.go
# particao: A ve k=111, B ve k=222, concorrencia detectada pelo vetor
# reconciliacao: ambos k=222; k=111 descartado pela regra publicada
# 23b ok: ...
```

**Leitura:** é o mesmo teorema da aula 23c de C e do R6 de Rust, agora na terceira linguagem — a repetição é o ponto: quando o aluno reconhece "empate de Lamport + vetores incomparáveis = conflito" em qualquer sintaxe, o conceito virou dele. Em Go, a implementação esconde a rede atrás de estruturas em memória, e é por isso que a aula insiste no rótulo *simulação*: o que está sendo provado é a álgebra da ordem, não o comportamento de sockets. Quem confundir os dois planos aprova demonstração como se fosse consenso — o erro que o G6 inteiro existe para prevenir.

### Por que Lamport ordena e só o vetor detecta (a divisão de trabalho)

O relógio de Lamport dá ordem total `(lamport, autor, seq)` — qualquer dois eventos são comparáveis, e a reaplicação em ordem crescente converge os dois nós para o mesmo estado. Mas "comparável" não é "causal": dois eventos com Lamport 5 e 6 podem ser causa-efeito ou coincidência, e o número sozinho não distingue. O vetor distingue: `V_A <= V_B` componente a componente significa "A viu tudo que B viu" (causalidade); vetores incomparáveis significam concorrência real — e é só nesse caso que a regra de desempate (último por chave vence) precisa existir. A partição bilateral executa a distinção: `k=111` de um lado e `k=222` do outro têm vetores incomparáveis (cada nó escreveu sem ver o outro), logo conflito, logo regra publicada, logo `k=111` descartado — descarte nomeado na saída, não silenciado no código. Sem o vetor, a reconciliação aplicaria "o maior Lamport vence" até em causa-efeito, e efeito seria descartado como se fosse concorrência: ordem total sem detecção é convergência para o estado errado com confiança total.

**Erros comuns:** usar relógio de parede para "último vence"; achar que Lamport detecta concorrência (quem detecta é o vetor); reconciliar sem publicar a regra nem o descarte; trocar os logs sem deduplicar por `(autor, seq)`; aplicar desempate em causa-efeito (sem vetor, toda ordem vira conflito — e reconciliação vira loteria com regra bonita); chamar a simulação de consenso (álgebra da ordem provada, sockets não — o transporte é a 23a, citado como fronteira).

**Exercícios:** [pistas](exercicios/23b_pistas.md) e [soluções](exercicios/23b_solucoes.md): (1) inverter o desempate e prever o novo vencedor; (2) terceiro nó isolado e anti-entropia par-a-par; (3) chaves diferentes na partição (nada se descarta — prove pela regra de aplicação); (4) especificar, sem implementar, eleição + quorum + destino do lado minoritário.

**Checklist:** Lamport e vetor mantidos nas quatro operações; concorrência decidida pelos vetores; descarte nomeado na reconciliação; fronteira com consenso declarada; `go vet`/`-race` limpos.

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

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124), caps. sobre monitoramento e SLOs.
