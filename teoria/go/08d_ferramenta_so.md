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

**Erros comuns:** reportar `HeapSys` como "uso de memória"; achar que goroutine parada custa thread; esperar falta maior em alocação anônima; usar a ferramenta em produção sem notar que ela lê o **próprio** processo.

**Exercícios:** [pistas](exercicios/08d_pistas.md) e [soluções](exercicios/08d_solucoes.md): (1) forçar um ciclo de GC (`runtime.GC()`) e mostrar `HeapSys` antes/depois, explicando o que não muda; (2) trocar o toque de páginas por leitura de um arquivo temporário de 32 MiB e mostrar qual contador se move; (3) contar threads com 0, 32 e 320 trabalhadores dormindo e explicar a curva; (4) apontar o que a ferramenta não vê (páginas compartilhadas, cache de página do arquivo) e por que PSS/RSS do `/proc/<pid>/smaps` contaria outra história.

**Checklist:** cada número ligado ao caminho até o hardware; menor ≠ maior sem hesitar; limitação de plataforma declarada; `go vet` e `-race` limpos.
