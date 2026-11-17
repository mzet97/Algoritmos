---
layout: default
---

# Aula 10a — pistas graduais

Volte aos [exercícios](../10a_arquitetura_e_memoria_virtual.md#8-exercícios-e-avaliação) antes de ler as soluções. Abra apenas a pista necessária.

1. **Primeira pista:** `bloco[0]` só pode ser escrito depois de `malloc` bem-sucedido. **Segunda:** C não especifica instruções, MMU ou posição física do bloco.
2. **Primeira pista:** a linha do arquivo `/proc/PID/maps` tem intervalo de endereços antes das permissões. **Segunda:** procure a faixa cujo início é menor ou igual ao endereço observado e cujo fim é maior. Não assuma que o bloco aparecerá com o rótulo `[heap]`.
3. **Primeira pista:** anote valores concretos em três linhas, sem alterar o programa. **Segunda:** mudança de endereço pode ter várias causas; uma amostra pequena não isola ASLR, compilador ou alocador.
4. **Primeira pista:** pergunte o que cada estrutura guarda: tradução, dado ou mapeamento/estado de página. **Segunda:** para medir localidade futuramente, compare padrões de acesso no mesmo buffer, controle tamanho e número de repetições e relate ruído; ainda assim, separar cache e TLB exige metodologia adicional.
5. **Primeira pista:** o exemplo imprime valores internos e uma identificação de processo. **Segunda:** antes de multiplicar uma contagem não confiável por `sizeof *p`, verifique tanto overflow de `size_t` quanto um limite operacional de memória.
