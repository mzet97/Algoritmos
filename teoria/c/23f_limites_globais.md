---
layout: default
---

# Aula 23f — Limites globais medidos: quando o teto é do servidor, não da conexão (C5)

**Competências SDD:** NET, SEC, CON. **Pré-requisitos:** aulas [23b](23b_fuzzing_e_limites.md) (teto do parser), [31](31.md) (servidor `poll` com teste Python) e [23d](23d_consenso_e_quorum.md) (decisão por contagem, não por intenção). Toolchain registrada: GCC com `-std=c11 -Wall -Wextra -Wpedantic -Werror` (e build ASan/UBSan limpo).

**Objetivo observável:** ao terminar, você deverá rodar o teste, apontar qual `assert` prova cada teto (por IP, global, linha, bytes) e explicar por que o excesso de bytes silencia em vez de fechar — e qual bug real o teste pegou antes de qualquer execução de bancada.

**Motivação:** a 23b pôs teto no parser (linha de 1 KiB, mutação sob sanitizer); a 31 pôs o servidor de pé. Faltava o teto do *servidor*: quantas conexões ao todo, quantas por IP, quantos bytes por janela — e, crucialmente, o que acontece com quem estoura. Esta aula entrega os três tetos sobre a base das duas, com o teste provando que cliente lento não paralisa os demais e que a rajada é avisada, silenciada e depois recuperada.

## 1. Contrato antes do código

O [servidor](exemplos/23f_limites.c) (`poll`, sem threads) e o [teste](exemplos/test_23f.py) (sobe, exercita, derruba; falha se houver ASan/UBSan no `stderr`):

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

Tetos: 8 conexões simultâneas, 2 por IP, 64 KiB por janela de 1 s. Recusa em `accept` fecha com motivo (`CHEIO global`, `CHEIO por-ip`); excesso de bytes **silencia** até a próxima janela (`CHEIO bytes`, depois recupera). Linha longa é erro da conexão (`ERRO linha-longa`), nunca do processo.

## 2. Três decisões que o teste amarrou

**Recusar no `accept`, não depois.** Por-IP e global são decididos antes de alocar slot: `dprintf` + `close` imediato, sem ler nada — fechar sem dado não lido pendente entrega o motivo com FIN limpo, sem RST. O teste usa aliases de loopback (`127.0.0.2`–`.4`) porque, com teto por IP em 2, um IP só jamais encheria o global: o teste precisa de quatro identidades para provar o teto de oito.

**Drenar o backlog.** Um `accept` por iteração atrasa a contagem sob rajada e os tetos passam a mentir (conexão aceita tarde demais conta na janela errada). O laço de `accept` até `EAGAIN` com o ouvinte não-bloqueante é a correção — e quem volta ao `poll` antes de tocar nos clientes evita o bug da seção 3.

**Silenciar em vez de fechar na rajada.** Fechar com 146 KiB não lidos no buffer gera RST e o cliente nunca lê o motivo (testado: `SO_LINGER` não adianta — dado recebido e não lido descarta com RST de qualquer jeito). A política honesta é throttle com aviso: marca a conexão como surda, lê-e-descarta até a janela virar, avisa no próprio canal e recupera na janela seguinte. O teste prova as três fases: aviso, silêncio e eco funcionando depois.

## 3. Dois bugs reais que o teste pegou

**Fecha espúrio no nascimento.** A primeira versão caía no `TimeoutError` na 3ª conexão: o `poll` preenchia `revents` só até `nslots`, mas o laço de clientes visitava também os recém-aceitos — `revents` lixo de pilha, `fecha` aleatório. A correção é voltar ao `poll` (`continue`) sempre que o dreno aceitar alguém: cliente novo só é tocado na iteração seguinte, com `revents` válido. Conexão recém-aceita morrendo sem motivo é corrupção de contagem com sintoma de rede — o teste a distinguiu do flake porque era determinística com o probe de prontidão no meio.

**`fecha` que conta duas vezes.** O decremento incondicional de `n_conexoes` transformava qualquer duplo-fecho em vaga fantasma. A guarda (`fd < 0` retorna sem contar) é invariante de contador, não otimização.

## 4. Erros comuns e implicações defensivas

1. **Teto por conexão fingindo ser global:** limitar linha e achar que o servidor está protegido ignora o ataque de 8 conexões lentas — cada uma dentro de todos os tetos por conexão, todas juntas esgotando o servidor. Teto global se testa com N conexões, não com uma.
2. **Fechar a rajada com RST:** além de esconder o motivo, o RST descarta os ecos pendentes e o cliente conclui "servidor caiu" em vez de "fui limitado". Sinalizar antes de punir é protocolo; punir sem sinalizar é instabilidade com outro nome.
3. **Janela preguiçosa e teste impaciente:** o reset só acontece quando há tráfego — teste que dorme 1,2 s e manda de novo funciona porque o próprio envio dispara o reset. Assumir reset por tempo sem tráfego é esperar por evento que ninguém agenda.
4. **Contar o probe na conta:** a conexão de prontidão ocupa slot e IP até o FIN ser processado. Teste que não considera o probe na aritmética dos tetos mede o probe junto.

## 5. Ponte com as demais trilhas

C++ faz o mesmo trio de tetos em simulação na [23d](../cpp/23d_limites_globais.md) (orçamento em memória, sem socket); aqui o socket é real e o teste é o juiz. Go exige o mesmo par túnel+crachá na 20b sem teto de bytes; Rust isola cliente lento com backpressure na 14b. Em Python, o análogo é a 19a (cinco casos de framing, thread por conexão). O padrão "teto global + recusa com motivo + lento não paralisa" vale nas cinco trilhas — muda onde a contagem mora.

## 6. Critério de aprovação e checklist

- [ ] Rodo o teste e digo qual `assert` prova cada um dos quatro tetos.
- [ ] Explico por que bytes silenciam (RST) e por-IP/global fecham (FIN limpo).
- [ ] Descrevo o bug do `revents` lixo e por que o `continue` após o dreno o elimina.
- [ ] Mostro a recuperação na janela seguinte e digo o que a dispara (tráfego, não tempo).
- [ ] Nomeio o que falta: TLS, autenticação, shutdown gracioso, observabilidade (31, §5).

**Referências primárias:** manual do `poll`/`accept` (secção de *edge cases*: dreno até `EAGAIN`, `revents` só válido para fds consultados) e comportamento de `close` com dados não lidos (RST) em *UNIX Network Programming* (Stevens). Nenhum texto foi copiado — os dois bugs acima foram pegos executando este teste.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
