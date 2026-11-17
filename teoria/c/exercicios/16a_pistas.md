# Pistas da aula 16a

1. O caminho é: chamada na libc → (vDSO atende direto *ou* instrução de syscall com troca de modo) → kernel executa → retorno com troca de volta. Desenhe os dois ramos e marque onde cada um economiza.
2. No pai, troque `waitpid` por `sleep(5)` e rode `ps -o pid,stat,cmd -p <pid-do-filho>` em outro terminal: o estado `Z` é o zumbi. Depois restaure o `waitpid`.
3. `memset` de 8 MiB toca todas as páginas de uma vez; o laço passo-4096 toca as mesmas páginas uma a uma. Conte faltas nos dois casos e explique por que são iguais — e em que cenário difeririam (dica: páginas já residentes).
4. `/bin/true` sai com `0` após `exec` bem-sucedido; o caminho inexistente faz `execl` retornar e o filho sai com `127`. Registre os dois.
