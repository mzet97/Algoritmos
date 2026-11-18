---
layout: default
---

# Aula 16c — ISA, ABI e layout: o que o hardware promete e o que o `repr` garante (R4)

## Objetivos verificáveis

Ao final, você deverá: distinguir o que a ISA promete do que o `repr` garante, explicar layout e alinhamento, e prever quebra de ABI por mudança de layout.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir ISA, ABI, layout e alinhamento`;
- **Nível 2 — Aplicar:** `diagnosticar suposição de layout não garantida e corrigir com `repr(C)``;
- **Nível 3 — Projetar:** `projetar uma struct FFI justificando cada atributo de representação`.

**Competências SDD:** ARQ, MEM, SEC. **Pré-requisitos:** aulas [16a](16a_maquina_e_so.md) (espaço de usuário), [16b](16b_maquina_e_cache.md) (localidade) e [15a](15a_auditoria_unsafe.md) (auditoria de `unsafe`). Toolchain registrada: `rustc` 1.99.1 (stable).

**Objetivo observável:** ao terminar, você deverá executar o exemplo com `rustc --edition 2024 -D warnings`, mostrar os deslocamentos contratuais do `repr(C)`, o roundtrip de endianness e a chamada `extern "C"` por nome e por ponteiro — e explicar qual dessas três provas quebra primeiro num port entre ISAs.

**Motivação:** o R4 pedia ISA, ABI e cache sobre a base da 16a; a 16b mediu cache. Faltavam as outras duas letras: que instruções e registradores existem (ISA) e como os dois lados da fronteira concordam sobre tamanho, ordem e chamada (ABI). Esta aula fixa as três promessas em código que falha em compilação ou em `assert` quando a promessa é falsa — não em prosa.

## Contrato e exemplo

O [arquivo-fonte](exemplos/16c_isa_abi.rs) contém o programa completo (sem dependências externas):

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/16c_isa_abi.rs -o isa16c
./isa16c
# arch=x86_64
# layout repr(C): tam=16 alin=8 offs=(0,4,8)
# read_unaligned ok: 0x04030201
# endianness nativa ok: 0x01020304
# extern C ok: soma_c(40,2)=42 por nome e por ponteiro
# 16c ok: ISA nomeada, layout contratado, ABI atravessada
```

## 1. Três promessas, três provas

**ISA nomeada.** `std::env::consts::ARCH` diz onde o binário está rodando, e o `assert!` lista as ISAs que este livro cobre (x86_64, aarch64, riscv64). Parece trivial até o dia em que o binário roda emulado: a ISA do processo não é a ISA do silício, e experimento de desempenho que ignora isso mede o emulador.

**Layout contratado.** `repr(C)` promete ordem, deslocamentos e tamanho: `offset_of!` mais `size_of`/`align_of` transformam a promessa em `assert`. O `repr(Rust)` padrão não promete nada disso — e é por isso que o `TrapFrame` do kernel (aula 28a) é `repr(C)`: o assembly lê campo por deslocamento, e deslocamento só existe com ordem contratada. O `read_unaligned` mostra a outra ponta: hardware real entrega bytes desalinhados, e a API segura existe para não invocar UB ao lê-los.

### Lendo `tam=16 alin=8 offs=(0,4,8)` como contrato executável

Desmonte a linha: três campos (`u32`, `u32`, `u64` — 4+4+8 = 16 bytes de dado) ocupam 16 bytes totais porque o `u64` exige alinhamento 8 e cai no deslocamento 8 sem padding (4+4 = 8, múltiplo de 8 por sorte do layout, não por acaso do exemplo); `alin=8` é o máximo dos alinhamentos; `tam=16` é múltiplo de 8, então arrays do struct não quebram o alinhamento do elemento seguinte. Cada número tem um bug que ele proíbe: `offs` proíbe reordenação silenciosa (o `repr(Rust)` reordenaria os `u32` para junto do `u64` e mudaria o 8); `alin` proíbe leitura direta desalinhada (o assembly que lê o campo 2 no deslocamento 8 assume endereço múltiplo de 8); `tam` proíbe cauda sem padding (array com `tam` não-múltiplo desalinharia o segundo elemento). O `assert` cobra os três porque a fronteira (assembly, FFI, disco) lê os três — promessa que ninguém lê é comentário, e comentário não segura layout.

**ABI atravessada.** `extern "C" fn` com `#[unsafe(no_mangle)]` (a edição 2024 exige declarar que o nome é contrato manual — o mesmo recibo que a 28a pagou) é chamada pelo nome e por ponteiro de função. É a mesma convenção que o `trap.S` usa para chamar `trap_handler`: quem atravessa, atravessa igual em kernel e em espaço de usuário.

## 2. O que quebra no port (e a ordem em que quebra)

Primeiro o layout: `repr(C)` continua válido, mas tamanhos de tipos C (`long`, ponteiro) mudam — o `assert` de `size_of == 16` sobrevive em LP64 e morre onde `long` é 32 bits. Depois a endianness: o roundtrip `to_ne_bytes`/`from_ne_bytes` sobrevive sempre (ele é nativo por definição), mas qualquer byte gravado em disco por outra máquina deixa de ler igual — persistência exige `to_le/to_be` explícitos, nunca `ne`. Por último a ABI: o nome continua, mas a convenção de chamada (`extern "C"` em RISC-V vs x86-64) muda como argumentos viajam em registradores — e é por isso que o kernel relinka por alvo em vez de fingir portabilidade.

### Leitura guiada: seis linhas, três fronteiras

`arch=x86_64` ancora o relatório: tudo abaixo vale nesta ISA, e portar sem renomear a âncora é comparar incomparáveis (erro nº 4). A linha do layout é o contrato executável da subseção acima — cada número um `assert`, cada `assert` um bug proibido. As duas linhas de endianness distinguem memória de persistência: `read_unaligned` prova leitura segura de bytes alheios (o `0x04030201` atravessou deslocamento arbitrário sem UB), `to_ne_bytes` prova roundtrip no próprio processo — e nenhuma das duas prova leitura de disco alheio, que exigiria `le`/`be` explícitos. A linha `extern C ok` prova a travessia duas vezes (nome e ponteiro, `40+2=42`): por nome cobra o `no_mangle`; por ponteiro cobra a convenção de chamada com endereço indireto — duas formas de atravessar, dois `assert`s, porque nome certo com convenção errada chama o lugar certo do jeito errado.

## 3. Erros comuns e implicações defensivas

1. **Confiar no layout de `repr(Rust)`:** o compilador pode reordenar para economizar padding; código que lê campo por deslocamento num `repr(Rust)` é UB que hoje funciona. Fronteira com assembly ou FFI exige `repr(C)` — sem exceção, sem "só desta vez".
2. **Persistir com `to_ne_bytes`:** arquivo escrito numa máquina little-endian lê errado numa big-endian. Formato em disco usa endianness fixa declarada; `ne` é só para memória do próprio processo.
3. **`#[no_mangle]` sem `unsafe(...)`:** na edição 2024, não compila — e a mensagem ensina o ponto: exportar nome é ato inseguro porque cria contrato invisível ao borrow checker.
4. **Medir desempenho sem nomear a ISA:** benchmark emulado comparado com nativo não é comparação, é ficção. A primeira linha da saída (`arch=`) existe para o relatório citar.
5. **Testar a travessia só por nome:** `no_mangle` certo com convenção errada compila, linka e corrompe a pilha na chamada — o teste por ponteiro existe porque endereço indireto é outro caminho pelo mesmo contrato. Cobrir um e presumir o outro é cobrir metade da fronteira.
6. **Confundir roundtrip nativo com formato:** `to_ne_bytes` lido na mesma máquina sempre funciona — e é por isso que ele não prova nada sobre disco ou rede. Formato se testa com máquina de endianness oposta ou com bytes gravados à mão; roundtrip próprio é teste de API, não de interoperabilidade.

## 4. Ponte com as demais trilhas

C declara o mesmo layout com `offsetof` e `_Static_assert` na aula de ABI; C++ atravessa a mesma fronteira com `extern "C"` e `noexcept` na [13b](../cpp/13b_ffi_e_ameacas.md); Go esconde a ABI atrás do `cgo` (com custo documentado). Em Rust, o `repr(C)` é o idioma comum das três fronteiras — kernel, FFI e formato binário. A 28a mostra o preço dele onde dói mais (tabela Sv39); esta aula mostra o contrato onde dói menos.

## 5. Critério de aprovação e checklist

- [ ] Rodo o exemplo e aponto os três `assert` que virariam erro em port errado.
- [ ] Explico por que o `TrapFrame` do kernel precisa de `repr(C)`.
- [ ] Digo o que `read_unaligned` evita (UB de acesso desalinhado) e quando usá-lo.
- [ ] Descrevo o que quebra primeiro num port x86_64 → riscv64 (layout, depois ABI).
- [ ] Nomeio o que falta: interrupção e escalonamento em espaço de usuário (R4 segue com o kernel da 28a).

**Referências primárias:** *The Rust Reference*, capítulos de *Type Layout* (`repr(C)`) e *Unsafety* (`#[unsafe(no_mangle)]` na edição 2024); SysV ABI do alvo para a convenção `extern "C"`. Nenhum texto foi copiado — os deslocamentos acima foram confirmados executando este exemplo.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que compila.

**Para se aprofundar:**

1. *The Rustonomicon* ([doc.rust-lang.org/nomicon](https://doc.rust-lang.org/nomicon)); 2. Documentação do Tokio ([tokio.rs](https://tokio.rs)); 3. *The Rust Performance Book* ([nnethercote.github.io/perf-book](https://nnethercote.github.io/perf-book)).
