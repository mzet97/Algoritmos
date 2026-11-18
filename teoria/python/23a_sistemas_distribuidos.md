---
layout: default
---

# Aula 23a — Falhas parciais: dois processos, um log e nenhuma ilusão

## Objetivo da Aula

Construir dois processos que trocam mensagens com IDs, registram tudo em log durável e sobrevivem a atraso, duplicação, queda e reinício — e explicar exatamente o que *não* foi garantido. Pré-requisitos: aulas 13, 14a, 16a, 20 a 23. Não precisa saber consenso, relógios vetoriais, Kubernetes nem nuvem.

Ao final, você deverá: executar dois processos com um log, explicar falhas parciais sem ilusão, e declarar o que foi provado.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir falha parcial, log e quorum simples`;
- **Nível 2 — Aplicar:** `diagnosticar suposição de atomicidade e corrigir`;
- **Nível 3 — Projetar:** `projetar experimento distribuído justificando limites`.

## 1. A rede mente, atrasa e repete

Entre dois processos locais já moram todos os demônios dos distribuídos: mensagem que chega duas vezes (retry do cliente), que demora (fila, GC, escalonador), que nunca chega (queda no meio), processo que morre e volta sem memória do que fez. Falha parcial significa: *uma parte funciona e outra não, ao mesmo tempo*. O projeto que só funciona quando tudo funciona não é distribuído — é centralizado com mais passos.

## 2. Exatamente uma vez não existe; idempotente, sim

A rede pode duplicar qualquer mensagem, então "executar exatamente uma vez" é impossível de garantir no efeito. O par alcançável é **at-least-once na entrega + idempotência no efeito = efetivamente-uma-vez no estado**: cada operação carrega um ID único; quem recebe registra IDs já vistos e repete o resultado guardado em vez de reexecutar. Duplicata vira *cache hit*, não corrupção.

## 3. Log durável antes da resposta

Ordem das operações no nó que aceita escrita: validar → gravar no log e descarregar (`flush` + `fsync`) → aplicar em memória → responder. Responder antes de persistir inverte a garantia: queda entre resposta e disco vira mentira contada ao cliente. `fsync` por registro é lento e honesto; agrupar (`batch`) é otimização futura, não desta aula.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23a_distribuido.py) contém o programa completo: dois papéis no mesmo arquivo (`no` e `checar`). Só biblioteca padrão:

```bash
python3 teoria/python/exemplos/23a_distribuido.py checar
```

Arquitetura (primário/réplica com retry dirigido pelo cliente — *não* é consenso, *não* promete linearizabilidade):

- Cada nó serve `set`/`get` por JSON-linhas sobre TCP, com ID por operação.
- `set` novo: valida, persiste no log (`fsync`), aplica, tenta replicar ao par com timeout; se o par caiu, responde mesmo assim (disponibilidade primeiro, documentado).
- `set` repetido: devolve o resultado guardado sem reaplicar (idempotência).
- Na inicialização, cada nó relê seu log (recuperação): o que estava no disco volta ao estado.
- Cliente que não recebe resposta repete com o *mesmo* ID; quem recebe pode ter aplicado — repetir com ID novo criaria duplicata real.
- Cada conexão roda em thread própria e o trio checar→persistir→aplicar acontece sob um `Lock` (encaminhar fica *fora* do lock para não serializar I/O de rede). Aceitar-e-responder na mesma thread do `accept` travaria o encaminhamento mútuo — A esperando B enquanto B espera A — e só o timeout desfaria o nó. Concorrência aqui é correção, não luxo.

O modo `checar` executa o roteiro de falhas sozinho: dois nós locais, escrita e leitura, duplicata com valor diferente, `kill -9` num nó, escrita durante a queda, reinício com o mesmo log, reenvio dos IDs perdidos, atraso injetado no encaminhamento e igualdade final dos estados.

## 5. O que foi demonstrado e o que foi declarado como lacuna

Demonstrado: repetição sem duplo efeito, queda com serviço degradado, recuperação por replay, atraso limitado sem travar cliente. Declarado como lacuna: partição com escrita nos dois lados (sem quorum, o último a voltar diverge — o projeto *detecta* via log, não resolve); ordem total entre nós; failover automático; segurança de rede (sem TLS/autenticação: loopback local apenas).

## 6. Erros comuns e implicações defensivas

1. **Responder antes de persistir:** queda entre resposta e disco é mentira. Log primeiro, sempre.
2. **Retry com ID novo:** transforma repetição em duplicação real. O ID nasce no cliente e atravessa retries.
3. **Dedup só em memória:** reinício esquece o que já viu. O conjunto de IDs vistos mora no log, não na RAM.
4. **Timeout sem limite:** esperar para sempre é travar devagar. Timeout curto no encaminhamento, resposta degradada documentada.
5. **Achar que dois nós provam consenso:** provam replicação com retry dirigido. Consenso elege, ordena e sobrevive a partição — outro curso.
6. **Log sem `fsync`:** buffer do SO não é durabilidade. Queda de energia apaga o "persistido".

## 7. Ponte com as demais trilhas

Go mede o mesmo tradeoff com worker pool e race detector (aula 11a); Rust enquadra bytes com limites (aula 14a); C++ isola cliente lento com `poll` (aula 31). Em todas, a invariante é a mesma: entrada repetida não muda o estado duas vezes.

## 8. Exercícios obrigatórios

1. Desenhe a sequência de uma escrita com o par caído: onde o log salva, onde o cliente repete, onde o estado converge.
2. Mate o primário no meio de 100 escritas e conte quantas precisam de reenvio. Explique cada caso.
3. Injete atraso de 2 s no encaminhamento com timeout de 0,5 s: o cliente recebe resposta? De quem? Por quê?
4. Escreva nos dois nós durante uma partição simulada (parada mútua) e mostre a divergência nos logs. Proponha a regra de reconciliação — e seus limites.

Use [pistas graduais](exercicios/23a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar por que exatamente-uma-vez é impossível e o que idempotência entrega no lugar;
- ordenar validar → persistir → aplicar → responder, e dizer o que cada inversão quebra;
- demonstrar duplicata, queda, reinício e atraso com o roteiro, sem atalho;
- apontar o que o projeto *não* garante (partição com escrita bilateral, ordem total, failover);
- executar o exemplo e confrontar cada asserção com o contrato, não com a sorte.

**Próxima aula:** projeto didático de backend. O raciocínio desta aula sustenta serviços que sobrevivem à segunda-feira: log, retry com ID e degradação explícita.

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

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação do FastAPI ([fastapi.tiangolo.com](https://fastapi.tiangolo.com/)); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124).
