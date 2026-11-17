# Soluções comentadas da aula 23b

## 1. Duplicar a linha

Sem espaço entre as cópias, os tokens viram `PUT, id1, k1, v1PUT, id1, k1, v1` — 7 tokens, acima do máximo: tudo erro. A contagem de `put` cai e a de `erro` sobe; nenhum sucesso novo nasce, porque a concatenação destrói o quarto token. Com um espaço inserido na junção são 8 tokens: também erro. A lição: colar mensagens é exatamente o problema de framing da aula 23 — sem delimitador ou comprimento, dois pedidos viram zero pedidos.

## 2. Teto de 16

Falham primeiro os casos dirigidos com as linhas longas das bases (`PUT id-2 chave.x valor_9 F` e `GET chave-longa_0123456789`), por comprimento. O contrato novo (16) é incompatível com o protocolo da 23a: ou o teto volta a caber no protocolo, ou as bases e o enunciado mudam juntos. Mudar só o número e deixar os testes vermelhos "para depois" é como nasce regressão silenciosa.

## 3. Sem verificação de alfabeto

Qualquer mutação de um byte de token para fora do alfabeto passa a ser aceita: por exemplo `PUT id$ k1 v1` (o caso dirigido da aula, agora válido) ou mutações com `/`, que num sistema que usa a chave como nome de arquivo viraria travessia de diretório. O fuzzer encontra a classe em segundos; imprimir a primeira ocorrência dá a linha exata para a regressão. Sem o teste de alfabeto, a invariante "campo só contém `[A-Za-z0-9._-]`" deixa de existir e todo consumidor do campo herda a entrada crua.

## 4. Orçamento

Nesta máquina: ~0,02 s sem sanitizer (≈10 milhões de casos/s) e tipicamente 2–5× mais lento com ASan. Com 10 s de CI: dezenas de milhões de casos sem sanitizer, milhões com. Decisão razoável: todo commit roda o fuzz curto determinístico com ASan (a semente fixa torna o resultado binário: passou/falhou), e a varredura longa roda agendada. O que não se faz: fuzz sem sanitizer como único gate — crash silencioso por leitura de 1 byte não aparece sem ele.
