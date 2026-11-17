---
layout: default
---

# Especificação SDD da masterclass

**Estado:** contrato editorial v1. Este documento orienta a revisão; não declara os livros concluídos.

## 1. Público e resultado esperado

O estudante conhece variáveis, condicionais, laços e funções em Python ou JavaScript, mas pode nunca ter compilado um programa ou estudado arquitetura de computadores. Cada livro deve fornecer, na própria linguagem, os pré-requisitos para avançar até engenharia de sistemas. O objetivo é demonstrar competências por meio de explicações, programas, depuração, medições e projetos, sem sugerir equivalência a um diploma universitário.

Ao concluir um livro, o estudante deverá conseguir:

| ID | Competência observável | Evidência mínima |
| --- | --- | --- |
| FND | Explicar compilação ou interpretação, representação de dados, escopo e fluxo de execução | Programa pequeno executado e explicado sem copiar o exemplo |
| MAT | Usar lógica, relações, invariantes, indução básica e matemática necessária para analisar programas | Demonstração de correção de um algoritmo e análise de casos-limite |
| ALG | Escolher estruturas de dados e justificar correção e complexidade | Implementação e comparação medida de duas soluções |
| ARQ | Relacionar instruções, cache, memória virtual, processos e chamadas de sistema ao comportamento do programa | Experimento reproduzível e interpretação do resultado |
| MEM | Explicar tempo de vida, ownership, alocação e limites de acesso conforme a linguagem | Programa que detecta ou evita uma falha de memória e documenta o mecanismo |
| CON | Distinguir concorrência de paralelismo e preservar invariantes sob múltiplas CPUs | Teste concorrente, diagnóstico de corrida e correção justificada |
| NET | Implementar protocolo com framing, leitura parcial, limites e tratamento de falhas | Cliente e servidor que sobrevivem a entrada fragmentada e malformada |
| DST | Explicar falhas parciais, replicação, consistência e idempotência | Serviço distribuído testado com atraso, queda e repetição de mensagens |
| OS | Explicar isolamento, interrupções, syscalls, escalonamento e memória virtual | Laboratório de kernel em C, C++ ou Rust; ferramenta e experimento de espaço de usuário em Go ou Python |
| HPC | Medir e melhorar uso de CPU, cache, memória e paralelismo sem sacrificar correção | Benchmark reproduzível com hipótese, medição e interpretação |
| GPU | Explicar modelo de execução, transferências, memória e limites de aceleração | Implementação de referência e experimento comparativo; hardware real opcional |
| SEC | Identificar fronteiras de confiança, ameaças e falhas de segurança defensiva | Modelo de ameaças, teste de falha e correção no projeto final |

Essas competências são comuns aos cinco livros, mas a implementação e a profundidade de cada mecanismo dependem da linguagem. Go e Python não precisam fingir ser linguagens de kernel para ensinar o funcionamento de um sistema operacional.

## 2. Progressão obrigatória em cada livro

1. **Entrada:** ambiente, primeiro programa, sintaxe essencial e ponte explícita com Python e JavaScript. Definir cada termo antes do primeiro uso.
2. **Raciocínio:** matemática discreta aplicada, invariantes, testes, estruturas de dados, algoritmos, complexidade e representação de dados.
3. **Máquina e sistema:** compilação/runtime, arquitetura, hierarquia de memória, processos, syscalls, I/O, rede e memória virtual.
4. **Concorrência e distribuição:** sincronização, modelos de memória, falhas parciais, protocolos, consistência, observabilidade e operação.
5. **Especialização:** profiling, desempenho, HPC, GPU, interoperabilidade e projetos. Conteúdo que exige hardware especial ou ferramenta experimental é rotulado como laboratório opcional, com uma base estável obrigatória.

Os atuais números de aula são um ponto de partida, não um limite de capítulos. A revisão pode inserir aulas de ponte antes de tópicos avançados, mantendo redirecionamentos nos índices para não quebrar links existentes.

## 3. Contrato de módulo e de aula

Cada módulo deve declarar: competências por ID; conhecimentos anteriores; conceitos; práticas de memória, segurança e desempenho; exercícios; projeto ou experimento; evidência de aprovação; e fonte técnica primária para afirmações sensíveis a versão.

Cada aula deve conter, nesta ordem lógica:

1. objetivo observável e pré-requisitos;
2. motivação e problema concreto;
3. modelo mental com limites explícitos da simplificação;
4. explicação progressiva e exemplos completos quando chamados de completos;
5. comandos para executar e resultado esperado, com plataforma e versão testadas;
6. estado da memória e custos relevantes, quando aplicável;
7. falhas comuns, entrada não confiável e implicações defensivas;
8. comparação curta com Python/JavaScript no início e com as demais linguagens quando for instrutiva;
9. exercícios sem respostas na aula, pistas e soluções comentadas em arquivos separados;
10. checklist verificável e referências primárias.

Fragmentos que não compilam isoladamente devem ser rotulados **trecho conceitual**. Código intencionalmente inseguro deve ser rotulado **exemplo de falha**, explicar o risco e vir acompanhado de versão corrigida. Nunca apresentar código de demonstração como pronto para produção sem limites, testes e ameaça descritos.

## 4. Avaliação e rastreabilidade

Cada competência deve apontar para pelo menos uma aula, um exercício e uma avaliação prática. O índice de cada livro publicará essa matriz. Exercícios terão três camadas separadas: enunciado, pistas graduais e solução comentada. A rubrica de projeto atribuirá evidência para **correção**, **modelo conceitual**, **testes de falha**, **segurança**, **medição** e **comunicação das limitações**. Resolver somente o caminho feliz não aprova um projeto de sistemas.

A [rubrica SDD](RUBRICA_SDD.md) fixa níveis de evidência e o gate para aula, módulo, projeto e livro. Nenhum status `verificado` é inferido de contagem de palavras, presença de arquivos ou execução de apenas um exemplo.

O estudo autônomo precisa ser reproduzível: o leitor deve conseguir seguir as instruções em ambiente limpo, sem depender de arquivos locais não publicados, serviços pagos ou hardware especializado no núcleo obrigatório. O texto deve distinguir claramente Linux, Windows, macOS, WSL e emulação quando o comportamento variar.

## 5. Revisão técnica e política de ferramentas

| Gravidade | Definição | Bloqueia publicação? |
| --- | --- | --- |
| P0 | Exemplo perigoso sem aviso, afirmação falsa que induz erro grave, ou projeto não reproduzível no caminho obrigatório | Sim |
| P1 | Erro técnico, lacuna de pré-requisito, exercício sem critério de aprovação, ou exemplo completo que não executa | Sim |
| P2 | Comparação imprecisa, explicação insuficiente, portabilidade incompleta ou referência ausente | Não, mas entra na fila da próxima revisão |
| P3 | Estilo, formatação ou melhoria opcional | Não |

Usar toolchains estáveis no núcleo; registrar versões exatas verificadas por lançamento. Recursos nightly/experimentais e GPU física ficam em laboratórios opcionais. C/C++ usam warnings e sanitizers quando disponíveis; Go usa testes, race detector e fuzzing; Rust usa `cargo test`, `clippy` e ferramentas adicionais compatíveis; Python usa testes e checagem estática adequados ao capítulo. Nenhuma ferramenta substitui revisão humana dos modelos mentais.

Segurança defensiva é transversal: entrada hostil, parser e limites, memória, concorrência, exaustão de recursos, dependências, configuração, privilégios, logs sem vazamento de dados e recuperação de falhas. Os projetos finais incluem modelo de ameaças e testes de abuso controlado. Usar [NIST SSDF](https://csrc.nist.gov/pubs/sp/800/218/final) como referência de processo, sem transformar o livro em checklist de conformidade.

## 6. Projetos de conclusão e publicação

Em C, C++ e Rust, o percurso de kernel começa em RISC-V/QEMU e termina com comparação ou portabilidade de componentes em x86-64. A ordem interna ensina boot, exceções, gerenciamento de memória, syscalls, escalonamento, sincronização e I/O antes de multicore. Go e Python concluem com serviços e ferramentas distribuídas, observáveis e robustas, incluindo interação com recursos do SO. Todos os livros incluem experimentos de HPC/GPU, com implementação de referência acessível sem GPU dedicada.

Markdown é a fonte. O site é uma visualização navegável dos livros, não uma segunda cópia editável. A licença [MIT](../LICENSE) cobre o material original; fontes externas são citadas e não copiadas extensivamente. Publicação ocorre por volume após zerar P0/P1, verificar links, executar exemplos classificados como completos e percorrer um projeto do começo ao fim em ambiente limpo.

## 7. Ordem de entrega

1. Inventário e auditoria de todos os arquivos existentes; correção imediata dos P0 encontrados.
2. Piloto de C nas aulas 01, 04, 07, 14, 23 e 31; ajustar este contrato com as descobertas.
3. Livro de C completo; depois C++, Rust, Go e Python, cada um com sua matriz de competências e aceites.
4. Site navegável inicialmente com C; acrescentar os demais volumes após o respectivo gate editorial.

As referências acadêmicas de [sistemas operacionais](https://pdos.csail.mit.edu/6.1810/2025/overview.html), [sistemas distribuídos](https://pdos.csail.mit.edu/6.824/) e [desempenho](https://ocw.mit.edu/courses/6-172-performance-engineering-of-software-systems-fall-2018/) orientam o nível de laboratórios e avaliação, não são texto a reproduzir.
