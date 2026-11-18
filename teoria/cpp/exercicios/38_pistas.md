---
layout: default
---

# Aula 38: pistas graduais

Volte aos [exercícios](../38.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Com `max = 4`, volta **4** (verificado) — os 4 primeiros bytes, sem erro. `min(tamanho, max)` é truncamento honesto: o chamador pediu 4, recebeu 4, e sabe que há mais porque conhece o tamanho esperado (a tarefa confere). Leitura parcial só é erro quando o contrato promete tudo-ou-nada — aqui promete "até `max`".
2. `id = 2` com `N_ARQUIVOS = 2` nega (verificado: `ID2_NEGADO`). O maior válido é 1, o menor inválido é 2 — o par prende o off-by-one dos dois lados. `>=` em vez de `>` existe para que o limite seja o primeiro inválido, não o último válido: convenção que evita o `N_ARQUIVOS - 1` espalhado pelo código.
3. Uma linha na tabela + ler/conferir/ecoar na tarefa: `ARQ2_OK`, `FS_OK` (verificado). Nada mudou no handler nem na syscall — e `N_ARQUIVOS` se ajustou sozinho (`sizeof` divide sozinho). Tabela que cresce sem tocar no mecanismo é dado separado de código: o primeiro passo para fora da ROM.
4. **Igual** (verificado: `RELEITURA_IGUAL`). Sem descritor não há onde guardar posição — cada `sys_ler` recomeça do zero. É a diferença para o VFS da 32d em C: lá a posição mora no fd (`fd_pos[id]`), e a segunda leitura continua; aqui não há fd, há só `id` — leitura sem estado. O preço da simplicidade: recomeçar sempre; o valor: nada para corromper entre chamadas.
