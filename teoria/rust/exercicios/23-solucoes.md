---
layout: default
---

# Aula 23 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../23.md#exercícios-obrigatórios). Executado nesta rodada com `protoc` instalado localmente (3.19) e tonic 0.13. Dependencias: `cargo add tonic prost tokio --features tokio/full futures uuid --features v4` e, em build-dependencies, `tonic-build`.

Contrato estendido (`proto/pagamentos.proto`, o da aula mais o streaming do exercicio 2):

```protobuf
syntax = "proto3";
package pagamentos;

service SistemaPagamento {
    rpc ProcessarPagamento (PagamentoRequest) returns (PagamentoResponse);
    rpc MonitorarTransacoes (Vazio) returns (stream PagamentoResponse);
}

message Vazio {}

message PagamentoRequest {
    string usuario_id = 1;
    double valor = 2;
    string moeda = 3;
}

message PagamentoResponse {
    bool sucesso = 1;
    string transacao_id = 2;
}
```

Multiplos binarios (exercicio 1, no `Cargo.toml`):

```toml
[[bin]]
name = "server"
path = "src/server.rs"

[[bin]]
name = "client"
path = "src/client.rs"
```

`build.rs` (o da aula, sem mudancas):

```rust
fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("proto/pagamentos.proto")?;
    Ok(())
}
```

Servidor (`src/server.rs`):

```rust
use futures::Stream;
use std::pin::Pin;
use std::time::Duration;
use tonic::{transport::Server, Request, Response, Status};

pub mod pagamentos {
    tonic::include_proto!("pagamentos");
}

use pagamentos::sistema_pagamento_server::{SistemaPagamento, SistemaPagamentoServer};
use pagamentos::{PagamentoRequest, PagamentoResponse, Vazio};

#[derive(Default)]
pub struct MeuServicoPagamento {}

#[tonic::async_trait]
impl SistemaPagamento for MeuServicoPagamento {
    async fn processar_pagamento(
        &self,
        request: Request<PagamentoRequest>,
    ) -> Result<Response<PagamentoResponse>, Status> {
        let req = request.into_inner();
        println!("Processando {} {} para usuario {}", req.valor, req.moeda, req.usuario_id);
        Ok(Response::new(PagamentoResponse {
            sucesso: true,
            transacao_id: uuid::Uuid::new_v4().to_string(),
        }))
    }

    type MonitorarTransacoesStream =
        Pin<Box<dyn Stream<Item = Result<PagamentoResponse, Status>> + Send>>;

    async fn monitorar_transacoes(
        &self,
        _request: Request<Vazio>,
    ) -> Result<Response<Self::MonitorarTransacoesStream>, Status> {
        let fluxo = async_stream::stream! {
            for i in 0..3 {
                tokio::time::sleep(Duration::from_secs(1)).await;
                yield Ok(PagamentoResponse {
                    sucesso: true,
                    transacao_id: format!("monitorada-{}", i),
                });
            }
        };
        Ok(Response::new(Box::pin(fluxo)))
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let endereco = "127.0.0.1:50051".parse()?;
    println!("servidor grpc em {}", endereco);
    Server::builder()
        .add_service(SistemaPagamentoServer::new(MeuServicoPagamento::default()))
        .serve(endereco)
        .await?;
    Ok(())
}
```

(Adicione `async-stream` as dependencias para o `async_stream::stream!`, ou monte o stream com `futures::stream::unfold`.)

Cliente (`src/client.rs`):

```rust
use futures::StreamExt;

pub mod pagamentos {
    tonic::include_proto!("pagamentos");
}

use pagamentos::sistema_pagamento_client::SistemaPagamentoClient;
use pagamentos::{PagamentoRequest, Vazio};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut cliente = SistemaPagamentoClient::connect("http://127.0.0.1:50051").await?;

    let resposta = cliente
        .processar_pagamento(PagamentoRequest {
            usuario_id: String::from("user-42"),
            valor: 199.90,
            moeda: String::from("BRL"),
        })
        .await?;
    let interna = resposta.into_inner();
    println!("pagamento: sucesso={} tx={}", interna.sucesso, !interna.transacao_id.is_empty());

    let mut fluxo = cliente.monitorar_transacoes(Vazio {}).await?.into_inner();
    let mut recebidas = 0;
    while let Some(item) = fluxo.next().await {
        let pagamento = item?;
        println!("stream: {}", pagamento.transacao_id);
        recebidas += 1;
    }
    println!("stream encerrado apos {} itens", recebidas);
    Ok(())
}
```

Verificado: `cargo run --bin server` sobe em `127.0.0.1:50051`; `cargo run --bin client` imprime `pagamento: sucesso=true tx=true` e tres itens `monitorada-0..2` em ~3 segundos (um por segundo, como o contrato manda). O stream **termina** quando o gerador acaba — o cliente sai do `while let` sem erro, que e o sinal de fim bem-comportado.

**Autoverificacao:** os dois binarios sobem separados, o cliente unary e o stream funcionam na mesma execucao e mudar um tipo no `.proto` quebra a compilacao (experimente trocar `double` por `string`).
