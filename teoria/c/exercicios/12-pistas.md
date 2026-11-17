---
layout: default
---

# Aula 12: pistas graduais

Volte aos [exercicios](../12.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. A biblioteca comeca como um `.c` com as duas funcoes e um header com os prototipos; compile com `-c` antes de arquivar.
2. Quem arquiva e o `ar` (`ar rcs`), e quem usa a biblioteca inclui o header e linka o `.a` depois do fonte que o usa.
3. Para a diferenca: separe "codigo objeto ainda relocavel", "arquivo de objetos" e "programa ja ligado com endereco e bibliotecas resolvidos".
4. ABI pede o contrato no nivel binario: tamanhos, alinhamento, convencao de chamada e nomes de simbolos.
5. Para as quebras: pense em mudar assinatura de funcao publicada e mudar ordem/tamanho de campos de struct exposta.
