---
layout: default
---

# Aula 23g - TLS com autenticação por token: o túnel não basta, o crachá decide

**Competências SDD:** SEC, RED, CON. **Pré-requisitos:** aulas [23a](23a.md) (framing e timeouts), [23c](23c.md) (token entre nós) e [23d](23d_limites_globais.md) (tetos globais). Biblioteca registrada: OpenSSL 3.5.9 (`-lssl -lcrypto`).

**Objetivo observável:** ao terminar, você deverá executar o exemplo com `g++` e `clang++` (e sob ASan/UBSan), mostrar o token certo passando e o errado negado dentro do mesmo túnel TLS e explicar por que o exemplo usa pinagem de primeiro contato (TOFU) em vez de CA — e o que muda para produção.

**Motivação:** a 23c autenticou com token em texto puro: quem vê o fio, rouba o crachá. Criptografar o transporte sem autenticar a aplicação é o erro simétrico: túnel perfeito, passageiro errado. Esta aula fecha os dois lados sobre a base da 23d — TLS de verdade (handshake OpenSSL, certificado, `SSL_read`/`SSL_write`) com o token da 23c viajando dentro dele. E declara o que o exemplo NÃO resolve: sem CA, a confiança no servidor nasce no primeiro contato.

**Como ler esta aula (tres niveis):** *fundamental* -- pin TOFU e as duas negacoes; *intermediario* -- handshake e o modelo de ameaca em tres clausulas; *aprofundamento* -- mTLS e sessao resumida (fora do contrato).

## 1. Contrato antes do código

- Servidor em `127.0.0.1` com porta efêmera (sem disputa de porta na máquina) e certificado autofirmado **efêmero** gerado em memória via API OpenSSL: nada é lido de disco, nada sobrevive ao processo, nada é confiável fora dele.
- O cliente fixa a impressão SHA-256 do certificado recebida pelo pipe (TOFU): fala só com quem tem aquela impressão. Em produção, essa etapa seria verificação de cadeia contra CA — o exemplo não finge que o pipe é uma CA.
- Duas voltas na mesma base: token certo → `OK`, token errado → `NEGADO`. Sem token (linha vazia ou outro formato), a resposta também é `NEGADO`: o parse não distingue "errado" de "ausente", e essa indistinção é proposital — atacante não ganha oráculo de formato.
- Fora do contrato: autenticação mútua (só o cliente valida o servidor), revogação, sessão resumida, certificado persistido.

## 2. Onde cada decisão mora

**O certificado nasce e morre no processo.** `EVP_RSA_gen` + `X509_new` + `X509_sign` criam a credencial com validade de 10 minutos; o contexto do servidor a referencia e a libera no fim. Não há arquivo, logo não há permissão de arquivo para errar, nem chave esquecida no disco para vazar. O preço: todo restart gera outra identidade — exatamente por isso o cliente não pode "conhecer" o servidor de antemão.

### Anatomia do handshake que o exemplo executa

Siga os bytes, lado a lado: o cliente abre TCP, cria o `SSL*` sobre o socket e chama `SSL_connect`; o servidor aceita, cria o seu `SSL*` e chama `SSL_accept`. Dentro dessas duas chamadas viajam, nesta ordem: (1) o cliente anuncia o que sabe falar (versões, cifras); (2) o servidor responde com a cifra escolhida e o certificado autofirmado; (3) o cliente valida (aqui: compara a impressão SHA-256 com a do pipe, em vez de cadeia CA); (4) os dois derivam as chaves de sessão e trocam a mensagem de "mãos dadas". Só depois do passo 4 o primeiro byte do token atravessa o socket — e atravessa cifrado com chave efêmera, o que dá *forward secrecy*: mesmo que a chave privada do certificado vaze amanhã, a sessão de hoje continua ilegível, porque a chave de sessão não foi transportada, foi derivada. O ponto de ensino: `SSL_read`/`SSL_write` depois do handshake são `read`/`write` com cifra — o programador que entende onde o handshake termina (passo 4) nunca manda segredo no passo 1.

### TOFU, com o modelo de ameaça escrito por extenso

Pinagem de primeiro contato diz: "confio nesta impressão porque chegou por um canal que confio (o pipe local), e daqui em diante só falo com quem a repetir". O modelo de ameaça tem três cláusulas: (a) o primeiro contato é seguro por construção (pipe entre pai e filho, mesma máquina, sem rede); (b) contatos seguintes detectam troca de identidade (atacante com outro certificado gera outra impressão e é negado); (c) o primeiro contato numa rede hostil NÃO é seguro — e é exatamente esse o caso que a produção resolve com CA. A honestidade editorial está em escrever (c): o exemplo não "resolve confiança", ele transfere a confiança do pipe para o pin e declara o pipe como raiz. Quem lê "TOFU" sem (c) sai achando que pinar é alternativa à CA; quem lê com (c) entende que pinar é *adiar* a CA para um canal lateral que, aqui, existe de verdade.

**O pin viaja pelo pipe, não pelo TLS.** Publicar a impressão antes do primeiro `accept` evita o deadlock (pai bloqueado lendo, filho bloqueado aceitando) e modela honestamente o TOFU: a raiz de confiança chega por canal lateral. O cliente compara a impressão do handshake com a do pipe — `SSL_VERIFY_NONE` na CA com pinagem explícita, em vez de verificação silenciosamente desligada.

**O token viaja depois do handshake, nunca antes.** `SSL_connect` completo precede qualquer byte de aplicação nos dois lados. Token antes do túnel seria a 23c de novo; túnel sem token seria transporte anônimo. A ordem é o contrato.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23g_tls.cpp) contém servidor e cliente (fork, `g++` e `clang++`, `-Werror` nos dois):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23g_tls.cpp -o srv23g -lssl -lcrypto
./srv23g
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23g_tls.cpp -o srv23g_clang -lssl -lcrypto
./srv23g_clang   # idêntico
```

Com sanitizers (exigido para ponte: ASan/UBSan limpos):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/cpp/exemplos/23g_tls.cpp -o srv23g_asan -lssl -lcrypto
./srv23g_asan
```

Saída esperada (determinística; a porta varia, o conteúdo não):

```text
token 's3nha-da-23c': OK
token 'errada': NEGADO
23g ok: TLS com pin TOFU + token certo passa, errado nega
```

### Leitura guiada: duas negações, um túnel

A saída tem três linhas e conta dois experimentos: o token certo atravessa o túnel e volta `OK` (prova que o handshake completou — sem ele, nenhum byte de aplicação passaria); o errado atravessa o MESMO túnel e volta `NEGADO` (prova que o túnel não autentica ninguém sozinho — cifra sem crachá é transporte anônimo). Se a segunda linha fosse `OK`, o bug estaria na comparação do token; se a primeira fosse `NEGADO`, o bug estaria no handshake ou no pin. A terceira linha só sai se as duas anteriores saíram certas — é o veredito sobre o par, não sobre cada um. E a porta varia a cada execução sem quebrar nada: efêmera por construção, impressa para depuração, irrelevante para o contrato — o que é observável (conteúdo) separa-se do que é acidental (porta), e o teste cola só o observável.

## 4. Erros comuns e implicações defensivas

1. **Verificação desligada sem pin:** `SSL_VERIFY_NONE` sozinho é cliente que fala com qualquer um — o exemplo só o usa porque a pinagem decide depois. Um dos dois tem de amarrar a identidade; nenhum dos dois é configuração, é vulnerabilidade.
2. **Token antes do handshake:** mandar o segredo para "negociar" o túnel expõe o segredo a quem não provou nada. Primeiro o túnel, depois o crachá — a ordem inversa anula os dois.
3. **Oráculo de formato:** distinguir "token ausente" de "token errado" na resposta ensina o atacante a forma do pedido. `NEGADO` único para os dois casos fecha o oráculo.
4. **Chamar TOFU de PKI:** o pipe deste experimento é honesto porque é local e efêmero; numa rede real, primeiro contato sem CA é interceptável. Produção exige cadeia verificada — o exemplo lista isso no contrato em vez de prometer.
5. **Certificado em disco "só para teste":** chave de teste commitada vira chave de produção em algum deploy distraído. Certificado efêmero em memória elimina a categoria inteira de vazamento.
6. **Comparar token com `==` em `std::string` sem pensar em tempo:** a comparação do exemplo é funcional, não de tempo constante — atacante medindo latência distingue prefixo certo de errado byte a byte (timing oracle). Em produção, token se compara em tempo constante; o exemplo declara a comparação como funcional e lista o timing como endurecimento futuro.
7. **Reutilizar a sessão sem revalidar o pin:** sessão TLS resumida (`SSL_session`) que pula o handshake pula a entrega do certificado — e sem certificado não há impressão para comparar. Resumir sem revalidar é herdar confiança de uma sessão que pode ter sido negociada antes de uma troca de identidade. Por isso "sessão resumida" está no fora-do-contrato: não é otimização inocente, é decisão de confiança.

## 5. Ponte com as demais trilhas

C cifra o análogo na [23e](../c/23e_rpc_sobre_consenso.md) sem TLS (numerar e confirmar em texto puro); Go exige quorum com token na 23c sem túnel; Rust audita a fronteira na 15a sem transportá-la. Em JavaScript, o análogo é `https://` + `Authorization: Bearer` — mesma divisão (transporte cifra, cabeçalho autentica), outra pilha. O padrão "túnel + crachá + negação única" vale nas quatro.


### Pesos da avaliacao

| Criterio | Peso |
|---|---|
| TLS com pin executado + token certo passa, errado nega | 35% |
| Modelo de ameaca enunciado nas tres clausulas | 25% |
| Diagnostico (prever deadlock pin-antes-do-accept) | 25% |
| Documentacao (comandos, saidas) | 15% |

**Referencias primarias:** [OpenSSL docs](https://www.openssl.org/docs/); [RFC 8446 (TLS 1.3)](https://www.rfc-editor.org/rfc/rfc8446).

## 6. Critério de aprovação e checklist

- [ ] Rodo com `g++` e `clang++` e aponto onde o handshake termina e o token começa.
- [ ] Explico o TOFU deste exemplo e o que uma CA mudaria (e onde no código).
- [ ] Mostro o token errado negado e justifico o `NEGADO` único.
- [ ] Descrevo o deadlock que o pin-antes-do-accept evita.
- [ ] Enuncio as três cláusulas do modelo de ameaça do TOFU (primeiro contato seguro, troca detectada, rede hostil fora).
- [ ] Explico por que sessão resumida é decisão de confiança, não otimização inocente.
- [ ] Nomeio o que falta: mTLS, revogação, sessão resumida, certificado persistido com permissão certa, comparação em tempo constante.

**Referências primárias:** [RFC 8446 (TLS 1.3)](https://www.rfc-editor.org/rfc/rfc8446.html) para o handshake que o OpenSSL executa aqui; documentação do OpenSSL (`SSL_CTX_new`, `X509_digest`) para a API usada. Nenhum texto foi copiado — o exemplo é implementação própria sobre essas interfaces.

## 7. Exercícios obrigatórios

1. (Previsão) Troque 1 bit do pin antes da 2ª volta (`pin_ruim[0] ^= 1`): preveja — negação limpa ou `assert`? Rode e explique: o pin detecta troca de identidade (cláusula b do TOFU) — e por que a comparação é igualdade total, não "parecido".
2. (Diagnóstico) Envie o segredo sem o prefixo (`"s3nha-da-23c\n"` cru): preveja `OK` ou `NEGADO`. Rode e explique por que o formato (`TOKEN ` + segredo) é o contrato — segredo sozinho, fora do formato, é linha qualquer.
3. (Projeto) Implemente throttle de autenticação: após 3 `NEGADO`s, a conexão seguinte recebe `BLOQUEADO` sem nem avaliar o token (teste: certo, 3 erradas, certo → `BLOQUEADO`). Cuidado com a armadilha: recusar sem ler o pedido gera RST e o cliente nunca lê o motivo — drena antes de recusar (a lição da 23f, agora sob TLS).
4. (Previsão) Apague a comparação do pin (`(void)pin`): preveja — o cliente com pin `"qualquer-um"` passa? Rode e explique o que o TLS garante sozinho (cifra, forward secrecy) e o que ele não garante sem o pin (identidade: túnel anônimo aceita MITM com outro certificado).

Use [pistas graduais](exercicios/23g_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23g_solucoes.md) depois de registrar sua própria explicação.

---

### Para se aprofundar

1. Eric Rescorla, *SSL and TLS* (Addison-Wesley, ISBN 978-0201615982); 2. OWASP Authentication Cheat Sheet ([cheatsheetseries.owasp.org](https://cheatsheetseries.owasp.org/cheatsheets/Authentication_Cheat_Sheet.html)); 3. Documentacao do OpenSSL ([openssl.org/docs](https://www.openssl.org/docs/)).
