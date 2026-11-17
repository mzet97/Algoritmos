# Solucoes comentadas da aula 28a

## 1. Indice hostil no caminho da trap

Com indice fora do intervalo, o kernel entra no panic_handler e desliga com 0x3333: quem controla o registrador que virou indice controla a vida do kernel. A correcao de desenho e nao indexar com dado de trap sem validar antes (comparacao explicita e caminho de TRAP_INESPERADA), reservando indexacao direta para indices constantes ou provados. Bounds check e rede de seguranca de desenvolvimento; em kernel, a rede e o fim do sistema.

## 2. Tipo Uart

struct Uart; impl Uart { fn putc(&self, b: u8) { unsafe { write_volatile(UART as *mut u8, b) } } } — os chamadores perdem o unsafe porque a operacao nao tem pre-condicao: qualquer byte, a qualquer momento, pode ir para o THR. O risco nao diminuiu; ele ganhou endereco unico, que e o que auditoria precisa.

## 3. align e o assert

Sem align(4096), satp aponta para o meio de uma pagina qualquer e o walker le lixo como PTE: causa 12 na primeira instrucao de modo-S. O debug_assert em kmain pega em build de debug antes de ligar a traducao; em release, so o tipo garante. Teste sem boot possivel: um teste de usuario que importa o tipo e afirma align_of e o endereco — o defeito vira falha de teste, nao de boot.

## 4. Inventario de unsafe

Irredutiveis (hardware nao tem API segura): escrita MMIO da UART, leitura/escrita de CSR (satp, mie, mstatus, pmp), ecall das tarefas e a chamada para o assembly lancar_tarefa. Eliminaveis por tipo: alinhamento da tabela (feito, repr align), enderecos MMIO constantes encapsulados (Uart/CLINT como tipos), e o layout do TrapFrame — esse ultimo continua manual porque o outro lado e assembly; o tipo repr(C) documenta, nao prova.
