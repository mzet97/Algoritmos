# Pistas da aula 12a

1. O caminho é: chamada do programa → vDSO (se exportar `getpid`) ou instrução de syscall com troca de modo → kernel → retorno. Desenhe os dois ramos e marque onde cada um economiza.
2. Some os tamanhos e alinhamentos na ordem declarada, arredondando cada campo para o próximo múltiplo do seu alinhamento e o total para o alinhamento da struct. Confira com `static_assert`.
3. No pai, troque `waitpid` por `sleep(5)` e rode `ps -o pid,stat,cmd` no PID do filho em outro terminal: o estado `Z` é o zumbi. Restaure o `waitpid`.
4. `std::fill` toca as mesmas páginas, então as faltas coincidem quando nada está residente. Diferença aparece ao retocar: o segundo toque não gera falta. Meça as duas passagens e explique.
