# Pistas da aula 23b

1. Duplicar a linha cola dois pedidos sem separador: `PUT id1 k1 v1PUT id1 k1 v1`. Conte os tokens antes de rodar e preveja a classe. Se a sua duplicação inserir um espaço no meio, a previsão muda — as duas variantes ensinam algo; registre qual você implementou.
2. Com teto 16, `PUT id-2 chave.x valor_9 F` (26 bytes) e `GET chave-longa_0123456789` (26) morrem por comprimento, não por conteúdo. Casos dirigidos com linhas curtas sobrevivem. Quem está errado é a sua escolha de bases para o novo contrato — ou o contrato novo, se a intenção era continuar aceitando o protocolo da 23a.
3. Sem alfabeto, procure uma mutação que troque um caractere de `id1` por algo como `$`, `/` ou byte de controle: o parser aceita e o campo carrega o byte proibido para dentro do estado. A linha exata sai do fuzzer se você imprimir a primeira mutação aceita com byte fora de `[A-Za-z0-9._-]`.
4. Meça com `time`: divida 200 000 pelo tempo em cada modo. ASan tipicamente custa 2–5×; o orçamento de 10 s compra milhões de casos nos dois modos — a decisão real é sobre variância de CI, não sobre o total.
