---
layout: default
---

# Aula 23b — Relógios, ordem causal e consistência declarada em Go

**Competências SDD:** DST, SEC, MAT. **Pré-requisitos:** [23a](23a_protocolos_distribuidos.md) (dois nós com persistência e idempotência) e 22–26. Esta ponte completa o G6 no ponto que a 23a declarou aberto: relógios/ordem e a política de consistência quando os dois lados escrevem.

**Objetivo observável:** ordenar eventos com Lamport, detectar concorrência com relógios vetoriais, reproduzir a partição bilateral (`k=111` contra `k=222`), reconciliar por anti-entropia com ordem total publicada `(lamport, autor, seq)` convergindo os dois nós — e nomear a escrita descartada.

**Contrato:** simulação determinística em processo (o transporte é a 23a). Reconciliação = trocar logs, reaplicar em ordem total crescente, o último por chave vence; a perda é declarada na saída. Consenso (eleição/quorum) continua fora — especificado como fronteira, não implementado. O [arquivo-fonte](exemplos/23b_relogios.go) é o programa completo:

```bash
go vet teoria/go/exemplos/23b_relogios.go
go run -race teoria/go/exemplos/23b_relogios.go
# particao: A ve k=111, B ve k=222, concorrencia detectada pelo vetor
# reconciliacao: ambos k=222; k=111 descartado pela regra publicada
# 23b ok: ...
```

**Leitura:** é o mesmo teorema da aula 23c de C e do R6 de Rust, agora na terceira linguagem — a repetição é o ponto: quando o aluno reconhece "empate de Lamport + vetores incomparáveis = conflito" em qualquer sintaxe, o conceito virou dele. Em Go, a implementação esconde a rede atrás de estruturas em memória, e é por isso que a aula insiste no rótulo *simulação*: o que está sendo provado é a álgebra da ordem, não o comportamento de sockets. Quem confundir os dois planos aprova demonstração como se fosse consenso — o erro que o G6 inteiro existe para prevenir.

**Erros comuns:** usar relógio de parede para "último vence"; achar que Lamport detecta concorrência (quem detecta é o vetor); reconciliar sem publicar a regra nem o descarte; trocar os logs sem deduplicar por `(autor, seq)`.

**Exercícios:** [pistas](exercicios/23b_pistas.md) e [soluções](exercicios/23b_solucoes.md): (1) inverter o desempate e prever o novo vencedor; (2) terceiro nó isolado e anti-entropia par-a-par; (3) chaves diferentes na partição (nada se descarta — prove pela regra de aplicação); (4) especificar, sem implementar, eleição + quorum + destino do lado minoritário.

**Checklist:** Lamport e vetor mantidos nas quatro operações; concorrência decidida pelos vetores; descarte nomeado na reconciliação; fronteira com consenso declarada; `go vet`/`-race` limpos.
