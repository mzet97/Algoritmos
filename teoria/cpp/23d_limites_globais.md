---
layout: default
---

# Aula 23d - Limites globais: o servidor inteiro cabe num orçamento

**Objetivo:** impor os tetos do servidor — conexões simultâneas, bytes por janela, conexões por IP — sobre a base da [23a](23a.md), que já tem os tetos por conexão. Pré-requisitos: aulas 23, 23a e 23b. Linux ou WSL2. Sem QEMU e sem GPU.

Ao terminar, você deverá dizer, para cada abuso (enchente de conexões, rajada de bytes, lentas que ocupam tudo), qual teto o recusa — e onde o check mora no loop.

Esta aula é o módulo CPP-7 na parte de limites: a 23a endureceu a conexão; esta endurece o servidor. O que falta depois dela: TLS/autenticação (certificado, não token) e fuzzing sistemático do parser.

**Como ler esta aula (tres niveis):** *fundamental* -- tres tetos e a tabela de onde cada check mora; *intermediario* -- o mapa check/ponto do loop; *aprofundamento* -- teto sem timeout e a 23a que o cobra.

## 1. Onde o teto por conexão não alcança

Trinta e dois clientes, cada um dentro do seu teto de 64 KiB de saída, somam 2 MiB de pressão — e nenhum teto individual reclama. O orçamento global existe porque recurso é compartilhado: soma de comportamentos legítimos também derruba. Três tetos, três abusos:

1. **Conexões simultâneas (32):** a 33ª é recusada; vaga liberada é reutilizada. Lenta que ocupa slot sem consumir cai aqui — o tempo limite da 23a a remove, o teto a impede de levar as outras junto.
2. **Por IP (4):** o quinto do mesmo IP cai mesmo com vagas globais. Um hospedeiro, quatro vozes — contenção de origem, não de destino.
3. **Bytes de entrada por janela (1 MiB):** rajada acima do orçamento é cortada; janela nova reabre. Pico passa, abuso contínuo não.

## 2. Por que três tetos, e não um?

Porque cada abuso estoura uma dimensão diferente: quantidade (conexões), origem (IP) e volume (bytes). Um teto só de conexões deixa a rajada passar; só de bytes deixa as lentas ocuparem tudo; só por IP deixa a enchente distribuída entrar. Defesa em profundidade não é ter muitos tetos — é ter um teto por dimensão que o atacante controla.

## 3. Onde cada check mora no loop da 23a

O exemplo simula a admissão isolada, mas o objetivo da aula exige dizer onde cada check entraria no servidor real — então aqui está o mapa, ponto a ponto do loop `poll`:

| Ponto do loop | Check | Por que aqui e não depois |
|---|---|---|
| `accept` retorna | teto global (32) + por IP (4): contar → comparar → admitir | depois do `accept`, o descritor já existe; recusar aqui é fechar na hora, antes de alocar estado. Admitir antes de contar vaza uma vaga por decisão (erro nº 1) |
| `poll` sinaliza legível | janela de bytes (1 MiB): somar → comparar → cortar ou drenar | o byte já chegou ao kernel; o teto decide se a aplicação o consome ou o descarta. Cortar aqui protege o processamento, não o socket |
| fim da iteração / `close` | liberar vaga global + contador do IP | vaga não devolvida é vazamento de capacidade (erro nº 2). Liberação mora no caminho de saída — todos eles, incluindo erro e timeout |
| `nova_janela` (timer) | zerar o contador de bytes, manter o resto | janela é tempo, não castigo: só o contador de volume reabre; conexões e IPs continuam valendo. Reabrir tudo junto transformaria pico em passe-livre |

A regra geral: teto se cobra no ponto onde o recurso é consumido (admissão, leitura, saída, tempo) — nunca "no fim do loop, se der". Cada linha da tabela é um lugar onde o recurso muda de dono, e é ali que o orçamento é conferido.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23d_limites.cpp) simula a admissão (`g++` e `clang++`, `-Werror` nos dois):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23d_limites.cpp -o srv23d
./srv23d
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23d_limites.cpp -o srv23d_clang
./srv23d_clang   # idêntico
```

Com sanitizers (exigido para ponte: ASan/UBSan limpos):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/cpp/exemplos/23d_limites.cpp -o srv23d_asan
./srv23d_asan
```

Saída esperada (determinística):

```text
conexoes: teto global de 32 cobrado, vaga reutilizada
por IP: 5a conexao do mesmo IP recusada
bytes: rajada cortada na janela, liberada na proxima
lentas: 32 ociosas bloqueiam a vitima (tempo limite resolve na 23a)
23d ok: tres tetos, tres abusos recusados
```

Um bug real foi pego no caminho: a primeira versão admitia 32 conexões do mesmo IP e esperava sucesso — o teto por IP (4) recusava a quinta. O teste estava errado, não o teto: lenta de verdade se distribui, e o exemplo agora distribui em 8 origens.

### Leitura guiada: cinco linhas, três dimensões

As três primeiras linhas cobrem as três dimensões (quantidade, origem, volume) — cada uma com verbo próprio: conexões "cobradas" (o teto contou e admitiu até 32), IP "recusado" (a 5ª caiu), bytes "cortados e liberados" (o corte é temporário, a janela reabre). A quarta linha é a confissão: 32 ociosas bloqueiam a vítima *neste estágio*, e o parêntese aponta para a 23a — teto sem timeout é meia defesa, e a aula diz isso na saída em vez de esconder no texto. A quinta é o veredito sobre o conjunto. Repare que a saída documenta um bloqueio real (as 32 ociosas) como comportamento observado, não como falha: sob teto sem timeout, servidor cheio de lentas é o estado correto do modelo — e é exatamente por ser correto que o timeout precisa existir.

## 5. Erros comuns e implicações defensivas

1. **Contar o IP depois de decidir:** admitir e só então incrementar vaza uma a mais por condição de corrida lógica. A ordem é contar, comparar, admitir.
2. **Esquecer de liberar:** vaga não devolvida é vazamento de capacidade — o servidor "cheio" com 3 clientes reais. Todo `admitir` tem um `liberar` no caminho de saída.
3. **Janela que nunca reabre:** teto de bytes sem `nova_janela` vira negação permanente após o primeiro pico. Janela é tempo, não castigo.
4. **Tratar recusa como erro interno:** 33ª conexão recusada é operação normal sob carga, não incidente. Loga-se como capacidade, alerta-se por tendência.
5. **Achar que teto substitui timeout:** teto limita quantos entram; timeout limita quanto tempo ficam. Lenta dentro de todos os tetos só sai pelo relógio.
6. **Cobrar o teto longe do consumo:** verificar conexões "no fim do loop" admite 32 + N antes de olhar — a vaga vaza entre o `accept` e o check. Teto se cobra onde o recurso muda de dono (a tabela da §3), nunca em ponto conveniente.
7. **Somar sem janela:** acumular bytes "desde o boot" transforma o primeiro pico legítimo em bloqueio permanente. Todo contador de volume precisa de expiração — sem `nova_janela`, o teto é armadilha para tráfego normal com o tempo.

## 6. Ponte com as demais trilhas

C cobra teto de linha e conexões na 32c; Go limita corpo e conexões no middleware da aula 20; Python corta corpo com `MaxBytesReader`. Em JavaScript, o análogo é o rate limit do gateway — mesma soma, mesma recusa, outro nome.


### Pesos da avaliacao

| Criterio | Peso |
|---|---|
| Tetos executados + mapa check/ponto preenchido | 35% |
| Timeout declarado como falta (nao fingido) | 25% |
| Diagnostico (prever recusa por teto antes de rodar) | 25% |
| Documentacao (comandos, saidas) | 15% |

**Referencias primarias:** `man poll`, `man accept` para dreno e contagem; *UNIX Network Programming* (Stevens).

## 7. Critério de aprovação e checklist

- [ ] Aponto os três tetos e o abuso que cada um recusa, com o número.
- [ ] Mostro onde cada check entraria no loop da 23a (admissão, consumo, liberação).
- [ ] Explico por que a lenta cai no teto de conexões e sai pelo timeout.
- [ ] Distingo recusa normal (capacidade) de incidente (tendência).
- [ ] Nomeio o que falta: TLS/mTLS e fuzzing sistemático do parser.

**Referências primárias:** [OWASP Rate Limiting](https://cheatsheetseries.owasp.org/cheatsheets/Denial_of_Service_Cheat_Sheet.html) para as dimensões de abuso; `man 2 poll` para onde os checks moram no loop. Nenhum teto desta aula substitui autenticação — limite diz *quantos*, identidade diz *quem*.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*

---

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1*.
