---
layout: default
---

# Aula 28b — Vetorização e NUMA em Go: o empate também é resultado

## Objetivos verificáveis

Ao final, você deverá: explicar por que Go não expõe SIMD diretamente (assembly/intrínsecos), medir localidade e efeito NUMA, e aceitar o empate como resultado quando a medição manda.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir SIMD, NUMA e first-touch`;
- **Nível 2 — Aplicar:** `diagnosticar conclusão sem medição e corrigir com experimento`;
- **Nível 3 — Projetar:** `projetar um experimento justificando o veredito, inclusive o empate`.

**Competências SDD:** HPC, GPU, ARQ. **Pré-requisitos:** [28a](28a_hpc.md) (baseline, escala e conta da viagem) e G3–G6. Esta ponte completa o G7 sobre a base da 28a.

**Objetivo observável:** verificar a soma sequencial e a soma com quatro acumuladores contra uma referência `float64` (tolerâncias do modelo de erro), medir as duas nesta máquina, contar os nós NUMA por `/sys` e declarar o multinó como simulado enquanto houver um nó só.

**Contrato:** corretude antes de tempo; razão reportada com o ambiente (incluindo se o binário roda sob `-race`, que muda os dois lados). O [arquivo-fonte](exemplos/28b_vetorizacao.go) é o programa completo:

```bash
go vet teoria/go/exemplos/28b_vetorizacao.go
go run teoria/go/exemplos/28b_vetorizacao.go
# erro vs float64: seq=4.19e-05 4acum=4.27e-06
# seq=0.0067s 4acum=0.0069s razao=0.97x nos_numa=1
# 28b ok: corretude verificada, empate medido e NUMA contado
```

**Leitura — a que interessa:** em C e Rust, os quatro acumuladores venceram por ~3×; **em Go, empataram (0,97×)**. O compilador Go já quebra a cadeia de dependência/vetoriza a soma simples neste padrão, e a técnica manual não adiciona nada além de código. Publicar o empate é a aula: otimização manual se justifica por medição na sua toolchain, não por folclore trazido de outra linguagem. O erro numérico, esse sim, mantém a assinatura esperada (quatro acumuladores erram ~10× menos contra o `float64`), porque ele é propriedade da aritmética, não do compilador. NUMA: um nó contado; first-touch e memória remota ficam desenhados e rotulados como não executados, como nas outras trilhas.

### Por que o empate é evidência (e não ausência de evidência)

Um empate publicado com método carrega três informações: (a) a técnica foi aplicada corretamente (o erro ~10× menor prova que os acumuladores existem e somam certo — empate por bug de implementação teria erro igual ou pior); (b) o compilador já fazia o trabalho (a soma simples vetorizada pelo gc/ssa chega ao mesmo throughput); (c) a conclusão é local à toolchain (noutra versão do Go, noutra arquitetura, o manual pode voltar a vencer — e é por isso que o ambiente acompanha o número). Empate sem (a) é inconclusivo; com (a), é veredito: "nesta toolchain, neste padrão, técnica manual não paga". E o contraste com C/Rust ensina a lição de portabilidade: otimização é hipótese sobre (código, compilador, máquina) — mudar um dos três sem remediar é importar conclusão vencida. O exercício 4 (laço manual por índices) testa exatamente a fronteira: se o compilador "muda de ideia" sem `range`, a hipótese era sobre o padrão reconhecido, não sobre a linguagem.

**Erros comuns:** prometer o ganho de C em Go sem medir; comparar tempos sob `-race` com tempos sem ele na mesma tabela; exigir igualdade bit a bit entre ordens de soma; chamar simulação de multinó de medição multinó; ler empate como "otimização inútil em geral" (é veredito local: mesma técnica, outra toolchain, outro resultado); publicar razão sem dizer se o binário estava sob `-race` (o detector muda os dois lados — o contrato exige o ambiente junto do número).

**Exercícios:** [pistas](exercicios/28b_pistas.md) e [soluções](exercicios/28b_solucoes.md): (1) Kahan contra quatro acumuladores em erro e tempo; (2) repetir com `-race` e quantificar o que o detector faz com cada lado; (3) desenhar o experimento first-touch para 2 nós com a frase que o rotula como não executado aqui; (4) testar uma soma com laço manual por índices (sem `range`) e ver se o compilador muda de ideia.

**Checklist:** oráculo `float64` e tolerâncias derivadas; empate publicado sem maquiagem; ambiente de cada número declarado; NUMA contado com limite; `go vet` limpo.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Computer Organization and Design: The Hardware/Software Interface, RISC-V Edition*, Patterson e Hennessy (Morgan Kaufmann, ISBN 978-0128203316); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. Manual do programador Linux (`man 2 mmap`, `man 2 syscall`).
