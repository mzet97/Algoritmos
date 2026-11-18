---
layout: default
---

# Soluções comentadas da aula 23b

## 1. Linha do tempo de `op2`

`PUT op1` chega aos dois (replicação). B morre por `SIGKILL`. `PUT op2` aplica só em A — B nem existe para receber. O lixo sem `\n` entra no fim do log de B, que reinicia: replay traz `k1` e ignora a cauda, então `GET g6 k3` responde vazio — B nunca viu `op2`, e adivinhar seria mentir. O reenvio com o mesmo `id` aplica uma vez em B e encaminha a A, que detecta duplicata: converge sem duplicar.

## 2. Sem o selo, `k3` final quebra

O reenvio cola `PUT op2 k3 30\n` no lixo sem terminador, formando uma linha que não começa com `PUT`. Na segunda reinicialização (par lento), o replay descarta a linha inteira: B perde `k3` e o check de igualdade final falha em `k3`. Uma cauda rasgada sem selo engole todos os registros seguintes — o selo existe para isolar o lixo uma vez, não para consertá-lo.

## 3. `O_WRONLY` vira `EBADF` e `ERRO sem-disco`

O selo lê o último byte para decidir; fd aberto só-escrita recusa leitura com `EBADF`. `gravar_log` devolve falso e todo `PUT` — mesmo saudável — responde `ERRO sem-disco`. A correção é abrir `O_RDWR`: o modo de acesso tem de cobrir tudo que o protocolo do arquivo exige, e aqui exige ler antes de escrever.

## 4. Atraso de 5 s: quem desiste é o `recv`, não o dado

Medido: resposta em ~2,0 s, cenário segue verde. O atraso dorme antes do `connect` em B, mas A não espera B acordar: o `recv` do encaminhamento tem timeout de 2 s e a resposta do par é ignorada de qualquer jeito (`melhor esforço`). A desiste de ouvir, responde `OK` ao cliente e segue. O dado não se perde porque a ordem é aplicar-antes-de-encaminhar: B já persistiu `k4` com `fsync` antes de dormir, e A já tinha aplicado antes de encaminhar. Moral em duas partes: timeout no fio protege a disponibilidade (teto de 2 s, não de 5 s), e aplicar-antes-de-encaminhar protege a convergência.
