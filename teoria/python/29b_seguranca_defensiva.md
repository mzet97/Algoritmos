---
layout: default
---

# Aula 29b — Segurança defensiva em Python: a fronteira é o programa

**Competências SDD:** SEC, NET, MEM, OS. **Pré-requisitos:** 08, 13, 20–26 e [19a](19a_protocolos_tcp.md). Esta é a ponte do PY9: revisão de ameaças e falhas operacionais integrando o que as aulas introduziram disperso.

**Objetivo observável:** demonstrar dois riscos clássicos de Python com a correção executável ao lado: `pickle` de entrada não confiável (o veneno é gerado e **não** desserializado; a fronteira é JSON com esquema e tetos) e injeção de comando via `subprocess` (corrigida com lista de argumentos, sem shell, padrão validado).

**Contrato e exemplo:** nenhuma entrada externa vira objeto Python sem passar por validação de fronteira (tipo, campos, tamanhos); nenhum comando externo é montado por concatenação de strings. O [arquivo-fonte](exemplos/29b_seguranca.py):

```bash
python3 teoria/python/exemplos/29b_seguranca.py
# pickle hostil gerado (73 bytes) e NAO desserializado
# json com esquema: valido passa, 6 abusos recusados na fronteira
# subprocess sem shell: injecao tratada como texto, contagem correta
# 29b ok: ...
```

**Leitura:** `pickle.loads` em bytes de fora não é "desserializar", é executar: o `__reduce__` do exemplo chamaria `print` na leitura, e com `os.system` no lugar seria comando arbitrário — por isso o veneno desta aula é apenas **gerado e lido como código**, nunca carregado. JSON não é magicamente seguro: sem esquema, ele devolve tipos e tamanhos que o atacante escolhe; a validação da fronteira (objeto, campos permitidos, comprimentos) é o que transforma formato em contrato. No `subprocess`, a diferença entre vulnerável e seguro é uma linha: `shell=True` com string montada interpreta `;`, aspas e `$()`; lista de argumentos entrega cada item como um `argv`, e a tentativa de injeção vira um padrão de busca que não casa com nada. Tipagem e GIL não aparecem nesta aula como controles de segurança, porque não são.

### Por que o veneno é gerado e nunca carregado (a disciplina da demonstração)

O exemplo gera os 73 bytes hostis e os mostra como código — ler o `__reduce__` no fonte prova a execução sem executá-la. Essa é a disciplina de demonstrações de segurança: o artefato do ataque existe (73 bytes contados, conteúdo inspecionado), mas o gatilho nunca é apertado no ambiente de ensino. Quem carrega "só para ver" troca demonstração por incidente: `pickle.loads` não tem modo seguro, sandbox ou confirmação — cada execução é arbitrária por definição do formato. A mesma disciplina vale para a injeção: o padrão malicioso chega ao `argv` como texto e não casa com nada — o teste prova a neutralização contando o resultado correto, nunca executando o comando do atacante "para confirmar". Segurança se demonstra pelo que foi recusado e neutralizado, nunca pelo dano executado.

### Leitura guiada: veneno, fronteira, argv

`pickle hostil gerado (73 bytes) e NAO desserializado` documenta as duas metades da disciplina: o ataque existe (73 bytes) e não foi executado (`NAO`) — a maiúscula é proposital, é o veredito sobre o gatilho. `json com esquema: valido passa, 6 abusos recusados` é a fronteira trabalhando: um válido prova que o contrato não é negação geral, seis recusados provam que cada dimensão (tipo, campo, tamanho) é cobrada. `subprocess sem shell: injecao tratada como texto` é a neutralização contada: a tentativa virou padrão de busca, o resultado veio correto — injeção que não altera o resultado é texto, e texto o programa trata todo dia.

**Erros comuns:** "só desserializo pickle de fonte confiável" sem definir quem garante a fonte; validar depois de construir o objeto; `shell=True` "porque o comando é interno" com um argumento externo dentro; logar o pedido inteiro (segredos e dados pessoais entram no log pela porta da depuração); teto ausente achando que JSON pequeno é garantido; carregar o veneno "só para ver" (demonstração com gatilho é incidente — o artefato se inspeciona, nunca se executa); contar tipos do GIL como controle de segurança (concorrência e tipagem não recusam entrada — fronteira recusa, e fronteira é validação com esquema e teto).

**Exercícios:** [pistas](exercicios/29b_pistas.md) e [soluções](exercicios/29b_solucoes.md): (1) modelo de ameaças de uma página do servidor da 19a (superfície, abuso, limite, resíduo); (2) trocar a validação manual por um esquema com tipos e tamanhos declarados uma única vez, e mostrar um abuso novo que ele pega; (3) auditar um `subprocess` com `shell=True` de uma aula anterior e reescrever sem shell; (4) limites de recurso do serviço da 19a: conexões, tempo e memória por cliente, com o teste de abuso correspondente.

**Checklist:** fronteira única com esquema e tetos; nenhum `pickle`/`eval`/`shell=True` no caminho de entrada; abuso testado e recusado; risco residual escrito.
