# Soluções comentadas da aula 14a

## 1. Caminho da leitura

`f.read(65536)` executa bytecode `CALL` → CPython chama a implementação de `BufferedReader.read` → libc/`os.read` → syscall `read` com o descritor → kernel consulta o page cache: acerto copia páginas para o buffer do usuário; falta lê do disco, pode bloquear e atende outros processos antes → cópia para o objeto `bytes` retornado. Falhas possíveis: descritor inválido, permissão, I/O de disco, interrupção por sinal.

## 2. Segunda leitura mais rápida

Nada no programa mudou; mudou o estado do SO: as páginas do arquivo ficaram residentes no page cache após a primeira leitura. A segunda mede cópia de memória, a primeira mediu disco (ou parte dele). Por isso benchmark de I/O exige `drop_caches` controlado ou, no mínimo, declarar que o cache estava quente.

## 3. `PermissionError` real

```bash
echo segredo > /tmp/14a_sem_leitura.txt
chmod 000 /tmp/14a_sem_leitura.txt
python3 -c "open('/tmp/14a_sem_leitura.txt').read()"
# PermissionError: [Errno 13] Permission denied
chmod 644 /tmp/14a_sem_leitura.txt  # limpar depois
```

Exceção exata: `PermissionError` (subclasse de `OSError`, errno 13). Rodar como root anula o experimento — root lê tudo; use seu usuário.

## 4. 100 MB, `del` e o que volta

```python
import tracemalloc
tracemalloc.start()
bloco = [0] * (100 * 1024 * 1024 // 28)  # ~100 MB em objetos pequenos
print(tracemalloc.get_traced_memory())
del bloco
print(tracemalloc.get_traced_memory())
```

O `tracemalloc` cai (objetos liberados), mas o RSS do processo raramente volta ao valor inicial: o alocador retém arenas para reuse e devolve páginas ao SO quando quer, não quando você deleta. Moral: `del` libera objetos, não promete devolver memória ao sistema.
