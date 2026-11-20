---
layout: default
---

# Aula 00 — Ponte de partida: o que muda vindo de JavaScript

## Nesta aula

Você vai aprender a mapear o que JavaScript ensinou (closures, assincronia, tipagem dinâmica) para o que muda em Python (indentação, GIL, protocolos), e prever quais hábitos JS geram erros em Python.

Você vai ao terminar, você deverá dizer, para cada hábito trazido de JavaScript, o que continua valendo em Python e o que quebra — e executar um programa que demonstra as quatro quebras sem nenhuma falhar.

JavaScript e Python parecem primos — e são, até o dia em que `"1" + 1` dá `"11"` de um lado e `TypeError` do outro. Esta aula traduz os reflexos: igualdade, identidade, referência e entrada externa, antes que o primeiro bug silencioso cobre a aula.

## 1. Contrato antes do código

- **Nomes são etiquetas:** `b = a` não copia; dois nomes passam a apontar o mesmo objeto. Rebinding (`a = [9]`) troca a etiqueta, não o objeto — o outro nome nem percebe.
- **`==` é valor, `is` é identidade:** `[1] == [1]` é verdade, `[1] is [1]` é falso. O `is` só serve para identidade real (`None`, sentinelas); comparar valor com `is` é pedir o interning de presente.
- **Sem coerção implícita:** `"1" + 1` levanta `TypeError`. Conversão é chamada explícita (`int(texto)`), e a chamada falha em texto inválido — trate, não presuma.
- **Refcount é detalhe do CPython:** `sys.getrefcount` existe aqui e pode não existir em outro interpretador. Semântica (o objeto some quando ninguém o alcança) não se escreve contando referência.
- **Entrada externa é texto até prova em contrário:** chega como string, passa por teto de tamanho e validação de formato, e só então vira dado. Sem teto, sem confiança.
- **Fora do contrato:** `async`, classes e o GIL — cada um tem sua aula. Esta ponte é sobre nomes, valores e a porta de entrada.

## 2. Onde o reflexo quebra (e onde continua)

| Reflexo de JavaScript | Em Python |
|---|---|
| `"1" + 1` dá `"11"` | `TypeError`; converta explícito e trate o erro |
| `===` estrito vs `==` solto | `==` já é valor; `is` é identidade, não "mais estrito" |
| `const` protege o objeto | Não há `const`: nome maiúsculo é convenção, mutação é possível |
| GC resolve tudo, sempre igual | CPython usa refcount + coletor de ciclos; outro runtime, outra estratégia |
| `prompt()` devolve texto e "vai" | `input()` também devolve texto — e texto sem teto é ataque |

O que continua: laço, condicional, função, closure e o hábito de testar. O que muda é onde a certeza mora: em JS, na convenção; em Python, no objeto — e na fronteira, onde o texto vira dado.

## 3. Exemplo completo e executável

```python
def classificar_idade(texto: str) -> str | None:
    texto = texto.strip()
    if not texto or len(texto) > 3:
        return None
    if not texto.isdecimal():
        return None
    return "maior" if int(texto) >= 18 else "menor"


def main() -> None:
    # Nomes e objetos: dois nomes, um objeto; rebinding nao e mutacao.
    a = [1, 2]
    b = a
    b.append(3)
    assert a == [1, 2, 3] and a is b
    a = [9]
    assert b == [1, 2, 3] and a is not b
    print("nomes: apelido muta junto, rebinding separa")

    # Igualdade vs identidade: == valor, is objeto.
    assert [1] == [1] and [1] is not [1]
    assert "ok" == "ok"
    print("igualdade: == compara valor, is compara identidade")

    # Sem coercao implicita: "1" + 1 e TypeError, nao "11" nem 2.
    try:
        "1" + 1  # type: ignore[operator]
    except TypeError:
        print("coercao: '1' + 1 levanta TypeError (sem adivinhacao)")
    else:
        raise AssertionError("deveria levantar TypeError")

    # Entrada externa: texto ate prova em contrario, com teto e validacao.
    assert classificar_idade(" 21 ") == "maior"
    assert classificar_idade("17") == "menor"
    assert classificar_idade("") is None
    assert classificar_idade("abc") is None
    assert classificar_idade("1" * 4) is None
    print("entrada: valida passa, vazia/nao numerica/longa recusada")
    print("testes ok")


if __name__ == "__main__":
    main()
```

Execução (só biblioteca padrão):

```bash
python3 00_ponte.py
```

Saída esperada (determinística):

```text
nomes: apelido muta junto, rebinding separa
igualdade: == compara valor, is compara identidade
coercao: '1' + 1 levanta TypeError (sem adivinhacao)
entrada: valida passa, vazia/nao numerica/longa recusada
testes ok
```

Note o que o exemplo *não* faz: não conta referências para decidir nada. `sys.getrefcount` existe no CPython e foi deixado fora do programa de propósito — detalhe de implementação não entra em contrato de aula.

### O que a memória faz aqui?

`b = a` copia 8 bytes de ponteiro, não a lista: dois nomes, um objeto. `a = [9]` aloca objeto novo e move só a etiqueta. `classificar_idade` nunca materializa além de 3 caracteres: teto antes de `int()`, e `isdecimal` antes do teto estourar em `'1' * 1000000` — ordem é a garantia.

## 4. Erros comuns e implicações defensivas

1. **Comparar valor com `is`:** funciona para `None` e pequenos inteiros por interning, quebra no resto. `is` é identidade; valor se compara com `==`.
2. **Somar sem converter:** `"total: " + total` com `total` vindo de fora levanta na hora do relatório. Converta na fronteira, não na formatação.
3. **Confiar no refcount:** escrever `del` achando que liberou "na hora" prende o programa ao CPython. Liberação é do runtime; determinismo, do `with`.
4. **Aceitar texto sem teto:** `int(texto)` em string de 1 MB é negação de serviço com sintaxe válida. Teto de tamanho precede parsing — sempre.
5. **Tratar `strip` como validação:** limpar não é validar. Depois do `strip`, ainda se pergunta: vazio? dígitos? tamanho?

## 5. Ponte com as demais trilhas

Go recusa a coerção na compilação; C soma bytes sem perguntar o tipo; Rust tranca a conversão no `parse` com `Result`. Em JavaScript, a coerção é regra da linguagem; em Python, é exceção levantada. A pergunta é sempre a mesma: *quem decide o que `"1" + 1` significa* — e quando você fica sabendo.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. Luciano Ramalho, *Python Fluente* (Novatec/O'Reilly, ISBN 978-1492056355); 2. Documentação oficial em [docs.python.org/3](https://docs.python.org/3/); 3. *Tutorial oficial* em [docs.python.org/3/tutorial](https://docs.python.org/3/tutorial/).

## 6. Ao final, você consegue

- [ ] Distingo etiqueta de objeto e prevejo quem muda quando o apelido muta.
- [ ] Uso `==` para valor e `is` para identidade, e digo quando cada um mente.
- [ ] Converto texto em número tratando o erro, com teto antes do parsing.
- [ ] Explico por que refcount não entra em decisão de programa.
- [ ] Rejeito entrada vazia, não numérica e longa, cada uma pelo motivo certo.

**Referências primárias:** [Modelo de dados de Python](https://docs.python.org/3/reference/datamodel.html) para nomes/objetos e `==`/`is`; [Comparações em JavaScript (MDN)](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Equality_comparisons_and_sameness) para o contraste honesto. Nenhuma promessa sobre outro interpretador além do CPython testado.

## 6. Exercícios obrigatórios

1. (Previsão) Troque `a = [9]` por `a.append(9)` e preveja `b` antes de rodar. Rode e explique a diferença — quando o apelido sente e quando ele nem percebe.
2. (Diagnóstico) Rode `int("257") is int("257")` e `None is None`: preveja os dois antes de rodar. Explique qual `is` é legítimo e qual pede interning de presente — e por que `257 == 257` continua verdadeiro nos dois mundos.
3. (Projeto) Sem tocar em `classificar_idade`, preveja a saída para `"-5"`, `"018"`, `"1111"` e `" 21 "`, um motivo por recusa ou aceite. Rode e explique — incluindo por que `int("018")` funciona mas `018` no fonte não compilaria.
4. (Previsão) Remova o `len(texto) > 3` e chame `classificar_idade("1" * 1000000)`: preveja hang, `MemoryError` ou outra exceção. Rode e explique com a mensagem — quem impõe o limite real e por que o teto da função continua necessário.

Use [pistas graduais](exercicios/00_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/00_solucoes.md) depois de registrar sua própria explicação.
