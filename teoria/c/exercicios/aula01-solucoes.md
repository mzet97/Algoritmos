---
layout: default
---

# Aula 01: solucoes comentadas

Compare apenas depois de tentar os [exercicios](../01.md#13-exercicios-obrigatorios). Valores alternativos igualmente corretos sao aceitos.

1. `gcc -std=c11 -Wall -Wextra -Wpedantic -O0 -g exemplos/aula01.c -o aula01` seguido de `.\aula01.exe` (PowerShell) ou `./aula01` (Linux/macOS). A saida original e `idade = 32`, `altura = 1.78`, `inicial = A`, cada item em uma linha. Registre a versao real do seu compilador.
2. Se `idade` virar `33`, `altura` virar `1.80` e `inicial` virar `'B'`, as tres linhas passam a terminar em `33`, `1.80` e `B`. `%.2f` mostra duas casas decimais; a representacao binaria do `double` nao equivale necessariamente a um decimal exato.
3. Acrescente `long distancia = 1000L;` no corpo de `main` e `printf("distancia = %ld\n", distancia);`. `%ld` espera `long`; `%d` espera `int` e nao serve como substituto.
4. Com `return 1;`, a saida textual permanece igual, mas o estado do processo passa a indicar falha por convencao. No PowerShell leia `$LASTEXITCODE`; em Bash, `echo $?` imediatamente apos o programa.
5. Compilar transforma codigo fonte, com ajuda de assembler e linker, num executavel para o ambiente alvo. Executar cria um processo que roda as instrucoes do executavel; a compilacao nao executa automaticamente o `main`.
6. `aula01.i` tem ~570 linhas (o `.c` tem 13): a diferenca vem da expansao de `stdio.h`. `aula01.s` contem o rotulo `main` em assembly; `aula01.o` e relocavel (`file` diz ELF 64-bit relocatable) — falta o startup da biblioteca C e os enderecos finais, que o linker resolve. Na referencia: `.s` com 54 linhas.
7. O aviso e `format '%f' expects argument of type 'double', but argument 2 has type 'int'`. `%f` le 8 bytes como `double` onde so ha 4 de `int`: o valor lido e lixo de representacao, e a linguagem nao define qual — por isso nao ha "saida correta" para prever. Correcao: `printf("%d\n", numero);`.
8. Os enderecos diferem entre runs (ASLR). Com `-O2`, o `.s` cai para ~44 linhas: a otimizacao elimina moldura e simplifica a sequencia de chamada, mas o programa observa o mesmo comportamento. O que some e custo de moldura, nao semantica.

**Pre-teste (gabarito curto):** (1) codigo de maquina/objeto, nao necessariamente executavel final; (2) ponto de entrada logico com codigo de saida `int`; (3) nao — formato e tipo precisam combinar; (4) nao necessariamente (registrador, eliminacao); (5) nao — so prova que passou na compilacao e linkagem.

**Pos-teste (gabarito curto):** (1) pre-processador (`.i`), compilador (`.s`), assembler (`.o`), linker (executavel); (2) informa sucesso/falha; quem le e o processo pai/shell (`$?`, `$LASTEXITCODE`); (3) `printf("%d\n", 10);` + aviso transcrito; (4) ASLR — o SO randomiza o layout do processo; (5) `void *`, por contrato de `%p`; sem a conversao o tipo nao confere.

**Autoverificacao:** o arquivo modificado compila sem avisos, os formatos conferem com os tipos e a explicacao diferencia fonte, executavel e processo.
