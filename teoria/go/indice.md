# Indice Geral da Trilha de Go

---

**Estado editorial:** rascunho. As 31 aulas existentes formam uma base aproveitavel, mas ainda nao satisfazem, por si so, o contrato da masterclass. Consulte o [mapa SDD do livro de Go](MAPA_SDD.md) para pre-requisitos, lacunas, competencias e avaliacoes antes de tratar qualquer modulo como concluido. O [registro de auditoria](../AUDITORIA_POR_ARQUIVO.csv) distingue revisao parcial de aprovacao editorial.

## Visao Geral

Este arquivo organiza a trilha completa de Go em `d:\TI\git\Algoritmos\teoria\go`, cobrindo:

- prompt-base da trilha;
- fase principal de `01.md` a `19.md`;
- plano de especializacao;
- fase estendida de `20.md` a `31.md`.

Use este indice como ponto de entrada rapido para estudo, revisao e navegacao entre os modulos.

---

## Arquivos Base

- `prompt.md`: prompt-base da trilha de Go
- `19_plano_estudos.md`: plano de estudos da fase avancada

---

## Trilha Principal

### Fundamentos da Linguagem

1. `01.md` - Introducao ao Go, instalacao, ferramenta `go`, estrutura de um programa, `package main`, `func main`, `fmt.Println`, variaveis, constantes e tipos basicos
2. `02.md` - Variaveis, constantes, tipos basicos, inferencia e controle de fluxo
3. `03.md` - Funcoes, multiplos retornos, escopo e organizacao basica do codigo
4. `04.md` - Arrays, slices, strings, bytes e runes
5. `05.md` - Maps e structs
6. `06.md` - Metodos, interfaces, composicao e embedding
7. `07.md` - Tratamento de erros, `error`, `panic`, `recover` e `defer`
8. `08.md` - Packages, modulos, imports e organizacao de projetos
- [`08a_raciocinio_e_algoritmos.md`](08a_raciocinio_e_algoritmos.md) - Aula de ponte: invariantes, inducao, busca binaria, BFS, hash e custo antes de ponteiros e memoria
- [`08b_maquina_memoria_e_so.md`](08b_maquina_memoria_e_so.md) - Aula de ponte: runtime, heap contra RSS, syscall, `/proc` e SO pelo espaco de usuario
9. `09.md` - Ponteiros, memoria, escape analysis e modelo de alocacao
10. `10.md` - Concorrencia com goroutines, channels e `select`

### Engenharia de Software em Go

11. `11.md` - Sincronizacao com `WaitGroup`, `Mutex`, atomics e race detector
- [`11a_modelo_de_memoria.md`](11a_modelo_de_memoria.md) - Aula de ponte: happens-before, data race contra corrida logica e pool limitado com invariante
12. `12.md` - Generics em Go
13. `13.md` - Arquivos, I/O, `io`, `bufio`, `os`, `path/filepath` e serializacao
14. `14.md` - Testes, benchmarks, profiling e ferramentas da toolchain
15. `15.md` - `context`, cancelamento, timeouts e codigo concorrente robusto
16. `16.md` - Redes, HTTP, clientes, servidores e backend idiomatico
17. `17.md` - Performance tuning, `pprof`, alocacoes e uso eficiente de memoria
18. `18.md` - Integracao com C/C++ via `cgo`, syscalls e proximidade com o sistema
19. `19.md` - Boas praticas para servicos, CLIs, automacao e sistemas de producao

---

## Plano de Especializacao

- `19_plano_estudos.md` organiza a fase avancada em modulos de:
  - backend de alta performance e servicos de rede
  - observabilidade, confiabilidade e operacao
  - performance, runtime e proximidade com o sistema
  - projetos praticos de engenharia

---

## Trilha Estendida

### Modulo 1 - Backend de Alta Performance e Servicos de Rede

20. `20.md` - Servidores HTTP de Producao
21. `21.md` - WebSockets, Streaming e Fan-In/Fan-Out
22. `22.md` - Banco de Dados, Pools e Transacoes
23. `23.md` - gRPC e Comunicacao entre Servicos
- [`23a_protocolos_distribuidos.md`](23a_protocolos_distribuidos.md) - Aula de ponte: framing com teto, replicacao com idempotencia, log duravel e falhas injetadas

### Modulo 2 - Observabilidade, Confiabilidade e Operacao

24. `24.md` - Logs, Metricas, Tracing e `pprof` em Servicos Reais
25. `25.md` - Resiliencia: Retries, Circuit Breakers, Timeouts e Quotas
26. `26.md` - Workers, Filas, Schedulers e Jobs Confiaveis

### Modulo 3 - Performance, Runtime e Proximidade com o Sistema

27. `27.md` - Scheduler do Runtime, Garbage Collector e Latencia
28. `28.md` - Syscalls, `mmap`, Arquivos Grandes e I/O Avancado
- [`28a_hpc.md`](28a_hpc.md) - Aula de ponte: benchmark honesto, escala limitada por banda, contenção e conta da viagem para GPU
29. `29.md` - `cgo` Avancado, ABI e Integracao com Bibliotecas Nativas

### Modulo 4 - Projetos Praticos

30. `30.md` - Projeto pratico: construindo um reverse proxy / load balancer em Go
31. `31.md` - Projeto pratico: construindo uma ferramenta de infraestrutura e automacao de producao

---

## Sugestoes de Leitura

### Caminho Linear

Se voce quiser seguir a trilha como curso:

1. leia `01.md` a `19.md`, incluindo as aulas de ponte `08a_raciocinio_e_algoritmos.md` e `08b_maquina_memoria_e_so.md` depois de `08.md` e a aula `11a_modelo_de_memoria.md` depois de `11.md`
2. leia `19_plano_estudos.md`
3. siga `20.md` a `31.md`

### Caminho por Objetivo

Se o foco for linguagem e base:

- `01.md` a `12.md`

Se o foco for backend e servicos:

- `16.md`
- `19.md`
- `20.md` a `23.md`
- `30.md`

Se o foco for operacao e confiabilidade:

- `15.md`
- `24.md` a `26.md`
- `31.md`

Se o foco for performance, runtime e sistema:

- `17.md`
- `18.md`
- `27.md` a `29.md`

Se o foco for concorrencia:

- `10.md`
- `11.md`
- `15.md`
- `21.md`
- `26.md`

---

## Fechamento

O conjunto atual cobre uma trilha de Go do zero ate uma fase estendida de engenharia aplicada, com enfase em:

- concorrencia idiomatica;
- runtime e custo real;
- backend e comunicacao entre servicos;
- observabilidade e confiabilidade;
- integracao com sistema e C;
- profiling e latencia;
- projetos praticos.

Este indice serve como mapa geral da trilha.
