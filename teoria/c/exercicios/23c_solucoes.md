# Soluções comentadas da aula 23c

## 1. Desempate invertido

Com `(lamport, -autor, seq)`, no empate de Lamport 1 o evento do autor 0 passa a ser o último aplicado: os dois nós convergem para `k=111` e quem é descartado é `k=222`. A convergência não mudou porque ela depende de os dois lados aplicarem os mesmos eventos na mesma ordem total — qualquer ordem total serve; o vencedor é consequência da regra escolhida. É por isso que a regra é contrato publicado: trocar o desempate troca o dado que sobrevive.

## 2. Três nós com um isolado

Vetores de três componentes: A e B trocam entre si (componentes 0 e 1 crescem juntos), C escreve sozinho (componente 2). Enquanto C não encontra ninguém, os eventos dele são concorrentes com tudo que A/B fizeram no mesmo período. Encontros par-a-par em qualquer ordem que conecte os três ao longo do tempo (A↔B, depois B↔C) propagam todos os eventos a todos: B funciona como ponte. Se C nunca reencontrar ninguém, não há anti-entropia que resolva — convergência exige contato eventual, e declará-lo é parte da resposta.

## 3. Chaves diferentes não conflitam

A reconciliação descarta apenas quando dois eventos disputam **a mesma chave**: `aplicar` sobrescreve por chave, então `ka` de A e `kb` de B sobrevivem os dois à sincronização. O log é compartilhado inteiro, mas o conflito é por chave. Se uma implementação descartasse o log "mais curto", estaria jogando fora escritas que ninguém disputou.

## 4. O que falta para consenso (especificação)

Três decisões que esta aula não toma: (1) **eleição** — um mecanismo que escolhe um líder por termo e impede dois líderes simultâneos com quorum de votos; (2) **quorum de escrita** — uma escrita só é confirmada quando a maioria a registrou, de modo que qualquer eleição seguinte a enxerga; (3) **destino do lado minoritário** — quem escreveu fora do quorum tem a escrita rejeitada ou revertida, e o cliente fica sabendo. Sem essas três, "consenso" é só replicação otimista com reconciliação — que é exatamente o que esta aula implementou e nomeou.
