# Pistas da aula 14a

1. O caminho é: `f.read` → bytecode avaliado → chamada à libc/`os` → syscall `read` → page cache ou disco → cópia para o objeto `bytes` → retorno. Marque onde cada camada pode falhar.
2. Rode duas vezes seguidas sem reiniciar nada. A segunda encontra as páginas no cache do SO. Explique por que o programa não mudou e o tempo sim.
3. `chmod 000 arquivo` como dono e tente ler como você mesmo; ou use um diretório de outro usuário. Registre o nome exato da exceção.
4. Meça RSS e `tracemalloc` antes, depois de alocar e depois do `del`. O RSS raramente volta: o alocador devolve páginas ao SO por conta própria, não por `del`.
