---
layout: default
---

# Aula 04 (C++): pistas graduais

Volte aos [exercicios](../04.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. A classe guarda nome e preco como membros privados; a interface publica comeca pequena.
2. O construtor inicializa na lista de membros (`: nome_(...), preco_(...)`), nao por atribuicao no corpo.
3. `mostrar()` nao altera o objeto: marque-o `const`.
4. No destrutor que imprime, crie o objeto dentro de um bloco e observe **quando** a mensagem aparece.
5. RAII amarra o recurso ao tempo de vida do objeto; pense em quem chama a liberacao se voce esquecer.
