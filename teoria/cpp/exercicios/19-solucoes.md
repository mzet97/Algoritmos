---
layout: default
---

# Aula 19 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../19.md#19-exercicios-obrigatorios). Exercicios de desenho e politica; a rubrica cobra decisoes escritas, nao codigo novo.

1. Ownership explicito, no formato que voce deve entregar (exemplo com um modulo de cache):

| Recurso | Dono | Quem empresta | Fim de vida |
| --- | --- | --- | --- |
| Buffer de entrada | A funcao que le (`std::vector`) | Visoes `span`/`string_view` durante a chamada | Saida da funcao |
| Socket aceito | `SocketFd` da conexao ([aula 18](18-solucoes.md)) | fd cru para `send`/`recv` | Fim da conexao ou `release()` |
| Entradas do cache | O objeto cache | Referencias constantes | Eviccao ou destruicao do cache |

A coluna que nao pode ficar vazia e "quem destroi": recurso sem dono nomeado e vazamento esperando carga.

2. Politica de erro de referencia para uma biblioteca pequena: erros de programacao (pre-condicao violada) sao `assert`/contrato e nao acontecem em release; erros de ambiente e entrada voltam como `std::expected` na fronteira publica (aula 11); excecoes ficam dentro dos `.cpp` e nunca atravessam a API (aula 17); nada e impresso pela biblioteca. Um mecanismo primario por camada, documentado no header.

3. Interface da CLI: pares e relatorios em `stdout` (consumivel por pipe), progresso e erros em `stderr`, e codigos 0/1/2 para sucesso/falha de execucao/uso errado — o desenho executado na [aula 16](16-solucoes.md) deste livro e na [aula 31 de Python](../../python/31.md).

4. Sinais de API publica grande demais (tres bastam, liste cinco): o cliente precisa montar tres objetos antes da primeira chamada util; ha mais de um jeito obvio de fazer a mesma coisa; funcoes aceitam `bool` posicional ilegivel; tipos internos vazam nas assinaturas; e metade da superficie existe "para o futuro". Cada item vira custo de aprendizado, teste e compatibilidade para sempre.

5. Fluxo minimo de um projeto pequeno, em comandos repetiveis:

```bash
# debug + sanitizers: achar defeitos de memoria e UB
g++ -std=c++20 -g -O1 -fsanitize=address,undefined -o app_debug src.cpp
./app_debug < casos.txt
# release: o artefato que voce mede e entrega
g++ -std=c++20 -O2 -o app src.cpp
./app < casos.txt
```

Mesma entrada nos dois sabores; divergencia de saida e um defeito, nao um "modo". O [regress.sh](../../regress.sh) deste repositorio e esse fluxo institucionalizado.

**Autoverificacao:** tabela de ownership sem dono vazio, politica de erro com um mecanismo por camada e os tres canais da CLI desenhados antes de qualquer flag.
