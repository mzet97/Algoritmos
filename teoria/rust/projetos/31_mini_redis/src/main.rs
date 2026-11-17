use dashmap::DashMap;
use std::sync::Arc;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpListener;

// O Estado Global: Nosso Banco de Dados
// DashMap é como um RwLock<HashMap>, mas fragmentado internamente (Sharded).
// Ele permite alta concorrência de leitura e escrita simultâneas em chaves diferentes.
type Db = Arc<DashMap<String, String>>;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // 1. Inicializa o banco de dados vazio
    let db: Db = Arc::new(DashMap::new());

    // Laboratório sem autenticação: aceite apenas conexões da própria máquina.
    let listener = TcpListener::bind("127.0.0.1:6379").await?;
    println!("🚀 Mini-Redis rodando na porta 6379");

    // 2. Event Loop de Aceite de Conexões
    // TcpListener aceita novas conexoes; cada conexao vira um socket independente.
    loop {
        let (mut socket, addr) = listener.accept().await?;
        println!("Cliente conectado: {}", addr);

        let db_clone = db.clone();

        // 3. Spawna uma task para cada cliente
        tokio::spawn(async move {
            let mut buffer = [0; 1024];

            // Loop de processamento de comandos DO cliente
            loop {
                // Lê os bytes da rede
                let bytes_lidos = match socket.read(&mut buffer).await {
                    Ok(n) if n == 0 => return, // Conexão fechada pelo cliente
                    Ok(n) => n,
                    Err(e) => {
                        eprintln!("Erro na leitura: {}", e);
                        return;
                    }
                };

                // Tenta converter os bytes para uma string UTF-8.
                // UTF-8 e a codificacao de texto padrao do Rust para strings.
                // Se os bytes nao forem texto valido, ignoramos aquele pacote.
                if let Ok(comando_str) = std::str::from_utf8(&buffer[..bytes_lidos]) {
                    // Remove quebras de linha (\r\n) que vêm do telnet/nc
                    let comando = comando_str.trim();

                    // Executa a lógica de banco de dados
                    let resposta = processar_comando(comando, &db_clone);

                    // Devolve a resposta pela rede
                    if let Err(e) = socket.write_all(resposta.as_bytes()).await {
                        eprintln!("Erro ao escrever no socket: {}", e);
                        return;
                    }
                }
            }
        });
    }
}

// 4. O Parser do Protocolo e Execução
fn processar_comando(comando: &str, db: &Db) -> String {
    // Divide o comando por espaços em até 3 partes: ["SET", "chave", "valor..."]
    let partes: Vec<&str> = comando.splitn(3, ' ').collect();

    if partes.is_empty() {
        return "-ERR Comando vazio\r\n".to_string();
    }

    match partes[0].to_uppercase().as_str() {
        "SET" => {
            if partes.len() < 3 {
                return "-ERR Sintaxe do SET: SET <chave> <valor>\r\n".to_string();
            }
            let chave = partes[1].to_string();
            let valor = partes[2].to_string();
            
            // O DashMap gerencia a sincronizacao do fragmento internamente
            db.insert(chave, valor);
            "+OK\r\n".to_string()
        }
        "GET" => {
            if partes.len() < 2 {
                return "-ERR Sintaxe do GET: GET <chave>\r\n".to_string();
            }
            let chave = partes[1];
            
            // Tenta pegar a referência da chave (Read Lock fino)
            match db.get(chave) {
                Some(valor) => format!("+{}\r\n", valor.value()),
                None => "$-1\r\n".to_string(), // Formato Redis para "Não encontrado" (Null)
            }
        }
        "DEL" => {
            if partes.len() < 2 {
                return "-ERR Sintaxe do DEL: DEL <chave>\r\n".to_string();
            }
            let chave = partes[1];
            
            // Remove a chave e retorna a quantidade de itens removidos
            if db.remove(chave).is_some() {
                ":1\r\n".to_string()
            } else {
                ":0\r\n".to_string()
            }
        }
        "PING" => "+PONG\r\n".to_string(),
        _ => format!("-ERR Comando desconhecido '{}'\r\n", partes[0]),
    }
}
