# Pistas da aula 11a

1. Os quatro atos: `go` (criação), envio→recebimento em channel, `Unlock`→`Lock` em `Mutex`, `Add`→`Wait` em `WaitGroup`. Para cada um, escreva o que foi escrito antes e o que passa a ser visível depois.
2. Adicione `done := make(chan struct{})`, um `select` com `<-done` no worker e `close(done)` no meio do envio. Conte quantos jobs foram processados e quantos ficaram na fila.
3. Troque `c.mu.Lock()/Unlock()` por acesso direto a `c.n` e rode com `-race`. O relatório mostra as duas goroutines e as duas linhas em corrida: transcreva pilhas e endereços.
4. Com buffer 0, cada envio espera um worker livre. Para 300 jobs e 4 workers rápidos, o produtor avança no ritmo dos workers; sob carga real, ele trava. Dimensione o buffer pelo desacoplamento desejado, não pelo número de workers.
