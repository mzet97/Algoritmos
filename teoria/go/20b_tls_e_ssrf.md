---
layout: default
---

# Aula 20b — TLS com certificado efêmero e guarda anti-SSRF (G5)

## Nesta aula

Você vai aprender a configurar TLS com certificado efêmero, validar entradas para impedir SSRF, e explicar o que o TLS garante (e não garante).

Você vai ao terminar, você deverá executar o exemplo, mostrar o token certo passando e o errado negado dentro do TLS, listar os sete alvos SSRF recusados antes de qualquer byte viajar — e explicar por que o exemplo usa pinagem TOFU em vez de CA.

quatro aulas deste livro adiam o TLS ("faltam TLS", "não precisa de TLS", "futuramente terminar TLS"). Esta aula quita a dívida do lado do protocolo: servidor HTTPS em `127.0.0.1` com certificado autofirmado **efêmero** gerado em memória, cliente que fixa a impressão SHA-256 e token viajando dentro do túnel. E fecha a outra ponta da confiança: o cliente que *sai* precisa do guarda anti-SSRF, porque URL é entrada do atacante — `http://169.254.169.254/` não é endereço, é pedido de credencial da nuvem.

## 1. Contrato antes do código

O [arquivo-fonte](exemplos/20b_tls_ssrf.go) contém servidor e cliente (porta efêmera, sem disputa):

```bash
go vet teoria/go/exemplos/20b_tls_ssrf.go && go run -race teoria/go/exemplos/20b_tls_ssrf.go
# token certo: OK
# token errado: NEGADO
# sem token: NEGADO
# ssrf bloqueado: http://169.254.169.254/latest/meta-data/
# ssrf bloqueado: http://127.0.0.1/
# ssrf bloqueado: http://10.0.0.7/admin
# ssrf bloqueado: file:///etc/passwd
# ssrf bloqueado: http://localhost@127.0.0.1/
# ssrf bloqueado: http://0.0.0.0/
# ssrf bloqueado: http://[::ffff:127.0.0.1]/
# 20b ok: TLS com pin TOFU + token, SSRF recusado antes do byte
```

### Leitura guiada: três portas, sete muros

As três primeiras linhas são as portas: certo passa, errado nega, ausente nega — e "ausente nega igual a errado" é a negação única que fecha o oráculo de formato (a mesma decisão da 23g de C++). As sete seguintes são os muros: cada `ssrf bloqueado` é uma conexão que nunca existiu — o guarda recusou antes do primeiro byte, e "antes do byte" é o que separa guarda de filtro (filtro vê resposta e decide; guarda impede o pedido). A ordem das linhas é o argumento: primeiro as portas (quem pode entrar), depois os muros (para onde não se sai) — autenticação de entrada e validação de saída, as duas pontas da confiança, no mesmo teste. E o veredito nomeia os dois mecanismos (pin TOFU + token, SSRF recusado) porque a aula quita duas dívidas de uma vez: o TLS adiado por quatro aulas e o SSRF que ninguém tinha cobrado.

Fora do contrato: CA verificada (o pin TOFU modela primeiro contato; produção exige cadeia), autenticação mútua, revogação, DNS com validação DNSSEC (o guarda resolve e checa **todos** os IPs — rebinding entre checagem e conexão segue declarado como limite).

## 2. Onde cada decisão mora

**O certificado nasce e morre no processo.** ECDSA P-256 + `x509.CreateCertificate` com 10 minutos de validade; a chave nunca toca o disco. Preço: todo restart troca a identidade — por isso o cliente não pode "conhecer" o servidor de antemão e o pin viaja por canal lateral (aqui, memória compartilhada do próprio teste; em produção, o canal lateral seria a CA).

**O pin decide, o `InsecureSkipVerify` obedece.** `VerifyPeerCertificate` compara o SHA-256 do certificado apresentado com o pin e só aceita igualdade. `InsecureSkipVerify: true` sem o callback seria vulnerabilidade; com o callback, é "a CA sou eu, e eu verifico exatamente isto". A distinção importa: desligar verificação e verificar por conta própria são operações opostas, embora usem o mesmo campo.

**O guarda recusa antes de conectar.** Esquema fora de http/https, userinfo embutido (`localhost@127.0.0.1` — o `@` transforma "localhost" em credencial e o host real no IP), IP literal privado/reservado/metadata, nome que resolve para privado: tudo volta `SSRF bloqueado` sem `Get` nenhum. Redirect nunca é seguido (`CheckRedirect` retorna erro): redirect é o SSRF contando com a segunda chance — e a segunda chance não existe aqui. O limite declarado: entre o `LookupIP` e o `Dial`, o DNS pode mudar (rebinding); fechar isso exige resolver-e-conectar no mesmo passo, trabalho futuro listado no checklist.

### Anatomia dos sete bloqueios (cada um ensina um truque)

Os sete alvos não são sete exemplos do mesmo erro — são sete classes: `169.254.169.254` é o metadata da nuvem (o alvo clássico: credencial por HTTP, sem autenticação, ao alcance de qualquer fetch); `127.0.0.1` é o loopback (o servidor atacando a si mesmo — admin local sem senha, porque "só localhost acessa"); `10.0.0.7` é a rede privada (pivô para dentro: do SSRF para a intranet, onde vivem dashboards sem auth); `file:///etc/passwd` é a troca de esquema (o fetch que vira leitura de arquivo — o guarda de URL que só entende http é guarda pela metade); `localhost@127.0.0.1` é o userinfo (o parser vê "localhost", o dial vê "127.0.0.1" — mesma string, dois hosts, e o guarda tem de decidir pelo que o dial vê); `0.0.0.0` é o curinga (o parser vê "qualquer interface", o dial vê localhost — e o ramo literal do guarda só o alcança com `IsUnspecified`, predicado que o ramo de nomes já tinha); `::ffff:127.0.0.1` é o loopback fantasiado de IPv6 (o parser vê 16 bytes, o dial vê 127.0.0.1 — `IsLoopback` precisa enxergar através do mapeamento). A regra "público ou negado" cobre as sete sem enumerá-las: cada bloqueio é instância, não item de lista — e é por isso que lista mínima perde (erro nº 3): lista cobre instâncias conhecidas, regra cobre classes.

## 3. Erros comuns e implicações defensivas

1. **Verificação desligada sem pin:** o campo sozinho não protege; é o callback que protege. Auditoria de TLS em Go começa por essa dupla, não pelo `MinVersion`.
2. **Token antes do handshake:** mandar o `Bearer` para negociar o túnel expõe o segredo a quem não provou nada. Primeiro o túnel (pin conferido), depois o crachá — a ordem é o contrato.
3. **Bloquear só `169.254.169.254`:** a lista inclui loopback, privadas, link-local e nomes que resolvem para privado. SSRF por lista mínima é corrida contra a criatividade do atacante; a regra é "público ou negado", não "conhecido ou negado".
4. **Seguir redirect "só para GET":** redirect de URL pública para IP privado é o bypass clássico do guarda ingênuo. Negar redirect inteiro é mais simples e mais seguro que validar o destino do redirect.
5. **Chamar TOFU de PKI:** o pin deste exemplo é honesto porque nasce no mesmo processo; numa rede real, primeiro contato sem CA é interceptável. Produção exige cadeia verificada — o exemplo lista isso no contrato em vez de prometer.

## 4. Ponte com as demais trilhas

C++ faz o mesmo par túnel+crachá na [23g](../cpp/23g_tls_e_autenticacao.md) (OpenSSL, mesmo TOFU, mesma negação única); C declara TLS como futuro na 23e; Rust audita a fronteira sem transportá-la na 15a. Em JavaScript, o análogo é `https://` + `Authorization: Bearer` no cliente e validação de URL de webhook no servidor — mesma divisão (transporte cifra, cabeçalho autentica, destino se valida). O padrão "túnel + crachá + negação única + guarda de saída" vale nas quatro trilhas.

## 5. Critério de aprovação e checklist

- [ ] Rodo o exemplo e aponto onde o handshake termina e o token começa.
- [ ] Explico o TOFU deste exemplo e o que uma CA mudaria (e onde no código).
- [ ] Listo os sete bloqueios SSRF e digo qual passaria num guarda de lista mínima (`0.0.0.0` passava até no nosso — faltava `IsUnspecified` no ramo literal).
- [ ] Descrevo o ataque de rebinding DNS que este guarda não cobre.
- [ ] Nomeio o que falta: mTLS, revogação, CA verificada, resolve-e-conecta atômico.

**Referências primárias:** [RFC 8446 (TLS 1.3)](https://www.rfc-editor.org/rfc/rfc8446.html) para o handshake que `crypto/tls` executa; documentação dos pacotes `crypto/x509` e `net/http` para a API usada. Nenhum texto foi copiado — o exemplo é implementação própria sobre essas interfaces.

## 6. Exercícios obrigatórios

1. (Previsão) Corrompa um byte do pin (`pin[0] ^= 0xFF` após `certEfemero`): preveja se o handshake passa e se o token viaja. Rode e explique com a mensagem de erro — onde a falha acontece, antes ou dentro do HTTP.
2. (Diagnóstico) Compare as saídas de token errado e sem token (status e corpo): preveja se um atacante distingue os dois casos. Rode e explique o nome dessa propriedade e qual oráculo ela fecha.
3. (Projeto) O exemplo já bloqueia `http://0.0.0.0/` e `http://[::ffff:127.0.0.1]/` sem enumerá-los: remova `|| ip.IsUnspecified()` do ramo literal e preveja qual dos dois passa. Rode e explique — qual predicado cobre cada um e por que o curinga exigiu o predicado que o ramo de nomes já tinha.
4. (Previsão) Troque `InsecureSkipVerify: true` por `false`, mantendo o `VerifyPeerCertificate`: preveja se o pin ainda autentica. Rode e explique com a mensagem de erro — quem decide agora, o pin ou a cadeia da CA do sistema.

Use [pistas graduais](exercicios/20b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/20b_solucoes.md) depois de registrar sua própria explicação.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124), caps. sobre monitoramento e SLOs.
