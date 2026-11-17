# Pistas da aula 33

1. Copia de frame de tarefa e o tipo de erro que ela esconde: se Tarefa for copiavel, duplicar um TrapFrame salvo e barato e silencioso. Delete o construtor de copia e veja onde o escalonador dependia dela sem dizer.
2. O construtor que nao roda nao da mensagem nem erro: o objeto nasce zerado pela .bss. A prova e pela ausencia — compare com a mesma escrita feita de kmain, que aparece.
3. Sem -fno-exceptions, um throw procura __cxa_throw e a maquinaria de unwind. O erro de link nomeia o simbolo; leia-o como a lista do que um runtime hosted pagaria por voce.
4. size nos dois elfs: diferencas pequenas sao metadados e secoes de excecao zeradas; diferenca grande em .text pede explicacao simbolo a simbolo com nm --size-sort.
