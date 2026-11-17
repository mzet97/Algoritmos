# Pistas da aula 16a

1. O caminho é: `read` na biblioteca padrão → chamada ao kernel com troca de modo → kernel lê zero bytes do dispositivo nulo → retorno. `/dev/null` nunca bloqueia nem falha aqui: o número mede a travessia, não I/O.
2. Linha de 64 bytes comporta oito `u64`. Passo 8 usa uma palavra por linha; passo 64 pula linhas inteiras e pode acionar (ou confundir) o prefetcher. Meça os três e explique a curva.
3. Use um script que sai com `exit 3` e leia `status.code()`. Compare com morte por sinal (se souber gerar uma) e o `None` correspondente.
4. Meça RSS antes, depois de alocar e depois do `drop` + nova medição. O RSS raramente volta ao valor inicial: o alocador retém páginas por conta própria.
