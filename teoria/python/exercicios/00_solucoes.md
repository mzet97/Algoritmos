---
layout: default
---

# Solucoes comentadas da aula 00

1. Com `a.append(9)`, `b` vira `[1, 2, 3, 9]`: `append` muta o objeto que os dois nomes compartilham. Com `a = [9]`, `b` fica em `[1, 2, 3]`: o `=` move só a etiqueta de `a` para um objeto novo. Apelido sente mutação, não rebinding — e é por isso que parâmetro mutável como default acumula entre chamadas (mesmo objeto, outra aula).
2. `int("257") is int("257")` dá `False`: dois objetos distintos, mesmo valor — `is` recusa porque 257 está fora da faixa de pequenos inteiros do CPython. `None is None` dá `True`: `None` é singleton, identidade real. `257 == 257` é verdadeiro sempre, porque `==` pergunta valor. Comparar valor com `is` funciona até 256 por acidente de implementação (erro nº 1) — e acidente não é contrato.
3. `"-5"` → `None` (sinal não é decimal, `isdecimal` recusa antes de tudo); `"018"` → `"maior"` (`int("018")` vale 18 — zeros à esquerda o `int()` aceita, embora o literal `018` seja `SyntaxError` no fonte: string não é literal); `"1111"` → `None` (teto de 3 antes do parsing); `" 21 "` → `"maior"` (`strip` come as bordas, sobra dígito válido). Cada recusa tem motivo distinto: formato, tamanho, vazio — e a ordem (strip, vazio, teto, dígitos) é a garantia (erro nº 5).
4. Sem o `len(texto) > 3`, `classificar_idade("1" * 1000000)` levanta `ValueError: Exceeds the limit (4300 digits)` — não hang, não `MemoryError`: desde o 3.11, `int()` de texto gigante é recusado por limite de dígitos (CVE de DoS por conversão). O teto da função continua necessário porque o limite do interpretador é 4300, não 3: sem teto, `"9" * 4000` viraria número válido onde só cabem 3 dígitos — validação de formato não se terceiriza para limite de DoS (erro nº 4).
