---
layout: default
---

# Aula 32c — Modelo de ameaças dos três projetos: o que cada fronteira recusa

**Competências SDD:** SEC, NET, DST e OS. **Pré-requisitos:** aulas [31](31.md) (servidor), [23a](23a_sistemas_distribuidos.md) (distribuído) e [32a](32a_kernel_riscv.md) (kernel). Sem elas, esta aula é uma lista de medos; com elas, é o contrato que faltava aos projetos.

**Objetivo observável:** ao terminar, você deverá nomear a fronteira de confiança de cada projeto, listar sua superfície de ataque, e executar a tabela de abusos — cada abuso recusado em código, com o limite que o recusou.

**Motivação:** os três projetos funcionam no caminho feliz. O caminho feliz é onde o atacante não está. Esta aula coloca o atacante na sala: o cliente que manda 1 MiB onde cabem 1024 bytes, o par que repete a escrita de ontem, o programa de usuário que passa `NULL` como ponteiro na syscall. Cada um recebe a mesma resposta: *recusa nomeada, com limite publicado*.

## 1. Contrato antes do código

- **Servidor 31 (rede hostil):** toda byte vem do adversário. Linha acima de 1024 bytes é rejeitada; NUL embutido é rejeitado (framing, não string C); além de 64 conexões, a nova é recusada — nunca enfileirada em silêncio.
- **Distribuído 23a (par que mente por acidente ou malícia):** mensagem sem `id` válido é rejeitada; `id` repetido é idempotente (responde sem reexecutar); o log só confirma o que passou pelo `fsync`.
- **Kernel 32a (usuário que pede qualquer coisa):** syscall valida ponteiro e tamanho antes de tocar: `NULL`, zero ou absurdo é negado no modo usuário. O kernel confia no kernel; em todo o resto, desconfia por padrão.
- **Transversal:** log sem segredo (sem senha, sem token), com limite e origem para auditar; configuração inválida falha rápido na partida, nunca no meio da noite.
- **Fora do contrato:** criptografia, autenticação mútua e sandbox de processo — o modelo assume rede e programa hostis, não máquina comprometida. Endurecer o hospedeiro é outro curso.

## 2. Por que recusar em vez de tolerar?

Tolerar entrada hostil é transferir a decisão para o código que menos entende o risco: o parser interno, o loop de replicação, o handler da syscall. A recusa na fronteira faz o oposto — decide onde há contexto (o limite publicado) e protege tudo o que vem depois. O preço é disponibilidade aparente: clientes malformados veem erro. Mas erro explícito é diagnosticável; corrupção silenciosa é incidente às 3h da manhã sem linha de log que explique.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/32c_ameacas.c) contém o programa completo:

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

Cada linha é uma fronteira exercitada: o teto que recusa, o replay que não reexecuta, a syscall que nega, o log que não vaza. O programa não simula o atacante — ele executa as decisões que o atacante força.

### O que a memória faz aqui?

Limites antes de alocação: nenhum `malloc` acontece antes do tamanho passar pelo teto. NUL embutido tratado como dado de framing, nunca como terminador. Nada aqui aloca sob controle do adversário sem teto prévio — essa é a regra de memória da aula.

## 4. Erros comuns e implicações defensivas

1. **Validar depois de usar:** checar o tamanho após copiar é autópsia. Teto primeiro, cópia depois.
2. **Tratar replay como erro:** repetição não é ataque, é rede. O certo é responder igual sem reexecutar — idempotência, não punição.
3. **Logar o segredo para "debugar":** o log vaza para onde o erro viaja. Segredo em log é segredo publicado.
4. **Confundir autenticação com autorização:** saber quem chama não diz o que ele pode pedir. A syscall valida o pedido mesmo de chamador conhecido.
5. **Ameaçador genérico ("o sistema deve ser seguro"):** ameaça sem abuso nomeado e sem teste é desejo. A tabela desta aula é o formato exigido nos projetos.

## 5. Ponte com as demais trilhas

O formato — fronteira, abuso, limite, teste — é o mesmo cobrado nos projetos de C++, Rust, Go e Python pela rubrica de projeto (correção, falhas, segurança, medição, comunicação). Em JavaScript, a fronteira é o formulário e a query string: mesmo trio, mesmos tetos, mesmo replay (o duplo clique que cria dois pedidos). A ameaça muda de endereço, não de forma.

## 6. Checklist do que esta aula garante

- [ ] Nomeio as três fronteiras e digo o que cada uma recusa, com o número do limite.
- [ ] Executo a tabela de abusos e aponto onde cada recusa mora no código do projeto real.
- [ ] Mostro um log que audita sem vazar e uma config inválida que falha na partida.
- [ ] Distingo replay (idempotente) de ataque (recusado) e autenticação de autorização.
- [ ] Declaro o que o modelo não cobre: cripto, mTLS e hospedeiro comprometido.

**Referências primárias:** [NIST SSDF](https://csrc.nist.gov/pubs/sp/800/218/final) como referência de processo (não checklist); `man 2 read`/`man 2 recv` para parcialidade de leitura; a [rubrica de projeto SDD](../RUBRICA_SDD.md) para as dimensões cobradas. Modelo de ameaças sem teste de abuso é ensaio — esta aula exige os dois.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
