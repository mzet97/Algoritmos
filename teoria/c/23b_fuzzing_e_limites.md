---
layout: default
---

# Aula 23b — Fuzzing determinístico e limites: o parser sob ataque controlado

**Competências SDD:** SEC, NET e CON. **Pré-requisitos:** aulas 15, 20, [23](23.md), [23a](23a_sistemas_distribuidos.md) e 11 (sanitizers). Você precisa saber o que é um parser de linhas, compilar com warnings e rodar ASan/UBSan.

**Objetivo observável:** ao terminar, você deverá executar um fuzzer determinístico de 200 000 mutações sobre o parser do protocolo da aula 23a, provar sob ASan/UBSan que nenhuma mutação causa crash, leitura fora do limite ou resultado indefinido, e enunciar os limites que o parser impõe antes de qualquer alocação: teto de linha, teto de token, alfabeto fechado e número máximo de tokens.

**Motivação:** a aula 23a testou o parser com casos escolhidos a dedo: malformado, truncado, longo. Casos escolhidos a dedo provam que você pensou naqueles casos — e só neles. Fuzzing inverte a pergunta: em vez de adivinhar a entrada que quebra, gera-se entrada quebrada em massa e observa-se se o programa sobrevive com comportamento definido. É a diferença entre "testei o erro" e "o erro não me acha primeiro".

## 1. Contrato antes do código

Parser desta aula (`analisar(buf, len, out)`):

- **Pré-condição:** nenhuma sobre o conteúdo. `buf` pode não terminar em `'\0'`; o parser trabalha **só** com `[0, len)` — presumir terminador em buffer de rede é a leitura fora do limite clássica.
- **Pós-condição:** o resultado é sempre um de `PARSE_PUT`, `PARSE_GET` ou `PARSE_ERRO`. Não existe quarto estado, nem campo pela metade: em erro, nada do pedido é aproveitável.
- **Limites, verificados antes de copiar qualquer byte:** linha ≤ 512, `id` ≤ 31, `chave` ≤ 63, `valor` ≤ 127, no máximo 5 tokens (mais a sentinela), alfabeto `[A-Za-z0-9._-]`. Acima do teto é `ERRO`, nunca "corta e aceita" — truncar entrada hostil e aceitar o resto é aceitar entrada que ninguém enviou.
- **Orçamento:** o fuzzer roda 200 000 iterações com semente fixa (99). Falha se reproduz byte a byte; `time(NULL)` como semente destruiria isso.

## 2. Mutação não é aleatoriedade

O fuzzer parte de cinco linhas válidas e aplica, por iteração, 1 a 3 mutações sorteadas por LCG: trocar um byte (0–127, incluindo controles e espaço), apagar um byte, inserir um byte, truncar. Cada mutação ataca uma hipótese diferente:

| Mutação | Hipótese atacada |
|---|---|
| Trocar byte | o alfabeto e a comparação de operação rejeitam qualquer byte fora do contrato |
| Apagar byte | tokens que somem não fundem vizinhos em algo aceitável por acidente |
| Inserir byte | tokens a mais e separadores duplicados não criam um pedido novo válido por sorte |
| Truncar | meio token, meia operação e buffer sem `'\0'` continuam sendo erro definido |

O invariante pós-parse é verificado a cada sucesso: campos dentro dos tetos. Se o parser devolver `PARSE_PUT`, os três campos respeitam os limites — o fuzz testa a pós-condição, não só a ausência de crash. E as três classes (`put`, `get`, `erro`) precisam aparecer: um fuzzer que só gera erro não testou o caminho feliz, e um que só gera sucesso não testou limite nenhum.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23b_fuzz.c) contém o programa completo:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23b_fuzz.c -o /tmp/23b
/tmp/23b
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/23b_fuzz.c -o /tmp/23b_asan
/tmp/23b_asan   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística, semente 99; contagens exatas nesta implementação):

```text
fuzz: 200000 iteracoes put=22572 get=22456 erro=154972 (semente 99)
23b ok: parser definido sob mutacao, tetos e truncamentos
```

Leitura honesta dos números: ~77% das mutações viram erro — esperado, porque o espaço de linhas válidas é minúsculo perto do espaço de bytes. Os ~23% que sobrevivem como `PUT`/`GET` válidos são os casos interessantes: mutações em posições que não mudam a semântica (um dígito trocado por outro dígito, por exemplo). São eles que exercitam o caminho de sucesso com entrada que ninguém escreveria a mão. Os 200 000 casos rodam em ~0,02 s sem sanitizer: orçamento de fuzz se mede em casos por segundo, e parser lento é parser pouco fuzzado.

### O que a memória faz aqui?

Nada é alocado no parser: tokens são fatias `[ini, fim)` sobre o buffer de entrada, copiados só no sucesso e só dentro dos tetos declarados nas struct de destino. Zero `malloc` por linha significa zero vazamento por linha hostil — a entrada mais maliciosa do mundo custa o mesmo que a mais benigna: tempo de varredura limitado pelo teto de 512. Limite de recurso também é isto: custo por entrada limitado *antes* de saber se ela é válida.

## 4. Erros comuns e implicações defensivas

1. **Fuzzar com `srand(time(NULL))`:** a falha que aparece às 3h não se reproduz às 9h. Semente fixa e registrada, ou o fuzz é teatro.
2. **Parser que presume `'\0'`:** `strlen`/`strtok` sobre buffer de rede lê até achar um zero — possivelmente na página seguinte. Comprimento explícito ou nada.
3. **Truncar e aceitar:** cortar a linha em 512 e processar o prefixo aceita um pedido que o cliente nunca enviou por inteiro. Acima do teto é erro, sempre.
4. **Só contar crash:** parser que não crasha mas aceita campo acima do teto quebrou o contrato do mesmo jeito. Verifique a pós-condição de cada sucesso.
5. **Fuzz sem casos dirigidos:** mutação raramente constrói, do zero, o caso "5º token inválido" que você já conhece. Casos dirigidos primeiro, caos depois — os dois, nesta ordem.
6. **Confundir fuzz local com segurança:** 200 000 mutações determinísticas são regressão, não prova. Cobertura maior (libFuzzer/AFL) é laboratório opcional; o obrigatório é o parser sobreviver ao que está no repositório.

## 5. Ponte com Python, JavaScript e as demais trilhas

Python fuzzaria o mesmo parser com `hypothesis` ou mutação de `bytes` — e o bug equivalente seria presumir que todo `bytes` decodifica em UTF-8. JavaScript tropeçaria em string que é sequência de UTF-16, não de bytes. Em C, o contrato `(ponteiro, comprimento)` é explícito e o preço de ignorá-lo é leitura fora do limite, não exceção: por isso aqui o fuzz roda sob ASan, que transforma "lê um byte a mais" de corrupção silenciosa em relatório.

## 6. Exercícios obrigatórios

1. Adicione a mutação "duplicar a linha inteira" e preveja, antes de rodar, o que muda nas contagens. Confirme e explique a diferença.
2. Troque o teto de linha para 16 e rode: quais casos dirigidos falham primeiro? Ajuste as bases ou o contrato — e justifique qual dos dois estava errado.
3. Remova a verificação de alfabeto (`eh_token_char` sempre 1) e encontre, com o fuzzer, um contraexemplo concreto que o parser passa a aceitar indevidamente. Registre a linha exata.
4. Meça casos/segundo com e sem ASan e calcule quantos casos o seu orçamento de CI (digamos, 10 s) compra em cada modo. Decida qual modo roda em cada commit e qual roda à noite.

Use [pistas graduais](exercicios/23b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23b_solucoes.md) depois de registrar sua própria explicação.

## 7. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- enunciar os quatro limites do parser e onde cada um é verificado;
- explicar por que `(ponteiro, comprimento)` e nunca presumir `'\0'`;
- rodar as 200 000 mutações com semente fixa e reproduzir uma falha plantada;
- verificar a pós-condição de cada sucesso, não só a ausência de crash;
- declarar o que o fuzz **não** prova e o que cobertura maior mudaria;
- compilar com `-Werror` e passar limpo sob ASan/UBSan.

**Próxima aula:** 24 — heap real e fragmentação. O parser desta aula não aloca nada por linha; a aula 24 mostra o que acontece com quem aloca.
