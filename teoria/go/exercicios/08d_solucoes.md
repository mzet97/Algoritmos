# Solucoes comentadas da aula 08d

GC devolve objetos, nao necessariamente memoria ao SO: HeapSys estavel com HeapAlloc menor e o comportamento correto. Leitura de arquivo materializa page cache (contabilidade do kernel, nao do seu RSS na mesma proporcao). Threads do SO respondem a bloqueio real, nao a contagem de goroutines. E a limitacao classica de RSS: compartilhado contado muitas vezes; PSS existe para repartir.
