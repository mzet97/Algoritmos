---
layout: default
---

# Aula 27 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../27.md#exercícios-obrigatórios).

1. O trecho adaptado para o laco principal (no kernel da [aula 28a](28a_pistas.md), o UART do QEMU `virt` fica em `0x1000_0000`):

```rust
// Dentro do laco principal (nunca retorna):
loop {
    // Safety: 0x1000_0000 e o registrador de dados/status do UART0
    // no mapa do QEMU virt (ver aula 28a). read_volatile impede o
    // compilador de transformar a leitura em constante ou elimina-la,
    // porque o valor muda por acao do hardware, nao do programa.
    let status = unsafe { core::ptr::read_volatile(0x1000_0000 as *const u32) };
    if status > 10 {
        // Safety: mesmo registrador; a escrita volatil chega ao device.
        unsafe { core::ptr::write_volatile(0x1000_0000 as *mut u32, 0) };
    }
    core::hint::spin_loop();
}
```

Declaracao de verificacao: este trecho **nao foi executado** nesta rodada — ele pertence ao kernel freestanding e muda o comportamento do laco principal; a verificacao dele e bootar o kernel da 28a com a modificacao e observar o UART, que fica como laboratorio guiado. O que esta verificado aqui e a semantica: sem `volatile`, um laco que le o mesmo endereco sem efeitos observaveis pode ter a leitura removida pelo otimizador (o programa "para de ver" o botao); com `volatile`, cada volta le de verdade. Endereco de MMIO nao e memoria: cache, reordenacao e otimizacao precisam ser contidos explicitamente, e em hardware real o endereco e o registrador exato vem do manual/PAC do chip, nao de um numero magico.

2. PAC (Peripheral Access Crate) e a camada gerada automaticamente a partir do arquivo SVD do fabricante: tipos e constantes para cada registrador, quase uma transcricao do manual, ainda unsafe na pratica. HAL (Hardware Abstraction Layer) e a camada escrita a mao sobre o PAC que entrega API segura e idiomatica — `led.on()`, `uart.write(...)` — escondendo registradores atras de tipos com donos e estados. Regra curta: PAC e o que o chip **tem**; HAL e como voce **usa** sem tocar registrador cru.

**Autoverificacao:** o endereco explicado como device (nao memoria), o papel do `volatile` dito sem mencionar "thread" e PAC/HAL separados em uma frase cada.
