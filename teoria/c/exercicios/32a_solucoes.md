# Soluções comentadas da aula 32a

## 1. Syscall de saída (a7 = 1)

No tratador, junto ao caso 9: se `a7 == 1`, imprima `(char)f[F_A0]` com `kputc` e avance `mepc` em 4. As tarefas trocam `tarefa_putc` por montar `a0`/`a7` e dar `ecall`. O que muda no modelo de confiança: modo-S perde o acesso direto ao dispositivo — a gigapage de dispositivos pode perder R/W para as tarefas em seguida, e toda saída passa a ser mediada, escalonável e auditável pelo kernel. É a diferença entre biblioteca compartilhada e sistema operacional.

## 2. Quantum

Com troca a cada tick (10 ms), as tarefas alternam ~50 vezes por segundo cada uma; caracteres por segundo caem quando o quantum encolhe porque cada troca custa salvar/restaurar 264 bytes de frame mais a entrada/saída da trap. Com quantum de 50 ticks, a intercalação `A`/`B` fica em blocos longos. A medição honesta reporta caracteres/s e trocas/s lado a lado: escalonador se avalia por justiça (contagem por tarefa) e por custo (vazão total), nunca por um dos dois sozinho.

## 3. Falta fora do contrato

A leitura de `0x200000000` produz causa 13/15 com `mtval` nesse endereço; o tratador não o reconhece, imprime `FALTA_INESPERADA` (com causa e `mtval` no diagnóstico) e desliga `0x3333`. Mapear sob demanda qualquer endereço que uma tarefa pedir destruiria o valor da falta como sinal: ponteiro selvagem viraria página válida e o erro apareceria depois, longe e sem causa. Em kernel de produção a resposta análoga é matar a tarefa — aqui, sem isolamento entre tarefas, desligar é a versão honesta de matar o sistema inteiro.

## 4. Páginas de 4 KiB para uma pilha

A raiz ganha uma entrada não-folha para uma tabela L1, que ganha uma entrada não-folha para uma L0, que mapeia as 2 páginas de 4 KiB da pilha (8 KiB). Total: 3 tabelas de 4 KiB para mapear 8 KiB — a sobrecarga é visível de propósito. O que se compra: permissão e presença por página (a pilha vizinha pode ser protegida, a página de guarda pode faltar de propósito). O que se paga: memória de tabelas, mais passos no walk a cada miss de TLB e código de alocação de tabelas que a gigapage não precisava.
