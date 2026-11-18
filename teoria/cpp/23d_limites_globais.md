---
layout: default
---

# Aula 23d - Limites globais: o servidor inteiro cabe num orçamento

**Objetivo:** impor os tetos do servidor — conexões simultâneas, bytes por janela, conexões por IP — sobre a base da [23a](23a.md), que já tem os tetos por conexão. Pré-requisitos: aulas 23, 23a e 23b. Linux ou WSL2. Sem QEMU e sem GPU.

Ao terminar, você deverá dizer, para cada abuso (enchente de conexões, rajada de bytes, lentas que ocupam tudo), qual teto o recusa — e onde o check mora no loop.

Esta aula é o módulo CPP-7 na parte de limites: a 23a endureceu a conexão; esta endurece o servidor. O que falta depois dela: TLS/autenticação (certificado, não token) e fuzzing sistemático do parser.

## 1. Onde o teto por conexão não alcança

Trinta e dois clientes, cada um dentro do seu teto de 64 KiB de saída, somam 2 MiB de pressão — e nenhum teto individual reclama. O orçamento global existe porque recurso é compartilhado: soma de comportamentos legítimos também derruba. Três tetos, três abusos:

1. **Conexões simultâneas (32):** a 33ª é recusada; vaga liberada é reutilizada. Lenta que ocupa slot sem consumir cai aqui — o tempo limite da 23a a remove, o teto a impede de levar as outras junto.
2. **Por IP (4):** o quinto do mesmo IP cai mesmo com vagas globais. Um hospedeiro, quatro vozes — contenção de origem, não de destino.
3. **Bytes de entrada por janela (1 MiB):** rajada acima do orçamento é cortada; janela nova reabre. Pico passa, abuso contínuo não.

## 2. Por que três tetos, e não um?

Porque cada abuso estoura uma dimensão diferente: quantidade (conexões), origem (IP) e volume (bytes). Um teto só de conexões deixa a rajada passar; só de bytes deixa as lentas ocuparem tudo; só por IP deixa a enchente distribuída entrar. Defesa em profundidade não é ter muitos tetos — é ter um teto por dimensão que o atacante controla.

## 3. Exemplo completo e executável

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

## 4. Erros comuns e implicações defensivas

1. **Contar o IP depois de decidir:** admitir e só então incrementar vaza uma a mais por condição de corrida lógica. A ordem é contar, comparar, admitir.
2. **Esquecer de liberar:** vaga não devolvida é vazamento de capacidade — o servidor "cheio" com 3 clientes reais. Todo `admitir` tem um `liberar` no caminho de saída.
3. **Janela que nunca reabre:** teto de bytes sem `nova_janela` vira negação permanente após o primeiro pico. Janela é tempo, não castigo.
4. **Tratar recusa como erro interno:** 33ª conexão recusada é operação normal sob carga, não incidente. Loga-se como capacidade, alerta-se por tendência.
5. **Achar que teto substitui timeout:** teto limita quantos entram; timeout limita quanto tempo ficam. Lenta dentro de todos os tetos só sai pelo relógio.

## 5. Ponte com as demais trilhas

C cobra teto de linha e conexões na 32c; Go limita corpo e conexões no middleware da aula 20; Python corta corpo com `MaxBytesReader`. Em JavaScript, o análogo é o rate limit do gateway — mesma soma, mesma recusa, outro nome.

## 6. Critério de aprovação e checklist

- [ ] Aponto os três tetos e o abuso que cada um recusa, com o número.
- [ ] Mostro onde cada check entraria no loop da 23a (admissão, consumo, liberação).
- [ ] Explico por que a lenta cai no teto de conexões e sai pelo timeout.
- [ ] Distingo recusa normal (capacidade) de incidente (tendência).
- [ ] Nomeio o que falta: TLS/mTLS e fuzzing sistemático do parser.

**Referências primárias:** [OWASP Rate Limiting](https://cheatsheetseries.owasp.org/cheatsheets/Denial_of_Service_Cheat_Sheet.html) para as dimensões de abuso; `man 2 poll` para onde os checks moram no loop. Nenhum teto desta aula substitui autenticação — limite diz *quantos*, identidade diz *quem*.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
