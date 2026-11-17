# Pistas da aula 19a

1. Contador global de conexoes ativas com Lock; acima do teto, responder OCUPADO e fechar. O cliente excedente observa recusa, nao espera infinita.
2. Deadline: recv com timeout por conexao; quem nao manda nada em N segundos perde a conexao. Sem isso, conexao muda segura thread para sempre.
3. Framing binario [u32 len][payload]: fragmentacao continua existindo (o len tambem fragmenta), mas some a ambiguidade do delimitador dentro do payload.
4. Ameacas fora de loopback: quem conecta, custo por conexao, ausencia de autenticacao/TLS e teto de log/disco do servico.
