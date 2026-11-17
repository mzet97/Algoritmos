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

**Autoverificacao:** o arquivo modificado compila sem avisos, os formatos conferem com os tipos e a explicacao diferencia fonte, executavel e processo.
