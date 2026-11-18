---
layout: default
---

# Aula 30c: soluções comentadas

Compare após tentar os [exercícios](../30c_numa_primeiro_toque.md#9-exercícios-obrigatórios). Experimentos sobre `teoria/c/exemplos/30c_numa.c` com `gcc -std=c11 -Wall -Wextra -Wpedantic -Werror`. Números desta máquina (1 nó); refaça na sua e compare ordens de grandeza.

1. Previsão: cai — de 18,9 GiB/s para 1,1 GiB/s (verificado, ~17×). As linhas de cache tocadas são as mesmas; o que multiplica por 64 é o trabalho por linha (soma + incremento por byte). Passo 64 mede a banda da máquina; passo 1 mede o overhead do laço. Quem mede com passo 1 e conclui "memória lenta" mediu o próprio código.

2. Diagnóstico: sem a `soma`, com `-O2`, a varredura "roda" em 0,00 s a 1.908.450 GiB/s (verificado). O laço sem efeito observável foi eliminado (DCE) — o cronômetro mediu o otimizador. A `soma` impressa no final é a âncora: todo benchmark honesto consome o que mede (`black_box`, acumulador impresso, checksum — o nome muda, a função é a mesma).

3. Projeto: escrita 14,6 GiB/s contra leitura 21,4 GiB/s (verificado). A escrita aloca a linha antes de escrever (write-allocate: traz a linha, depois modifica) — tráfego extra que a leitura pura não paga. Detalhe honesto: `memset` repetido pode bater em cache quente e otimizações de streaming store mudam o quadro; o ponto do exercício é o caminho da linha, não o recorde. Em produção, a diferença decide onde zerar buffers grandes (perto do uso, não longe).

4. Previsão: não muda dentro do ruído (1 nó = um só destino para qualquer política). Interleave espalha páginas entre nós; com um nó, "espalhar" ancora tudo no mesmo lugar. As três linhas do protocolo (`membind=0` local, `membind=1` remoto, `--interleave=all` espalhado) só distinguem com 2+ nós — aqui são promessa escrita, e a aula as declara assim em vez de fingir medição.
