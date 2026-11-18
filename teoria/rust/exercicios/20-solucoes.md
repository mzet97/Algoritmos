---
layout: default
---

# Aula 20 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../20.md#exercícios-obrigatórios). Projeto: `cargo add axum tokio --features tokio/full serde --features derive serde_json`. O servidor completo (base da aula + os dois exercicios):

```rust
use axum::{
    extract::{ConnectInfo, Path, State},
    http::StatusCode,
    routing::{get, post},
    Json, Router,
};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::net::SocketAddr;
use std::sync::{atomic::{AtomicUsize, Ordering}, Arc, RwLock};
use tokio::net::TcpListener;
use tokio::sync::Mutex;

#[derive(Clone)]
struct AppState {
    nome_servidor: String,
    contador_requisicoes: Arc<AtomicUsize>,
    ultimos_usuarios: Arc<RwLock<Vec<String>>>,
    acessos_por_ip: Arc<Mutex<HashMap<String, u8>>>,
}

#[derive(Deserialize)]
struct CriarUsuarioReq {
    nome: String,
    idade: u8,
}

#[derive(Serialize)]
struct RespostaPadrao {
    mensagem: String,
    requisicao_numero: usize,
}

#[tokio::main]
async fn main() {
    let state = AppState {
        nome_servidor: String::from("Servidor-Rust-Axum/1.0"),
        contador_requisicoes: Arc::new(AtomicUsize::new(0)),
        ultimos_usuarios: Arc::new(RwLock::new(Vec::new())),
        acessos_por_ip: Arc::new(Mutex::new(HashMap::new())),
    };

    let app = Router::new()
        .route("/", get(handler_raiz))
        .route("/usuarios/{id}", get(handler_buscar_usuario))
        .route("/usuarios", post(handler_criar_usuario))
        .route("/ultimos-usuarios", get(handler_ultimos_usuarios))
        .with_state(state);

    // ConnectInfo exige o make-service com informacao de conexao.
    let listener = TcpListener::bind("127.0.0.1:3000").await.unwrap();
    println!("servidor em http://127.0.0.1:3000");
    axum::serve(listener, app.into_make_service_with_connect_info::<SocketAddr>())
        .await
        .unwrap();
}

async fn handler_raiz(
    State(estado): State<AppState>,
    ConnectInfo(endereco): ConnectInfo<SocketAddr>,
) -> Result<String, (StatusCode, String)> {
    let ip = endereco.ip().to_string();
    let total = {
        let mut mapa = estado.acessos_por_ip.lock().await;
        let contador = mapa.entry(ip).or_insert(0);
        *contador = contador.saturating_add(1);
        *contador
    };
    if total > 5 {
        return Err((StatusCode::TOO_MANY_REQUESTS, String::from("limite de 5 acessos por IP")));
    }
    let atual = estado.contador_requisicoes.fetch_add(1, Ordering::Relaxed);
    Ok(format!(
        "Bem-vindo ao {}! Voce e o visitante numero {}.",
        estado.nome_servidor,
        atual + 1
    ))
}

async fn handler_buscar_usuario(Path(id): Path<u32>) -> String {
    format!("Buscando usuario com ID: {}", id)
}

async fn handler_criar_usuario(
    State(estado): State<AppState>,
    Json(payload): Json<CriarUsuarioReq>,
) -> Json<RespostaPadrao> {
    {
        // Escopo curto: a guarda cai antes de qualquer await posterior.
        let mut ultimos = estado.ultimos_usuarios.write().unwrap();
        ultimos.push(payload.nome.clone());
        while ultimos.len() > 5 {
            ultimos.remove(0);
        }
    }
    let atual = estado.contador_requisicoes.fetch_add(1, Ordering::Relaxed);
    Json(RespostaPadrao {
        mensagem: format!("Usuario {} de {} anos criado com sucesso!", payload.nome, payload.idade),
        requisicao_numero: atual + 1,
    })
}

async fn handler_ultimos_usuarios(State(estado): State<AppState>) -> Json<Vec<String>> {
    let ultimos = {
        let leitura = estado.ultimos_usuarios.read().unwrap();
        leitura.clone()
    };
    Json(ultimos)
}
```

Trechos verificados nesta rodada: criar 7 usuarios deixa `/ultimos-usuarios` com os **5 ultimos** em ordem de criacao, e o sexto `GET /` do mesmo IP responde `429` (os cinco primeiros, `200`). Dois cuidados que sao o exercicio: a guarda do `RwLock` morre dentro do bloco antes de qualquer `.await` (segurar `std::sync` guard atraves de await pode bloquear o executor e ate travar), e o rate limiter conta por IP extraido de `ConnectInfo` — sem `into_make_service_with_connect_info`, esse extractor nem existe.

Limite declarado: contador por IP em memoria cresce sem expiracao neste laboratorio; producao pediria janela de tempo e limpeza periodica.

**Autoverificacao:** historico truncado em 5 apos 7 criacoes, `429` na sexta visita da raiz e nenhuma guarda viva atraves de `.await` no seu codigo.
