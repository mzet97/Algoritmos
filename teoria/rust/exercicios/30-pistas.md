---
layout: default
---

# Aula 30 (Rust): pistas graduais

Volte aos [exercicios](../30.md#exercícios-práticos) antes de abrir as solucoes. A solucao executada desta aula e o projeto [30_load_balancer](../projetos/README.md), verificado por `teoria/verificar_projetos.py`.

1. Timeout por tentativa e um `tokio::time::timeout` em volta do `connect` **e** de cada leitura, nao do processo inteiro.
2. Health check ativo e uma tarefa periodica que testa cada backend e mantem a lista de saudaveis separada da lista de todos.
3. O round-robin escolhe so entre saudaveis; se a lista esvaziar, a resposta honesta e erro, nao tentativa cega.
4. Logging por conexao com endereco do backend escolhido e o que transforma "esta lento" em diagnostico.
