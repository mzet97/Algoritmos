---
layout: default
---

# Soluções comentadas da aula 13a

## 1. Prova do invariante

- **Início:** `ini = 0`, `fim = n`; a fatia é o vetor inteiro e a promessa vale.
- **Preservação:** com `ini < fim`, `meio` está dentro da fatia. Igualdade entrega a pós-condição; os ramos descartam metades que a ordenação prova estarem do lado errado.
- **Término:** `fim - ini` diminui estritamente; ao sair, `ini == fim` e `-1` é correto. Com `n == 0`, o corpo nunca executa e `v` nem é tocado — por isso `NULL` é aceitável.

## 2. Remoção com tombstones

```c
/* Estados: 0 = nunca usado, 1 = ocupado, 2 = apagado (tombstone). */
static int tabela_remover(Tabela *t, unsigned int chave) {
    size_t pos = (size_t)chave % TAB_CAP;
    for (size_t i = 0; i < TAB_CAP; i++) {
        size_t s = (pos + i) % TAB_CAP;
        if (t->slots[s].ocupado == 0) {
            return 0; /* nunca passou daqui: chave ausente */
        }
        if (t->slots[s].ocupado == 1 && t->slots[s].chave == chave) {
            t->slots[s].ocupado = 2;
            t->usadas--;
            return 1;
        }
    }
    return 0;
}
```

`buscar` deve pular estado `2` e parar só no estado `0`; `inserir` pode reutilizar o primeiro tombstone do caminho, mas precisa continuar sondando para não duplicar chave adiante. Custo: uma tabela cheia de tombstones degrada a busca para `O(cap)` mesmo vazia de fato — por isso tabelas reais disparam rehash por contagem de tombstones.

## 3. Sementes

As três sementes devem passar. A semente fixa torna a sequência de 500 vetores reproduzível: uma falha aponta o vetor exato. Com `srand(time(NULL))`, cada execução testa vetores diferentes e uma falha pode não se repetir — inútil como regressão.

## 4. Primeira ocorrência

```c
static long busca_primeira(const int *v, size_t n, int alvo) {
    long resposta = -1L;
    size_t ini = 0, fim = n;
    while (ini < fim) {
        size_t meio = ini + (fim - ini) / 2;
        if (v[meio] == alvo) {
            resposta = (long)meio;
            fim = meio; /* continua a esquerda: pode haver outro igual antes */
        } else if (alvo < v[meio]) {
            fim = meio;
        } else {
            ini = meio + 1;
        }
    }
    return resposta;
}
```

O invariante ajustado: "se há ocorrência em `v[ini..fim)`, `resposta` guarda a primeira à direita da fatia ou ainda não achou nenhuma". `fim - ini` continua diminuindo, então termina; ao sair, `resposta` é a primeira ocorrência ou `-1`.
