# Solucoes comentadas da aula 33

## 1. Tarefa nao copiavel

Com o construtor de copia deletado, qualquer duplicacao acidental de Tarefa (passagem por valor, atribuicao de structs no escalonador) vira erro de compilacao na hora, em vez de dois donos do mesmo frame salvo. O que continua por sua conta: a semantica da troca (qual frame e o atual), que tipo nenhum decide.

## 2. Construtor global mudo

Nenhuma mensagem aparece porque ninguem chama os construtores: nao ha runtime, .init_array nao e percorrida e a .bss zerada entrega o objeto em estado bruto. Num hosted, o loader e o __libc_start_main percorrem .init_array antes do main e registram saidas via __cxa_atexit. A correcao aqui e inicializacao explicita em kmain (preparar_tarefa), nomeada e auditavel.

## 3. O simbolo que falta

O link acusa simbolos como __cxa_throw/_Unwind_Resume (os nomes exatos sao o recibo): e a maquinaria de excecoes que o -fno-exceptions dispensa e que, ligada sem runtime, nao tem quem a sustente. Excecao em kernel freestanding nao e recurso esperando uso; e uma dependencia nao instalada.

## 4. Tamanhos

Os dois kernel.elf diferem pouco: secoes de debug/metadados e, conforme as flags, tabelas de excecao vazias. Diferenca relevante em .text viria de codigo diferente de verdade — e a aula exige diff comportamental zero, logo diferenca grande e suspeita, nao curiosidade.
