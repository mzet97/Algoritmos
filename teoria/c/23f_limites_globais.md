---
layout: default
---

# Aula 23f — Limites globais medidos: quando o teto é do servidor, não da conexão (C5)

Você vai ao terminar, você deverá rodar o teste, apontar qual `assert` prova cada teto (por IP, global, linha, bytes), explicar por que o excesso de bytes silencia em vez de fechar (RST), por que a recusa em `accept` fecha com FIN limpo — e contar os dois bugs reais que o teste pegou antes de qualquer execução de bancada (o `revents` lixo e o RST da rajada).

teto por conexão protege a conexão; ataque real abre N conexões. Sem teto global, 8 clientes lentos — cada um dentro de todos os tetos por conexão — esgotam o servidor que passou em todos os testes unitários. Esta aula entrega os três tetos sobre a base da 23b e da 31, com o teste provando que cliente lento não paralisa os demais, que a rajada é avisada, silenciada e depois recuperada — e que recusar no lugar errado quebra o protocolo antes do atacante chegar.

## 1. Contrato antes do código

O [servidor](exemplos/23f_limites.c) (`poll`, sem threads, sem `accept` bloqueante) e o [teste](exemplos/test_23f.py) (sobe, exercita, derruba; falha se houver ASan/UBSan no `stderr`):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23f_limites.c -o srv23f
python3 teoria/c/exemplos/test_23f.py srv23f
# por-ip ok: 3a conexao do mesmo IP recusada
# global ok: 9a conexao simultanea recusada
# lentos ok: rapido atendido com 7 lentos ocupando slots
# linha ok: linha longa e erro da conexao, servidor vivo
# bytes ok: rajada silenciada com aviso e recuperada na janela seguinte
# 23f ok: tetos por-ip, global e de bytes; lentos nao paralisam
```

Tetos: 8 conexões simultâneas, 2 por IP, linha de 1023 bytes, 64 KiB por janela de 1 s. Recusa em `accept` fecha com motivo (`CHEIO global`, `CHEIO por-ip`) e FIN limpo — sem nada lido, sem RST. Excesso de bytes **silencia** até a próxima janela (`CHEIO bytes` no próprio canal, leitura-e-descarte, depois recupera). Linha longa é erro da conexão (`ERRO linha-longa`), nunca do processo. Servidor que sobrevive a todos os abusos continua ecoando — a última linha do teste prova com um ECHO após a tempestade.

## 2. Três decisões que o teste amarrou

**Recusar no `accept`, não depois.** Por-IP e global são decididos antes de alocar slot: `dprintf` + `close` imediato, sem ler nada. Fechar sem dado não lido pendente entrega o motivo com FIN limpo — o cliente lê `CHEIO por-ip\n` e EOF, deterministicamente. Se o servidor lesse primeiro e fechasse depois (com dados não lidos no buffer), o kernel responderia RST e o motivo se perderia: FIN conta a história, RST a apaga. O teste usa aliases de loopback (`127.0.0.2`–`.4`) porque, com teto por IP em 2, um IP só jamais encheria o global de 8 — o teste precisa de quatro identidades para provar o teto de oito, e essa aritmética está no teste, não na documentação.

**Drenar o backlog.** Um `accept` por iteração atrasa a contagem sob rajada: conexões aceitas tarde demais contam na janela errada, e os tetos passam a mentir (negar o que deveriam aceitar, aceitar o que deveriam negar). O laço de `accept` até `EAGAIN` com o ouvinte não-bloqueante é a correção — e quem volta ao `poll` (`continue`) antes de tocar nos clientes evita o bug da seção 4. Drenar é também justiça: sem dreno, a 9ª conexão espera uma iteração inteira enquanto o servidor processa dados — latência de recusa vira loteria do escalonador.

**Silenciar em vez de fechar na rajada.** Fechar com 146 KiB não lidos no buffer gera RST — testado empiricamente, inclusive com `SO_LINGER` (que não adianta: dado recebido e não lido descarta com RST de qualquer jeito). Sem o motivo, o cliente conclui "servidor caiu" em vez de "fui limitado" — e a conclusão errada vira chamado de incidente. A política honesta é throttle com aviso: marca a conexão como surda, lê-e-descarta até a janela virar, avisa `CHEIO bytes` no próprio canal, recupera na janela seguinte. O teste prova as três fases: aviso recebido, silêncio durante a janela, ECHO funcionando depois. Throttle com aviso é protocolo; fechar com RST é punição sem intimação.

## 3. O servidor, por inteiro

O programa abaixo é o [servidor](exemplos/23f_limites.c), reproduzido integralmente — o teste o compila deste texto:

```c
/* Aula 23f — limites globais medidos sobre a base da 23b e da 31.
 *
 * Servidor ECHO por linhas (poll, sem threads) com tres tetos globais:
 * (1) conexoes simultaneas, (2) conexoes por IP, (3) bytes recebidos por
 * janela de 1 s. Cliente lento ocupa slot sem falar e nao paralisa os
 * demais; quem estoura teto cai com mensagem e sem vazar o slot.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23f_limites.c -o /tmp/23f
 * Executar: /tmp/23f PORTA  (o teste python sobe, exercita e derruba)
 */
#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

enum {
    MAX_CLIENTS = 32,
    TETO_CONEXOES = 8,
    TETO_POR_IP = 2,
    TETO_BYTES_JANELA = 65536,
    LINE_CAP = 1024,
    OUT_CAP = 4096
};

typedef struct {
    int fd;
    uint32_t ip;
    int surda; /* estourou bytes: le-e-descarta ate a proxima janela */
    char line[LINE_CAP];
    size_t line_len;
    char out[OUT_CAP];
    size_t out_pos;
    size_t out_len;
} Client;

static Client clientes[MAX_CLIENTS];
static int n_conexoes = 0;
static size_t bytes_janela = 0;
static uint64_t janela_ini_ms = 0;

static uint64_t agora_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

/* Nova janela de 1 s: zera o contador e dessilencia todo mundo. */
static void nova_janela(uint64_t agora)
{
    janela_ini_ms = agora;
    bytes_janela = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
        clientes[i].surda = 0;
}

static void talvez_nova_janela(uint64_t agora)
{
    if (agora - janela_ini_ms >= 1000)
        nova_janela(agora);
}

static int por_ip(uint32_t ip)
{
    int n = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clientes[i].fd >= 0 && clientes[i].ip == ip)
            n++;
    return n;
}

static void fecha(int i)
{
    if (clientes[i].fd < 0)
        return; /* ja fechado: nao conta duas vezes */
    close(clientes[i].fd);
    clientes[i].fd = -1;
    n_conexoes--;
}

static void recusa(int fd, const char *motivo)
{
    dprintf(fd, "CHEIO %s\n", motivo);
    close(fd);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s PORTA\n", argv[0]);
        return 2;
    }
    for (int i = 0; i < MAX_CLIENTS; i++)
        clientes[i].fd = -1;

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0)
        return 1;
    int um = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &um, sizeof um);
    struct sockaddr_in end;
    memset(&end, 0, sizeof end);
    end.sin_family = AF_INET;
    end.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    end.sin_port = htons((uint16_t)atoi(argv[1]));
    if (bind(srv, (struct sockaddr *)&end, sizeof end) < 0 || listen(srv, 16) < 0)
        return 1;
    {
        int fl = fcntl(srv, F_GETFL, 0);
        if (fl < 0 || fcntl(srv, F_SETFL, fl | O_NONBLOCK) < 0)
            return 1;
    }

    janela_ini_ms = agora_ms();
    for (;;) {
        struct pollfd pf[MAX_CLIENTS + 1];
        pf[0].fd = srv;
        pf[0].events = POLLIN;
        int nslots = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientes[i].fd < 0)
                continue;
            pf[1 + nslots].fd = clientes[i].fd;
            pf[1 + nslots].events = POLLIN;
            if (clientes[i].out_len > clientes[i].out_pos)
                pf[1 + nslots].events |= POLLOUT;
            nslots++;
        }
        if (poll(pf, (nfds_t)(nslots + 1), 100) < 0) {
            if (errno == EINTR)
                continue;
            return 1;
        }
        int aceitou = 0;
        if (pf[0].revents & POLLIN) {
            /* Drena o backlog inteiro: um accept por iteracao atrasa
               a contagem sob rajada e os tetos passam a mentir. */
            for (;;) {
                struct sockaddr_in de;
                socklen_t n = sizeof de;
                int fd = accept(srv, (struct sockaddr *)&de, &n);
                if (fd < 0)
                    break;
                aceitou = 1;
                uint32_t ip = de.sin_addr.s_addr;
                talvez_nova_janela(agora_ms());
                if (n_conexoes >= TETO_CONEXOES)
                    recusa(fd, "global");
                else if (por_ip(ip) >= TETO_POR_IP)
                    recusa(fd, "por-ip");
                else {
                    int slot = -1;
                    for (int i = 0; i < MAX_CLIENTS; i++)
                        if (clientes[i].fd < 0) {
                            slot = i;
                            break;
                        }
                    if (slot < 0) {
                        recusa(fd, "slots");
                    } else {
                        clientes[slot].fd = fd;
                        clientes[slot].ip = ip;
                        clientes[slot].surda = 0;
                        clientes[slot].line_len = 0;
                        clientes[slot].out_pos = clientes[slot].out_len = 0;
                        n_conexoes++;
                    }
                }
            }
            /* Volta ao poll antes de tocar nos clientes: o pf desta
               iteracao nao contem os recem-aceitos e o revents deles
               seria lixo de pilha (fecha espurio no nascimento). */
            if (aceitou)
                continue;
        }
        int idx = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientes[i].fd < 0)
                continue;
            idx++;
            short rev = pf[idx].revents;
            if (rev & (POLLERR | POLLHUP | POLLNVAL)) {
                fecha(i);
                continue;
            }
            if (rev & POLLOUT) {
                ssize_t w = write(clientes[i].fd, clientes[i].out + clientes[i].out_pos,
                                  clientes[i].out_len - clientes[i].out_pos);
                if (w <= 0) {
                    fecha(i);
                    continue;
                }
                clientes[i].out_pos += (size_t)w;
                if (clientes[i].out_pos == clientes[i].out_len)
                    clientes[i].out_pos = clientes[i].out_len = 0;
            }
            if (rev & POLLIN) {
                talvez_nova_janela(agora_ms());
                char tmp[4096];
                ssize_t r = read(clientes[i].fd, tmp, sizeof tmp);
                if (r <= 0) {
                    fecha(i);
                    continue;
                }
                if (clientes[i].surda)
                    continue; /* silenciada: le-e-descarta ate a janela virar */
                bytes_janela += (size_t)r;
                if (bytes_janela > TETO_BYTES_JANELA) {
                    /* Nao fecha: fechar com rajada nao lida gera RST e o
                       cliente nunca leria o motivo. Silencia ate a
                       proxima janela e avisa no proprio canal. */
                    const char *msg = "CHEIO bytes\n";
                    size_t m = strlen(msg);
                    memcpy(clientes[i].out, msg, m);
                    clientes[i].out_pos = 0;
                    clientes[i].out_len = m;
                    clientes[i].surda = 1;
                    clientes[i].line_len = 0;
                    continue;
                }
                for (ssize_t k = 0; k < r; k++) {
                    if (clientes[i].line_len < LINE_CAP - 1)
                        clientes[i].line[clientes[i].line_len++] = tmp[k];
                    if (tmp[k] == '\n') {
                        clientes[i].line[clientes[i].line_len] = '\0';
                        char resp[OUT_CAP];
                        int n = snprintf(resp, sizeof resp, "ECO %s", clientes[i].line);
                        size_t off = clientes[i].out_len;
                        if (off == 0)
                            clientes[i].out_pos = 0;
                        if ((size_t)n < sizeof clientes[i].out - off) {
                            memcpy(clientes[i].out + off, resp, (size_t)n);
                            clientes[i].out_len = off + (size_t)n;
                        }
                        clientes[i].line_len = 0;
                    }
                }
                if (clientes[i].line_len == LINE_CAP - 1 && memchr(clientes[i].line, '\n', clientes[i].line_len) == NULL) {
                    /* Linha sem fim dentro do teto: erro so desta conexao. */
                    const char *msg = "ERRO linha-longa\n";
                    size_t m = strlen(msg);
                    memcpy(clientes[i].out, msg, m);
                    clientes[i].out_pos = 0;
                    clientes[i].out_len = m;
                    clientes[i].line_len = 0;
                }
            }
        }
    }
}
```

E o [teste](exemplos/test_23f.py) que o exercita — também integral, pois o teste é metade do contrato:

```python
#!/usr/bin/env python3
"""Teste dos tetos globais do servidor 23f (C): por-IP, global e bytes.

Uso: python3 test_23f.py BINARIO
Sobe o servidor em porta efemera e prova: 3a conexao do mesmo IP cai
com CHEIO por-ip; a 9a conexao simultanea (2 por IP em 127.0.0.1-4) cai
com CHEIO global; rajada acima de 64 KiB na janela silencia com aviso e
recupera; e cliente rapido e atendido enquanto lentos ocupam os demais slots.
"""
from __future__ import annotations

import socket
import subprocess
import sys
import time
from pathlib import Path
from typing import Final

HOST: Final = "127.0.0.1"


def free_port() -> int:
    with socket.socket() as probe:
        probe.bind((HOST, 0))
        return probe.getsockname()[1]


def conecta(origem: str, porta: int) -> socket.socket:
    s = socket.socket()
    s.settimeout(10)
    s.bind((origem, 0))
    s.connect((HOST, porta))
    return s


def linha(s: socket.socket) -> bytes:
    buf = b""
    while not buf.endswith(b"\n"):
        parte = s.recv(256)
        if not parte:
            break
        buf += parte
    return buf


def main() -> None:
    if len(sys.argv) != 2:
        print("uso: test_23f.py BINARIO", file=sys.stderr)
        raise SystemExit(2)
    binario = Path(sys.argv[1]).resolve(strict=True)
    porta = free_port()
    with subprocess.Popen([str(binario), str(porta)], stderr=subprocess.PIPE) as srv:
        try:
            for _ in range(100):
                try:
                    with conecta(HOST, porta):
                        break
                except OSError:
                    time.sleep(0.05)
            else:
                raise AssertionError("servidor nao iniciou")

            # Teto por IP: 2 passam calados, a 3a cai com motivo.
            lentos = [conecta("127.0.0.1", porta), conecta("127.0.0.1", porta)]
            time.sleep(0.5)
            with conecta("127.0.0.1", porta) as extra:
                assert linha(extra) == b"CHEIO por-ip\n"
            print("por-ip ok: 3a conexao do mesmo IP recusada")

            # Teto global: 2 por IP em .1-.4 = 8; a 9a cai.
            for ip in ("127.0.0.2", "127.0.0.3", "127.0.0.4"):
                lentos.append(conecta(ip, porta))
                lentos.append(conecta(ip, porta))
            time.sleep(0.5)
            with conecta("127.0.0.5", porta) as alem:
                assert linha(alem) == b"CHEIO global\n"
            print("global ok: 9a conexao simultanea recusada")

            # Lento nao paralisa: libera 1 slot e o rapido e atendido.
            lentos.pop(0).close()
            time.sleep(0.5)
            with conecta("127.0.0.1", porta) as rapido:
                rapido.sendall(b"ECO oi\n")
                assert linha(rapido) == b"ECO ECO oi\n"
            print("lentos ok: rapido atendido com 7 lentos ocupando slots")

            # Teto de linha: 2000 bytes sem \n fecham so esta conexao.
            with conecta("127.0.0.1", porta) as longa:
                longa.sendall(b"x" * 2000)
                assert linha(longa) == b"ERRO linha-longa\n"
            print("linha ok: linha longa e erro da conexao, servidor vivo")

            # Teto de bytes: 210 KiB de linhas curtas silenciam a conexao
            # ate a proxima janela (fechar aqui geraria RST com a rajada
            # nao lida e o cliente nunca leria o motivo).
            with conecta("127.0.0.1", porta) as rajada:
                rajada.sendall(b"ECO x\n" * 30000)
                achou = False
                resto = b""
                while not achou:
                    parte = rajada.recv(4096)
                    assert parte, "conexao caiu sem avisar"
                    resto += parte
                    while b"\n" in resto:
                        uma, resto = resto.split(b"\n", 1)
                        if uma == b"CHEIO bytes":
                            achou = True
                            break
                assert achou, "CHEIO bytes nao chegou"
                # Esvazia ecos velhos da rajada e espera a janela virar:
                # o reset e preguicoso (so acontece quando ha trafego).
                rajada.settimeout(0.3)
                try:
                    while rajada.recv(4096):
                        pass
                except socket.timeout:
                    pass
                rajada.settimeout(10)
                time.sleep(1.2)
                rajada.sendall(b"ECO de-novo\n")
                assert linha(rajada) == b"ECO ECO de-novo\n"
            print("bytes ok: rajada silenciada com aviso e recuperada na janela seguinte")

            # Servidor segue vivo depois de todas as recusas.
            with conecta("127.0.0.1", porta) as vivo:
                vivo.sendall(b"ECO fim\n")
                assert linha(vivo) == b"ECO ECO fim\n"
            print("23f ok: tetos por-ip, global e de bytes; lentos nao paralisam")
        finally:
            if srv.poll() is None:
                srv.terminate()
            try:
                _, stderr = srv.communicate(timeout=2)
            except subprocess.TimeoutExpired:
                srv.kill()
                _, stderr = srv.communicate()
            assert b"ERROR: AddressSanitizer" not in stderr, stderr
            assert b"runtime error:" not in stderr, stderr


if __name__ == "__main__":
    main()
```

## 4. Dois bugs reais que o teste pegou

**Fecha espúrio no nascimento.** A primeira versão caía em `TimeoutError` na 3ª conexão: o `poll` preenchia `revents` só até `nslots`, mas o laço de clientes visitava também os recém-aceitos no dreno — `revents` lixo de pilha, `fecha` aleatório, contagem corrompida. A falha era determinística com o probe de prontidão no meio (e intermitente sem ele — o pior tipo). A correção é voltar ao `poll` (`continue`) sempre que o dreno aceitar alguém: cliente novo só é tocado na iteração seguinte, com `revents` válido. Conexão recém-aceita morrendo sem motivo é corrupção de contagem com sintoma de rede — o teste a distinguiu do flake porque reproduzia sob demanda.

**`fecha` que conta duas vezes.** O decremento incondicional de `n_conexoes` transformava qualquer duplo-fecho em vaga fantasma: o teto global passava a aceitar 9 com 8 ocupados. A guarda (`fd < 0` retorna sem contar) é invariante de contador, não otimização — e o teste a exercita toda vez que um FIN chega junto com um erro no mesmo fd.

**RST que engole o motivo.** A primeira política de rajada fechava a conexão — e o teste recebia `ConnectionResetError` em vez de `CHEIO bytes`: com 146 KiB não lidos no buffer, `close` responde RST e o motivo se perde (testado empiricamente, inclusive com `SO_LINGER`, que não adianta). A política mudou para throttle com aviso *por causa* do teste: silencia, avisa no canal, recupera na janela seguinte. Teste que muda o desenho é teste que trabalha.

## 5. Erros comuns e implicações defensivas

1. **Teto por conexão fingindo ser global:** limitar linha e achar que o servidor está protegido ignora o ataque de 8 conexões lentas — cada uma dentro de todos os tetos por conexão, todas juntas esgotando o servidor. Teto global se testa com N conexões simultâneas reais, não com uma.
2. **Fechar a rajada com RST:** além de esconder o motivo, o RST descarta os ecos pendentes e o cliente conclui "servidor caiu" em vez de "fui limitado". Sinalizar antes de punir é protocolo; punir sem sinalizar é instabilidade com outro nome — e o teste prova a ordem.
3. **Janela preguiçosa e teste impaciente:** o reset só acontece quando há tráfego (não há timer dedicado). Teste que dorme 1,2 s e manda de novo funciona porque o próprio envio dispara o reset. Assumir reset por tempo sem tráfego é esperar por evento que ninguém agenda.
4. **Contar o probe na aritmética:** a conexão de prontidão ocupa slot e IP até o FIN ser processado. Teste que não considera o probe mede o probe junto — e o teto "falha" por um de contagem do teste, não do servidor.
5. **Recusar sem motivo:** `close` silencioso economiza um `dprintf` e custa um diagnóstico. Toda recusa carrega o nome do teto (`CHEIO global`, `CHEIO por-ip`, `CHEIO bytes`) — motivo é parte do protocolo, não gentileza.
6. **Confiar no `n_conexoes` sem guarda:** contador que só incrementa e nunca valida diverge na primeira condição de corrida entre `accept` e `close`. A guarda do `fecha` e o `continue` pós-dreno são as duas metades do mesmo invariante.
7. **Linha-longa que mata o servidor:** transformar erro de framing em `desligar`/`abort` converte qualquer cliente malformado em DoS. Erro de *conexão* fecha (ou avisa) a conexão; só erro de *processo* desliga o processo — e o teste prova que o servidor segue vivo após cada abuso.

## 6. Ponte com as demais trilhas

C++ faz o mesmo trio de tetos em simulação na [23d](../cpp/23d_limites_globais.md) (orçamento em memória, sem socket — determinístico, sem fio); aqui o socket é real e o teste é o juiz, porque a 31 já provou que o servidor real é testável. Go exige o mesmo par túnel+crachá na 20b sem teto de bytes (o teto lá é de taxa no cliente); Rust isola cliente lento com backpressure na 14b (fila limitada em vez de teto global — mesma família, outro membro). Em Python, o análogo é a 19a (cinco casos de framing, thread por conexão) — e a comparação ensina: thread-por-conexão testa framing, `poll` com tetos testa o servidor. O padrão "teto global + recusa com motivo + lento não paralisa" vale nas cinco trilhas — muda onde a contagem mora e quem a audita (aqui, o teste Python).

## 7. Ao final, você consegue

- [ ] Rodo o teste e digo qual `assert` prova cada um dos cinco comportamentos (por IP, global, rápido-entre-lentos, linha, bytes).
- [ ] Explico por que bytes silenciam (RST engole o motivo) e por que por-IP/global fecham (FIN limpo, nada lido).
- [ ] Descrevo o bug do `revents` lixo: onde o lixo nasce (pf além de nslots), quando morde (iteração com dreno) e por que o `continue` o elimina.
- [ ] Mostro a recuperação na janela seguinte e digo o que a dispara (tráfego, não tempo — reset preguiçoso).
- [ ] Aponto a aritmética dos aliases de loopback no teste e o papel do probe na contagem.
- [ ] Nomeio o que falta: TLS, autenticação, shutdown gracioso, observabilidade (31, §5) — e digo qual teto cada um exigiria.
- [ ] Escrevo um sexto abuso (ex.: rajada lenta de 1 byte/s por 70 s — dribla a janela?) e digo qual teto o pegaria (nenhum: taxa por conexão falta — e essa é a próxima aula que o checklist pede).

**Referências primárias:** manual do `poll`/`accept` (dreno até `EAGAIN`, `revents` válido só para fds consultados), `man 7 socket` (comportamento de `close` com dados não lidos: RST), e *UNIX Network Programming* (Stevens) para throttle vs close. Nenhum texto foi copiado — os dois bugs acima foram pegos executando este teste, e a política de throttle nasceu deles.

## 8. Exercícios obrigatórios

1. (Previsão) Suba `TETO_POR_IP` para 8 e preveja, antes de rodar o teste: qual dos cinco comportamentos quebra primeiro, e em que linha do teste? Rode e explique a aritmética dos aliases que deixou de fechar.
2. (Diagnóstico) Apague o `continue` pós-dreno (troque por `(void)aceitou;`): preveja o sintoma — recusa espúria, contagem corrompida ou teste verde? Rode o teste e explique de onde vem o `revents` que morde o recém-aceito.
3. (Projeto) Escreva o sexto abuso: um cliente que envia uma linha por segundo durante 10 s. Preveja — algum teto morde? Rode contra o servidor e explique qual teto falta (taxa por conexão) e por que nenhum dos três existentes o pegaria.
4. (Previsão) Troque a política da rajada de silenciar para fechar (`close` direto ao estourar a janela): preveja o que o teste recebe — `CHEIO bytes` ou `ConnectionResetError`? Rode e explique por que fechar com 146 KiB não lidos apaga o motivo.

Use [pistas graduais](exercicios/23f_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23f_solucoes.md) depois de registrar sua própria explicação.

---


A execucao bem-sucedida sozinha nao aprova: a maior parcela esta em demonstrar que voce entendeu o porque.

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1* (Addison-Wesley, 3. ed., ISBN 978-0131411555).
