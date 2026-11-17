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

**Erros comuns:** "só desserializo pickle de fonte confiável" sem definir quem garante a fonte; validar depois de construir o objeto; `shell=True` "porque o comando é interno" com um argumento externo dentro; logar o pedido inteiro (segredos e dados pessoais entram no log pela porta da depuração); teto ausente achando que JSON pequeno é garantido.

**Exercícios:** [pistas](exercicios/29b_pistas.md) e [soluções](exercicios/29b_solucoes.md): (1) modelo de ameaças de uma página do servidor da 19a (superfície, abuso, limite, resíduo); (2) trocar a validação manual por um esquema com tipos e tamanhos declarados uma única vez, e mostrar um abuso novo que ele pega; (3) auditar um `subprocess` com `shell=True` de uma aula anterior e reescrever sem shell; (4) limites de recurso do serviço da 19a: conexões, tempo e memória por cliente, com o teste de abuso correspondente.

**Checklist:** fronteira única com esquema e tetos; nenhum `pickle`/`eval`/`shell=True` no caminho de entrada; abuso testado e recusado; risco residual escrito.
