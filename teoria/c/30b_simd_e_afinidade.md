---
layout: default
---

# Aula 30b — SIMD verificado, afinidade e NUMA sem propaganda

Você vai ao terminar, você deverá verificar três implementações de produto interno (escalar, quatro acumuladores, AVX+FMA) contra um oráculo em `double`, medir as razões de tempo entre elas, fixar uma thread numa CPU com `sched_setaffinity` e confirmar pela máscara relida, e contar os nós NUMA da máquina — declarando o que uma máquina de um nó só não permite concluir.

a aula 30 apresentou SIMD como conceito; a 28a ensinou a não acreditar em speedup sem baseline. Falta juntar as duas: vetor otimizado que ninguém verificou é só um jeito mais rápido de estar errado — e, em ponto flutuante, "errado" nem precisa de bug: basta somar numa ordem diferente. Esta aula fecha a lacuna de C7 (NUMA, afinidade, SIMD com verificação).

## 1. Contrato antes do código

Produto interno de dois vetores de `float` com `n = 10 000 000`:

- **Pré-condição:** vetores com o mesmo `n`; valores gerados por LCG de semente fixa (7).
- **Pós-condição numérica:** cada implementação em `float` é comparada com a referência em `double`. Escalar deve ficar abaixo de 2% de erro relativo; quatro acumuladores e AVX, abaixo de 0,2% — e os dois devem ser **mais** precisos que o escalar. Igualdade bit a bit entre implementações `float` **não** é exigida nem esperada: ponto flutuante não é associativo.
- **Pós-condição de afinidade:** após `sched_setaffinity` para uma CPU, a máscara relida por `sched_getaffinity` contém exatamente aquela CPU. Se o ambiente recusar, o programa registra a recusa em vez de fingir sucesso.
- **Fora do contrato:** concluir qualquer coisa sobre NUMA além da contagem de nós. Esta máquina tem 1 nó; política de alocação entre nós (first-touch, interleave) não é demonstrável aqui e a aula não finge que é.

## 2. O achado que manda na aula: o escalar é o impreciso

Somar 10 milhões de `float`s em sequência acumula erro de arredondamento a cada passo, porque a soma parcial cresce e cada parcela nova perde bits ao alinhar o expoente. Quatro acumuladores somam quatro somas parciais menores e combinam no fim: erram **menos**, não só andam mais rápido. Medido nesta máquina, contra o oráculo em `double`:

```text
erro_vs_double: escalar=1.16e-02 4acum=1.00e-03
```

O escalar erra 1,16%; quatro acumuladores, 0,10% — uma ordem de grandeza melhor. A primeira versão deste exemplo comparava `float` contra `float` com tolerância de 1e-4 e **reprovava a implementação certa**. A lição de verificação é a aula inteira: oráculo precisa ser mais preciso que o objeto testado, e tolerância se deriva do modelo de erro, não do otimismo.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/30b_simd.c) contém o programa completo:

```bash
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/30b_simd.c -o /tmp/30b -lm
/tmp/30b
```

Um binário só, sem `-march=native`: a função AVX é compilada com atributo de alvo (`avx2,fma`) e só é chamada se `__builtin_cpu_supports` anunciar as duas extensões em tempo de execução. Sem esse dispatch, um binário `-march=native` morreria com `SIGILL` numa CPU mais antiga que a da compilação — portabilidade se decide na largada, não no primeiro relatório de crash. A cauda que não completa 8 elementos cai para o laço escalar, elemento a elemento.

Saída esperada (tempos variam; erros e contagens são determinísticos nesta máquina — 24 CPUs disponíveis, 1 nó NUMA):

```text
cpus_disponiveis=24 nos_numa=1
afinidade: thread fixada na cpu 0 e confirmado
erro_vs_double: escalar=1.16e-02 4acum=1.00e-03
escalar: 0.0069s  4acum: 0.0025s  razao: 2.77x
avx+fma: 0.0020s  razao_vs_escalar: 3.31x     (dispatch em tempo de execucao)
30b ok: corretude por tolerancia, afinidade confirmada, NUMA contado
```

Três leituras honestas:

1. **Quatro acumuladores: 2,77×.** O ganho não vem de instrução vetorial, e sim de quebrar a cadeia de dependência: o somatório escalar espera cada soma terminar para somar a próxima; quatro cadeias independentes alimentam as unidades de ponto flutuante em paralelo. É a mesma razão pela qual o compilador, sem `-ffast-math`, **não pode** fazer isso por você: reordenar somas muda o resultado, e o padrão da linguagem proíbe o compilador de mudar seu resultado.
2. **AVX+FMA: 3,31× sobre o escalar — não 8×.** Oito `float`s por registrador não significam oito vezes mais rápido: o laço lê 80 MB por passada e a parede seguinte é a banda de memória (a conta da 28a). Quem prometer 8× está vendendo largura de registrador como se fosse vazão.
3. **Afinidade confirmada, não presumida.** Fixar e reler a máscara é o que separa "pedi" de "aconteceu". E `cpus_disponiveis=24` é a máscara do processo (cpuset de container conta como limite), não o folheto da máquina.

## 4. Erros comuns e implicações defensivas

1. **Exigir `==` entre somas `float`:** ordens diferentes dão resultados diferentes, todos "certos" dentro do modelo. Compare contra oráculo mais preciso, com tolerância derivada.
2. **`-ffast-math` para "desbloquear" vetorização:** ele autoriza o compilador a reordenar, assumir ausência de NaN/Inf e contrair operações — muda a semântica numérica do programa inteiro para ganhar um laço. Se usar, o contrato numérico novo é seu, documentado.
3. **Prometer largura SIMD como speedup:** 8 por registrador é teto aritmético; banda, latência e o restante do programa (Amdahl) decidem o real.
4. **Afinidade sem confirmação:** `sched_setaffinity` pode falhar ou ser herdado de forma inesperada; releia a máscara. Em container, a máscara já vem cortada pelo cpuset.
5. **Concluir sobre NUMA com um nó:** first-touch e interleave só existem com 2+ nós. Contar os nós e parar aí é a resposta certa nesta máquina.
6. **Medir com a thread fixada sem registrar:** afinidade muda a medição (migração some, cache estabiliza). O exemplo restaura a máscara original antes de medir e diz por quê.

## 5. Ponte com Python, JavaScript e GPU

NumPy faz o mesmo produto interno chamando BLAS — que usa exatamente acumuladores múltiplos e SIMD, com o mesmo erro de somatório menor que o laço Python ingênuo teria (se um laço Python de 10 milhões fosse viável). Na GPU da aula 30a, a redução paralela é quatro-acumuladores levado ao extremo: milhares de somas parciais combinadas em árvore — mais rápida e, pelo mesmo argumento, com erro diferente do sequencial. Quem entendeu esta aula não pergunta "qual é o resultado exato?", e sim "qual é o modelo de erro de cada ordem de soma?".

## 6. Exercícios obrigatórios

1. Implemente a soma de Kahan (compensada) para o produto interno e meça o erro contra o oráculo em `double`. Ela ganha dos quatro acumuladores? A que custo de tempo?
2. Repita a medição com a thread fixada (sem restaurar a máscara) e sem fixar, 20 repetições cada, e compare a dispersão (máx/mín), não só a média. Explique a diferença pelo escalonador.
3. Compile com `-ffast-math` e registre o que muda: tempos, erros e quais asserções do contrato continuam valendo. Escreva a frase que você colocaria no README do projeto sobre o que esse flag autorizou.
4. Desenhe o experimento de first-touch que você rodaria numa máquina de 2 nós para mostrar o custo de acessar memória remota — e deixe explícito por que ele não roda nesta máquina.

Use [pistas graduais](exercicios/30b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/30b_solucoes.md) depois de registrar sua própria explicação.

## 7. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- explicar por que quatro acumuladores são mais rápidos **e** mais precisos que um;
- derivar a tolerância de comparação do modelo de erro, contra oráculo em `double`;
- medir e interpretar a razão AVX sem prometer a largura do registrador;
- fixar afinidade, confirmar pela máscara relida e dizer o que o cpuset muda;
- contar os nós NUMA e declarar o que não é demonstrável com um nó;
- compilar com `-Werror`, verificar antes de cronometrar e reportar dispersão.


**Referências primárias:** [What Every Computer Scientist Should Know About Floating-Point Arithmetic (Goldberg)](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html) para o modelo de erro; *Intel Intrinsics Guide* para AVX/FMA usados com dispatch em tempo de execução; `man sched_setaffinity` para afinidade como contrato com o SO.

**Próxima aula:** 31 — o servidor. Desempenho medido aqui volta como pergunta operacional lá: quanto deste ganho sobrevive a clientes reais?

---

### Para se aprofundar

1. Randall Bryant e David O'Hallaron, *Computer Systems: A Programmer's Perspective* (Pearson, 3. ed., ISBN 978-0134092669), caps. 5-6; 2. Agner Fog, manuais de microarquitetura ([agner.org/optimize](https://www.agner.org/optimize/)); 3. John Patterson e John Hennessy, *Computer Organization and Design, RISC-V Edition* (Morgan Kaufmann, ISBN 978-0128203316).
