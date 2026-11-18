---
layout: default
---

# Aula 30c: pistas graduais

Volte aos [exercícios](../30c_numa_primeiro_toque.md#9-exercícios-obrigatórios) antes de abrir as soluções.

1. Passo 1 toca 64 MiB de endereços; passo 64 toca 1 MiB de endereços nas *mesmas* linhas de cache. O dado que viaja é o mesmo (uma linha por vez) — o que muda é o trabalho por linha: 64 somas e 64 incrementos contra 1. A banda cai pelo overhead, não pelo dado. Passo 64 mede a máquina; passo 1 mede o laço.
2. Sem consumo observável, o laço não tem efeito — e o `-O2` o apaga inteiro. O tempo vai a ~zero e a banda a milhões de GiB/s: mais rápida que a luz, porque nada viajou. A `soma` impressa no final é a âncora que impede a eliminação: benchmark honesto consome o que mede.
3. Escrita aloca a linha antes de escrever (write-allocate: lê a linha, depois modifica) — a menos do truque sem-tempo desta aula, escrita pura tende a custar *mais* que leitura pura pelo tráfego extra. Meça e compare ordens de grandeza, não dígitos: o ponto é o caminho da linha, não o recorde.
4. Com 1 nó, toda política de interleave ancora no mesmo lugar — não há "outro nó" para espalhar. A banda repete a local dentro do ruído. O protocolo (local, remoto, espalhado) só distingue com 2+ nós; aqui ele é promessa escrita, não medição feita — e a aula o declara assim.
