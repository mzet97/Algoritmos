---
layout: default
---

# Aula 01: pistas graduais

Leia apenas a pista do exercicio em que travou. Volte ao enunciado em [01.md](../01.md#13-exercicios-obrigatorios).

1. Execute `gcc --version`. Compile o arquivo fornecido a partir de `teoria/c` com `-std=c11 -Wall -Wextra -Wpedantic`; no Windows o executavel gerado tem extensao `.exe`.
2. Mude um valor de cada tipo por vez. Antes de executar, escreva a linha que espera observar.
3. A tabela de formatos da aula associa `long` a `%ld`. Um literal do tipo `long` pode ter o sufixo `L`.
4. Execute o programa e depois consulte `$LASTEXITCODE` no PowerShell ou `$?` em Bash. Faca a consulta imediatamente: um comando posterior pode alterar esse valor.
5. Pense no que existe antes de iniciar o processo e no que a CPU executa depois que o sistema carrega o binario.
6. Cada flag gera um arquivo: `-E` (fonte expandida), `-S` (assembly texto), `-c` (objeto binario). Compare o numero de linhas do `.c` com o do `.i` e procure de onde veio a diferenca.
7. Leia o aviso como sentenca: ele diz qual tipo o formato espera e qual tipo voce entregou. A pergunta do exercicio nao e "o que sai", e "por que a linguagem nao promete nada aqui".
8. Rode duas vezes e compare os enderecos digito a digito. Para o `.s`, gere com `-O0` e com `-O2` e conte as linhas de cada um antes de tentar explicar a diferenca.
