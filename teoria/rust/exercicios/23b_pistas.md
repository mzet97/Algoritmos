---
layout: default
---

# Pistas da aula 23b (Rust)

1. O panico precisa acontecer com a trava adquirida para envenenar de verdade: coloque-o dentro da secao critica, num PUT com id combinado. A recuperacao pelo replay significa descartar o estado em memoria e reconstruir regs/por_id do log — que ja e a funcao replay desta aula.
2. Semaphore artesanal: um par (Mutex<usize>, Condvar); adquirir espera enquanto o contador esta no teto, liberar decrementa e notifica. O cliente excedente deve esperar conectado, nao receber erro — meça a diferenca.
3. Registro com comprimento+CRC: grave [len][payload][crc32 do payload]; no replay, pare no primeiro registro cujo comprimento ou CRC nao fecha e descarte dali para frente. A cauda rasgada cai exatamente nesse corte.
4. Modelo de ameacas em loopback: qualquer processo local conecta; PUT infinito custa disco (log sem teto). Sair de 127.0.0.1 exige autenticacao, TLS e teto de log — nesta ordem de conversa.
