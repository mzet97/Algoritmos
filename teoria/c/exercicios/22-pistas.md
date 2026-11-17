---
layout: default
---

# Aula 22: pistas graduais

Volte aos [exercicios](../22.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. No `pipe`: quem escreve fecha a ponta de leitura, quem le fecha a de escrita; o leitor termina quando `read` devolve 0.
2. Para domain socket contra TCP local: compare o que voce ganha ao sair da pilha de rede e o que perde em portabilidade/enderecamento.
3. Memoria compartilhada troca copias por coordenacao: quem garante que o outro processo nao le no meio da escrita?
4. No protocolo sobre o pipe: defina tamanho ou delimitador antes do conteudo, como nas aulas de rede.
5. Para sincronizar shared memory: pense em mutex entre processos, semaforos e numeros de sequencia — e em quem recupera se um lado morre no meio.
