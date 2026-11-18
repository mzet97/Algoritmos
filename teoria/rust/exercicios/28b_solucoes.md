---
layout: default
---

# Solucoes comentadas da aula 28b

## 1. EntradaIdt como tipo

#[repr(C, packed)] struct com campos offset_baixo: u16, seletor: u16, ist: u8, tipo_atributos: u8, offset_medio: u16, offset_alto: u32, reservado: u32. nova(endereco) fatia com mascaras e shifts. O teste com 0x1122334455667788 afirma as tres fatias e os campos fixos (seletor 0x08, tipo 0x8E). O erro classico de montar o offset de uma vez nao compila contra este tipo.

## 2. Stub para kmain Rust

O stub e o boot.S inteiro deste componente ate o modo longo; a unica mudanca e o destino do ultimo salto: em vez de seguir em assembly, ele acerta a pilha e chama kmain (extern "C", #[unsafe(no_mangle)]). Tudo antes disso continua assembly porque sao instrucoes que so fazem sentido naquela ordem privilegiada — Rust comeca onde o modo longo comeca.

## 3. Pilha de trap precisa

RISC-V: a troca para a pilha de trap acontece sempre, porque foi programada via mscratch no trap_entry. x86 neste componente: sem IST e sem troca de anel, a CPU segue na pilha atual; com IST configurado no TSS, ela troca para a pilha do vetor. A correspondencia correta e condicional: mscratch equivale ao IST quando o IST esta configurado; sem ele, nao ha equivalente em acao.

## 4. O que falta e por que basta

Kernel x86 completo pediria syscall, APIC com timer, acordar APs e drivers — outro projeto inteiro. O R8 promete kernel RISC-V completo (entregue na 28a) e etapa comparativa (entregue aqui): cobrar o segundo kernel inteiro seria trocar a promessa depois da entrega. A fronteira declarada na aula e a resposta.
