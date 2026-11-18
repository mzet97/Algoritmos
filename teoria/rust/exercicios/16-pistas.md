---
layout: default
---

# Aula 16 (Rust): pistas graduais

Volte aos [exercicios](../16.md#11-exercícios-obrigatórios) antes de abrir as solucoes.

1. As funcoes `extern "C"` da libm se declaram num bloco `extern`; o wrapper seguro valida antes/depois.
2. Exportar e o caminho inverso: `#[unsafe(no_mangle)] pub extern "C"`, ponteiros opacos e um `destruir` para cada `criar`.
3. Callback C entra como `extern "C" fn` opcional; o sorter copia para ordenar sem tocar o original — declare.
4. Bindgen gera o feio; wrappers seguros sao escritos a mao por cima. Sem bindgen instalado, declare a lacuna.
5. Formato binario com `#[repr(C)]`: valide magic e tamanhos **antes** de qualquer leitura de campo.
6. cxx e um projeto a parte com build proprio — fora do ambiente, documente o desenho.
