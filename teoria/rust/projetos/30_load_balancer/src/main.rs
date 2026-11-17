use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;
use tokio::io::copy_bidirectional;
use tokio::net::{TcpListener, TcpStream};

// 1. O Estado do nosso Load Balancer
struct BalancerState {
    // Lista de IPs/Portas dos nossos servidores backend
    backends: Vec<String>,
    // O índice atômico para o algoritmo Round-Robin
    current_index: AtomicUsize,
}

impl BalancerState {
    fn new(backends: Vec<String>) -> Self {
        Self {
            backends,
            current_index: AtomicUsize::new(0),
        }
    }

    // Função que decide qual servidor receberá a próxima requisição
    fn get_next_backend(&self) -> String {
        // Incrementa o índice atomicamente (thread-safe, sem Mutex bloqueante!)
        let index = self.current_index.fetch_add(1, Ordering::Relaxed);
        
        // Faz o módulo (%) para voltar a 0 quando chegar no fim da lista
        let selected = index % self.backends.len();
        
        // Clona a string do IP escolhido
        self.backends[selected].clone()
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Lista de servidores falsos que nosso Load Balancer vai proteger.
    // (Para testar de verdade, você teria que ter servidores rodando nessas portas).
    let backends = vec![
        "127.0.0.1:8081".to_string(),
        "127.0.0.1:8082".to_string(),
        "127.0.0.1:8083".to_string(),
    ];

    let state = Arc::new(BalancerState::new(backends));

    // Faz o bind do nosso Load Balancer na porta 8080
    let listener = TcpListener::bind("0.0.0.0:8080").await?;
    println!("⚖️ Load Balancer rodando na porta 8080");

    // O Event Loop Principal
    loop {
        // Aceita uma nova conexão do Cliente. Isso NÃO bloqueia o loop inteiro,
        // apenas suspende esta task até um cliente conectar.
        let (mut client_socket, client_addr) = listener.accept().await?;
        println!("Nova conexão de: {}", client_addr);

        // Clona a referência do estado (barato, é um Arc)
        let state_clone = state.clone();

        // 2. Faz o "Spawn" de uma nova Task (Green Thread) para cada cliente.
        // Uma task do Tokio e mais leve que uma thread do SO.
        // Ela existe para termos concorrencia massiva sem custo de milhares de threads nativas.
        // Assim o loop volta instantaneamente a escutar novos clientes.
        tokio::spawn(async move {
            let backend_addr = state_clone.get_next_backend();
            println!("Roteando {} -> {}", client_addr, backend_addr);

            // 3. Conecta no servidor de backend
            match TcpStream::connect(&backend_addr).await {
                Ok(mut backend_socket) => {
                    // 4. A Mágica do Zero-Cost Proxy!
                    // copy_bidirectional é uma função do Tokio altamente otimizada.
                    // Ela cria dois buffers e fica bombeando bytes do Cliente para o Backend
                    // e do Backend para o Cliente simultaneamente.
                    // O await só termina quando um dos dois lados fechar a conexão.
                    match copy_bidirectional(&mut client_socket, &mut backend_socket).await {
                        Ok((bytes_up, bytes_down)) => {
                            println!(
                                "Conexão encerrada. Up: {} bytes, Down: {} bytes",
                                bytes_up, bytes_down
                            );
                        }
                        Err(e) => eprintln!("Erro ao copiar dados: {}", e),
                    }
                }
                Err(e) => {
                    eprintln!("O Backend {} está fora do ar! Erro: {}", backend_addr, e);
                    // Opcional: Se o backend caiu, você poderia mandar um erro HTTP 502
                    // para o client_socket aqui.
                }
            }
        });
    }
}
