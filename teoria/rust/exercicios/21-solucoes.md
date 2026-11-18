---
layout: default
---

# Aula 21 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../21.md#exercícios-obrigatórios). Projeto: `cargo add axum --features ws tokio --features full futures`. Servidor com salas e heartbeat:

```rust
use axum::{
    extract::{
        ws::{Message, WebSocket, WebSocketUpgrade},
        Path, State,
    },
    response::IntoResponse,
    routing::get,
    Router,
};
use futures::{SinkExt, StreamExt};
use std::collections::HashMap;
use std::sync::Arc;
use std::time::Duration;
use tokio::net::TcpListener;
use tokio::sync::{broadcast, RwLock};

#[derive(Clone)]
struct AppState {
    salas: Arc<RwLock<HashMap<String, broadcast::Sender<String>>>>,
}

impl AppState {
    async fn sala(&self, nome: &str) -> broadcast::Sender<String> {
        {
            let leitura = self.salas.read().await;
            if let Some(tx) = leitura.get(nome) {
                return tx.clone();
            }
        }
        let mut escrita = self.salas.write().await;
        escrita
            .entry(nome.to_string())
            .or_insert_with(|| broadcast::channel(100).0)
            .clone()
    }
}

#[tokio::main]
async fn main() {
    let state = AppState {
        salas: Arc::new(RwLock::new(HashMap::new())),
    };
    let app = Router::new()
        .route("/ws/{sala}", get(handler_websocket))
        .with_state(state);
    let listener = TcpListener::bind("127.0.0.1:3000").await.unwrap();
    println!("websocket em ws://127.0.0.1:3000/ws/NOME-DA-SALA");
    axum::serve(listener, app).await.unwrap();
}

async fn handler_websocket(
    ws: WebSocketUpgrade,
    Path(sala): Path<String>,
    State(state): State<AppState>,
) -> impl IntoResponse {
    ws.on_upgrade(move |socket| gerenciar_conexao(socket, state, sala))
}

async fn gerenciar_conexao(socket: WebSocket, state: AppState, nome_sala: String) {
    let tx = state.sala(&nome_sala).await;
    let mut rx = tx.subscribe();
    let (mut enviador, mut receptor) = socket.split();

    // Tarefa de envio: mensagens da sala + heartbeat de 30s.
    let mut tarefa_envio = tokio::spawn(async move {
        loop {
            match tokio::time::timeout(Duration::from_secs(30), rx.recv()).await {
                Ok(Ok(mensagem)) => {
                    if enviador.send(Message::Text(mensagem.into())).await.is_err() {
                        break;
                    }
                }
                Ok(Err(_)) => break, // canal fechado ou atrasado demais
                Err(_) => {
                    // 30s sem mensagens da sala: heartbeat.
                    if enviador.send(Message::Ping(Vec::new().into())).await.is_err() {
                        break;
                    }
                }
            }
        }
    });

    // Tarefa principal: o que o cliente diz vai para a sala dele.
    while let Some(Ok(mensagem)) = receptor.next().await {
        if let Message::Text(texto) = mensagem {
            let _ = tx.send(format!("[{}] {}", nome_sala, texto));
        }
    }

    tarefa_envio.abort();
}
```

Verificado nesta rodada com clientes WebSocket reais: dois clientes em `geral` trocam mensagens entre si; um terceiro em `outra` nao recebe nada de `geral`; e as mensagens chegam prefixadas com a sala de origem. O heartbeat mora na tarefa de envio como um terceiro desfecho do `timeout`: mensagem da sala, canal morto ou 30s de silencio virando `Ping` (clientes WebSocket respondem `Pong` automaticamente nas bibliotecas usuais; quem nao le nem responde acaba descartado quando o envio falha).

Limites declarados: salas nunca sao removidas do mapa quando esvaziam (vazamento lento e conhecido deste desenho) e nao ha autenticacao nem limite por cliente — os degraus seguintes estao na [aula 23a de C++](../../cpp/23a.md) em espirito, e na rubrica deste livro.

**Autoverificacao:** isolamento entre salas demonstrado com tres clientes, prefixo da sala nas mensagens e o `Ping` aparecendo apos 30s de silencio (reduza a constante para demonstrar rapido, declarando a troca).
