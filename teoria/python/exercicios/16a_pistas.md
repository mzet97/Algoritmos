# Pistas da aula 16a

1. `total += 1` lê, soma e escreve em três passos de bytecode. Com 8 threads, duas podem ler o mesmo valor antes de qualquer escrita: uma atualização some. Desenhe o entrelaçamento.
2. Rode 5 vezes e anote os 5 totais sem lock. A dispersão é o dado: explique-a com escalonamento, não com sorte.
3. Ache o `n` em que processos empatam com threads na sua máquina, subindo de 100 mil. Anote CPU, Python, tempos e o ponto de virada.
4. Use `threading.Event` checado no laço do worker e `join` com timeout no final. Prove que todas terminam listando estados finais.
