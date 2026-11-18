---
layout: default
---

# Aula 30c — NUMA: onde a memória mora, medida com honestidade

**Competências SDD:** HPC, ARQ e OS. **Pré-requisitos:** aulas [28a](28a_benchmark_honesto.md) (protocolo de medição) e [30b](30b_simd_e_afinidade.md) (SIMD, afinidade, 1 nó contado aqui). Esta aula fecha o que a 30b declarou em aberto: o multinó.

**Objetivo observável:** ao terminar, você deverá contar os nós NUMA da máquina, explicar first-touch e interleave, medir a banda local — e declarar quando a comparação entre nós **não** foi feita, em vez de fingi-la.

**Motivação:** em máquina multinó, "memória" não é um lugar só: cada página mora num nó, e o núcleo que a toca de longe paga a viagem a cada cache line. Programa que ignora isso mede bem no laptop e sangra no servidor. Esta aula ensina a regra de moradia antes que o servidor cobre a aula.

## 1. Contrato antes do código

- **Detecção:** contar `/sys/devices/system/node/node*`. Nesta máquina, o resultado é 1 — e o programa imprime isso em vez de supor.
- **First-touch:** a página é ancorada no nó de quem primeiro a toca. `memset` inicial na thread que vai usar o buffer não é "aquecimento" — é escritura de moradia.
- **Interleave:** espalhar páginas round-robin pelos nós (`numactl --interleave=all`). Ajuda quando o acesso é imprevisível; atrapalha quando o acesso é local por construção.
- **Medida local honesta:** varredura de 4×64 MiB com banda em GiB/s. O número vale como ordem de grandeza e como baseline do protocolo abaixo.
- **Fora do contrato:** a comparação local × remoto — sem 2+ nós, ela seria ficção. O protocolo está escrito; a medição, declaradamente pendente.

## 2. Por que moradia decide banda?

Cache line de 64 bytes viaja inteira: tocar um byte remoto busca os 63 vizinhos pelo interconexo. Em loop sequencial sobre gigabytes, a diferença entre "todas as linhas locais" e "metade remota" não é ruído — é o teto do programa. First-touch acerta a moradia quando o padrão é partível (cada thread ancora o seu pedaço); interleave distribui o azar quando o padrão é imprevisível. Escolher sem medir o padrão é superstição com flag.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/30c_numa.c) contém o programa completo:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/30c_numa.c -o /tmp/30c
/tmp/30c
```

Saída esperada (nesta máquina; a banda varia, a estrutura não):

```text
numa: nos detectados em /sys = 1
numa: varredura 4x64MiB em 0.06s = 4.2 GiB/s (soma=...)
numa: 1 no nesta maquina — first-touch vs interleave sem efeito
mensuravel aqui; o protocolo abaixo vale quando houver 2+ nos:
  numactl --cpunodebind=0 --membind=0 ./prog   (local)
  numactl --cpunodebind=0 --membind=1 ./prog (remoto)
  numactl --interleave=all ./prog              (espalhado)
30c ok: nos contados, banda local medida, limite multinó declarado
```

O protocolo pendente, para a máquina com 2+ nós: rode as três linhas, anote banda e variância, e só então decida moradia. Sem isso, qualquer afirmação sobre ganho NUMA é palpite com unidade.

### O que a memória faz aqui?

`memset` inicial ancora as páginas (first-touch), a varredura pula de 64 em 64 bytes (uma cache line por toque, sem falsa precisão de byte), e a soma impede o otimizador de apagar o trabalho — o mesmo `black_box` moral das pontes de benchmark, sem depender de API externa.

## 4. Erros comuns e implicações defensivas

1. **Afinar thread sem fixar memória:** `sched_setaffinity` no CPU errado com páginas no nó errado é o pior dos dois mundos — declare os dois juntos.
2. **Medir uma vez e publicar:** banda varia com vizinhos e frequência. Baseline exige repetição e ambiente anotado, como manda a 28a.
3. **Interleave como padrão:** espalhar o que era local transforma acerto em loteria. Interleave é remédio para acesso imprevisível, não vitamina.
4. **Fingir multinó com um nó:** `numactl` com um só nó sempre dá "local". Publicar comparação nessas condições é fabricar evidência.
5. **Confundir banda com latência:** varredura sequencial mede banda; ponteiro que pula mede latência. O programa desta aula mede a primeira; a segunda é outro experimento.

## 5. Ponte com as demais trilhas

C++ repete o protocolo com `numactl` e afinidade de threads; Rust ancora com thread dedicada por pedaço; Go e Python sofrem o mesmo efeito sob o runtime (o GC também mora em algum nó). Em JavaScript, o análogo é a localidade de `TypedArray` por worker — a viagem existe, só muda de nome.

## 6. Checklist do que esta aula garante

- [ ] Conto os nós da máquina e digo onde li.
- [ ] Explico first-touch com as próprias palavras e aponto o `memset` que ancora.
- [ ] Digo quando interleave ajuda e quando atrapalha, com um exemplo de cada.
- [ ] Meço a banda local e anoto ambiente e repetição.
- [ ] Declaro sem rodeios: sem 2+ nós, comparação entre nós é protocolo escrito, não medição feita.

**Referências primárias:** `man 8 numactl`, `man 2 set_mempolicy` e a documentação de [NUMA no kernel](https://www.kernel.org/doc/html/latest/vm/numa.html). Nenhuma afirmação de ganho multinó nesta aula — por decisão editorial, o limite é o conteúdo.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
