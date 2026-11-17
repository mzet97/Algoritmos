# Pistas da aula 29b

1. Modelo de ameacas da 19a: superficie (porta, linhas, teto), abuso (conexoes, linha infinita, flood), limites e o que sobra (loopback confiavel?).
2. Esquema unico: um validador que declara campos/tipos/tamanhos; abuso novo = primeiro caso que o esquema antigo deixava passar.
3. shell=True auditado: reescrever com lista; se o comando precisa de pipeline, ele vira dois processos ligados por voce, nao por um shell.
4. Limites por cliente com teste: conexoes simultaneas, tempo ocioso e bytes por conexao, cada um com o abuso que o prova.
