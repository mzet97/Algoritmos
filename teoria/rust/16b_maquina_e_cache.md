---
layout: default
---

# Aula 16b — Máquina vista de Rust: localidade, syscall e página

**Competências SDD:** ARQ, OS, MEM. **Pré-requisitos:** aulas 15–16, [16a](16a_maquina_e_so.md) (o mesmo território em espaço de usuário) e a aula 10a de C (hierarquia de cache e página). Esta ponte fecha o R4: ISA e ABI continuam nas aulas 15–16; aqui medem-se cache, fronteira de syscall e página a partir de um binário Rust comum.

**Objetivo observável:** medir, no mesmo binário, a soma de uma matriz de 2048×2048 por linhas e por colunas (mesma conta, razões esperadas >10×), o custo de 200 mil leituras de 1 byte de `/dev/zero` contra acessos a memória já mapeada, e ler o tamanho virtual do processo de `/proc/self/statm`.

**Motivação:** "Rust é rápido como C" é uma afirmação sobre código gerado; esta aula a verifica onde ela pode falhar sem ser culpa da linguagem: padrão de acesso à memória e custo de fronteira com o kernel. O hardware não sabe em que linguagem o laço foi escrito — e é exatamente por isso que medir é mais instrutivo do que repetir o slogan.

## Contrato e exemplo

O [arquivo-fonte](exemplos/16b_cache.rs) contém o programa completo:

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/16b_cache.rs -o /tmp/16b
/tmp/16b
# linhas=1.48ms colunas=22.2ms razao=15.03x
# 200k reads de 1 byte=79ms vs 200k acessos a memoria=71us (razao 1119x)
# 16b ok: ...
```

Três leituras. **Localidade:** por linhas, cada linha de cache de 64 bytes rende 8 `u64`; por colunas, cada passo de 16 KiB (uma linha da matriz) toca uma linha nova e uma página nova — 15× medido. O detalhe que quase estragou a aula: soma de inteiros é associativa, e o otimizador **reescreveu o percurso por colunas em percurso por linhas** na primeira versão (razão 0,98×). A correção é `black_box` por coluna, que ancora o percurso pretendido. Benchmark que o compilador pode transformar mede o otimizador, não a memória — e descobrir isso é conteúdo da aula, não vergonha dela.

**Syscall:** 200 mil `read` de 1 byte levam ~79 ms (~400 ns por travessia com cópia); 200 mil acessos à memória, ~71 µs. A razão de três ordens de grandeza é o preço da fronteira usuário/kernel por operação pequena — a razão pela qual `read` de 1 byte em laço é um antipadrão que nenhuma linguagem conserta.

**Página:** `/proc/self/statm` expõe o tamanho virtual em páginas do próprio processo: o SO continua sendo uma fonte de verdade consultável, agora de dentro de um programa Rust sem `unsafe` e sem crates.

## Erros comuns

1. **Benchmark que o otimizador reescreve:** soma associativa, laço sem efeito observável, resultado descartado. Âncoras (`black_box`, saída usada) são parte do instrumento.
2. **Matriz pequena demais:** com a matriz cabendo no LLC, linhas e colunas empatam e a aula "prova" que localidade não importa. O tamanho se escolhe para estourar o cache que se quer medir.
3. **Comparar syscall com função:** a comparação honesta é contra acesso à memória já mapeada — é ela que mostra que o custo é a fronteira, não a linguagem.
4. **Generalizar a razão:** 15× e 1119× são desta máquina e destes tamanhos. O que viaja é o método; o número fica com o ambiente registrado.

## Exercícios, pistas e solução

Exercícios em [pistas](exercicios/16b_pistas.md) e [soluções](exercicios/16b_solucoes.md): (1) achar o tamanho em que a razão linhas/colunas nasce, varrendo potências de 2; (2) trocar `/dev/zero` por um arquivo real e por um pipe, e ordenar os custos explicando cada termo; (3) medir o custo por chamada de `read` com buffers de 1, 64 e 4096 bytes e mostrar a amortização; (4) ler `/proc/self/maps` e apontar as regiões do próprio binário (texto, heap, pilha, vdso).

**Checklist:** percursos ancorados contra o otimizador; tamanhos escolhidos para o nível de cache declarado; custo de syscall separado em fixo e por byte; nenhum número sem máquina e flags (`-O`) declaradas.
