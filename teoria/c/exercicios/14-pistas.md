# Aula 14 - Pistas graduais

Consulte as pistas depois de tentar os exercícios da aula.

## Contador com mutex e atomic

1. Identifique todos os acessos ao mesmo inteiro, incluindo a leitura final.
2. Proteja a operação inteira de leitura, soma e escrita. Junte as threads antes de ler o resultado.
3. O contador atomico pode usar ordem relaxada somente porque ele nao publica outros dados.

## Produtor e consumidor

1. A fila e o predicado de disponibilidade pertencem ao mesmo mutex.
2. O consumidor testa o predicado em um laço; esperar libera o mutex e o readquire.
3. Defina encerramento quando o produtor terminar, para que o consumidor nao espere para sempre.

## Tempo de vida

1. Cada thread tem stack propria; isso nao impede que outra thread receba um ponteiro para ela.
2. O objeto apontado precisa continuar vivo ate o ultimo uso pela outra thread.
