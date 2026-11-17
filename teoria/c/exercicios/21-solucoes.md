---
layout: default
---

# Aula 21: solucoes comentadas

Compare apos tentar os [exercicios](../21.md#20-exercicios-obrigatorios). Estes exercicios sao de desenho; a rubrica cobra fluxos completos, nao codigo.

1. Thread-per-connection:

```text
accept ──> cria thread ──> recv (bloqueia) ──> processa ──> send ──> recv ...
                              │                                   │
                        cliente ocioso =                  fim: fecha e a
                        thread parada                     thread termina
```

Cada conexao tem um fluxo de controle proprio que pode usar codigo bloqueante comum. O preco aparece parado: a thread ociosa continua existindo.

2. Event loop com `poll` (o servidor da [aula 20](20-solucoes.md)):

```text
        ┌──────────────────────────────────────────────┐
        │                                              v
poll espera ──> servidor pronto? aceita (nao bloqueante)
        │
        └──> clientes prontos? acumula/le uma parte de cada ──> volta a esperar
```

Um fluxo so, nenhum bloqueio: cada cliente e um estado (fd + buffer + quanto ja leu), nao uma thread.

3. Custos por conexao:

| Custo | Thread-per-connection | Event loop |
| --- | --- | --- |
| Memoria por conexao ociosa | Pilha da thread (tipicamente MBs virtuais) + estruturas | Entrada no array de fds + buffer do protocolo (KBs) |
| CPU ociosa | Trocas de contexto ao acordar/adormecer | Quase zero: o kernel segura a espera |
| Complexidade | Baixa: codigo linear por cliente | Alta: maquina de estados e tetos por conexao |
| Bloqueio acidental | Afeta so aquela conexao | Um bloqueio trava **todas** as conexoes |

4. Se a maioria das conexoes esta ociosa (chats, notificacoes, sensores), o modelo por thread paga pilha e escalonamento por quem nao tem trabalho — o teto chega por memoria e contexto, nao por CPU util. O event loop paga por conexao quase so o estado do protocolo, entao o limite sobe ordens de grandeza; o preco e disciplina de nunca bloquear no loop.

5. Hibrido comum: o event loop aceita, enquadra e responde o que e rapido (cache, estado em memoria); trabalho que pode bloquear ou demorar (disco sincrono, computacao pesada, biblioteca legada) e entregue a um pool de threads com **fila limitada**. A fila limitada e o que transforma sobrecarga em rejeicao rapida em vez de latencia infinita — o pool devolve o resultado ao loop por um canal/fd que o `poll` tambem vigia.

**Autoverificacao:** seus dois desenhos mostram onde cada conexao espera, a tabela tem um custo que cresce por conexao em cada modelo e o hibrido declara o que nunca entra no loop e o que acontece quando a fila enche.
