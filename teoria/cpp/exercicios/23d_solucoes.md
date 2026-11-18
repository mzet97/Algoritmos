---
layout: default
---

# Aula 23d: soluções comentadas

Compare após tentar os [exercícios](../23d_limites_globais.md#8-exercícios-obrigatórios). Experimentos sobre `teoria/cpp/exemplos/23d_limites.cpp` com `g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: `abertas` vai a **−1** (verificado). `liberar` decrementa sem checar — vaga fantasma que admite além da capacidade real. O abuso real: FIN duplicado, ou timeout que libera uma conexão já fechada pelo par. A guarda (`abertas > 0`, IP conhecido) é o par do `fecha` com guarda da 23f em C: contador que só anda para um lado mente — e mente a favor do atacante.

2. Diagnóstico: **aceito** (verificado: `consumir SIZE_MAX apos 900KiB: ACEITO (wrap)`). `bytes_janela + n` estoura `size_t` e volta para baixo do teto — o teste `soma > teto` perde para a volta. A correção compara sem somar:

```cpp
if (n > max_bytes_entrada - bytes_janela) {
    return false;
}
```

(`bytes_janela <= max` sempre, então a subtração nunca dá a volta.) Com a correção, o gigante cai — e todo pedido acima do teto cai, em qualquer janela. Teto com aritmética que dá a volta é porta aberta com placa de "fechado".

3. Projeto: o quarto teto conta admissões por janela (verificado: 2 entram, a 3ª cai, janela nova reabre — com as fases originais intactas). Detalhe que o exercício revela: o **default precisa preservar o comportamento antigo** (`max_novas_por_ip = max_por_ip`); teto novo que quebra teste velho é regressão, não proteção. O que ele pega: reconexão em rajada do mesmo IP — os três tetos medem ocupação (conexões), volume (bytes) e identidade (por IP); nenhum mede *ritmo de chegada*. Com vagas e abaixo do volume, a rajada passava limpa; agora espera a próxima janela.

4. Previsão: **recusado para sempre** — 2 MiB > 1 MiB em qualquer janela (verificado: o `assert(!consumir(2 << 20))` passa). Teto de janela limita a *soma*, não o pedido: pedido maior que o teto nunca cabe na soma. Carga grande se negocia em pedaços — cada um cobrado na janela — nunca num pedido que o teto jamais admitiria. Protocolo que aceita pedido gigante "para depois fatiar" cobra o teto do fatiador, não do protocolo.
