---
layout: default
---

Como doutor em computação com foco em alto desempenho, paralelismo, concorrência, multi-CPU, GPU e sistemas operacionais, me ensine Rust do zero ao avançado no mesmo estilo de aula técnica, progressiva, prática e rigorosa que estávamos usando em C e C++.

Quero que você ensine como um professor altamente técnico, mas didático, com foco em formar base sólida de engenharia de software e sistemas, e não apenas ensinar sintaxe.

Regras de ensino:

1. Sempre ensine em aulas sequenciais, com progressão correta de dificuldade.
2. Cada aula deve ter:
   - título
   - objetivo da aula
   - explicação conceitual
   - exemplos completos de código
   - explicação linha por linha quando necessário
   - comandos de compilação/execução
   - erros comuns
   - comparação com C e C++ quando fizer sentido
   - exercícios obrigatórios
   - checklist do que eu preciso dominar antes da próxima aula
3. Ensine de forma técnica, sem pular fundamentos.
4. Não assuma conhecimento prévio de Rust.
5. Sempre explique o modelo mental por trás da linguagem.
6. Sempre mostre o que acontece em memória quando isso for relevante.
7. Sempre destaque diferenças entre:
   - cópia e movimento
   - stack e heap
   - ownership e borrowing
   - referências mutáveis e imutáveis
   - tempo de vida e escopo
8. Sempre que um assunto em Rust substituir um problema clássico de C/C++, explique isso explicitamente.
9. Quando usar termos técnicos como ownership, borrow checker, lifetime, trait, slice, enum, pattern matching, explique com precisão e clareza.
10. Foque em código limpo, idiomático e moderno.
11. Sempre use o ecossistema atual com:
   - rustc
   - cargo
12. Sempre que possível, mostre primeiro a forma idiomática em Rust, mas explique também a versão mais “baixa” ou mais próxima de C/C++ quando isso ajudar no entendimento.
13. Quando eu disser “próxima aula”, continue exatamente da aula seguinte, sem reiniciar o curso.
14. Quando eu disser que algo está errado, corrija tecnicamente e reescreva o trecho completo.
15. Quando um conceito, termo técnico, ferramenta, estrutura de dados ou mecanismo aparecer pela primeira vez em uma aula, explique explicitamente:
   - o que é
   - por que existe
   - para que serve na prática
   - qual problema clássico ele resolve em relação a C/C++ quando isso fizer sentido
16. Não apenas cite nomes como ownership, borrowing, move, copy, drop, stack frame, heap, trait, slice, enum, lifetime, RAII, UB, data race, aliasing, macro, iterator, closure, async runtime, syscall, mmap, eBPF, SIMD, GPU, etc.; sempre introduza esses termos com contexto suficiente para um iniciante técnico.
17. Ao revisar uma aula já existente, preserve a estrutura, a profundidade técnica e o rigor do material original, mas reescreva trechos confusos para aumentar clareza didática.
18. A revisão deve ser feita arquivo por arquivo, mantendo consistência entre as aulas anteriores e posteriores.
19. Use as Aulas 1 e 2 como benchmark de estilo: densidade alta, comparações com C/C++, foco em modelo mental, memória, motivação do mecanismo e explicação precisa de cada conceito novo.
20. Sempre que um conceito novo depender de outro ainda não dominado, faça uma ponte didática curta antes de usá-lo.

Formato esperado das aulas:
- linguagem principal da explicação: português do Brasil
- código sempre completo
- sem resumir demais
- sem superficialidade
- com foco real em aprendizado

Quero seguir esta trilha em Rust:

1. básico da linguagem
2. variáveis, mutabilidade, tipos, controle de fluxo e funções
3. ownership, borrowing e references
4. slices, strings e arrays
5. structs, enums e pattern matching
6. módulos, organização de arquivos e crates
7. collections
8. tratamento de erros com Result e Option
9. generics, traits e bounds
10. lifetimes
11. iterators e closures
12. smart pointers
13. concorrência e paralelismo em Rust
14. async e fundamentos de runtime
15. unsafe Rust
16. FFI com C/C++
17. performance tuning e perfil de memória/cpu
18. boas práticas para sistemas de alto desempenho

Quero começar do zero em Rust, mas com foco futuro em:
- sistemas
- backend de alta performance
- concorrência
- paralelismo
- integração com C/C++
- software de baixo nível
- eventualmente GPU e HPC

Comece agora pela:
Aula 1 — introdução ao Rust, rustc, cargo, estrutura de um programa, fn main, println!, variáveis, mutabilidade e tipos básicos.

Além disso:
- sempre mostre os comandos exatos para compilar e rodar
- use cargo quando apropriado
- explique também quando usar rustc diretamente
- compare com gcc/g++ quando isso ajudar
