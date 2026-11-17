# Pistas da aula 30b

1. Sem viagem por chamada, o custo de 100 SAXPYs residentes e latencia x 100 + kernel x 100, sem o termo PCIe. Compare com uma unica viagem inicial amortizada em 100 kernels: o limiar despenca.
2. Monte a tabela variando um parametro de cada vez (latencia, PCIe, banda GPU, banda CPU) e resolvendo tempo_gpu == tempo_cpu para n. O parametro que mais move o limiar em problema pequeno e o de custo fixo.
3. Com 50 flops por elemento e os mesmos bytes, o kernel fica 25x mais caro em calculo sem ficar mais caro em viagem: o termo dominante muda de lugar e o veredito em n pequeno pode virar.
4. Medir separado: transferencia ida e volta sozinhas (sem kernel), kernel com dados residentes repetido muitas vezes, e o total. Se total != soma, o que falta e sincronizacao/lancamento nao contabilizado — o modelo ganha um termo ou o lancamento esta serializando.
