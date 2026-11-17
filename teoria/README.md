---
layout: default
---

# Masterclass aberta de programação de sistemas

Este diretório reúne cinco livros independentes, em português brasileiro. Cada livro parte de programação básica e desenvolve os conceitos necessários para estudar sistemas, concorrência, redes, desempenho e segurança defensiva na sua linguagem. Conhecer outro livro não é pré-requisito.

| Livro | Ponto de entrada | Estado editorial |
| --- | --- | --- |
| C | [Índice](c/indice.md) | Piloto SDD em revisão |
| C++ | [Índice](cpp/indice.md) | Auditoria em andamento |
| Rust | [Índice](rust/indice.md) | Auditoria técnica em andamento |
| Go | [Índice](go/indice.md) | Auditoria em andamento |
| Python | [Índice](python/indice.md) | Auditoria em andamento |

O [contrato SDD](ESPECIFICACAO_SDD.md) define competências, padrões de aula, evidências e critérios de publicação. A [auditoria editorial](AUDITORIA.md) registra o que já existe e o que ainda precisa de revisão. Os índices acima mostram a ordem atual das aulas; nenhuma trilha deve ser tratada como livro concluído apenas por ter todos os arquivos numerados.

A [rubrica de avaliação](RUBRICA_SDD.md) explica o que precisa ser demonstrado para concluir um exercício, módulo ou projeto.

## Como estudar

1. Escolha uma linguagem e comece pela aula 1 do respectivo índice.
2. Execute os exemplos, responda aos exercícios sem consultar as soluções e explique os modelos mentais com suas palavras.
3. Faça os projetos e registre testes, medições e limitações. Avance quando conseguir produzir as evidências descritas no contrato da aula.

Os laboratórios de kernel usarão primeiro RISC-V em QEMU e depois compararão x86-64. C, C++ e Rust terão projetos de núcleo. Go e Python aprofundarão ferramentas de sistema e sistemas distribuídos. Recursos que exigem hardware específico ou toolchain experimental serão identificados como opcionais.

O material é gratuito e original, sob a [licença MIT](../LICENSE). Referências externas são fontes para estudo e verificação técnica; não substituem a explicação e os exercícios destes livros.
