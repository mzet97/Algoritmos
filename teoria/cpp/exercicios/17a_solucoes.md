---
layout: default
---

# Soluções comentadas da aula 17a

## 1. O que o handler pode fazer

Escrever `parar = 1` e retornar. Ler o flag no laço em ponto seguro (`!parar`). `cout`, `malloc`, `lock` e `exit` no handler disputam estado com o código interrompido — corrida que nem o TSan enxerga direito, porque sinal não é thread.

## 2. Mutex contra atômico, dois regimes

Sem contenção real (o exemplo: cada thread soma local? não — aqui ambas somam o mesmo contador; o `atomic` vence por não serializar em futex). Com contenção feroz no mesmo contador, ambos sofrem pela linha de cache compartilhada, e a diferença cai. A partição (`parciais[w]`) remove a contenção: some local, combine no fim — padrão que escala de verdade quando o trabalho é partível.

## 3. WARNING do TSan (padrão documentado — transcreva o seu onde o TSan funcionar)

```text
WARNING: ThreadSanitizer: data race (pid=...)
  Write of size 8 at 0x... by thread T3:
    #0 main::{lambda()#1}::operator() ...
  Previous write of size 8 at 0x... by thread T1:
    #0 main::{lambda()#2}::operator() ...
```

Duas escritas em `total` sem sincronização. Restaurar o `lock_guard` silencia: toda escrita passa pela exclusão, e a leitura final acontece após todos os `join` (que também sincronizam).

## 4. TERM contra IGN

Com o handler instalado, `timeout -s TERM 1` entrega SIGTERM durante o `sleep_for`: o flag interrompe as iterações restantes e o programa sai pelo caminho normal (código 0, mensagem impressa). Com `signal(SIGTERM, SIG_IGN)`, o kernel descarta o sinal e nem o `timeout` percebe: o programa completa as 3 iterações. Ignorar sinal não é tratá-lo — documente a escolha.
