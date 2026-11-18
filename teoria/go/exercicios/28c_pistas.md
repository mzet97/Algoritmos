---
layout: default
---

# Aula 28c: pistas graduais

Volte aos [exercícios](../28c_multino_simulado.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. Metade da soma vem do trabalhador com a semente trocada — o checksum combinado carrega metade de cada mundo. O erro nº 6 já avisa: checksum prova transporte sobre derivação idêntica, não conta certa. (Verificado: `checksum divergiu: 1.5393200013312e+13 vs 1.53931947704315e+13`.)
2. `%.6g` entrega 6 dígitos significativos; a soma tem 14. O `panic` mostra os dois lados — compare quantos dígitos sobrevivem no combinado. Protocolo que abrevia número abrevia veredito. (Verificado: `checksum divergiu: 1.539321e+13 vs 1.53931947704315e+13`.)
3. O coordenador lê a resposta com `ReadString` e valida o prefixo `OK` — qualquer outra coisa vira erro de protocolo na hora, sem esperar nada. O deadline só estouraria se o trabalhador calasse; falando errado, ele é pego na leitura. (Verificado: `trabalhador falhou: protocolo: ERRO induzido`.)
4. O checksum é determinístico (mesma semente, mesma conta) — instrumento não muda soma. O tempo muda porque o `-race` serializa e aloca: compare os dois pares GB/s e ms. (Verificado: sem `-race`, `5.911 GB/s em 8.5ms`; com `-race`, `0.682 GB/s em 73.8ms`; checksum igual; razão ~0.52x nos dois.)
