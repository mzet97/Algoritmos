---
layout: default
---

# Aula 15a — Auditoria de `unsafe`: cada bloco com invariante escrita

## Nesta aula

Você vai aprender a auditar cada bloco `unsafe` registrando a invariante assumida, classificar `unsafe` justificável vs. irredutível, e apontar o que a biblioteca padrão observa e não observa.

Você vai ao terminar, você deverá escrever, para cada bloco `unsafe`, a invariante que ele assume, o teste que a cobra e o que quebra se ela falhar — e executar os quatro pares desta aula verdes.

"revisei com cuidado" não é auditoria: cuidado não falha em teste, invariante escrita falha. O método desta aula é mecânico de propósito: liste os blocos `unsafe`, escreva a invariante de cada um como comentário `SAFETY`, e escreva o teste que morre se a invariante mentir. Quatro blocos, quatro invariantes, quatro testes — nem um a mais, nem um a menos.

## 1. Contrato antes do código

- **Uma invariante global:** `dados[..tamanho]` inicializados, `dados[tamanho..]` não. Vale em todo ponto observável; cada método a preserva na saída se a assume na entrada.
- **Bloco 1 (`nova`):** array de `MaybeUninit` não exige inicialização; `tamanho = 0` torna a invariante vacuamente verdadeira.
- **Bloco 2 (`push`):** escreve o slot `tamanho` antes de incrementar — e só com `tamanho < N` checado antes.
- **Bloco 3 (`pop`):** decrementa antes de ler; lê `assume_init_read` só onde a invariante garante inicializado.
- **Bloco 4 (`Drop`):** descarta exatamente `dados[..tamanho]` com `assume_init_drop`; o resto não precisa de `drop`.
- **Fora do contrato:** Miri (não instalado nesta máquina — declarado, não contornado), `bindgen`/`cxx` para FFI gerada e prova formal. A auditoria aqui é invariante + teste + `clippy`; Miri entraria como próxima camada, não como substituto.

## 2. Por que o Drop conta com número?

Porque o erro de `Drop` manual é silencioso nos dois sentidos: descartar a menos vaza, descartar a mais é double-drop. Contar `Drop`s com um atômico transforma os dois erros no mesmo sintoma observável — o número diverge de 2. Auditoria sem número é opinião; com número, é teste.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/15a_auditoria.rs) contém o programa (`rustc --edition 2024 -D warnings`, `clippy` limpo):

```bash
rustc --edition 2024 -O -D warnings teoria/rust/exemplos/15a_auditoria.rs -o /tmp/15a_rs
/tmp/15a_rs
```

Saída esperada (determinística):

```text
pilha: vazio, cheio, ordem LIFO e recusa sem perda
drop: exatamente os 2 valores tiveram Drop (pop + descarte)
15a ok: 4 blocos unsafe, 4 invariantes, 4 testes que as cobram
```

Cada linha cobra uma invariante: vazio/cheio/LIFO testam os blocos 1–3; a contagem de `Drop`s testa o bloco 4. Um erro real foi pego pelo compilador no caminho: `Conta` sem `Debug` não compila com `.unwrap()` — a correção deriva o trait em vez de trocar o teste por `match` cerimonial.

## 4. Erros comuns e implicações defensivas

1. **Escrever inicializado e ler antes de assumir:** inverter a ordem no `push` (incrementar antes de escrever) publica lixo como inicializado. Ordem é invariante, não estilo.
2. **Esquecer o `Drop` manual:** `MaybeUninit` não descarta sozinho; sem o bloco 4, `String`s vazam em silêncio — sem relatório, sem erro, só RSS crescendo.
3. **Contar `Drop` sem atômico:** o teste precisa sobreviver a si mesmo; contador comum sob concorrência futura mente. Atômico desde o primeiro dia.
4. **Chamar isto de prova:** invariante + teste não prova ausência de UB — prova presença de disciplina. Miri, quando instalado, é a próxima camada; esta aula é a anterior, não a substituta.
5. **FFI sem dono escrito:** atravessar a fronteira sem dizer quem libera é o bug da [13b de C++](../cpp/13b_ffi_e_ameacas.md) de volta. Todo `unsafe` que toca ponteiro estrangeiro deve nomear o dono no comentário `SAFETY`.

## 5. Ponte com as demais trilhas

C escreve "quem aloca, libera" no cabeçalho; C++ tranca no handle opaco da 13b; Go cobra o `C.free` de quem alocou em C. Em JavaScript, o análogo é o objeto que detém o `ArrayBuffer` transferido — dono único, mesma invariante, outra sintaxe.

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

## 6. Ao final, você consegue

- [ ] Listo os 4 blocos `unsafe` e recito a invariante de cada um sem olhar o código.
- [ ] Aponto o teste que morre para cada invariante quebrada.
- [ ] Explico por que o `Drop` conta com atômico e o que cada divergência significaria.
- [ ] Declaro o que a auditoria não cobre: Miri, FFI gerada e prova formal.
- [ ] Escrevo `SAFETY` em todo `unsafe` novo — sem comentário, sem merge.

**Referências primárias:** [Rustonomicon](https://doc.rust-lang.org/nomicon/) para o modelo de `unsafe`; [`MaybeUninit`](https://doc.rust-lang.org/std/mem/union.MaybeUninit.html) para as garantias exatas de cada método usado. Nenhuma afirmação sobre Miri nesta aula — ferramenta ausente é lacuna, não detalhe.

## 7. Exercícios obrigatórios

1. (Previsão) No `pop`, leia `dados[tamanho]` sem decrementar antes: preveja — silêncio ou pânico? Rode e explique os dois destinos (cheia: índice 2 em len 2, pânico; parcial: lê lixo dentro dos limites, UB silencioso) — e por que o pânico observado é sorte, não segurança.
2. (Diagnóstico) No `push`, apague a checagem de `cheia`: preveja o 3º `push` numa pilha de 2. Rode e explique — o pânico de índice é o backstop *safe* (verificação de limites); o `unsafe` de verdade (a inicialização) nem chegou a rodar. Checagem é contrato, não truque.
3. (Projeto) Adicione `espiar(&self) -> Option<&T>` com `assume_init_ref`: escreva a invariante (só empresta, não move nem invalida), teste vazia/cheia/sem-remover. Rode e explique por que `&T` exige o mesmo cuidado que mover — referência pendurada em `pop`+`push` seria o próximo UB.
4. (Previsão) No `Drop`, descarte `0..N` em vez de `0..tamanho`: preveja a contagem de `Drop`s. Rode e explique o 5 no lugar do 2 — descartar não-inicializado executa `Drop` em lixo, e lixo com `Drop` é double-free em potencial.

Use [pistas graduais](exercicios/15a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/15a_solucoes.md) depois de registrar sua própria explicação.
