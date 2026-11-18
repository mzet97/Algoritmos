---
layout: default
---

# Soluções comentadas da aula 16a

## 1. Caminho do `getpid`

Ramo vDSO (quando o kernel exporta `getpid` na página compartilhada): chamada → leitura do valor na vDSO → retorno, sem troca de modo. Ramo clássico: chamada → instrução de syscall (`syscall` em x86-64) com troca para modo kernel → kernel lê o PID da `task_struct` → retorno com troca de volta. Por isso a régua mede o *mínimo*: com vDSO, você cronometrou uma chamada de função; sem vDSO, duas trocas de modo mais o trabalho.

## 2. Zumbi proposital e correção

```c
pid_t z = fork();
if (z == 0) {
    _exit(0); /* filho termina na hora */
}
sleep(5); /* pai nao colhe: filho vira Z */
/* observe: ps -o pid,stat,cmd -p <pid> mostra Z */
waitpid(z, NULL, 0); /* correcao: colher libera a entrada */
```

O estado `Z` confirma: sem código, sem memória relevante, mas com entrada ocupada até a colheita.

## 3. `memset` contra toque página a página

Ambos materializam as mesmas páginas e geram o mesmo número de faltas menores (uma por página nova), porque falta menor conta páginas materializadas, não instruções que as tocam. Difeririam se parte das páginas já estivesse residente (ex.: retocar o bloco): o segundo toque não gera falta. Moral: o contador mede eventos do SO, não esforço do seu laço.

## 4. `exec` que funciona e `exec` que falha

```c
pid_t f = fork();
if (f == 0) {
    execl("/bin/true", "true", (char *)NULL);
    _exit(127); /* so chega aqui se o exec falhar */
}
int e = 0;
waitpid(f, &e, 0);
/* /bin/true -> WEXITSTATUS(e) == 0; caminho inexistente -> 127 */
```

O `127` é convenção desta aula (como shells usam para "comando não encontrado"), não do kernel: documente a convenção no contrato da sua ferramenta.
