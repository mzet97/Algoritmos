# Aula 23 - Soluções comentadas e rubrica

## Parser de linha

[aula23.c](../exemplos/aula23.c) mostra o núcleo executavel: o estado conserva a linha parcial entre chamadas e continua apos cada LF no mesmo bloco. NUL e excesso sao rejeitados antes de copiar para fora do array. EOF com len maior que zero significa mensagem incompleta. O exemplo imprime linhas; num servidor, a funcao que recebe a linha faria a validacao semantica.

## Cabecalho de tamanho

Para formato [tipo:1][tamanho:4][payload], guarde estado de cabecalho parcial. Converta os quatro bytes por deslocamentos sobre uint32_t, em ordem de rede, sem fazer cast de struct. Rejeite tipo desconhecido e comprimento maior que MAX_PAYLOAD antes de alocar. Confira que tamanho <= SIZE_MAX - 5 antes de somar cabecalho. Depois espere exatamente o payload anunciado; qualquer byte seguinte inicia novo quadro. EOF no meio do cabecalho ou payload e erro de protocolo.

## Backpressure

Uma politica valida: fila de saida limitada por cliente; ao exceder o limite, suspender leitura ate drenar abaixo de um limiar menor. Outra politica didatica e fechar o cliente. Em ambos os casos, estime o limite agregado para todas as conexoes e mantenha timeout contra clientes que enviam um byte por intervalo longo.

## Rubrica de 10 pontos

- 3: fragmentacao e varias mensagens por leitura corretas.
- 2: limites e calculos de tamanho sem overflow.
- 2: EOF parcial, NUL e entrada invalida com politica explicita.
- 2: teste reproduzivel de entradas adversas.
- 1: backpressure e consumo maximo de memoria justificados.
