---
layout: default
---

# Aula 00: pistas graduais

Volte aos [exercícios](../00_ponte_javascript.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. `append` muda o objeto; `=` muda a etiqueta. A pergunta é onde `b` olha: para o objeto ou para a etiqueta de `a`. (Verificado: com `append`, `b == [1, 2, 3, 9]`; com rebinding, `b == [1, 2, 3]`.)
2. Um dos dois `is` compara identidade real de objeto único; o outro compara dois objetos distintos que valem o mesmo. Só um deles o interning garante. (Verificado: `int("257") is int("257")` dá `False`; `None is None` dá `True`.)
3. Sinal não é dígito (`isdecimal` recusa); zero à esquerda o `int()` aceita (mas o literal `018` nem compila); 4 dígitos batem no teto; espaço nas bordas o `strip` come. (Verificado: `None`, `"maior"`, `None`, `"maior"`.)
4. Sem o teto, o `isdecimal` varre 1 MB (rápido) e o `int()` levanta antes de alocar o gigante: o limite de 4300 dígitos do 3.11+. Leia a mensagem — ela cita dígitos, não memória. (Verificado: `ValueError: Exceeds the limit (4300 digits)`.)
