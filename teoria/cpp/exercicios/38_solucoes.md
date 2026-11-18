---
layout: default
---

# Aula 38: soluções comentadas

Compare após tentar os [exercícios](../38.md#7-exercícios-obrigatórios). Variantes sobre `teoria/cpp/kernel38/`, compiladas com as flags do `Makefile` do estágio e rodadas no QEMU.

1. Previsão: retorna **4** (verificado: `MAX4_N=4`, `FS_OK` depois). `n = min(11, 4)`: o kernel devolve o que coube, sem erro. Leitura parcial é dado porque o contrato diz "até `max`" — erro seria prometer tudo-ou-nada e entregar metade. Quem decide o resto é o chamador: ele conhece o tamanho esperado (a tarefa confere `n0 != sizeof - 1`) e pede de novo... recomeçando do zero, como prova o exercício 4.

2. Diagnóstico: **negado** (verificado: `ID2_NEGADO`, `FS_OK` depois). Com `N_ARQUIVOS = 2`, o 1 passa e o 2 cai em `id >= N_ARQUIVOS` → `-1`. O par (maior válido, menor inválido) é o que prende o off-by-one: um lado só prova metade. `>=` faz do limite o primeiro inválido — convenção que dispensa `N_ARQUIVOS - 1` espalhado: o tamanho da tabela é a primeira resposta "não".

3. Projeto: com `novo.txt` na tabela + ler/conferir/ecoar na tarefa: `ARQ2_OK`, `FS_OK`, `DONE` (verificado). Não mudou: handler, syscall, convenção de retorno — e `N_ARQUIVOS` se ajustou sozinho via `sizeof`. Tabela separada de mecanismo: o handler percorre o que a tabela declara. É o primeiro passo para fora da ROM — o dia em que a tabela vier de outro suporte, o mesmo `sys_ler` serve.

4. Previsão: **igual** (verificado: `RELEITURA_IGUAL`, `FS_OK` depois). Sem descritor não há onde guardar posição: cada chamada recomeça do byte 0. Compare com o VFS da 32d em C: lá a posição mora no fd (`fd_pos[id]`), a segunda leitura continua de onde a primeira parou, e fechar libera o cursor. Aqui não há cursor porque não há `open` — há só `id`. O preço: recomeçar sempre (ler o fim exige ler o começo). O valor: nada para corromper, vazar ou sincronizar entre chamadas — ROM sem estado é ROM sem bug de estado.
