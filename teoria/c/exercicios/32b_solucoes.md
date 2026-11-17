# Soluções comentadas da aula 32b

## 1. Divide-error (vetor 0)

A entrada 0 aponta para um handler próprio. Diferenças para o `int3`: a CPU empilha também um código de erro (para `#DE` ele é sempre 0, mas ocupa o slot), e o RIP salvo aponta **para a instrução que dividiu** — `iretq` direto reexecutaria a divisão e faltaria para sempre, o análogo exato de esquecer o `mepc += 4` no `ecall`. As saídas honestas: avançar o RIP salvo para depois da instrução (exige decodificar o comprimento dela) ou tratar como fatal e desligar com diagnóstico. Nesta aula, desligar com marcador próprio é a resposta proporcional; fingir retorno é a errada.

## 2. Páginas de 4 KiB no primeiro MiB

Sem PS no PD, cada entrada do PD aponta para uma PT com 512 entradas de 4 KiB. Para cobrir 1 MiB: PML4 (já existe) → PDPT (já existe) → PD (vira tabela de ponteiros) → 1 PT (512 × 4 KiB = 2 MiB de cobertura, metade usada). São 4 tabelas no caminho contra 3 com páginas de 2 MiB. A granularidade compra proteção por página (a página de guarda, a página sem X); o walk paga um nível a mais por miss de TLB.

## 3. Syscall pela entrada 0x80

Montar a entrada `0x80` da IDT como a do `int3`, com um handler que lê `RAX`: `1` = imprimir o byte em `RDI` (ou o buffer em `RSI`), demais = erro declarado. A correspondência com a 32a é direta: número em registrador contratado, argumentos em registradores, retorno pelo mesmo caminho da exceção. As duas diferenças que importam: no x86 o endereço de retorno foi empilhado pela instrução (não há `mepc` para somar), e a entrada de IDT usada por software (`int 0x80`) precisa de DPL 3 para ser alcançável de anel 3 — neste componente tudo roda em anel 0, então o DPL fica como exercício de leitura do descritor.

## 4. Boot real contra Multiboot

BIOS legada entregaria modo real de 16 bits, sem GDT utilizável e sem mapa de memória confiável além do `int 15h`; UEFI entregaria o contrário: modo longo já ativo, paging ligado, serviços de firmware substituindo o acesso direto a dispositivos. O Multiboot fica no meio didático: protegido de 32 bits, paging desligado, mapa de memória em estrutura própria. Sobrevivem intactos de qualquer um dos três: a necessidade de serial precoce, a IDT desenhada campo a campo, o modelo salvar→tratar→restaurar e o desligamento como convenção de teste. O que nunca sobrevive é a ilusão de que "o bootloader deixou tudo pronto".
