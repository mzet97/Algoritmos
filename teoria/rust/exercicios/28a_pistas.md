# Pistas da aula 28a

1. O indice vindo de registrador entra na indexacao de um array estatico: fora do intervalo, o bounds check chama o panic_handler — que desliga. No caminho da trap, desligar por um indice significa que dado de trap (controlado por quem provocou a trap) escolhe se o kernel vive.
2. O tipo Uart guarda o endereco como constante privada; putc e seguro porque escrever num registrador de UART nao tem pre-condicao violavel por quem chama. Os unsafe de MMIO somem dos chamadores e se concentram no impl — o risco total nao some, fica contavel num lugar so.
3. O debug_assert sobre o endereco da tabela roda em kmain (antes de ligar satp) nos builds de debug; em release ele some. Ele pega o defeito no boot de debug, nao no de producao — por isso o align no tipo continua sendo a correcao de verdade.
4. Irredutiveis: MMIO, CSR e a fronteira com o assembly. Eliminavel com tipo: alinhamento (feito), endereco de UART (exercicio 2), layout do frame (repr C continua manual contra o .S).
