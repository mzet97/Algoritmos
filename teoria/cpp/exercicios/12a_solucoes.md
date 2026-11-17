# Soluções comentadas da aula 12a

## 1. Caminho do `getpid`

Ramo vDSO (quando o kernel exporta `getpid` na página compartilhada): chamada → leitura na vDSO → retorno, sem troca de modo. Ramo clássico: chamada → instrução de syscall com troca para modo kernel → kernel lê o PID da `task_struct` → retorno com troca de volta. A régua mede o *mínimo*: com vDSO, cronometrou-se chamada de função; sem vDSO, duas trocas de modo.

## 2. Previsão de `sizeof`

`Cheia`: `a` ocupa [0,1), preenche até 8; `b` ocupa [8,16); `c` ocupa [16,17), preenche até 24 = 3×8. Total 24. `Compacta`: `b` em [0,8); `a`,`c` em [8,10); preenche até 16 = 2×8. Total 16. Regra geral: offset de cada campo = próximo múltiplo do seu `alignof`; tamanho total = próximo múltiplo do maior `alignof`.

## 3. Zumbi proposital e correção

```cpp
const pid_t z = fork();
if (z == 0) {
    _exit(0); // filho termina na hora
}
sleep(5); // pai nao colhe: filho vira Z
// observe: ps -o pid,stat,cmd -p <pid>
// correcao:
int e = 0;
waitpid(z, &e, 0);
```

Estado `Z`: sem código, sem memória relevante, mas com entrada ocupada até a colheita.

## 4. `fill` contra toque página a página

Ambos materializam as mesmas páginas: as faltas coincidem na primeira passagem. A diferença aparece na segunda passagem sobre o mesmo vetor: zero faltas novas, pois as páginas já estão residentes. Moral: o contador mede eventos do SO, não esforço do seu laço — e `vector` já zera na construção, então parte das páginas pode ser materializada antes do seu laço.
