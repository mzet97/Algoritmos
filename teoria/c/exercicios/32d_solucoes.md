---
layout: default
---

# Aula 32d: soluções comentadas

Compare após tentar os [exercícios](../32d_vfs_minimo.md#9-exercícios-obrigatórios). Todas as variantes rodadas no QEMU (`qemu-system-riscv64 -machine virt -cpu rv64 -smp 1 -bios none -kernel kernel.elf`), partindo de `teoria/c/kernel32d/` numa cópia de trabalho.

1. Previsão: `/dado` nega, `/dados//const.txt` resolve (verificado: `PREFIXO_NEGADO_OK`, `BARRA_DUPLA_OK`, `VFS_OK`). O prefixo morre em `m == n` — 4 ≠ 5, antes de qualquer comparação de conteúdo. A barra dupla some no `while (*p == '/') p++` do andador: separador repetido é ruído, não componente. Duas regras, dois destinos: comprimento protege contra prefixo, o pulo de barras tolera digitação — e nenhuma das duas aceita `..`, que tem regra própria.

2. Diagnóstico: sem a guarda, o teste **continua passando** — `VFS_OK`, sem `DOTDOT_PASSOU` (verificado). O `..` vira componente comum, procurado entre os filhos da raiz, não achado, `-1` pelo caminho da busca. O veredito igual é o perigo: a negação passa a depender de a árvore não conter um filho chamado `..`. A guarda nega no parse — antes de existir busca, cache, log ou estado. Com outro suporte montado (o futuro que o contrato lista), a busca poderia achar; o parse negaria do mesmo jeito. Contrato se escreve no parse, não na sorte da tabela.

3. Projeto: uma linha na tabela (`{"novo.txt", 0, 2, arq_novo, ...}`) + abrir/ler/fechar na tarefa → `NOVO_OK`, `VFS_OK` (verificado), sem tocar em `resolver`, `fd_alocar` ou nas três regras. O andador não sabe quantos arquivos existem — só anda, confere e nega. É isso que "resolvedor é função da tabela" significa: a árvore cresceu e o código de resolução nem ficou sabendo. O dia em que a tabela vier de outro dispositivo, o mesmo andador serve.

4. Previsão: reabrir dá fd válido, segundo `close` dá **−1**, `read` pós-close dá **−1** (verificado: `FD_REUSO_NEGADO_OK`). Fechar marca `fd_no[id] = -1`; qualquer uso posterior bate no guarda. Se o close duplo aceitasse, liberaria o slot de quem o reutilizou; se o `read` aceitasse, copiaria bytes do arquivo errado (ou do lixo) para o usuário. Tabela de fds é estado compartilhado do kernel — guarda não é cortesia, é invariante.
