---
layout: default
---

# Rubrica SDD de aulas, módulos e projetos

Esta rubrica transforma o [contrato editorial](ESPECIFICACAO_SDD.md) em decisões de aprovação observáveis. Ela vale para os cinco livros. Uma nota alta não substitui a revisão de precisão técnica nem autoriza declarar um exemplo didático como pronto para produção.

## 1. Escala comum

| Nível | Evidência |
| --- | --- |
| 0 — ausente | Não existe artefato ou não foi possível executá-lo. |
| 1 — iniciado | Há implementação ou explicação, mas faltam casos essenciais, justificativa ou reprodução. |
| 2 — funcional | O caminho principal funciona e os conceitos básicos são explicados; limites e falhas ainda não estão demonstrados. |
| 3 — competente | O estudante demonstra correção, casos-limite, falhas relevantes, segurança defensiva e medição reproduzível conforme o módulo. |
| 4 — aprofundado | Além do nível 3, compara alternativas, explica trade-offs e investiga um resultado inesperado com evidência. |

**Passagem de módulo:** nível 3 ou 4 em todas as competências obrigatórias do módulo, com os artefatos indicados no seu mapa SDD. Não se compensa ausência de segurança com desempenho, nem falha de correção com qualidade de escrita. Laboratórios opcionais não bloqueiam o núcleo obrigatório.

## 2. Rubrica de exercício

Um exercício aprovado inclui:

1. **Contrato:** entradas, saídas, pré-condições e casos especiais definidos pelo aluno.
2. **Raciocínio:** invariante, argumento de correção ou modelo mental adequado ao problema.
3. **Execução:** programa ou resposta reproduzível conforme o enunciado, sem arquivos locais ocultos.
4. **Limites:** pelo menos as classes de erro, faixa, memória ou concorrência pertinentes.
5. **Explicação:** o aluno consegue justificar uma decisão sem recitar a solução comentada.

Para exercícios introdutórios, medição de desempenho e modelo de ameaças só são exigidos quando o enunciado os apresenta. Para rede, FFI, kernel e concorrência, a avaliação de falhas e segurança é obrigatória.

## 3. Rubrica de projeto de sistemas

| Dimensão | Evidência exigida para nível 3 |
| --- | --- |
| Correção | Fluxos principais e casos de borda passam em testes independentes; invariantes principais estão declarados. |
| Reprodutibilidade | Instruções, versões e ambiente permitem construir e executar a partir de checkout limpo. |
| Falhas | Entrada inválida, truncamento, desconexão, repetição, atraso, esgotamento ou corrida são ensaiados conforme a natureza do projeto. |
| Segurança defensiva | Fronteiras de confiança, limites de recursos, privilégios, segredos e superfícies de ataque são documentados e testados. |
| Memória e concorrência | Ownership/tempo de vida e sincronização são explicados; ferramentas de diagnóstico aplicáveis não revelam falha conhecida. |
| Desempenho | Há baseline, hipótese, medição com ambiente descrito e interpretação sem alegar ganho não demonstrado. |
| Comunicação | README ou relatório explica arquitetura, limites, decisões rejeitadas e trabalho ainda necessário. |

Projetos de kernel em C, C++ e Rust precisam mostrar boot, isolamento e pelo menos uma extensão de syscall, memória ou escalonamento em QEMU antes da etapa x86-64. Projetos distribuídos precisam demonstrar uma falha parcial; um servidor único que responde HTTP não prova competência em sistemas distribuídos. Experimentos de GPU com placa física são opcionais, mas o modelo de execução e a comparação com referência CPU são obrigatórios.

## 4. Gate editorial de publicação

O autor ou revisor só muda uma aula para **verificada** no [registro por arquivo](AUDITORIA_POR_ARQUIVO.csv) quando:

- conceitos novos aparecem após seus pré-requisitos e uma leitura de ponta a ponta encontra a explicação prometida;
- exemplos rotulados completos foram executados na plataforma e toolchain declaradas, com saída registrada;
- exemplos de falha estão claramente rotulados, têm mecanismo explicado e correção ou exercício de correção;
- links locais funcionam, referências sensíveis a versão apontam para fonte primária e conteúdo externo foi usado com atribuição;
- exercícios têm enunciado, pistas, solução separada e critério de aprovação;
- achados P0/P1 da aula estão fechados com evidência, sem teste enfraquecido ou aviso ocultando defeito.

Um **módulo verificado** exige todas as suas aulas verificadas e um percurso completo feito em ambiente limpo. Um **livro publicável** exige todos os módulos obrigatórios e projeto final no nível 3 ou 4; o site pode exibir partes em revisão, mas deve identificá-las como tal. O responsável registra limitações de ferramentas e hardware em vez de afirmar que verificou o que não executou.

## 5. Como registrar evidência

Para cada validação, anote: arquivo ou competência, plataforma, versão de compilador/runtime, comando, resultado observado, casos de falha executados, resultado da revisão técnica e limite ainda aberto. A coluna `estado_editorial` do registro continua `rascunho` durante revisões parciais. A ausência de Jekyll, QEMU, GPU ou outra ferramenta de um laboratório é uma lacuna de verificação, não uma aprovação presumida.
