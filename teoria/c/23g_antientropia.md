---
layout: default
---

# Aula 23g — Anti-entropia por resumos: sincronizar pelo que mudou (C6)

**Competências SDD:** DST, SEC. **Pré-requisitos:** aulas [23c](23c_relogios_e_particao.md) (vetores, concorrência detectada, anti-entropia com ordem total) e [23d](23d_consenso_e_quorum.md) (versão vence por termo). Toolchain registrada: GCC com `-std=c11 -Wall -Wextra -Wpedantic -Werror` (e build ASan/UBSan limpo).

**Objetivo observável:** ao terminar, você deverá executar o exemplo, mostrar os três cenários (0, 1 e 3 faixas) e explicar por que a versão decide a direção da cópia — e em que caso o resumo mente.

**Motivação:** a 23c sincroniza mandando os logs inteiros: funciona, mas o custo é proporcional ao tamanho da loja, não ao tamanho da divergência. Quando dois nós passam 99% do tempo iguais, mandar 100% para conferir 1% é desperdício — e em enlace caro, é o desperdício que impede a sincronização frequente. Esta aula troca o protocolo: 8 resumos FNV-1a (um por faixa de 8 chaves) decidem o que viaja; faixa igual nem entra no fio. A versão por entrada decide a direção; o resumo decide o escopo.

## 1. Contrato antes do código

O [arquivo-fonte](exemplos/23g_antientropia.c) contém a simulação determinística (dois nós, sem rede, sem tempo — o objeto é a regra de sincronização):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23g_antientropia.c -o sins23g
./sins23g
# iguais: 0 faixas, 0 entradas (resumo decide, dado nem viaja)
# 1 chave a mais em b: 1 faixa diverge, 0 entradas voltam (versao de b vence)
# reverso: 1 faixa, 1 entrada, lojas iguais (sincroniza o que diverge)
# 3 faixas divergentes: 3 faixas, 3 entradas, resto nem viajou
# 23g ok: resumo decide, faixa igual nao viaja, divergente sincroniza
```

Fora do contrato: colisão tratada (FNV-1a de 32 bits colide em teoria; a aula declara a probabilidade em vez de fingir impossibilidade), remoção/tumba (só `presente`, sem GC de deletados), resumo hierárquico (Merkle de verdade, com prova por caminho — trabalho futuro listado no checklist).

## 2. Onde cada decisão mora

**O resumo decide o escopo.** `resumo_faixa` condensa 8 entradas em 32 bits; faixas iguais são puladas sem tocar nos dados. O ganho é medido em faixas, não em bytes de fio (a simulação não tem fio): 64 chaves, 1 divergente → 1/8 do custo da sincronização total. A honestidade da medida: o exemplo conta faixas e entradas, não bytes — byte de fio seria invenção sem socket.

**A versão decide a direção.** Divergência não significa "copia tudo": só viaja a entrada cuja versão no destino é menor. O cenário 2 mostra os dois lados — cópia cega traria o valor velho por cima do novo; com versão, 0 entradas voltam quando o destino está à frente. É a mesma regra de "termo vence" da 23d, aplicada por entrada em vez de por log.

**O resumo pode mentir (e a aula diz quando).** FNV-1a de 32 bits tem colisão: duas faixas diferentes com o mesmo resumo sincronizam "nada" e divergem para sempre. A probabilidade com 8 faixas é desprezível, mas "desprezível" não é "zero" — produção usa 64+ bits ou Merkle com re-check. Declarar a colisão no contrato é o que separa resumo didático de promessa falsa.

## 3. Erros comuns e implicações defensivas

1. **Sincronizar sem versão:** copiar a faixa inteira na direção errada apaga escrita mais nova. Resumo sem versão é detecção sem resolução — detecta, mas resolve errado.
2. **Resumir o ponteiro em vez do conteúdo:** FNV sobre endereço ou sobre struct com padding não-inicializado varia entre processos e entre boots. O `memcpy` campo a campo para o buffer existe para o resumo ser função do *dado*, não do *layout na memória*.
3. **Tratar colisão como impossível:** 32 bits colidem; teste que nunca viu colidir não prova ausência. O contrato promete detecção "quase sempre" e lista o upgrade (64 bits, Merkle) em vez de calar.
4. **Mandar tudo "porque é simples":** simplicidade que custa 64× o necessário em cada rodada não é simples — é o motivo pelo qual sincronização frequente morre em enlace caro. O resumo é a diferença entre "sincroniza sempre" e "sincroniza quando dá".

## 4. Ponte com as demais trilhas

C++ resolve a mesma divergência por cura com termo novo na [23f](../cpp/23f_eleicao_commit_rpc.md) (descarte nomeado, sem resumo); Go repara na leitura com vetor na 23c (paga o custo total por leitura); Rust declara o conflito e repara em duas passadas na 23d. Em Python, o análogo honesto é comparar `hashlib` por partição antes de copiar arquivos (rsync didático). O padrão "resumo decide escopo, versão decide direção" vale nas cinco trilhas — muda a função de hash e o tamanho da faixa.

## 5. Critério de aprovação e checklist

- [ ] Rodo o exemplo e digo, por cenário, quantas faixas viajaram e por quê.
- [ ] Explico por que o cenário 2 copia 0 entradas num sentido e 1 no outro.
- [ ] Descrevo um caso em que o resumo mente (colisão) e o que muda com 64 bits.
- [ ] Justifico o `memcpy` campo a campo (padding não-inicializado quebraria o resumo).
- [ ] Nomeio o que falta: Merkle hierárquico, tumbas com GC, resumo de 64 bits.

**Referências primárias:** o algoritmo de *anti-entropy* com Merkle trees do Dynamo (DeCandia et al., SOSP 2007) como origem do padrão resumo-escopo/versão-direção; especificação da FNV-1a para a função usada. Nenhum texto foi copiado — a implementação é própria, em um processo, sem rede.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
