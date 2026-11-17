---
layout: default
---

# Aula 23c — Vetorização em Rust: a cadeia de dependência é o inimigo

**Competências SDD:** HPC, GPU, ARQ. **Pré-requisitos:** aulas 17, 23a (baseline de HPC), [23b](23b_sistemas_distribuidos.md) e a aula 30b de C (o mesmo fenômeno com acumuladores, lá medido com AVX e aqui sem intrinsics). Você precisa saber iteradores, `chunks_exact` e por que somar `f32` em ordens diferentes muda o resultado.

**Objetivo observável:** verificar uma soma sequencial e uma soma com quatro acumuladores contra uma referência em `f64` (com tolerâncias derivadas do modelo de erro), medir a razão entre elas e contar os nós NUMA da máquina por `/sys` — declarando o que um nó só não permite concluir, como na 30b de C.

**Motivação:** o R7 pedia vetorização e NUMA sobre a baseline da 23a. Em Rust seguro não há intrinsics portáveis estáveis para fazer na mão o que a 30b de C fez com AVX: o caminho é escrever o laço de um jeito que o LLVM possa vetorizar — quebrar a cadeia de dependência com acumuladores independentes e dar-lhe fatias de tamanho conhecido (`chunks_exact`). A aula mede se funcionou em vez de acreditar.

## Contrato e exemplo

O [arquivo-fonte](exemplos/23c_vetorizacao.rs) contém o programa completo:

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/23c_vetorizacao.rs -o /tmp/23c_r
/tmp/23c_r
# erro vs double: seq=3.96e-5 4acum=1.92e-5
# seq=0.0013s 4acum=0.0004s razao=3.48x nos_numa=1
# 23c ok: ...
```

Leituras: os quatro acumuladores vencem por 3,48× porque quatro cadeias de soma independentes deixam o pipeline (e o vetorizador) trabalhar — e erram **menos** contra o `f64` (1,92e-5 contra 3,96e-5), pelo mesmo motivo da 30b de C: somas parciais menores perdem menos por alinhamento de expoente. A prova numérica é contra `f64`, nunca `f32` contra `f32` com tolerância apertada (a primeira versão da 30b de C reprovou a implementação certa exatamente assim). Sem intrinsics, sem `unsafe`: slices com comprimento conhecido e aritmética comum são o que o otimizador precisa — confira com o relatório de vetorização do LLVM se quiser ver a instrução, sabendo que o desempenho medido é a evidência que fica.

NUMA: `nos_numa=1` nesta máquina (contagem de `/sys/devices/system/node`), e portanto first-touch e acesso remoto **não são demonstráveis aqui** — ficam desenhados nos exercícios, rotulados como não executados, como nas trilhas irmãs.

## Erros comuns

1. **Comparar `f32` com `==` entre ordens de soma:** ordens diferentes, resultados diferentes, ambos certos. Oráculo em precisão maior e tolerância do modelo de erro.
2. **Escrever o laço "esperto" que o compilador não entende:** índices manuais e aliasing obscuro derrotam o vetorizador. Fatias e iteradores são informação para o otimizador.
3. **Atribuir o ganho a "SIMD" sem medir a cadeia:** parte do ganho é ILP escalar. A aula afirma a razão e o mecanismo (cadeias independentes), não a instrução específica.
4. **Concluir sobre NUMA com um nó:** contar e parar. O resto é desenho de experimento, rotulado.

## Exercícios, pistas e solução

Exercícios em [pistas](exercicios/23c_pistas.md) e [soluções](exercicios/23c_solucoes.md): (1) soma de Kahan contra os quatro acumuladores em erro e tempo; (2) 2, 4 e 8 acumuladores: onde a razão satura e por quê; (3) o experimento first-touch desenhado para 2 nós, com a frase do que o rotula como não executado aqui; (4) inspecionar o assembly gerado (`--emit asm`) e apontar uma instrução vetorial no laço dos acumuladores, ou explicar a ausência.

**Checklist:** oráculo em `f64` com tolerâncias derivadas; razão medida com repetição; NUMA contado e limite declarado; `-D warnings` limpo; nenhum `unsafe` para o caminho rápido.
