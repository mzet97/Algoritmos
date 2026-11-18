---
layout: default
---

# Aula 17 (C++): pistas graduais

Volte aos [exercicios](../17.md#17-exercicios-obrigatorios) antes de abrir as solucoes. (A ponte de concorrencia e a [17a](17a_pistas.md).)

1. A API C-style expoe funcoes livres e tipos opacos; a classe fica atras do `.cpp`.
2. O handle e um ponteiro para tipo incompleto no header C; `create` aloca, `destroy` libera e aceita nulo.
3. Sem `extern "C"`, o nome exportado sai decorado e o cliente C nao encontra o simbolo.
4. `std::string` tem layout e ABI de C++; do outro lado da fronteira ninguem sabe ler seus campos.
5. Excecao nao atravessa a fronteira: capture tudo no `catch (...)` e devolva codigo.
