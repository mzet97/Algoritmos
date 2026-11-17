---
layout: default
---

# Aula 32: pistas graduais

Volte aos [exercicios](../32.md#18-exercicios-obrigatorios) antes de abrir as solucoes.

1. Remover e procurar a chave, liberar as copias internas e fechar o buraco no array; defina o retorno para chave ausente antes.
2. Carregar do arquivo e ler linha a linha separando no primeiro `=`; linha sem `=` e erro ou item ignorado — decida e documente.
3. O teste de atualizacao faz `set` duas vezes na mesma chave e confere valor e contagem.
4. Para listar sem expor o array: callback de visita com `void *` de contexto, como na [aula 18](18-solucoes.md).
5. Copiar as strings na entrada transfere a vida util para a loja; pense no que acontece se a loja guardasse o ponteiro do chamador.
6. Trocar busca linear por hash sem quebrar a API: o que o cliente ve (funcoes e contrato) e o que ele nao ve (o indice)?
