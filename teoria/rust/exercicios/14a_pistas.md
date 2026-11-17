# Pistas da aula 14a

1. O cliente envia 1 byte por `write`; o servidor acumula em `read_until` até o `\n`. Marque onde cada byte mora entre o envio e a resposta: buffer do SO, `BufReader`, `linha`.
2. Baixe o teto para 64 e liste os comprimentos que passam: só linhas de até 64 bytes recebem eco; o resto recebe o erro protocolado. O teste "grande demais" continua passando — explique por quê.
3. Sem `set_read_timeout`, o `read_until` bloqueia para sempre no cliente mudo: a thread vaza e `lentos` nunca sobe. Use `timeout 10` por fora e observe a thread presa.
4. Abra 40 conexões de uma vez sem ler as respostas: as 8 excedentes devem ser recusadas (contador sobe) enquanto as 32 recebem eco. Se todas travarem, o teto não está sendo respeitado.
