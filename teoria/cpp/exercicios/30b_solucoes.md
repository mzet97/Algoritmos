# Solucoes comentadas da aula 30b

## 1. Dados residentes, 100 kernels

Com x e y residentes, cada SAXPY paga latencia + kernel, sem PCIe: em n = 10 000, kernel de ~0,13 microssegundos contra latencia de 10 — a latencia passa a mandar sozinha, e mesmo assim 100 chamadas custam ~1 ms contra ~0,4 ms de CPU sequencial estimada... some com cuidado: a CPU tambem repetiria 100 vezes (0,39 ms). O ganho real de residencia aparece quando o encadeamento evita nao so a viagem, mas permite fundir kernels. A conta, nao o entusiasmo, decide.

## 2. Sensibilidade do limiar

Dobrando a latencia para 20 us, o limiar (onde tempo_gpu == tempo_cpu) sobe aproximadamente na proporcao do custo fixo extra dividido pela economia por elemento. Em problema pequeno, latencia e o parametro que manda; em problema grande, banda PCIe e banda da CPU mandam e a latencia some na divisao. A tabela de sensibilidade e o produto: ela diz onde medir melhor antes de decidir.

## 3. Kernel gordo (50 flops/elemento)

O tempo de kernel cresce ~25x sem mudar viagem nem latencia: em n = 10 000, o modelo passa a favorecer a GPU muito mais cedo, porque agora ha trabalho de calculo para amortizar o custo fixo. Intensidade aritmetica e o dial que move o veredito; SAXPY e o extremo desfavoravel usado na aula de proposito.

## 4. Plano de medicao real

Medir: (a) ida e volta de transferencia sem kernel (bytes/tempo = PCIe efetiva, nao a do folheto); (b) kernel residente em loop de 1000 lancamentos (tempo por lancamento = kernel + latencia de lancamento, separaveis variando n); (c) total de ponta a ponta. Se (c) > (a)+(b), o termo faltante costuma ser sincronizacao ou alocacao no device por chamada — que o modelo passa a incluir explicitamente. Modelo bom nao e o que acerta de primeira; e o que sabe dizer onde errou.
