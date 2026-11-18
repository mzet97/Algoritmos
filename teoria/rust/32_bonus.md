---
layout: default
---

# Aula 32 (Bônus) — Masterpiece B Avançado: Zero-Copy, INCR e Expiração (TTL)

## Objetivo do Bônus
Você pediu para seguir adiante, então aqui está o **Desafio Final** resolvido!
Como a trilha oficial acabou na Aula 31, esta aula bônus traz a **solução dos exercícios obrigatórios** do nosso Banco de Dados em Memória (Estilo Redis).

Vamos refatorar o código para atingir nível de produção:
1. Usar a crate `bytes` para **Zero-Copy** (evitando o `malloc` e conversão ineficiente para `String` em valores grandes).
2. Implementar o comando `INCR` de forma **Atômica por Chave** no `DashMap`.
3. Implementar a **Expiração de Chaves (TTL - Time To Live)** com limpeza preguiçosa (*Lazy Eviction*).

Importante: nesta aula, "atômico" significa que a atualização daquela chave acontece como uma operação lógica indivisível do ponto de vista dos outros clientes.

Isso **não** significa "lock-free". O `DashMap` usa sincronização interna por fragmentos; ele reduz contenção, mas ainda usa travas.

Use as [pistas](exercicios/32-pistas.md) somente depois de tentar; consulte as [solucoes comentadas](exercicios/32-solucoes.md) apos entregar sua versao.
---

## 1. Atualizando as Dependências

Precisamos adicionar a poderosa crate `bytes`, que atua como um ponteiro inteligente (`Arc`) para arrays de bytes brutos, permitindo fatiar e compartilhar memória lida da rede sem fazer cópias.

A crate **bytes** existe porque servidores de alta performance trafegam grandes volumes de dados binarios, e converter tudo para `String` ou copiar vetores repetidamente custa CPU, heap e cache.

```bash
cd mini_redis
cargo add bytes
```

---

## 2. A Nova Arquitetura do Valor (TTL e Zero-Copy)

No código anterior, usávamos `DashMap<String, String>`.
Agora usaremos `DashMap<String, ValorCache>`, onde `ValorCache` armazena os bytes brutos (`Bytes`) e o instante em que a chave deve morrer (`Option<Instant>`).

**TTL** significa *Time To Live*, ou tempo de vida. Esse mecanismo existe para dados temporarios como sessoes, tokens, caches e locks distribuidos. **Lazy eviction** significa remover chaves expiradas apenas quando alguem tenta acessa-las ou quando um limpador periodico passa por elas, em vez de disparar um timer individual para cada chave.
**TTL** significa *Time To Live*, ou tempo de vida. Esse mecanismo existe para dados temporarios como sessoes, tokens, caches e locks distribuidos.

**Lazy eviction** significa remover chaves expiradas apenas quando alguem tenta acessa-las ou quando um limpador periodico passa por elas, em vez de disparar um timer individual para cada chave.

### O Código Completo (`src/main.rs`)

```rust
use bytes::Bytes;
use dashmap::DashMap;
use std::sync::Arc;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpListener;
use tokio::time::{Instant, Duration};

// 1. A Estrutura de Dados do nosso Cache
#[derive(Clone)]
struct ValorCache {
    dado: Bytes,                  // Usamos Bytes em vez de String (menos copias e menos heap)
    expira_em: Option<Instant>,   // Momento em que a chave deve sumir
}

impl ValorCache {
    // Checa se a chave já passou da validade
    fn is_expirado(&self) -> bool {
        if let Some(tempo) = self.expira_em {
            return Instant::now() > tempo;
        }
        false
    }
}

type Db = Arc<DashMap<String, ValorCache>>;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let db: Db = Arc::new(DashMap::new());

    // Inicia uma Task em Background para limpar o Lixo (TTL Ativo)
    // O Redis faz isso: a cada X milissegundos ele varre chaves aleatórias
    // para limpar a memória.
    let db_bg = db.clone();
    tokio::spawn(async move {
        loop {
            tokio::time::sleep(Duration::from_secs(5)).await;
            // Retém no mapa apenas as chaves que NÃO expiraram
            db_bg.retain(|_chave, valor| !valor.is_expirado());
            // println!("Varredura de lixo concluída.");
        }
    });

    // Protótipo sem autenticação: somente clientes locais.
    let listener = TcpListener::bind("127.0.0.1:6379").await?;
    println!("🚀 Mini-Redis Avançado rodando na porta 6379");

    loop {
        let (mut socket, _addr) = listener.accept().await?;
        let db_clone = db.clone();

        tokio::spawn(async move {
            let mut buffer = [0; 4096]; // Buffer maior

            loop {
                let bytes_lidos = match socket.read(&mut buffer).await {
                    Ok(n) if n == 0 => return,
                    Ok(n) => n,
                    Err(_) => return,
                };

                // Em produção, usaríamos buffer parsing complexo.
                // Aqui, ainda usamos UTF-8 por simplicidade do comando,
                // mas guardamos o `Bytes` puro no banco!
                if let Ok(comando_str) = std::str::from_utf8(&buffer[..bytes_lidos]) {
                    let comando = comando_str.trim();
                    let resposta = processar_comando(comando, &db_clone);
                    let _ = socket.write_all(resposta.as_bytes()).await;
                }
            }
        });
    }
}

// 2. O Parser e Lógica dos Novos Comandos
fn processar_comando(comando: &str, db: &Db) -> String {
    let partes: Vec<&str> = comando.splitn(4, ' ').collect();
    if partes.is_empty() { return "-ERR Comando vazio\r\n".to_string(); }

    match partes[0].to_uppercase().as_str() {
        // SET chave valor [EX segundos]
        "SET" => {
            if partes.len() < 3 { return "-ERR Sintaxe: SET <chave> <valor> [EX segs]\r\n".to_string(); }
            
            let chave = partes[1].to_string();
            // Pega o valor e converte para `Bytes` (Em um server real de rede,
            // faríamos um `.freeze()` direto do buffer de leitura do TCP sem alocar a String)
            let valor_bytes = Bytes::copy_from_slice(partes[2].as_bytes());
            
            let mut expira_em = None;

            // Tratamento de TTL (Expiração)
            if partes.len() == 4 && partes[3].starts_with("EX") {
                // Sintaxe simples para o exemplo: SET foo bar EX10
                if let Ok(segs) = partes[3][2..].parse::<u64>() {
                    expira_em = match Instant::now().checked_add(Duration::from_secs(segs)) {
                        Some(instante) => Some(instante),
                        None => return "-ERR TTL fora dos limites\r\n".to_string(),
                    };
                }
            }

            let cache = ValorCache { dado: valor_bytes, expira_em };
            db.insert(chave, cache);
            "+OK\r\n".to_string()
        }

        "GET" => {
            if partes.len() < 2 { return "-ERR Sintaxe: GET <chave>\r\n".to_string(); }
            let chave = partes[1];
            
            // Tratamento de TTL Passivo (Lazy Eviction)
            // Se o usuário tentar ler uma chave expirada, deletamos na hora e retornamos None
            let expirado = {
                if let Some(valor) = db.get(chave) {
                    valor.is_expirado()
                } else {
                    false
                }
            };

            if expirado {
                db.remove(chave); // Deleta do banco
                return "$-1\r\n".to_string(); // Não encontrado
            }

            // Lê normalmente
            match db.get(chave) {
                Some(valor) => {
                    // Como guardamos `Bytes`, convertemos para string apenas para mostrar
                    let texto = std::str::from_utf8(&valor.dado).unwrap_or("BINARY_DATA");
                    format!("+{}\r\n", texto)
                }
                None => "$-1\r\n".to_string(),
            }
        }

        // O Desafio de Concorrência: INCR Atômico
        "INCR" => {
            if partes.len() < 2 { return "-ERR Sintaxe: INCR <chave>\r\n".to_string(); }
            let chave = partes[1].to_string();

            // Usamos a API `entry` do DashMap.
            // A API "entry" existe para transformar "ler, decidir e escrever"
            // em uma unica regiao critica coerente sobre a mesma chave.
            // Ela trava apenas o fragmento (shard) específico desta chave,
            // garantindo que NENHUMA outra thread modifique a chave no meio da nossa leitura+soma.
            let mut ref_mut = db.entry(chave.clone()).or_insert(ValorCache {
                dado: Bytes::from("0"),
                expira_em: None,
            });

            // Tenta converter o valor atual para número
            let atual_str = std::str::from_utf8(&ref_mut.dado).unwrap_or("0");
            
            match atual_str.parse::<i64>() {
                Ok(numero) => {
                    let novo_numero = match numero.checked_add(1) {
                        Some(valor) => valor,
                        None => return "-ERR Incremento fora dos limites\r\n".to_string(),
                    };
                    // Atualiza os bytes de forma atomica do ponto de vista logico da chave
                    ref_mut.dado = Bytes::copy_from_slice(novo_numero.to_string().as_bytes());
                    format!(":{}\r\n", novo_numero)
                }
                Err(_) => "-ERR O valor não é um inteiro ou está fora dos limites\r\n".to_string(),
            }
        }

        "DEL" => {
            if partes.len() < 2 { return "-ERR Sintaxe: DEL <chave>\r\n".to_string(); }
            if db.remove(partes[1]).is_some() { ":1\r\n".to_string() } else { ":0\r\n".to_string() }
        }

        "PING" => "+PONG\r\n".to_string(),
        _ => format!("-ERR Comando desconhecido '{}'\r\n", partes[0]),
    }
}
```

---

## 3. Testando o Banco de Dados Avançado

Se você compilar e rodar o projeto, poderá testar pelo `nc localhost 6379` e ver a atualização atômica por chave acontecendo:

```bash
# Teste de TTL (Expiração em 5 segundos)
SET sessão 12345 EX5
> +OK
GET sessão
> +12345
# (Espere 6 segundos...)
GET sessão
> $-1

# Teste do Incremento Atômico
SET contador 10
> +OK
INCR contador
> :11
INCR contador
> :12
```

---

## Resumo das Técnicas Aplicadas
1.  **TTL Ativo vs Passivo:** Como o Redis faz de verdade. Uma Task em background limpa lixo antigo a cada 5 segundos (ativo), e a função `GET` remove chaves vencidas na hora se alguém tentar lê-las antes do lixeiro passar (passivo).
2.  **`db.entry()`:** O segredo para resolver *Race Conditions* lógicas. O método `.entry()` do `DashMap` pega sincronização **no shard da chave**, impedindo que dois clientes enviem o comando `INCR` no mesmo milissegundo e terminem sobrescrevendo o cálculo um do outro.
3.  **`bytes::Bytes`:** Embora neste exemplo simples tenhamos convertido de string, a estrutura base para guardar blobs binários gigantescos e roteá-los sem clonar arrays na memória está pronta.

Este continua sendo um protótipo didático: falta um parser de protocolo que trate leituras parciais, limites de tamanho e entrada malformada antes de considerar uso fora da máquina local.

## Checklist de conclusão do bônus

- [ ] Verifiquei que uma chave expirada não é retornada por `GET` após o prazo e que a limpeza periódica remove dados vencidos.
- [ ] Executei dois clientes locais incrementando a mesma chave e conferi o resultado final sem perda de atualizações.
- [ ] Testei `INCR` com texto não numérico e com `i64::MAX`, confirmando resposta de erro sem encerrar o servidor.
- [ ] Expliquei por que o serviço escuta apenas em `127.0.0.1` e quais limites de protocolo e autenticação seriam exigidos para acesso remoto.
