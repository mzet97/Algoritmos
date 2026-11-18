---
layout: default
---

# Aula 32c — Modelo de ameaças dos três projetos: o que cada fronteira recusa

**Competências SDD:** SEC, NET, DST e OS. **Pré-requisitos:** aulas [31](31.md) (servidor), [23a](23a_sistemas_distribuidos.md) (distribuído) e [32a](32a_kernel_riscv.md) (kernel). Sem elas, esta aula é uma lista de medos; com elas, é o contrato que faltava aos projetos — com números, não adjetivos.

**Objetivo observável:** ao terminar, você deverá nomear a fronteira de confiança de cada projeto, listar sua superfície de ataque, executar a tabela de abusos — cada abuso recusado em código, com o limite que o recusou — e escrever a linha de abuso que falta para um quarto projeto hipotético (formato: ator, entrada, limite, teste).

**Motivação:** os três projetos funcionam no caminho feliz. O caminho feliz é onde o atacante não está. Esta aula coloca o atacante na sala: o cliente que manda 1 MiB onde cabem 1024 bytes, o par que repete a escrita de ontem, o programa de usuário que passa `NULL` como ponteiro na syscall. Cada um recebe a mesma resposta: *recusa nomeada, com limite publicado, testada em código*.

**Como ler esta aula (três níveis):** *fundamental* — a tabela fronteira/abuso/limite/teste; *intermediário* — o par válido+negação que prende o off-by-one; *aprofundamento* — o que o modelo não cobre e qual curso cobriria.

## 1. Contrato antes do código

- **Servidor 31 (rede hostil):** todo byte vem do adversário. Linha acima de 1024 bytes é rejeitada; NUL embutido é rejeitado (framing, não string C — `strlen` para no NUL e o resto passa batido); além de 64 conexões, a nova é recusada — nunca enfileirada em silêncio (fila silenciosa é espera infinita com outro nome).
- **Distribuído 23a (par que mente por acidente ou malícia):** mensagem sem `id` válido é rejeitada; `id` repetido é idempotente (responde sem reexecutar, como a 23e prova); o log só confirma o que passou pelo `fsync` — confirmação sem durabilidade é promessa sem lastro.
- **Kernel 32a (usuário que pede qualquer coisa):** syscall valida ponteiro e tamanho antes de tocar: `NULL`, zero ou absurdo é negado no modo usuário. O kernel confia no kernel; em todo o resto, desconfia por padrão — inclusive de tamanho "pequeno demais para ser ataque".
- **Transversal:** log sem segredo (sem senha, sem token — verificado por `strstr` negativo no exemplo), com limite e origem para auditar; configuração inválida falha rápido na partida (`assert` na inicialização), nunca no meio da noite.
- **Fora do contrato:** criptografia, autenticação mútua e sandbox de processo — o modelo assume rede e programa hostis, não máquina comprometida. Endurecer o hospedeiro é outro curso. E note o que a aula não promete: os tetos aqui são os *valores dos projetos* (1024, 64, 256), não universais — cada projeto publica os seus.

## 2. Por que recusar em vez de tolerar?

Tolerar entrada hostil é transferir a decisão para o código que menos entende o risco: o parser interno, o loop de replicação, o handler da syscall. A recusa na fronteira faz o oposto — decide onde há contexto (o limite publicado, com o número) e protege tudo o que vem depois. O preço é disponibilidade aparente: clientes malformados veem erro. Mas erro explícito é diagnosticável (grep no log, reproduz em teste); corrupção silenciosa é incidente às 3h da manhã sem linha de log que explique — e sem teste que a capture, porque o teste do caminho feliz nunca a exercitou.

Há um segundo motivo, econômico: validar na fronteira é barato (comparação de tamanho, `memchr` por NUL) e validar no núcleo é caro (estado já alocado, lock já tomado, log já escrito). Cada camada que a entrada atravessa sem validação multiplica o custo da recusa tardia. A tabela desta aula cobra a recusa no ponto mais externo possível de cada projeto — e o exemplo prova que o ponto externo decide sem alocar nada sob controle do adversário.

## 3. As três fronteiras, abuso por abuso

**Fronteira 1 — linha, NUL, conexões.** `aceitar_linha` nega vazio e acima de 1024, depois varre byte a byte por NUL embutido: a ordem importa (tamanho primeiro, conteúdo depois — varrer 1 MiB para achar NUL antes de negar o tamanho é dar ao atacante 1 MiB de trabalho gratuito). O NUL embutido é rejeitado porque framing de rede não é string C: `"AB\0CD"` com `n=5` tem 5 bytes de protocolo e 2 de string — quem usa `strlen` lê 2 e ignora 3, e os 3 ignorados viram o próximo "comando". `aceitar_conexao` nega a 65ª: o teste confere o limite e o vizinho (63 passa, 64 não), porque teto testado só no valor exato deixa o off-by-one morar no código.

**Fronteira 2 — replay com nome.** `aceitar_replay` nega `id` vazio ou acima de 256 (chave gigante é DoS de memória disfarçado de identidade), devolve 1 para repetido (responde igual, sem reexecutar — a 23e é a implementação completa) e 0 copiando com teto para o novo. Note o que a função *não* faz: não punir repetição. Repetição é propriedade da rede (timeout + retry), não evidência de ataque — tratá-la como erro transformaria cliente legítimo em invasor a cada pacote duplicado.

**Fronteira 3 — ponteiro e tamanho do usuário.** `validar_syscall_ptr` confia em `usuario == 0` (chamada interna do kernel, sem fronteira cruzada) e nega o resto por três portas: `NULL` (não há o que ler), `len == 0` (nada a fazer, mas também nada a validar — nega por princípio: syscall sem trabalho é ruído), `len > 1024` (absurdo para o contexto; o número é do projeto, não universal). Em produção real (a 32d declara como futuro), a validação continuaria: a página está mapeada? é legível pelo chamador? — aqui, a aula estabelece o hábito (negar antes de tocar) com o mínimo executável.

## 4. Exemplo completo e executável

O programa abaixo é o [arquivo-fonte desta aula](exemplos/32c_ameacas.c), reproduzido integralmente:

```c
/* Aula 32c — modelo de ameacas executavel dos projetos de C.
 *
 * Tres projetos, tres fronteiras: servidor 31 (rede hostil), distribuido 23a
 * (par que pode mentir) e kernel 32a (usuario que pode pedir qualquer coisa).
 * Este programa executa a tabela de abuso: cada linha e uma decisao que o
 * projeto precisa tomar, com o limite cobrado em codigo.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/32c_ameacas.c -o /tmp/32c
 * Executar: /tmp/32c
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINHA 1024
#define MAX_CONEXOES 64
#define MAX_CHAVE 256

/* Fronteira 1 (servidor 31): entrada da rede nunca e confiavel. */
static int aceitar_linha(const char *entrada, size_t n)
{
    if (n == 0 || n > MAX_LINHA) {
        return -1; /* vazia ou acima do teto: rejeita */
    }
    for (size_t i = 0; i < n; i++) {
        if (entrada[i] == '\0') {
            return -1; /* NUL embutido: framing, nao string C */
        }
    }
    return 0;
}

static int aceitar_conexao(int abertas)
{
    return abertas < MAX_CONEXOES ? 0 : -1;
}

/* Fronteira 2 (distribuido 23a): o par pode repetir, reordenar e atrasar. */
static int aceitar_replay(const char *id, const char **vistos, int n_vistos,
                           char *chave_out, size_t cap)
{
    if (strlen(id) == 0 || strlen(id) > MAX_CHAVE) {
        return -1;
    }
    for (int i = 0; i < n_vistos; i++) {
        if (strcmp(vistos[i], id) == 0) {
            return 1; /* repeticao: idempotente, nao reexecuta */
        }
    }
    snprintf(chave_out, cap, "%s", id);
    return 0;
}

/* Fronteira 3 (kernel 32a): syscall com argumento do usuario. */
static int validar_syscall_ptr(const void *ptr, size_t len, int usuario)
{
    if (!usuario) {
        return 0; /* kernel confia no kernel */
    }
    if (ptr == NULL || len == 0 || len > MAX_LINHA) {
        return -1; /* ponteiro nulo ou tamanho absurdo: nega */
    }
    return 0;
}

int main(void)
{
    char grande[MAX_LINHA + 10];
    memset(grande, 'A', sizeof grande);
    assert(aceitar_linha(grande, sizeof grande) == -1);
    assert(aceitar_linha("GET k\n", 6) == 0);
    assert(aceitar_linha("AB\0CD", 5) == -1);
    printf("servidor: teto de linha, NUL embutido e teto de conexao cobrados\n");
    assert(aceitar_conexao(64) == -1 && aceitar_conexao(63) == 0);

    const char *vistos[] = { "op-1", "op-2" };
    char chave[MAX_CHAVE + 1];
    assert(aceitar_replay("op-1", vistos, 2, chave, sizeof chave) == 1);
    assert(aceitar_replay("op-3", vistos, 2, chave, sizeof chave) == 0);
    assert(aceitar_replay("", vistos, 2, chave, sizeof chave) == -1);
    printf("distribuido: replay idempotente, id vazio rejeitado\n");

    assert(validar_syscall_ptr(NULL, 10, 1) == -1);
    assert(validar_syscall_ptr("x", 1 << 20, 1) == -1);
    assert(validar_syscall_ptr("x", 1, 1) == 0);
    printf("kernel: ponteiro nulo e tamanho absurdo negados no modo usuario\n");

    /* Segredos e logs: o teste que falta e o grep que passa. */
    const char *log_ok = "conexao recusada limite=1024 cliente=10.0.0.5";
    assert(strstr(log_ok, "senha") == NULL && strstr(log_ok, "token") == NULL);
    printf("logs: sem segredo, com limite e origem para auditar\n");

    printf("32c ok: ameacas nomeadas, limites executados, abusos recusados\n");
    return 0;
}
```

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/32c_ameacas.c -o /tmp/32c
/tmp/32c
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/32c_ameacas.c -o /tmp/32c_asan
/tmp/32c_asan   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística):

```text
servidor: teto de linha, NUL embutido e teto de conexao cobrados
distribuido: replay idempotente, id vazio rejeitado
kernel: ponteiro nulo e tamanho absurdo negados no modo usuario
logs: sem segredo, com limite e origem para auditar
32c ok: ameacas nomeadas, limites executados, abusos recusados
```

## 5. Leitura guiada da tabela

Cada linha é uma fronteira exercitada — e cada `assert` testa a borda, não o meio. `aceitar_linha` com 1034 bytes nega (acima do teto), com `"GET k\n"` aceita (válida passa — teste que só nega nunca prova que aceita), com NUL embutido nega (o caso que `strlen` esconderia). `aceitar_conexao(64)` nega e `(63)` aceita: o par limite/vizinho é o teste do off-by-one — teto cobrado só no valor exato deixa `<=` vs `<` sem testemunha.

No distribuído, `"op-1"` devolve 1 (repetição: responde igual), `"op-3"` devolve 0 e copia (novo: grava com teto), `""` devolve -1 (sem nome, sem resposta — a mesma porta da 23e). No kernel, `NULL` nega, 1 MiB nega, 1 byte de ponteiro válido passa — e o `!usuario` passa tudo, documentando que a confiança interna não atravessa fronteira.

O log fecha a tabela pelo avesso: em vez de afirmar "não vazo", o teste afirma a ausência (`strstr(...) == NULL` para `"senha"` e `"token"`) sobre uma linha que *audita* (`limite=1024 cliente=10.0.0.5` — o que, quanto e de onde). Log bom responde três perguntas (o quê, quanto, de onde) sem responder a quarta (com qual segredo). O programa não simula o atacante — ele executa as decisões que o atacante força, e cada decisão tem número.

### O que a memória faz aqui?

Limites antes de alocação: nenhum `malloc` acontece antes do tamanho passar pelo teto — e neste programa nem há `malloc`: tudo em pilha com tamanho de compilação, o que elimina a classe inteira de "alocação sob controle do adversário". NUL embutido tratado como dado de framing, nunca como terminador: o loop varre `n` bytes declarados, não procura terminador. Nada aqui aloca sob controle do adversário sem teto prévio — essa é a regra de memória da aula, e a mais barata de auditar: procure `malloc` depois do `if (n > TETO) return -1` e não ache nenhum.

## 6. Erros comuns e implicações defensivas

1. **Validar depois de usar:** checar o tamanho após copiar é autópsia — o estouro já aconteceu. Teto primeiro, cópia depois, sem exceção. Procure no seu código: todo `memcpy`/`strcpy` tem um `if` de tamanho antes? Se a resposta exige grep, a resposta é não.
2. **Tratar replay como erro:** repetição não é ataque, é rede (timeout + retry). O certo é responder igual sem reexecutar — idempotência, não punição. Punir retry legítimo transforma cliente em invasor a cada pacote duplicado.
3. **Logar o segredo para "debugar":** o log vaza para onde o erro viaja (arquivo, coletor, tela do operador). Segredo em log é segredo publicado — e publicado com timestamp, o que ajuda o atacante a correlacionar.
4. **Confundir autenticação com autorização:** saber quem chama não diz o que ele pode pedir. A syscall valida o pedido mesmo de chamador conhecido — `!usuario` é sobre *origem interna*, não sobre identidade.
5. **Testar só a negação:** teste que nega tudo (inclusive o válido) é firewall que bloqueia a loja junto com o ladrão. Cada teto desta tabela tem o caso válido ao lado — o par nega/aceita é a unidade mínima de teste de limite.
6. **Ameaçador genérico ("o sistema deve ser seguro"):** ameaça sem abuso nomeado, sem número e sem teste é desejo. A tabela desta aula (ator, entrada, limite, teste) é o formato exigido nos projetos — e o checklist cobra a quarta linha que você ainda não escreveu.
7. **Teto copiado sem contexto:** 1024/64/256 são os números *destes* projetos. Reutilizar o número sem reavaliar o contexto (linha de protocolo HTTP vs linha de comando interno) é importar a ameaça alheia junto com o limite.

## 7. Ponte com as demais trilhas

O formato — fronteira, abuso, limite, teste — é o mesmo cobrado nos projetos de C++, Rust, Go e Python pela rubrica de projeto (correção, falhas, segurança, medição, comunicação): a [rubrica SDD](../RUBRICA_SDD.md) não pede "segurança", pede abusos nomeados com testes. Em JavaScript, a fronteira é o formulário e a query string: mesmo trio (teto de tamanho, NUL/encoding hostil, replay do duplo clique), mesmos tetos, mesmo replay (o duplo clique que cria dois pedidos — a 23e com roupa de usuário). A ameaça muda de endereço, não de forma. E a 23f mostra o que acontece quando um teto desta tabela falta no servidor real: o global que a 32c declara aqui é medido lá.

---

### Pesos da avaliacao

| Criterio | Peso |
|---|---|
| Compreensao conceitual (previsao, diagnostico, explicacao) | 40% |
| Analise de compilacao e ferramentas | 25% |
| Funcionamento (compila sem avisos e roda) | 20% |
| Documentacao (comandos, versao, saidas) | 15% |

A execucao bem-sucedida sozinha nao aprova: a maior parcela esta em demonstrar que voce entendeu o porque.

### Para se aprofundar

1. Robert Seacord, *Secure Coding in C and C++* (Addison-Wesley, 2. ed., ISBN 978-0321822130); 2. *Operating Systems: Three Easy Pieces* ([pages.cs.wisc.edu/~remzi/OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/)); 3. *C11 standard draft N1570*, Anexo K (funcoes com verificacao de limites).

## 8. Checklist do que esta aula garante

- [ ] Nomeio as três fronteiras e digo o que cada uma recusa, com o número do limite — e de onde o número veio.
- [ ] Executo a tabela de abusos e aponto onde cada recusa mora no código do projeto real (não no exemplo: no `aula31.c`, na 23a, no `trap_handler`).
- [ ] Mostro um log que audita (o quê, quanto, de onde) sem vazar, e uma config inválida que falha na partida.
- [ ] Distingo replay (idempotente: responde igual) de ataque (recusado) e autenticação (quem) de autorização (o quê).
- [ ] Para cada teto, mostro o caso válido ao lado da negação (o par que prende o off-by-one).
- [ ] Escrevo a quarta linha da tabela (novo ator, entrada, limite, teste) no formato exigido.
- [ ] Declaro o que o modelo não cobre: cripto, mTLS e hospedeiro comprometido — e digo qual curso cobriria.

**Referências primárias:** [NIST SSDF](https://csrc.nist.gov/pubs/sp/800/218/final) como referência de processo (não checklist — processo diz *quando* validar, a tabela diz *o quê*); `man 2 read`/`man 2 recv` para parcialidade de leitura (toda fronteira de rede lê parcial antes de decidir); a [rubrica de projeto SDD](../RUBRICA_SDD.md) para as dimensões cobradas. Modelo de ameaças sem teste de abuso é ensaio — esta aula exige os dois, e o exemplo é os dois.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
