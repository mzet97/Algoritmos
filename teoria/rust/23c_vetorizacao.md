---
layout: default
---

# Aula 23c — Vetorização em Rust: a cadeia de dependência é o inimigo

Vetorizar em Rust é convencer o compilador de que o loop pode andar de quatro em quatro — e o inimigo é a cadeia de dependência que você mesmo escreveu. Medir a cadeia antes de culpar o hardware.

## Nesta aula

Você vai aprender a identificar a cadeia de dependência como inimiga da vetorização, reestruturar um laço para expor paralelismo de dados, e medir o ganho contra a linha de base.

Você vai verificar uma soma sequencial e uma soma com quatro acumuladores contra uma referência em `f64` (com tolerâncias derivadas do modelo de erro), medir a razão entre elas e contar os nós NUMA da máquina por `/sys` — declarando o que um nó só não permite concluir, como na 30b de C.

o R7 pedia vetorização e NUMA sobre a baseline da 23a. Em Rust seguro não há intrinsics portáveis estáveis para fazer na mão o que a 30b de C fez com AVX: o caminho é escrever o laço de um jeito que o LLVM possa vetorizar — quebrar a cadeia de dependência com acumuladores independentes e dar-lhe fatias de tamanho conhecido (`chunks_exact`). A aula mede se funcionou em vez de acreditar.

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

### A cadeia de dependência, em ciclos

Uma soma `f32` tem latência de ~4 ciclos: o acumulador único só pode iniciar uma soma a cada 4 ciclos, porque cada iteração espera a anterior — o pipeline de 4 estágios trabalha com 1 em uso. Quatro acumuladores independentes preenchem os 4 estágios: uma soma nova por ciclo, 4× de ILP escalar antes de qualquer vetor. E o vetorizador entra porque `chunks_exact(4)` dá ao LLVM fatias de tamanho conhecido sem aliasing: quatro somas independentes lado a lado são exatamente uma instrução SIMD de 4 vias esperando para nascer. O 3,48× medido é o produto parcial dos dois efeitos (não exatamente 4× por causa do laço, do resto e da banda) — e é por isso que a aula afirma "cadeias independentes" e não "SIMD": o mecanismo provado é a independência; a instrução específica é detalhe do alvo. O exercício 2 (2, 4, 8 acumuladores) mede onde a razão satura: quando o número de cadeias excede a profundidade do pipeline, acumulador extra é registrador ocupado sem ganho — e o ponto de saturação lido no gráfico é a profundidade efetiva do pipeline da sua máquina, medida sem abrir manual.

### Leitura guiada: erro menor E tempo menor (por que os dois juntos importam)

`erro vs double: seq=3.96e-5 4acum=1.92e-5` diz que a versão rápida é também a mais precisa — e as duas melhorias têm a mesma causa (somas parciais menores), não são coincidência. Se o erro tivesse piorado com a velocidade, a aula teria um dilema (rápido ou certo?); como melhorou junto, a conclusão é limpa: quebrar a cadeia é bom nos dois eixos. `seq=0.0013s 4acum=0.0004s razao=3.48x` é a razão com repetição, e `nos_numa=1` é a declaração de limite colada no número — a mesma disciplina da 23e: topologia acompanha banda, sempre. Repare que a tolerância é derivada do modelo de erro (item do checklist), não chutada: tolerância chutada que passa é sorte; tolerância derivada que passa é prova.

NUMA: `nos_numa=1` nesta máquina (contagem de `/sys/devices/system/node`), e portanto first-touch e acesso remoto **não são demonstráveis aqui** — ficam desenhados nos exercícios, rotulados como não executados, como nas trilhas irmãs.

## Erros comuns

1. **Comparar `f32` com `==` entre ordens de soma:** ordens diferentes, resultados diferentes, ambos certos. Oráculo em precisão maior e tolerância do modelo de erro.
2. **Escrever o laço "esperto" que o compilador não entende:** índices manuais e aliasing obscuro derrotam o vetorizador. Fatias e iteradores são informação para o otimizador.
3. **Atribuir o ganho a "SIMD" sem medir a cadeia:** parte do ganho é ILP escalar. A aula afirma a razão e o mecanismo (cadeias independentes), não a instrução específica.
4. **Concluir sobre NUMA com um nó:** contar e parar. O resto é desenho de experimento, rotulado.
5. **Aumentar acumuladores sem medir a saturação:** 8, 16, 32 acumuladores ocupam registradores e pressionam o renomeador sem preencher pipeline além da profundidade — razão que para de subir virou prova de ociosidade, não de potência. O exercício 2 existe para desenhar o joelho da curva em vez de presumir que "mais é mais".
6. **Tolerância chutada:** `1e-3` porque "parece razoável" aprova e reprova ao acaso do dado. Tolerância se deriva do modelo de erro (n operações × epsilon × crescimento) — e quando a medida passa folgada, a folga quantifica a margem, não a sorte.

## Exercícios, pistas e solução

Exercícios em [pistas](exercicios/23c_pistas.md) e [soluções](exercicios/23c_solucoes.md): (1) soma de Kahan contra os quatro acumuladores em erro e tempo; (2) 2, 4 e 8 acumuladores: onde a razão satura e por quê; (3) o experimento first-touch desenhado para 2 nós, com a frase do que o rotula como não executado aqui; (4) inspecionar o assembly gerado (`--emit asm`) e apontar uma instrução vetorial no laço dos acumuladores, ou explicar a ausência.

**Checklist:** oráculo em `f64` com tolerâncias derivadas; razão medida com repetição; NUMA contado e limite declarado; `-D warnings` limpo; nenhum `unsafe` para o caminho rápido.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que compila.

**Para se aprofundar:**

1. *The Rustonomicon* ([doc.rust-lang.org/nomicon](https://doc.rust-lang.org/nomicon)); 2. Documentação do Tokio ([tokio.rs](https://tokio.rs)); 3. *The Rust Performance Book* ([nnethercote.github.io/perf-book](https://nnethercote.github.io/perf-book)).
