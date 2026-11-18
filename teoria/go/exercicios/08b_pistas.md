---
layout: default
---

# Pistas da aula 08b

1. O caminho é: `os.Getpid` → runtime/vDSO ou syscall com troca de modo → kernel lê a `task_struct` → retorno. Desenhe os dois ramos e marque onde cada um economiza.
2. Fixe `GOMAXPROCS=1` contra o padrão num laço com soma paralela e meça os dois. Se o ganho some, procure o gargalo: soma atômica compartilhada, falsa partilha ou trabalho pequeno demais.
3. `chmod 000 arquivo` como dono e tente `os.ReadFile` como você mesmo; registre se o erro satisfaz `os.IsPermission`.
4. Meça `HeapAlloc` e RSS (via `/proc/self/status`) antes, depois de alocar e depois de soltar a referência com um `GC()` no meio. O heap volta ao runtime; ao SO, só quando o runtime quiser.
