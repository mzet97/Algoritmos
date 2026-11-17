# Sistema visual do site da masterclass

## Direção

Biblioteca técnica aberta: a página serve para escolher um livro e entender seu estado editorial. A composição usa uma faixa escura de entrada, papel claro para leitura e acento verde para ações. O texto dos livros permanece nos arquivos Markdown de `teoria/`.

## Referências e limites

- Referências consultadas: `taste-skill.md` (clareza editorial) e `mintlify.md` (legibilidade de documentação). Notion e Mintlify foram consideradas; Mintlify se aproxima mais de uma biblioteca técnica.
- Pesquisa de telas e imagem gerada não entraram: a experiência solicitada é um índice de documentação, sem mídia de produto ou imagem necessária à compreensão.
- Não há fonte, script ou biblioteca externa. Fontes do sistema garantem leitura mesmo sem rede.

## Tokens

| Papel | Valor | Uso |
| --- | --- | --- |
| Papel | `#f7f7f2` | fundo de leitura |
| Branco | `#ffffff` | superfícies de conteúdo |
| Tinta | `#122422` | texto e fundo de destaque |
| Texto secundário | `#43514f` | descrições |
| Linha | `#d6ddda` | separação |
| Verde | `#b8f36b` | ação sobre tinta escura |
| Verde profundo | `#176849` | links e foco sobre papel |

Escala de espaço: 4, 8, 12, 16, 24, 32, 48 e 64 px. Corpo: 16 px com altura de linha 1,65. Largura de leitura: até 72 caracteres; contêiner: até 1120 px. Tipografia: `system-ui` no texto e `ui-monospace` no código. Raios de 8 e 16 px.

## Elementos e estados

- Cabeçalho reutilizado pelo layout, com marca textual e links diretos. No celular os links quebram para a próxima linha, sem menu oculto.
- Botões e links têm estados de hover e foco visíveis; nenhum elemento decorativo se parece com ação.
- Cartão de C indica o piloto e liga ao índice local. Os quatro outros livros indicam revisão sem links prematuros. Estado editorial é texto, não apenas cor.
- Conteúdo Markdown usa a mesma escala de leitura, com tabelas roláveis e blocos de código que preservam a largura.

## Acessibilidade e desempenho

HTML semântico, link para pular a navegação, idioma `pt-BR`, rótulos descritivos, contraste alto e largura responsiva. Não há animações essenciais nem JavaScript; o CSS respeita movimento reduzido. O site só depende dos recursos estáticos do próprio repositório.
