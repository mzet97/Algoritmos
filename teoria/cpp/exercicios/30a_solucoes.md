# Solucoes comentadas da aula 30a

## 1. A curva de threads

A razao sobe rapido de 1 para poucas threads e achata muito antes de 24: matvec le n^2 numeros para escrever n; a intensidade e baixissima e o recurso disputado e a banda de memoria, que nao escala com nucleos. O joelho da curva e o ponto onde a banda satura; dai em diante, thread nova so adiciona coordenacao.

## 2. Repartir a linha

Com a linha repartida, o resultado e (parcial1 + parcial2 + ...) numa ordem ditada pela particao, nao pela posicao. Ponto flutuante nao e associativo: a igualdade bit a bit com o sequencial deixa de valer e a pos-condicao correta passa a ser erro relativo pequeno contra uma referencia em maior precisao (double), como na aula 30b de C. Quem mantem o assert de igualdade apos repartir a reducao escreveu um teste que reprova a implementacao certa.

## 3. Pool de threads

Criar e juntar 24 threads por chamada custava ~500 microssegundos contra um trabalho de 0,6 microssegundos em n = 40. Com pool, as threads ja existem: acordar, repartir e esperar custa uma fracao disso, e o caso pequeno volta a ser viavel — embora o sequencial puro continue dificil de bater quando nao ha sincronizacao nenhuma. Pool nao elimina coordenacao; elimina nascimento repetido.

## 4. float contra double

Trocar double por float corta o trafego pela metade. Como os dois lados (sequencial e paralelo) sao limitados por banda aqui, os tempos absolutos caem nos dois e a razao muda pouco. Se a razao mudasse muito, o gargalo nao era banda — era calculo — e a investigacao reabriria por ai.
