---
layout: default
---

# Aula 20b — TLS com certificado efêmero e guarda anti-SSRF (G5)

**Competências SDD:** NET, SEC. **Pré-requisitos:** aulas 16 (HTTP idiomático), 20 (servidores) e [23a](23a_protocolos_distribuidos.md) (framing, idempotência, falhas injetadas — sem TLS, declarado). Toolchain registrada: Go 1.26.8, só biblioteca padrão (`crypto/tls`, `crypto/x509`, `net/http`).

**Objetivo observável:** ao terminar, você deverá executar o exemplo, mostrar o token certo passando e o errado negado dentro do TLS, listar os cinco alvos SSRF recusados antes de qualquer byte viajar — e explicar por que o exemplo usa pinagem TOFU em vez de CA.

**Motivação:** quatro aulas deste livro adiam o TLS ("faltam TLS", "não precisa de TLS", "futuramente terminar TLS"). Esta aula quita a dívida do lado do protocolo: servidor HTTPS em `127.0.0.1` com certificado autofirmado **efêmero** gerado em memória, cliente que fixa a impressão SHA-256 e token viajando dentro do túnel. E fecha a outra ponta da confiança: o cliente que *sai* precisa do guarda anti-SSRF, porque URL é entrada do atacante — `http://169.254.169.254/` não é endereço, é pedido de credencial da nuvem.

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
# 20b ok: TLS com pin TOFU + token, SSRF recusado antes do byte
```

### Leitura guiada: três portas, cinco muros

As três primeiras linhas são as portas: certo passa, errado nega, ausente nega — e "ausente nega igual a errado" é a negação única que fecha o oráculo de formato (a mesma decisão da 23g de C++). As cinco seguintes são os muros: cada `ssrf bloqueado` é uma conexão que nunca existiu — o guarda recusou antes do primeiro byte, e "antes do byte" é o que separa guarda de filtro (filtro vê resposta e decide; guarda impede o pedido). A ordem das linhas é o argumento: primeiro as portas (quem pode entrar), depois os muros (para onde não se sai) — autenticação de entrada e validação de saída, as duas pontas da confiança, no mesmo teste. E o veredito nomeia os dois mecanismos (pin TOFU + token, SSRF recusado) porque a aula quita duas dívidas de uma vez: o TLS adiado por quatro aulas e o SSRF que ninguém tinha cobrado.

Fora do contrato: CA verificada (o pin TOFU modela primeiro contato; produção exige cadeia), autenticação mútua, revogação, DNS com validação DNSSEC (o guarda resolve e checa **todos** os IPs — rebinding entre checagem e conexão segue declarado como limite).

## 2. Onde cada decisão mora

**O certificado nasce e morre no processo.** ECDSA P-256 + `x509.CreateCertificate` com 10 minutos de validade; a chave nunca toca o disco. Preço: todo restart troca a identidade — por isso o cliente não pode "conhecer" o servidor de antemão e o pin viaja por canal lateral (aqui, memória compartilhada do próprio teste; em produção, o canal lateral seria a CA).

**O pin decide, o `InsecureSkipVerify` obedece.** `VerifyPeerCertificate` compara o SHA-256 do certificado apresentado com o pin e só aceita igualdade. `InsecureSkipVerify: true` sem o callback seria vulnerabilidade; com o callback, é "a CA sou eu, e eu verifico exatamente isto". A distinção importa: desligar verificação e verificar por conta própria são operações opostas, embora usem o mesmo campo.

**O guarda recusa antes de conectar.** Esquema fora de http/https, userinfo embutido (`localhost@127.0.0.1` — o `@` transforma "localhost" em credencial e o host real no IP), IP literal privado/reservado/metadata, nome que resolve para privado: tudo volta `SSRF bloqueado` sem `Get` nenhum. Redirect nunca é seguido (`CheckRedirect` retorna erro): redirect é o SSRF contando com a segunda chance — e a segunda chance não existe aqui. O limite declarado: entre o `LookupIP` e o `Dial`, o DNS pode mudar (rebinding); fechar isso exige resolver-e-conectar no mesmo passo, trabalho futuro listado no checklist.

### Anatomia dos cinco bloqueios (cada um ensina um truque)

Os cinco alvos não são cinco exemplos do mesmo erro — são cinco classes: `169.254.169.254` é o metadata da nuvem (o alvo clássico: credencial por HTTP, sem autenticação, ao alcance de qualquer fetch); `127.0.0.1` é o loopback (o servidor atacando a si mesmo — admin local sem senha, porque "só localhost acessa"); `10.0.0.7` é a rede privada (pivô para dentro: do SSRF para a intranet, onde vivem dashboards sem auth); `file:///etc/passwd` é a troca de esquema (o fetch que vira leitura de arquivo — o guarda de URL que só entende http é guarda pela metade); `localhost@127.0.0.1` é o userinfo (o parser vê "localhost", o dial vê "127.0.0.1" — mesma string, dois hosts, e o guarda tem de decidir pelo que o dial vê). A regra "público ou negado" cobre as cinco sem enumerá-las: cada bloqueio é instância, não item de lista — e é por isso que lista mínima perde (erro nº 3): lista cobre instâncias conhecidas, regra cobre classes.

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
- [ ] Listo os cinco bloqueios SSRF e digo qual passaria num guarda de lista mínima.
- [ ] Descrevo o ataque de rebinding DNS que este guarda não cobre.
- [ ] Nomeio o que falta: mTLS, revogação, CA verificada, resolve-e-conecta atômico.

**Referências primárias:** [RFC 8446 (TLS 1.3)](https://www.rfc-editor.org/rfc/rfc8446.html) para o handshake que `crypto/tls` executa; documentação dos pacotes `crypto/x509` e `net/http` para a API usada. Nenhum texto foi copiado — o exemplo é implementação própria sobre essas interfaces.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
