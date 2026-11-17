# Solucoes comentadas da aula 23b (Rust)

## 1. Envenenamento e recuperacao

Com um panic dentro da secao critica, os lock().unwrap() seguintes tambem panica: o no fica mudo sem ter caido. A recuperacao honesta nesta arquitetura e tratar o estado em memoria como suspeito e reconstruir pelo replay do log (a fonte de verdade que sobrevive), substituindo o Mutex envenenado por um novo com o estado relido. Alternativa valida: unwrap_or_else(|e| e.into_inner()) aceitando o estado como esta — so e defensavel se a secao critica for transacional (ou tudo-ou-nada), o que aqui ela quase e: persistir-e-aplicar tem uma janela entre sync_all e o push. A janela e exatamente o que o replay resolve no restart; em memoria, a decisao precisa estar escrita.

## 2. Teto de conexoes

Um contador sob Mutex com Condvar: conexao nova espera (com timeout declarado) enquanto ativas >= teto; ao fechar, decrementa e notify_one. O cliente excedente observa espera, nao recusa — comportamento de fila, com o custo declarado de segurar a conexao aberta. Sem timeout, fila infinita vira o novo problema.

## 3. Registro com CRC

Formato [u32 len][payload][u32 crc]: o replay le registros inteiros; no primeiro len absurdo, payload curto ou CRC divergente, para e descarta a cauda. Registros anteriores intactos sobrevivem a uma rasgadura em qualquer byte do ultimo registro — melhoria real sobre a linha de texto, que so protege contra rasgadura que quebra a sintaxe da linha.

## 4. Ameacas em loopback

Em 127.0.0.1, qualquer processo do usuario conecta: o modelo e "confianca no usuario local", e o abuso disponivel e exaustao (conexoes, disco do log, ids infinitos). Antes de sair do loopback: autenticacao de quem fala, TLS no canal, tetos de log/conexao/tempo e rotacao — sem isso, o no e um servico de apagar o proprio disco a pedido.
