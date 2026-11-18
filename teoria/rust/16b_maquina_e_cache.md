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

### O otimizador como adversário do benchmark (anatomia do 0,98×)

A primeira versão mediu razão 0,98× — linhas e colunas "empatadas". O culpado não era o cache: era o LLVM percebendo que a soma por colunas de inteiros (associativa, sem efeito observável intermediário) podia ser reordenada para o percurso por linhas. O benchmark media um programa que o compilador reescreveu para outro programa — e o número 0,98× era verdadeiro para o programa reescrito e falso para a pergunta. O `black_box` por coluna quebra a prova do otimizador: cada coluna lida vira efeito observável opaco, e reordenar deixaria de preservar semântica. Três regras gerais saem do episódio: (a) benchmark sem âncora contra reescrita mede o otimizador; (b) a âncora tem de estar no lugar que o otimizador exploraria (por coluna, não no total final — ancorar só o total ainda permite reordenar o meio); (c) descobrir a reescrita é diagnóstico, não fracasso — o 0,98× com explicação vale mais que 15× sem, porque o 15× sem âncora poderia ser outro 0,98× disfarçado.

**Página:** `/proc/self/statm` expõe o tamanho virtual em páginas do próprio processo: o SO continua sendo uma fonte de verdade consultável, agora de dentro de um programa Rust sem `unsafe` e sem crates.

### Leitura guiada: três linhas, três fronteiras do hardware

`linhas=1.48ms colunas=22.2ms razao=15.03x` é a hierarquia de cache em um número: mesmo dado (2048×2048), mesma conta, 15× de diferença só pelo percurso — e a razão carrega a âncora invisível (o `black_box` que impediu o 0,98×). `200k reads de 1 byte=79ms vs 200k acessos a memoria=71us` é a fronteira usuário/kernel: 1119× para fazer "a mesma coisa" quando a coisa atravessa o kernel a cada byte — o antipadrão do `read` de 1 byte quantificado, não adjetivado. A terceira é o veredito sobre o método, não sobre o número: localidade provada, fronteira precificada, página consultada. E o `/proc/self/statm` que o texto menciona sem mostrar número ensina por contraste: nem toda evidência é número publicado — tamanho virtual é contexto para interpretar os outros dois, e contexto se lê, não se afirma.

## Erros comuns

1. **Benchmark que o otimizador reescreve:** soma associativa, laço sem efeito observável, resultado descartado. Âncoras (`black_box`, saída usada) são parte do instrumento.
2. **Matriz pequena demais:** com a matriz cabendo no LLC, linhas e colunas empatam e a aula "prova" que localidade não importa. O tamanho se escolhe para estourar o cache que se quer medir.
3. **Comparar syscall com função:** a comparação honesta é contra acesso à memória já mapeada — é ela que mostra que o custo é a fronteira, não a linguagem.
4. **Generalizar a razão:** 15× e 1119× são desta máquina e destes tamanhos. O que viaja é o método; o número fica com o ambiente registrado.
5. **Ancorar só o total:** `black_box` no resultado final ainda permite ao otimizador reordenar o meio do percurso. A âncora mora onde a reescrita aconteceria — por coluna, a cada passo — ou não ancora nada.
6. **Ler statm como veredito:** tamanho virtual sem contexto (quanto é mapeado? residente? compartilhado?) vira número solto. Fonte do SO se consulta para interpretar medida, não para substituir medida.

## Exercícios, pistas e solução

Exercícios em [pistas](exercicios/16b_pistas.md) e [soluções](exercicios/16b_solucoes.md): (1) achar o tamanho em que a razão linhas/colunas nasce, varrendo potências de 2; (2) trocar `/dev/zero` por um arquivo real e por um pipe, e ordenar os custos explicando cada termo; (3) medir o custo por chamada de `read` com buffers de 1, 64 e 4096 bytes e mostrar a amortização; (4) ler `/proc/self/maps` e apontar as regiões do próprio binário (texto, heap, pilha, vdso).

**Checklist:** percursos ancorados contra o otimizador; tamanhos escolhidos para o nível de cache declarado; custo de syscall separado em fixo e por byte; nenhum número sem máquina e flags (`-O`) declaradas.
