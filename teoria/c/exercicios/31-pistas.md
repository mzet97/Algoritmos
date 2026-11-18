---
layout: default
---

# Aula 31 - Pistas graduais

1. Verifique O_NONBLOCK tanto no listener quanto em cada socket retornado por accept.
2. Se send transmite menos bytes, mantenha a parte restante e espere POLLOUT; nao repita indefinidamente no mesmo cliente.
3. Uma leitura pode conter multiplas linhas. Pare de interpretar depois de QUIT.
4. O EOF de leitura nao apaga respostas ja enfileiradas; envie-as antes de fechar, se o par ainda as aceitar.
5. Um cliente lento pode ser absorvido pelo buffer do kernel por algum tempo. Envie dados suficientes para testar a politica da fila da aplicacao, sem concluir cedo que ela nunca enche.
6. Para o teste de dois clientes, mantenha o primeiro conectado e sem consumir; o segundo deve obter PONG em tempo finito.
