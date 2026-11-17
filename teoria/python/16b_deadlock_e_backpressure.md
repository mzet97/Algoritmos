---
layout: default
---

# Aula 16b — Deadlock reproduzido e backpressure em Python

**Competências SDD:** CON, MEM, SEC. **Pré-requisitos:** [16a](16a_concorrencia_e_gil.md) (corrida demonstrada, `Lock`, threads/processos/asyncio medidos) e 15–16. Esta ponte completa o PY5: deadlock, backpressure e isolamento de processo sobre a base da 16a.

**Objetivo observável:** reproduzir um deadlock por ordem oposta de aquisição com detecção por timeout (threads travadas observadas vivas, sem matar o processo de teste), corrigir com ordem global de aquisição e medir o backpressure de uma `queue.Queue(maxsize=4)` com consumidor dormindo.

**Contrato e exemplo:** o cenário de deadlock usa threads `daemon` e `join(timeout)`: a detecção é "continuam vivas depois do prazo", nunca um palpite. O [arquivo-fonte](exemplos/16b_deadlock.py) é o programa completo:

```bash
python3 teoria/python/exemplos/16b_deadlock.py
# deadlock por ordem oposta: TRAVADO (detectado por timeout)
# ordem global de aquisicao: completa sem travar
# backpressure: 5o put esperou 0.300s (fila de 4, consumidor dormindo)
# 16b ok: ...
```

**Leitura:** Python não tem detector de deadlock de locks comuns: o programa travado e o programa pensando são indistinguíveis por dentro, e é por isso que a detecção aqui é externa (timeout + estado das threads) e a correção é estrutural (ordem global: todo mundo adquire A antes de B). A `Barrier` do cenário não é enfeite: sem ela, o deadlock dependeria de escalonamento e o teste seria intermitente — deadlock reproduzível é deadlock **coreografado**. Na fila, o `maxsize` é a política de backpressure: o quinto `put` espera 0,3 s porque o consumidor dorme 0,3 s; fila sem teto trocaria essa espera honesta por crescimento de memória até o limite do processo, que é a forma Python de transformar lentidão em OOM.

**Erros comuns:** diagnosticar deadlock "pelo CPU parado" (thread travada em lock também não usa CPU — e thread esperando I/O idem); corrigir com timeout de aquisição sem ordem global (o travamento vira loteria rara em vez de impossibilidade); fila ilimitada "para não bloquear o produtor"; matar o processo de teste e chamar isso de detecção.

**Exercícios:** [pistas](exercicios/16b_pistas.md) e [soluções](exercicios/16b_solucoes.md): (1) deadlock de três locks em ciclo, com a ordem global que o quebra; (2) `acquire(timeout=...)` com recuo: mostrar que ele mascara em vez de corrigir, contando quantas vezes o timeout dispara; (3) produtor rápido e consumidor lento com fila ilimitada: medir RSS crescendo e comparar com a limitada; (4) o mesmo trabalho com processos (`multiprocessing`) e a discussão do que o isolamento compra quando um worker trava.

**Checklist:** deadlock coreografado e detectado por timeout; correção estrutural (ordem), não sintomática; backpressure medida com teto declarado; nenhum `sleep` como sincronização (só como carga e coreografia).
