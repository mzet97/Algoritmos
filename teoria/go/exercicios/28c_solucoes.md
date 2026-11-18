---
layout: default
---

# Solucoes comentadas da aula 28c

1. Com `gera(c, 3.5)` só no trabalhador, o combinado diverge (`1.5393200013312e+13 vs 1.53931947704315e+13`): metade da soma usa um `c` que a referência não tem. O checksum combinado prova que o protocolo transportou as metades corretamente — a divergência acusa derivação, não transporte (erro nº 6). A ordem das linhas da saída existe por isso: referência primeiro (derivação íntegra), confronto depois.
2. Com `%.6g`, o combinado chega com 7 dígitos (`1.539321e+13`) contra 15 da referência: a serialização textual truncou a soma antes do confronto, e `!=` em `float64` não perdoa. `%.17g` é o round-trip canônico (17 dígitos garantem ida-e-volta exata de qualquer `float64`); protocolo que abrevia número fabrica divergência. A precisão do fio faz parte do contrato, como o framing da 23a.
3. O trabalhador que responde `ERRO induzido` é pego na leitura da resposta: `trabalhador falhou: protocolo: ERRO induzido`, sem esperar o deadline de 60 s. O deadline acusa quem cala (accept, envio, resposta que nunca chega); quem fala errado é pego pela validação de prefixo na fase de leitura. As três fases têm erros nomeados distintos — e o exercício mostra dois deles sem precisar de 60 segundos.
4. Sem `-race`: checksum idêntico (determinismo não depende de instrumento), `5.911 GB/s em 8.5ms` contra `0.682 GB/s em 73.8ms` sob `-race` — quase 9x de diferença absoluta, razão ~0.52x nos dois. Por isso correção se verifica com `-race` (ambos os lados instrumentados, confronto válido) e tabela se publica sem (número absoluto honesto). Quem compara tempo com `-race` contra binário limpo comete o erro nº 1; quem publica razão sem máquina e sem instrumento comete o nº 4.
