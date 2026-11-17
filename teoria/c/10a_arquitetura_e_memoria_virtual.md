---
layout: default
---

# Aula 10a — Da linguagem C ao espaço de endereços de um processo

**Competências SDD:** ARQ, MEM e SEC. **Pré-requisitos:** aulas 01, 04, 07 e 10; saber compilar C, distinguir array de ponteiro e explicar tempo de vida automático e alocado. Não é preciso conhecer assembly.

**Objetivo observável:** ao terminar, você deverá desenhar o caminho de um acesso a memória desde uma expressão C até a CPU; distinguir endereço virtual de físico, cache de TLB e falha de página de acesso inválido; executar o experimento desta aula e interpretar um mapa de memória sem inferir endereços físicos.

**Motivação:** em Python ou JavaScript, `v[i]` parece acessar diretamente um elemento. Em C, `v[i]` também é uma expressão da linguagem, mas compreender desempenho, isolamento e falhas de memória exige perguntar quais instruções foram geradas e o que o sistema operacional e a CPU fazem com o endereço usado. A mesma ponte será necessária para estudar syscalls, kernels, concorrência e HPC.

## 1. Quatro contratos diferentes

| Camada | Pergunta que responde | Exemplo |
| --- | --- | --- |
| Linguagem C | O programa é válido e qual resultado a linguagem garante? | `v[i]` só pode acessar um elemento válido do objeto. |
| Compilador e ABI | Como representar valores, passar argumentos e gerar instruções para um alvo? | O compilador pode manter `local` em registrador; a ABI combina como chamar `printf`. |
| Sistema operacional | Qual processo possui cada mapeamento e com quais permissões? | Linux expõe regiões em `/proc/PID/maps`. |
| CPU e microarquitetura | Como executar instruções, traduzir endereços e usar caches? | Uma TLB pode guardar traduções; um cache de dados guarda cópias de dados. |

Não confunda **ISA** com microarquitetura. A ISA define o conjunto de instruções visível ao software; implementações diferentes da mesma ISA podem ter caches, pipelines e latências diferentes. Como exemplo concreto, a [ISA RV32I de RISC-V](https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html) define instruções de carga e armazenamento para mover dados entre registradores e memória. O código C não determina uma sequência única dessas instruções: alvo, compilador e otimização mudam o resultado.

**Modelo mental, com limite:** `expressão C → instruções para uma ISA → execução pelo núcleo da CPU → tradução de endereço virtual → acesso a dados`. Essa seta resume várias etapas e não é uma linha do tempo obrigatória para toda instrução. Uma variável pode nem gerar acesso à memória, e um acesso pode ser reordenado ou eliminado quando a semântica de C permitir.

## 2. Processo e espaço de endereços virtual

Um **processo** é uma instância em execução com estado administrado pelo sistema operacional, incluindo threads, recursos e mapeamentos. Um ponteiro impresso por um programa hospedado comum representa um endereço no **espaço virtual desse processo**; o número exibido não revela, por si, uma posição física de RAM. Em sistemas com memória virtual, processos podem usar o mesmo endereço virtual para dados físicos distintos. O kernel configura e protege esses mapeamentos; [a documentação de memória virtual do Linux](https://docs.kernel.org/admin-guide/mm/concepts.html) e [a da Microsoft](https://learn.microsoft.com/en-us/windows/win32/memory/virtual-address-space) explicam essa abstração.

```text
Processo A: endereço virtual X ── tabela de páginas A ── quadro físico P
Processo B: endereço virtual X ── tabela de páginas B ── quadro físico Q
```

O desenho é **conceitual**: P e Q podem ser distintos, e compartilhamento explícito pode fazer dois processos referenciarem o mesmo quadro. Nem todo sistema que executa C possui MMU ou memória virtual; isso é uma característica da plataforma, não uma promessa do padrão C.

No Linux, `/proc/PID/maps` mostra faixas virtuais mapeadas e permissões `r`, `w`, `x`, compartilhada (`s`) ou privada (`p`). Ele **não** revela diretamente os quadros físicos nem prova onde cada objeto C foi colocado. Uma alocação por `malloc` pode aparecer em `[heap]` ou numa região anônima; não conclua a origem apenas pelo rótulo. Consulte o formato documentado em [`proc_pid_maps(5)`](https://man7.org/linux/man-pages/man5/proc_pid_maps.5.html).

## 3. Páginas, TLB e falha de página

O sistema divide regiões virtuais em **páginas** e mantém estruturas de tradução para **quadros** físicos. O tamanho de página é parâmetro da plataforma: o experimento consulta o valor usado pela interface POSIX em vez de assumir 4096 bytes. A MMU usa tabelas de páginas e pode consultar uma **TLB**, cache de traduções recentes. Se uma tradução não está na TLB, isso é um *TLB miss*; **não** significa automaticamente uma falha de página. O mecanismo completo depende da arquitetura, como descreve a [documentação de tabelas de páginas do Linux](https://docs.kernel.org/mm/page_tables.html).

Uma **falha de página** ocorre quando o acesso exige intervenção do sistema operacional, por exemplo para criar uma página sob demanda ou tratar uma permissão ausente. Ela pode ser parte normal do funcionamento; se o acesso não puder ser atendido, o processo pode receber um erro ou sinal. Não iguale toda falha de página a “segmentation fault”. Reservar um intervalo virtual, ter uma tradução válida e ter RAM física ocupada são fatos diferentes.

## 4. Cache de dados não é TLB

O **cache de dados** guarda cópias de dados usados pela CPU. A **TLB** guarda traduções de endereços. Um acesso pode sofrer falta em um, no outro, em ambos ou em nenhum. O padrão de acesso importa: percorrer elementos vizinhos tende a aproveitar **localidade espacial**; revisitar dados tende a aproveitar **localidade temporal**. Isso é hipótese de desempenho a medir numa máquina específica, não garantia de C. A [documentação de otimização da Intel](https://www.intel.com/content/www/us/en/developer/articles/technical/loop-optimizations-where-blocks-are-required.html) apresenta esses conceitos de localidade.

Também não confunda cache de CPU com *page cache* do sistema operacional, que armazena conteúdo de arquivos. Mais adiante, benchmarks devem separar tempo de CPU, acesso à memória, paginação, aquecimento de cache e ruído de outros processos. **Esta aula não mede latência de cache ou TLB.**

## 5. Onde ficam `global`, `local` e `malloc`?

Em C, `global` tem **duração estática**; uma variável local comum tem **duração automática**; o bloco devolvido por `malloc` tem **duração alocada**, encerrada por `free`. Essas são categorias da linguagem ([rascunho C11 N1570, seções 6.2.4 e 7.22.3](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)). “Dados”, “pilha” e “heap” são modelos usuais de implementação, não endereços ou segmentos exigidos pelo padrão. Uma variável automática pode estar em registrador; `malloc` pode obter memória de diferentes mecanismos do sistema.

```text
Trecho conceitual de um processo hospedado comum:
regiões de código/bibliotecas | dados globais | regiões alocadas | pilhas de threads
```

O desenho não indica ordem, tamanho, continuidade nem permissão universal das regiões. Não compare ponteiros de objetos não relacionados com `<` para deduzir essa ordem: a comparação relacional não é definida para objetos arbitrários. Imprima-os apenas como observações independentes com `%p` e conversão para `void *`.

## 6. Experimento reproduzível no Linux ou WSL

O arquivo completo é [`exemplos/10a_memoria.c`](exemplos/10a_memoria.c). Ele consulta o tamanho de página com [`sysconf(_SC_PAGESIZE)`](https://man7.org/linux/man-pages/man3/sysconf.3.html), pede 8 MiB a `malloc`, escreve em posições separadas por esse tamanho, imprime endereços virtuais e espera Enter. Usa `getpid` e `sysconf`, que são **POSIX, não ISO C**. Cada escrita é válida porque o deslocamento é menor que o tamanho alocado. Os valores impressos são diagnósticos locais, nunca identificadores estáveis.

Em um terminal Linux ou WSL, a partir de `teoria/c`:

```bash
gcc --version
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O0 -g exemplos/10a_memoria.c -o 10a_memoria
./10a_memoria
```

Enquanto o programa espera, abra outro terminal no **mesmo ambiente Linux/WSL** e substitua `12345` pelo PID que apareceu:

```bash
cat /proc/12345/maps
```

Localize as faixas que contêm os endereços impressos e anote as permissões. Pode haver rótulos `[heap]` e `[stack]`, mas o bloco de `malloc` não precisa cair na faixa `[heap]`. Volte ao primeiro terminal e pressione Enter para liberar o bloco. Se a entrada padrão já estiver em EOF, o programa termina sem esperar; rode-o interativamente para inspecionar `/proc`.

**Resultado esperado, sem inventar números:** PID positivo; tamanho de página positivo; bloco solicitado de 8.388.608 bytes; número positivo de posições tocadas; quatro endereços formatados pela implementação; mapeamentos legíveis enquanto o processo está vivo. Endereços, tamanho de página, permissões, rótulos, soma e contagem dependem do ambiente. Uma segunda execução pode mostrar endereços diferentes, mas isso não é um teste suficiente de ASLR.

Na validação desta aula com WSL2/Ubuntu, o endereço do bloco caiu numa região anônima `rw-p`, separada da região marcada `[heap]`. Esta é uma observação daquela execução, não uma regra sobre `malloc`.

**Windows nativo:** este exemplo usa interfaces POSIX e o arquivo `/proc/PID/maps`; para seguir o laboratório obrigatório no Windows, use WSL. A API Windows oferece `VirtualQuery` para consultar regiões do processo, mas exigiria outro programa e outra leitura de resultados; veja a [documentação da Microsoft](https://learn.microsoft.com/en-us/windows/win32/memory/working-with-pages). A versão testada nesta revisão foi GCC 13.3.0 no WSL2/Ubuntu; outras versões estáveis devem ser registradas pelo leitor.

## 7. Segurança e limites do experimento

Permissões de mapeamento ajudam o sistema a impedir leitura, escrita ou execução indevida. Isolamento de processos, randomização de endereços e políticas de execução reduzem risco, mas não tornam válido um acesso fora do objeto C. O compilador pode explorar comportamento indefinido antes que uma proteção do sistema operacional seja relevante. Portanto, teste **limites do objeto**, verifique `malloc`, trate entrada externa e libere o bloco uma vez. A [aula 10](10.md) mostra exemplos de comportamento indefinido.

Este programa imprime endereços deliberadamente para estudo. Em serviço real, evite registrar ponteiros em logs ou respostas a clientes: eles expõem detalhes internos e não são necessários para a maioria dos diagnósticos. Execute o laboratório apenas com um processo seu; não tente ler `/proc` de processos de terceiros. O experimento não mostra páginas físicas, TLB, conteúdo de caches, política de substituição, NUMA nem causalidade de desempenho.

## 8. Exercícios e avaliação

1. **ARQ/MEM:** desenhe as quatro camadas da seção 1 para a expressão `bloco[0] = 1`; diga o que C garante e o que depende da plataforma.
2. **ARQ:** execute o programa, salve o comando e a versão do compilador, registre o PID, o tamanho de página e a faixa/permissões de pelo menos dois endereços em `/proc/PID/maps`. Explique por que esses endereços não são endereços físicos.
3. **ARQ:** repita a execução três vezes. Compare os endereços sem afirmar que diferença, igualdade ou ordem entre eles prova uma política específica de ASLR ou de alocação.
4. **ARQ/HPC:** explique separadamente o que uma falta na TLB, uma falta no cache de dados e uma falha de página solicitam ao sistema. Proponha um experimento futuro que meça localidade sem alegar que este programa já a mediu.
5. **SEC/MEM:** identifique duas informações que não devem ser copiadas dos logs do experimento para um serviço público e duas verificações necessárias antes de usar um tamanho recebido pela rede em `malloc`.

As [pistas graduais](exercicios/10a_pistas.md) e [soluções comentadas](exercicios/10a_solucoes.md) estão separadas do enunciado.

**Rubrica de aprovação:** entregue diagrama, código/versão/comando, observações reais com PID removido se compartilhar o relatório publicamente, interpretação e limites. Aprova quem (a) executa sem avisos, (b) encontra corretamente duas regiões, (c) separa garantias de C das escolhas de SO/CPU, (d) distingue TLB/cache/página e (e) não trata os endereços vistos como prova de endereço físico ou desempenho. Corrija e refaça qualquer item faltante.

## 9. Checklist de domínio

- Consigo explicar ISA, compilador, processo e microarquitetura sem tratá-los como sinônimos.
- Consigo diferenciar endereço virtual, quadro físico, tabela de páginas e TLB.
- Consigo dizer por que cache de dados, TLB e page cache guardam coisas diferentes.
- Consigo relacionar duração estática, automática e alocada aos modelos práticos sem chamar o modelo de garantia da linguagem.
- Consigo ler `/proc/PID/maps` e reconhecer o que ele **não** mostra.
- Consigo formular uma hipótese de localidade e planejar medi-la sem inventar uma observação.

**Próxima aula:** [11 — depuração](11.md), usando símbolos, pilha de chamadas e instrumentação para investigar programas reais.
