---
layout: default
---

# Aula 08d — Ferramenta de observação do SO em Go

**Competências SDD:** ARQ, MEM, OS. **Pré-requisitos:** [08b](08b_maquina_memoria_e_so.md) (processos, `/proc`, RSS e custo), 09 e 13. Esta ponte entrega a **ferramenta** que o G3 pedia: um binário Go que observa o próprio processo e liga cada número ao caminho programa → runtime → kernel → hardware.

**Objetivo observável:** executar a ferramenta e interpretar: goroutines paradas e com 32 trabalhadores, `GOMAXPROCS`, threads do SO por `/proc/self/task`, faltas de página menores crescendo ao tocar 32 MiB novos (e as maiores paradas), `HeapAlloc`/`HeapSys`/`NumGC` do runtime.

**Contrato:** Linux (a fonte de verdade é `/proc`); página anônima nova nasce por falta **menor** — falta maior é disco, e as duas não se confundem no relatório. O [arquivo-fonte](exemplos/08d_ferramenta.go) é a ferramenta completa:

```bash
go vet teoria/go/exemplos/08d_ferramenta.go
go run -race teoria/go/exemplos/08d_ferramenta.go
# goroutines(parado)=... gomaxprocs=... threads_so=...
# page_faults: min A->B (+grande) maj C->C
# 08d ok: ...
```

**Leitura:** goroutine não é thread: 32 trabalhadores dormindo são 32 goroutines e (quase sempre) nenhuma thread nova — o runtime multiplexa. `HeapSys` é a memória pedida ao SO e `HeapAlloc` a que está viva: a diferença é o que o GC ainda não devolveu, e confundi-las é o erro clássico de relatório de memória em Go. As faltas menores sobem exatamente porque páginas anônimas são materializadas sob demanda (a aula 10a de C mediu o mesmo fenômeno pelo RSS); as maiores ficam paradas porque nada foi lido de disco. Limitação declarada: em macOS/Windows esta ferramenta não roda — o equivalente (`/proc` ausente) exigiria APIs de plataforma, e portar sem declarar seria fingir portabilidade que o código não tem.

### O caminho do número até o hardware (lendo a ferramenta por camadas)

Cada linha da saída atravessa quatro camadas — programa, runtime, kernel, hardware — e a aula cobra o caminho inteiro: `goroutines=32` nasce no escalonador do runtime (programa→runtime); `threads_so` se lê em `/proc/self/task` (runtime→kernel: o SO só conhece threads, e o número prova a multiplexação ao ficar parado); `page_faults min A→B` conta materialização de página anônima (kernel→hardware: a falta menor é o SO zerando frame físico sob demanda); `HeapAlloc/HeapSys` distingue vivo de pedido (runtime→kernel: o GC segura `HeapSys - HeapAlloc` para reuse futuro em vez de devolver com `munmap` a cada ciclo). Quem lê só a primeira camada ("32 goroutines, X MB") escreve relatório de sintoma; quem atravessa as quatro escreve diagnóstico — e o exercício 4 (o que a ferramenta NÃO vê: compartilhadas, page cache) marca a fronteira do instrumento: ferramenta que não declara cegueira vira oráculo, e oráculo sem calibração vira superstição.

**Erros comuns:** reportar `HeapSys` como "uso de memória"; achar que goroutine parada custa thread; esperar falta maior em alocação anônima; usar a ferramenta em produção sem notar que ela lê o **próprio** processo; ler `min A->B` como vazamento (falta menor em toque novo é materialização, não perda — vazamento se lê em `HeapAlloc` crescendo sem carga); comparar RSS entre runs sem notar page cache compartilhado (o SO conta uma vez, cada processo conta de novo).

**Exercícios:** [pistas](exercicios/08d_pistas.md) e [soluções](exercicios/08d_solucoes.md): (1) forçar um ciclo de GC (`runtime.GC()`) e mostrar `HeapSys` antes/depois, explicando o que não muda; (2) trocar o toque de páginas por leitura de um arquivo temporário de 32 MiB e mostrar qual contador se move; (3) contar threads com 0, 32 e 320 trabalhadores dormindo e explicar a curva; (4) apontar o que a ferramenta não vê (páginas compartilhadas, cache de página do arquivo) e por que PSS/RSS do `/proc/<pid>/smaps` contaria outra história.

**Checklist:** cada número ligado ao caminho até o hardware; menor ≠ maior sem hesitar; limitação de plataforma declarada; `go vet` e `-race` limpos.
