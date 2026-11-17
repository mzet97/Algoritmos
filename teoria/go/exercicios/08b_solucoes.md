# Soluções comentadas da aula 08b

## 1. Caminho do `os.Getpid`

Ramo vDSO (quando o kernel exporta `getpid` na página compartilhada): chamada → leitura na vDSO → retorno, sem troca de modo. Ramo clássico: chamada → instrução de syscall com troca para modo kernel → kernel lê o PID → retorno com troca de volta. A régua mede o *mínimo*: com vDSO, cronometrou-se chamada de função; sem vDSO, duas trocas de modo.

## 2. `GOMAXPROCS` que não ajuda

```go
// go run com GOMAXPROCS=1 contra o padrao; some 1e8 com 8 workers.
```

Se o trabalho é soma em variável compartilhada com mutex/atômica, o gargalo é contenção, não CPU: mais workers só aumentam a fila. Ganho aparece com partição sem compartilhamento (somas locais + redução final) e trabalho grande o bastante para dominar o custo de criação das goroutines.

## 3. `EACCES` real

```bash
echo segredo > /tmp/08b_sem_leitura.bin
chmod 000 /tmp/08b_sem_leitura.bin
```

`os.ReadFile` devolve `*PathError` com errno 13; `os.IsPermission(err)` é `true`. Rodar como root anula o experimento — root lê tudo; use seu usuário. Limpe o arquivo depois.

## 4. 100 MB, `GC()` e o que volta

```go
bloco := make([]byte, 100<<20)
_ = bloco
runtime.GC()
bloco = nil
runtime.GC()
```

`HeapAlloc` cai após soltar a referência e forçar o ciclo (objetos liberados no runtime), mas o RSS do processo raramente volta ao valor inicial: o runtime retém spans para reuse e devolve páginas ao SO por conta própria. Moral: soltar referência libera para o runtime, não promete devolver ao sistema.
