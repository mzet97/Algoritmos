# Soluções comentadas da aula 14a

## 1. Happens-before em pthreads

- `pthread_create` → início da thread: tudo antes da criação é visível dentro dela.
- `pthread_join` ← término da thread: tudo que a thread escreveu antes de retornar é visível após o `join`.
- `pthread_mutex_unlock` → próximo `pthread_mutex_lock` do mesmo mutex.
- `pthread_cond_wait` (com o mutex) → o sinalizador escreveu antes do `signal` sob o mesmo mutex.
- `pthread_once` → toda chamada posterior, em qualquer thread.

## 2. Sem mutex + TSan (padrão documentado)

```text
WARNING: ThreadSanitizer: data race (pid=...)
  Write of size 8 ... contador ... by thread T1
  Previous write of size 8 ... by thread T2
```

Duas escritas em `c.total` sem sincronização. Restaurar o `mutex` silencia: toda escrita passa por exclusão, e a leitura em `main` acontece após todos os `join`s. Neste WSL o TSan aborta até em programa trivial (incompatibilidade de mapeamento, ver aula C++ 17a): transcreva onde o seu TSan funcionar.

## 3. `if` contra `while`

Com `if`, um acordo espúrio sai de `pthread_cond_wait` com `pronto == 0`; o consumidor lê `valor == 0` e entrega lixo com cara de sucesso. Com `while`, ele volta a dormir. Spurious wakeup é permitido e raro — exatamente o tipo de bug que só aparece em produção, nunca no teste rápido.

## 4. Sinal antes da espera

`produtor` escreve `pronto = 1` sob o mutex e sinaliza; se o consumidor ainda nem esperou, o `signal` não acorda ninguém — mas também não precisa: ao chegar, o consumidor trava o mutex, vê `pronto == 1` e nem dorme. O estado carrega a informação; o sinal é só um cutucão para quem já dorme.
