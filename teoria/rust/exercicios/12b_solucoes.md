# Solucoes comentadas da aula 12b

## 1-4 (resumo comentado)

Remocao segue os tres casos da 13b de C com &mut Link no lugar de ponteiro duplo; o sucessor e o minimo da subarvore direita. A construcao pela mediana entrega altura [log2(n+1)] porque cada nivel divide o intervalo ao meio. A arena troca n alocacoes Box por um Vec unico: ganha localidade e perde a prova de dono por no — indices podem ficar obsoletos se o Vec for reorganizado, e esse e o preco declarado. Para entrada ordenada adversaria, a defesa de producao em Rust e nao implementar: BTreeMap/BTreeSet da std sao arvores balanceadas auditadas; BST manual e instrumento de aula, e dizer isso e a resposta madura.
