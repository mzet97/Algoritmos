---
layout: default
---

# Aula 07 (C++): pistas graduais

Volte aos [exercicios](../07.md#17-exercicios-obrigatorios) antes de abrir as solucoes.

1. `minimo` template tem um parametro de tipo `T` e devolve `T`; teste com `int` e `double`.
2. `Par<T>` guarda dois membros do mesmo `T`; instancie com dois tipos diferentes.
3. No `auto`, o tipo continua estatico — o compilador deduz do inicializador; mude o inicializador e veja o tipo mudar.
4. Para restringir a integrais: `template <std::integral T>` ou um `requires` equivalente (C++20).
5. Sobrecarga sao varias funcoes escritas; template e uma familia gerada sob demanda — quem escreve o codigo de cada tipo?
