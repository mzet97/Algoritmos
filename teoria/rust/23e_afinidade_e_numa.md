---
layout: default
---

# Aula 23e — Afinidade, primeiro toque e o laboratório multinó (R7)

**Competências SDD:** HPC, MEM, OS. **Pré-requisitos:** aulas [23a](23a_hpc_baseline.md) (baseline honesto) e [23c](23c_vetorizacao.md) (corretude por tolerância, NUMA contado). Toolchain registrada: `rustc` 1.99.1, `taskset` e `numactl` do sistema. Máquina registrada: 24 núcleos, **um** nó NUMA (`node0`: `0-23`).

**Objetivo observável:** ao terminar, você deverá executar o exemplo com máscara cheia e com `taskset -c 0`, mostrar checksums idênticos nos dois modos de inicialização, ler a afinidade de `/proc/self/status` e explicar por que a razão medida nesta máquina não prova nada sobre NUMA — e o que provaria.

**Motivação:** o R7 pedia afinidade/NUMA e multinó sobre a base da 23a; a 23c vetorizou e contou o nó. Faltava atravessar a fronteira entre "rodar em N threads" e "rodar nos N lugares certos": quem inicializa a página decide onde ela mora (primeiro toque), e quem prende a thread decide onde ela computa (afinidade). Numa máquina de um nó, o efeito é ~zero — e medir ~zero com honestidade é o experimento que autoriza o laboratório multinó em vez de fingi-lo.

## Contrato e exemplo

O [arquivo-fonte](exemplos/23e_afinidade.rs) contém o programa completo (tríade `a = b + s*c` sobre 64 MiB, checksum exato, tempos só impressos):

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/23e_afinidade.rs -o afim23e
./afim23e
# Cpus_allowed_list:  0-23
# nos_numa=1 threads=8 n=16777216
# primeiro_toque=0.010s (20.39 GB/s) tardio=0.008s (23.71 GB/s) razao=0.86x
# 23e ok: checksum igual nos dois modos, afinidade lida, 1 no = laboratorio multino declarado
taskset -c 0 ./afim23e
# Cpus_allowed_list:  0
# nos_numa=1 threads=8 n=16777216
# primeiro_toque=0.033s (6.10 GB/s) tardio=0.011s (19.15 GB/s) razao=0.32x
# 23e ok: checksum igual nos dois modos, afinidade lida, 1 no = laboratorio multino declarado
```

(Os GB/s variam com a máquina e a carga — o `assert` é sobre o checksum; todo o resto é medida publicada, não veredito.)

### Leitura guiada: as duas execuções lado a lado

Máscara cheia: `Cpus_allowed_list: 0-23` ancora o experimento (8 threads livres em 24 núcleos), `razao=0.86x` diz "modos indistinguíveis" — e indistinguível É o resultado esperado com um nó, não falha do experimento. Com `taskset -c 0`: a âncora vira `0` (todo mundo num núcleo), o primeiro-toque cai para 6.10 GB/s e a razão para 0.32x — queda de inicialização paralela espremida num núcleo, não de memória. As duas execuções terminam com o mesmo veredito (`checksum igual`, `1 no = laboratorio multino declarado`) porque o veredito cobra o que as duas provam (corretude + declaração de máquina), não o que só o multinó provaria (razão remoto/local). O contraste entre 0.86x e 0.32x ensina a distinção central: mesmo número (razão), causas opostas (ruído vs. oversubscription) — e só a âncora (`Cpus_allowed_list`) diz qual é qual.

## 1. O que o experimento mostra (e o que ele não mostra)

**Mostra:** os dois modos calculam igual (checksum exato, não tolerância — tríade é exata em `f32` aqui porque os operandos são representáveis e a ordem é a mesma). A afinidade é lida do processo, não presumida: `Cpus_allowed_list` prova onde o SO deixou rodar. E prender 8 threads num núcleo só derruba a banda do modo paralelo de inicialização (0.32x) — contenção de CPU, não de memória, e o número diz qual.

**Não mostra:** efeito NUMA. Com um nó só, primeiro toque e tardio moram no mesmo lugar — razão 0.86x é ruído, e publicá-la como "ganho/perda de NUMA" seria fraude. A aula declara a máquina (`nos_numa=1` impresso pelo programa) para que nenhum leitor cite esses números fora dela.

### Primeiro toque, por extenso: falta, zero, residência

O Linux aloca página virtual no `malloc` (aqui: no `Vec` de 64 MiB) e página física na primeira escrita — a falta que zera e mapeia. "Quem toca primeiro decide o nó" porque a falta executa no contexto de quem escreveu: o manipulador escolhe o nó local da CPU faultante (política padrão), instala a PTE e conta a página para aquele nó. Três consequências: (a) inicializar na main prende as 64 MiB no nó da main, mesmo que 8 threads calculem depois — e é por isso que o modo tardio paraleliza o cálculo mas serializa a residência; (b) paralelizar a inicialização espalha as faltas (e as páginas) pelas CPUs que tocam — em 2 nós, cada metade mora perto de quem a calculará; (c) tocar de novo não move: residência é decidida na falta, e migração exige `migratepages` explícito — ler de outro nó não "traz" a página para perto, só paga a distância toda vez. Mesmo nesta máquina de um nó, (a) e (c) são visíveis em princípio: o tardio paga 8 threads calculando sobre páginas que uma thread faultou — sem distância NUMA, mas com a mesma estrutura de quem-tocou-onde que o multinó cobraria em GB/s.

## 2. O laboratório multinó: protocolo, não fingimento

O que provaria NUMA: a mesma tríade em máquina de 2+ nós, com `numactl --cpunodebind/--membind` cruzando as quatro combinações (computa-no-nó-A/memória-no-nó-A, A/B, B/A, B/B) e a razão remoto/local publicada. O protocolo vive na [23b](23b_sistemas_distribuidos.md) (dois atores, atraso, replay) adaptado para latência de memória: mesma disciplina de hipótese→baseline→combinações→razão, outro mensurando. Sem a máquina, o laboratório fica como hipótese registrada com procedimento pronto — exatamente como a GPU física ficou na 23a — e não como gráfico bonito de outro computador.

## 3. Erros comuns e implicações defensivas

1. **Citar GB/s sem citar a máscara:** banda medida com `taskset -c 0` comparada com banda de máquina cheia não é regressão, é outro experimento. Todo número de banda viaja com `Cpus_allowed_list` e `nos_numa`, ou não viaja.
2. **Inicializar na main e chamar de "paralelo":** o modo tardio paraleliza o cálculo mas serializa a inicialização — e no NUMA real, a página mora onde foi tocada primeiro. Paralelizar o cálculo sem paralelizar o toque é otimizar a metade errada.
3. **`assert` em tempo:** razão com desigualdade (`t_4 < t_seq` da 23c) só vale quando o efeito é robusto à carga. Aqui o efeito esperado é zero — afirmar desigualdade seria transformar ruído em teste. Tempo se publica, checksum se afirma.
4. **Confundir contenção de CPU com localidade:** os 0.32x do `taskset -c 0` são 8 threads num núcleo, não memória remota. Diagnóstico errado (comprar NUMA) para sintoma certo (oversubscription) custa hardware à toa.
5. **Achar que ler de outro nó "traz" a página:** residência decide-se na falta; leitura remota paga a distância toda vez, sem migração implícita. Localidade se constrói no primeiro toque (ou com migração explícita), nunca na releitura — otimizar o loop de leitura sem tocar a inicialização é polir o termômetro.
6. **Publicar razão sem a âncora:** 0.86x e 0.32x são o mesmo tipo de número com causas opostas — sem `Cpus_allowed_list` e `nos_numa`, o leitor não distingue ruído de oversubscription de NUMA. Número de banda sem máscara e sem topologia é ruído com formatação.

## 4. Ponte com as demais trilhas

C faz o primeiro toque na [30c](../c/30c_numa_primeiro_toque.md) com a mesma declaração de nó único; C++ modela custo de transferência na 30b sem placa; Go mede contenção granular na 23a sem prender núcleo. Em Python, o análogo honesto é `numactl` + `numpy` com a mesma tríade — e a mesma declaração de máquina. O padrão "checksum afirma, tempo publica, máquina acompanha o número" vale nas cinco trilhas.

## 5. Critério de aprovação e checklist

- [ ] Rodo com máscara cheia e com `taskset -c 0` e explico cada razão (ruído vs oversubscription).
- [ ] Mostro o checksum igual e digo por que ele é exato aqui (e tolerância na 23c).
- [ ] Leio `Cpus_allowed_list` e `nos_numa` e digo por que os dois acompanham qualquer GB/s citado.
- [ ] Descrevo o protocolo das quatro combinações que provaria NUMA (e o que falta: a máquina).
- [ ] Nomeio o que falta: multinó medido, `membind` cruzado, GPU com transferência (R7 segue aberto).

**Referências primárias:** manual do `numactl`/`taskset` (afinidade e bind como mecanismo do SO) e política de primeiro toque do kernel Linux (página mora no nó de quem tocou primeiro). Nenhum texto foi copiado — os números acima foram medidos nesta máquina (24 núcleos, 1 nó).

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
