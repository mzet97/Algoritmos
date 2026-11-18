---
layout: default
---

# Aula 18 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../18.md#13-exercicios-obrigatorios). Exercicios de decisao; a rubrica cobra cenarios nomeados, nao codigo.

1. cgo **faz sentido** quando: (a) a biblioteca C e o padrao do dominio e nao tem equivalente Go maduro (codecs proprietarios, drivers de hardware do fornecedor); (b) voce precisa de uma API do sistema que so existe em C naquele alvo; (c) um legado C enorme e correto seria reescrito com risco maior que a fronteira. **Nao faz sentido** quando: (a) e so para "ser mais rapido" sem medicao (a chamada cgo tem custo proprio e impede otimizacoes); (b) existe biblioteca Go pura adequada (bancos, rede, cripto da std); (c) o ganho e numa funcao fria.

2. Ownership na fronteira e explicito porque ha **dois** gerentes de memoria: o GC do Go nao enxerga mallocs do C, e o C nao sabe que o GC pode mover/recolher objetos Go. As regras praticas: quem aloca em C libera em C (`C.free`); ponteiro Go passado ao C nao pode ser guardado pelo C depois da chamada (o GC pode te-lo movido/recolhido); e strings/buffers atravessam com dono e tempo de vida escritos no contrato da funcao, como na [aula 18 de C](../../c/exercicios/18-solucoes.md).

3. Custos operacionais de depender de compilador C: builds deixam de ser `GOOS=linux go build` em qualquer maquina (cada alvo exige seu gcc cross), o binario pode linkar dinamicamente com a libc do sistema (some o "um arquivo so" do Go), imagens de container crescem e pipelines de CI precisam da toolchain C instalada e versionada. Nada disso e proibitivo — tudo isso entra na conta **antes** da primeira chamada.

4. Antes de cgo num projeto futuro, a pesquisa minima: a necessidade existe na biblioteca padrao? Existe pacote Go puro mantido? O custo e medido ou e intuicao? Se as tres respostas forem "nao, nao, medido", ai sim desenhe a fronteira como na [aula 16 de Rust](../../rust/exercicios/16-solucoes.md): superficie pequena, wrappers seguros, ownership documentado.

5. Go puro compila para qualquer `GOOS/GOARCH` suportado de um unico host, com binario estatico que copia-e-roda. Com integracao nativa via cgo, cada destino pede compilador C daquele alvo e as dependencias dinamicas do sistema dele — portabilidade vira matriz de builds, nao um comando.

**Autoverificacao:** tres cenarios de cada lado com nomes concretos do seu dominio, a regra "quem aloca em C libera em C" e um custo operacional que nao seja "fica mais lento".
