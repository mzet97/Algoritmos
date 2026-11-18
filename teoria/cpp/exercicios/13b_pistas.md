---
layout: default
---

# Aula 13b: pistas graduais

Volte aos [exercícios](../13b_ffi_e_ameacas.md#8-exercícios-obrigatórios) antes de abrir as soluções.

1. `criar(0)` não significa "teto zero": o ternário troca 0 por 1024. Guardar 25 bytes passa. Pense em quem paga quando default silencioso encontra cliente que pediu 0 querendo dizer "nada entra" — a fronteira documenta ou o default vira permissão.
2. Sem a guarda, `static_cast<Armazem*>(nullptr)->ler(...)` acessa `this->itens` no endereço zero: falha de segmentação (exit 139). Dentro de C++ puro, referência nula "não existe"; na fronteira, `void*` nulo chega toda hora. A guarda existe porque FFI não tem o sistema de tipos do outro lado.
3. Espelhe `armazem_ler`: `noexcept`, checa nulo primeiro, `try` em volta, `catch (...)` devolvendo código. O teste tem três pernas: armazém novo (0), após dois `guardar` (2), `nullptr` (−1). Assinatura nova, mesmas regras — fronteira se estende por cópia do padrão, não por invenção.
4. O relatório é `heap-use-after-free`: o `vector` foi liberado pelo `delete`, o handle continua apontando. A fronteira recusa nulo porque nulo é barato de checar; rastrear destruição exigiria registro de handles vivos — estado global, lock, outro protocolo. O contrato divide: a fronteira nega o que é barato negar, o cliente zera o que destruiu.
