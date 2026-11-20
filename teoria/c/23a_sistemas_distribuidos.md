---
layout: default
---

# Aula 23a — Falhas parciais em C: dois nós, um log e nenhuma ilusão

Você vai ao terminar, você deverá executar dois nós chave-valor em C que trocam `PUT`/`GET` por linhas em loopback, persistem cada escrita em log com `fsync` antes de responder, recusam duplicar efeito em reenvio com o mesmo `id` e sobrevivem — com comportamento explicado — a duplicação, entrada malformada, linha truncada, queda com `SIGKILL`, cauda de log rasgada, reinício com replay e atraso injetado de 400 ms.

a aula 23 ensinou a enquadrar bytes; a aula 31 mostrará um servidor TCP completo. Falta a pergunta que separa servidor que funciona de sistema distribuído defensável: *o que acontece quando a outra ponta cai, atrasa ou recebe duas vezes a mesma operação?* Em um processo só, chamar a função duas vezes é erro de quem chama. Entre processos, a rede repete por você — retry é o comportamento normal, não o abuso.

## 1. Contrato antes do código

Protocolo desta aula (texto por linhas, somente loopback, teto de 512 bytes por linha):

```text
PUT <id> <chave> <valor> [F]
GET <chave>
```

Respostas: `OK <valor>`, `OK <valor> REPETIDO`, `NAO_ENCONTRADO` ou `ERR <motivo>`.

- **Pré-condição:** `id`, `chave` e `valor` são tokens sem espaço, de `[A-Za-z0-9._-]`, com tetos de 31, 63 e 127 bytes. Linha acima do teto ou token fora do alfabeto é `ERR` — entrada externa nunca é presumida benigna.
- **Pós-condição de `PUT` novo:** o registro está no log (com `fsync`) e na memória antes de o cliente receber `OK`. Se o par está acessível, ele recebe o encaminhamento antes da resposta; se caiu, o `OK` é **degradado** e documentado como tal.
- **Pós-condição de `PUT` repetido:** devolve o valor da **primeira** escrita com o mesmo `id`, marcado `REPETIDO`, sem reaplicar nem reencaminhar. Vale a primeira, nunca a última duplicata.
- **Caso especial `F`:** marca operação encaminhada pelo par. Quem recebe com `F` persiste e aplica, mas **não reencaminha** — sem isso, dois nós se encaminhariam para sempre.
- **Fora do contrato:** consenso, eleição, ordem total, partição com escrita nos dois lados, TLS e autenticação. Loopback local apenas; expor este protocolo em rede pública seria um P0.

## 2. Exatamente uma vez não existe; idempotente, sim

A rede pode duplicar qualquer mensagem: o cliente que não recebe resposta repete, e repetir é correto — desde que repita com o **mesmo `id`**. O par alcançável é *at-least-once na entrega + idempotência no efeito = efetivamente-uma-vez no estado*:

1. **Validar** tokens e tetos.
2. **Persistir** no log com `fflush` + `fsync` — responder antes de persistir é mentir ao cliente se o processo cair entre a resposta e o disco.
3. **Aplicar** na memória, sob o mesmo mutex que serializa checar-o-`id` → persistir → aplicar. Sem essa seção crítica única, duas threads com o mesmo `id` passam juntas pela checagem e duplicam.
4. **Encaminhar** ao par **fora do mutex**, antes de responder: I/O de rede dentro da seção crítica serializaria todas as conexões atrás de um par lento. Fora dela, o encaminhamento tem timeout de conexão de 2 s e melhor esforço — par caído degrada, não trava.
5. **Responder.**

O conjunto de `id`s vistos não mora só na RAM: ele é reconstruído pelo **replay** do log na inicialização. Reiniciar não esquece o que já foi aplicado. Linha malformada no replay — em especial a **cauda rasgada** de uma queda no meio da escrita — é ignorada e contada como não aplicada: quem a enviou não recebeu `OK`, logo repetir com o mesmo `id` é seguro.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23a_distribuido.c) é o programa completo (cerca de 480 linhas; como na aula 23a de Go, o arquivo é a única fonte — transcrever criaria duas fontes que divergem). Só biblioteca padrão do C11 + POSIX/pthreads. Dois modos no mesmo binário: `no` sobe um nó; sem argumentos (ou `checar`), o programa sobe dois processos filhos com `fork`/`execl`, executa o roteiro de falhas e mata os filhos com sinais reais.

Compile e execute:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -pthread teoria/c/exemplos/23a_distribuido.c -o /tmp/23a
/tmp/23a checar
```

Saída esperada nesta máquina (Linux x86_64, GCC 16.2.1; tempos do item 16 variam, os vereditos não):

```text
ok 1 - PUT inicial responde OK
ok 2 - replica recebe via encaminhamento
ok 3 - duplicata com valor diferente devolve a primeira (REPETIDO)
ok 4 - duplicata nao duplica efeito na replica
ok 5 - pedido malformado recebe ERR
ok 6 - servidor sobrevive a pedido malformado
ok 7 - conecta para truncar
ok 8 - servidor sobrevive a linha truncada
ok 9 - escrita com replica caida responde (degradado)
ok 10 - abre log de B para rasgar a cauda
ok 11 - reinicio faz replay e ignora cauda rasgada
ok 12 - escrita da queda ainda nao esta na replica
ok 13 - reenvio com mesmo id e REPETIDO na origem
ok 14 - reenvio dirigido entrega a escrita perdida
ok 15 - estados convergem apos reenvio
ok 16 - atraso de 400 ms atrasa mas nao trava o cliente
ok 17 - escrita atrasada chega a replica
DISTRIBUIDO_OK checks=17
```

O mesmo roteiro passa com sanitizers (nesta máquina, via clang 22, pois as bibliotecas `libasan`/`libubsan` do GCC não estão instaladas):

```bash
clang -std=c11 -Wall -Wextra -Wpedantic -Werror -pthread -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/23a_distribuido.c -o /tmp/23a_asan
/tmp/23a_asan checar   # 17/17, sem relatório de sanitizer
```

Para operar um nó manualmente:

```bash
/tmp/23a no 9001 /tmp/no-a.log 9002 0 &   # porta, log, porta do par, atraso de encaminhamento em ms
printf 'PUT id1 k1 v1\nGET k1\n' | nc 127.0.0.1 9001
```

### O que a memória e as threads fazem aqui?

Cada conexão aceita vira uma thread destacada que morre ao fechar a conexão. O estado — vetor dinâmico de registros, com a conta de crescimento verificada contra overflow antes do `realloc` — vive atrás de um único `pthread_mutex_t`. A seção crítica é mínima e ordenada (checar `id`, persistir, aplicar); o encaminhamento de rede acontece fora dela. O log é o único estado que sobrevive ao processo: todo o resto é reconstruído dele no replay. `SIGPIPE` é ignorado no `main`: cliente que fecha cedo não derruba o nó.

## 4. Erros comuns e implicações defensivas

1. **Responder antes de persistir:** queda entre resposta e disco é mentira contada ao cliente. Log com `fsync` primeiro, sempre.
2. **Retry com `id` novo:** transforma repetição em duplicação real. O `id` nasce no cliente e atravessa todos os reenvios.
3. **Dedup só em memória:** reinício esquece o que já viu. Os `id`s vistos moram no log, reconstruídos no replay.
4. **Encaminhar dentro do mutex:** um par lento serializa o nó inteiro atrás de I/O. Seção crítica protege estado, nunca espera rede.
5. **Encaminhar sem a marca `F`:** dois nós se reenviam a mesma operação para sempre. Toda replicação precisa declarar que já é réplica.
6. **Sem teto de linha nem alfabeto de token:** entrada hostil vira buffer sem fim e log injetável. Teto e validação antes de qualquer `snprintf` de estado.
7. **Achar que dois nós provam consenso:** provam replicação primário–réplica com retry dirigido pelo cliente. Quorum, eleição e ordem total são outro capítulo — declarado, não fingido.

## 5. Ponte com Python, JavaScript e as demais trilhas

Em Python, `dict` + `threading.Lock` escondem a seção crítica; em JavaScript, o *event loop* de uma thread só esconde a corrida — até um `await` no meio da checagem reabri-la. Em C, cada garantia custa código explícito: o mutex, o `fsync`, o timeout, a marca de encaminhamento. As trilhas irmãs medem o mesmo contrato: Python com dois processos e `kill -9` (aula 23a), Go com framing binário e `-race` (aula 23a), C++ com dois nós, log e selo de cauda (aula 23b). A invariante é idêntica nas quatro: *entrada repetida não muda o estado duas vezes.*

## 6. Exercícios obrigatórios

1. Desenhe a sequência de um `PUT` com o par caído: onde o log salva, o que o cliente recebe, quem repete o quê e quando os estados convergem.
2. Mate o nó B no meio de 100 escritas dirigidas a A e, após o reinício, conte quantas precisam de reenvio dirigido. Explique por que nenhuma pode ser "descoberta" sozinha nesta arquitetura.
3. Troque a ordem persistir→responder para responder→persistir e descreva a janela exata em que o sistema mente. Que teste do roteiro passaria a falhar — ou a sorte passaria a decidir?
4. Escreva nos dois nós durante uma partição simulada (os dois vivos, encaminhamento impossibilitado) e mostre a divergência nos logs. Proponha a regra de reconciliação e os limites dela.

Use [pistas graduais](exercicios/23a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23a_solucoes.md) depois de registrar sua própria explicação.

## 7. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar por que exatamente-uma-vez é impossível e o que a idempotência entrega no lugar;
- ordenar validar → persistir → aplicar → encaminhar → responder, e dizer o que cada inversão quebra;
- demonstrar duplicata com valor diferente, malformado, truncado, `SIGKILL`, cauda rasgada, replay, reenvio e atraso com o roteiro, sem atalho;
- apontar onde o mutex protege estado e por que o encaminhamento fica fora dele;
- declarar o que o projeto **não** garante: partição com escrita bilateral, ordem total, failover automático, segurança fora de loopback;
- compilar com `-Werror`, executar os 17 checks e confrontar cada `ok` com o contrato, não com a sorte.


**Referências primárias:** *Designing Data-Intensive Applications* (Kleppmann), capítulos 5, 8 e 9, para log, replicação e falhas parciais; `man fsync` para a durabilidade prometida.

**Próxima aula:** 24 — heap real e fragmentação. O log desta aula é o primeiro estado que sobrevive ao processo; a aula 24 pergunta o que acontece com a memória que não sobrevive.

---

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1* (Addison-Wesley, 3. ed., ISBN 978-0131411555).
