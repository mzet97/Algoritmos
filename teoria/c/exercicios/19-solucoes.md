---
layout: default
---

# Aula 19: solucoes comentadas

Compare apos tentar os [exercicios](../19.md#16-exercicios-obrigatorios).

1. A reescrita segue o padrao executado na [solucao da aula 18](18-solucoes.md) (ponteiro opaco, `create/destroy`, callback com `userdata`). Aplicado a uma API antiga, o passo novo e o inventario: liste tudo o que o header antigo expoe (campos, funcoes auxiliares, macros com acesso a campos) — cada item exposto vira funcao da nova API ou decisao documentada de remocao.

2 e 3. CLI com contrato de erro coerente e a convencao que ela implementa:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Convencao desta mini biblioteca:
   0 = sucesso; 1 = erro de uso (entrada invalida); 2 = erro de execucao.
   Quem imprime e a CLI; a biblioteca so devolve codigos e nao escreve
   em stdout/stderr. */

static int soma_argumentos(int argc, char *argv[], long *saida)
{
    long total = 0;
    for (int i = 1; i < argc; i++) {
        char *fim = NULL;
        long valor = strtol(argv[i], &fim, 10);
        if (fim == argv[i] || *fim != '\0') {
            return 1;
        }
        total += valor;
    }
    *saida = total;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "uso: %s INTEIRO [INTEIRO...]\n", argv[0]);
        return 1;
    }
    long total = 0;
    int rc = soma_argumentos(argc, argv, &total);
    if (rc != 0) {
        fprintf(stderr, "erro: argumento nao numerico\n");
        return rc;
    }
    printf("soma: %ld\n", total);
    return 0;
}
```

`./soma 2 3 4` imprime `soma: 9` e sai 0; `./soma 2 x` escreve em `stderr` e sai 1. Testes automatizados dependem exatamente dessa separacao: `stdout` comparavel, `stderr` livre para diagnostico e codigo de saida como contrato maquina-legivel ([aula 31](../31.md) cobra o mesmo do servidor).

4. Observabilidade minima de um servidor simples em C: log de inicio com porta e versao; contador de conexoes aceitas, concluidas e rejeitadas; contador de erros por classe (aceite, leitura, protocolo); tempo de atividade e, se houver filas, o tamanho atual delas. Tudo em texto estruturado (`chave=valor`) com carimbo de tempo — o suficiente para responder "esta vivo? esta aceitando? esta errando mais que ontem?" sem depurador.

5. Build e testes reprodutiveis sao o que separa "funcionou na minha maquina uma vez" de engenharia: o mesmo comando constroi do zero em outra maquina, os testes guardam decisoes contra regressao e o historico mostra qual mudanca quebrou o que. Sem isso, cada correcao e arqueologia. E por isso o `regress.sh` deste repositorio e tratado como parte do conteudo, nao como acessorio.

**Autoverificacao:** CLI com os tres canais usados corretamente, convencao de erro escrita antes do codigo e pelo menos um teste que confira codigo de saida (nao so a saida impressa).
