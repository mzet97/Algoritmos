---
layout: default
---

# Aula 30 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../30.md#exercícios-práticos). Estado executado: o load balancer da aula virou o projeto [30_load_balancer](../projetos/README.md) e o `teoria/verificar_projetos.py` confirma a sequencia ciclica de round-robin entre os backends 8081–8083 com backends reais (nesta maquina, em namespace de rede quando a 8080 esta ocupada). Os exercicios sao os endurecimentos sobre esse nucleo:

1. Timeout por tentativa (recorte a aplicar no `handle_connection` do projeto):

```rust
use std::time::Duration;
use tokio::io::copy_bidirectional;
use tokio::net::TcpStream;
use tokio::time::timeout;

async fn conectar_backend(endereco: &str) -> std::io::Result<TcpStream> {
    // 2s para conectar; sem isso, um backend morto segura a conexao
    // do cliente pelo timeout do sistema operacional.
    timeout(Duration::from_secs(2), TcpStream::connect(endereco))
        .await
        .map_err(|_| std::io::Error::new(std::io::ErrorKind::TimedOut, "timeout ao conectar"))?
}
```

O proxy em si precisa do mesmo tratamento por leitura/escrita: um cliente ou backend que para de responder no meio da copia nao pode segurar a tarefa para sempre. A constante (2s aqui) e politica declarada, nao detalhe.

2. Health check ativo: uma tarefa com `tokio::time::interval` tenta conectar em cada backend a cada N segundos e mantem `Vec<bool>` de saude; o round-robin pula os marcados como mortos e so volta a usa-los quando o teste passa. Sem isso, 1/3 das conexoes deste laboratorio cairia num backend morto para sempre.

3. Token bucket por cliente e testes de carga ficam como o proximo degrau declarado (nao implementados no projeto): o balde mora por IP no estado compartilhado e cada conexao consome um token antes do proxy; o teste de carga honesto mede solicitacoes/segundo e latencia por percentil com clientes concorrentes reais, nao um `curl` cronometrado.

4. HTTP de verdade (em vez de TCP puro) troca `copy_bidirectional` por um cliente/servidor HTTP (axum/hyper da [aula 20](20-solucoes.md)) e muda o problema: cabecalhos, keep-alive e roteamento por caminho entram no contrato — e por isso e um exercicio separado, nao um flag.

**Autoverificacao:** round-robin verde no verificador, timeout aplicado com constante declarada e a lista de saudaveis separada da lista total no seu desenho.
