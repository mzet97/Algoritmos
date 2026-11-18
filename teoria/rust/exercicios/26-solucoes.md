---
layout: default
---

# Aula 26 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../26.md#exercícios-obrigatórios-teóricos). Exercicios teoricos: a rubrica cobra o desenho e a justificativa, nao um binario.

1. Layout com `#[repr(C)]` e alinhamento de 16 bytes, conferido por programa:

```rust
#[repr(C)]
#[derive(Debug)]
struct Vec3 {
    x: f32,
    y: f32,
    z: f32,
    _preenchimento: f32, // completa o bloco de 16 bytes
}

#[repr(C)]
#[derive(Debug)]
struct Material {
    cor: Vec3,      // 16 bytes (com o preenchimento acima)
    intensidade: f32,
    ativo: u32,
    _reserva: [f32; 2], // fecha o segundo bloco de 16, sem buracos no meio
}

fn main() {
    println!("Vec3: tamanho={} alinhamento={}",
             std::mem::size_of::<Vec3>(), std::mem::align_of::<Vec3>());
    println!("Material: tamanho={} alinhamento={}",
             std::mem::size_of::<Material>(), std::mem::align_of::<Material>());
    let m = Material {
        cor: Vec3 { x: 1.0, y: 0.5, z: 0.25, _preenchimento: 0.0 },
        intensidade: 2.0,
        ativo: 1,
        _reserva: [0.0; 2],
    };
    println!("soma simbolica: {}", m.cor.x + m.intensidade + m.ativo as f32);
}
```

`Vec3` com 3 floats tem tamanho 12 mas alinhamento 4 em Rust comum; nos layouts de uniform/storage de GPU (std140/std430 e as regras do WGSL), vetores de 3 componentes sao tratados em blocos de 16 bytes. Declarar o quarto float — como preenchimento ou como um `w` com significado — torna o layout explicito e igual dos dois lados da fronteira, em vez de depender de como cada compilador preenche o buraco. `Material` fecha em 32 bytes: dois blocos de 16, nenhum desperdicio alem dos 8 bytes declarados no fim (que voce pode transformar em parametros futuros sem mudar o tamanho).

2. Decisao para o backend do radar (modelo de resposta): com uma equipe pequena e algoritmo dominado por varreduras regulares sobre grades grandes, a escolha defensavel e **wgpu/WGSL** (aula 25): uma base de codigo so, roda nas GPUs disponiveis e depura com as mesmas ferramentas do resto do sistema. SIMD na CPU (aula 24) e o plano B correto se o volume nao pagar a viagem de ida e volta dos dados — e a referencia obrigatoria de qualquer forma. rust-gpu (aula 26) e a opcao de maior custo de adocao hoje: ecossistema mais jovem, shaders em Rust de verdade, mas depuracao e contratacao mais dificeis; so se justifica se a equipe ja e Rust-first e o kernel em WGSL virou o gargalo de manutencao. O que reprova a resposta: escolher sem citar volume de dados, forma do algoritmo e custo de equipe.

**Autoverificacao:** tamanhos multiplos de 16 impressos pelo programa e a justificativa nomeando as tres opcoes com um motivo de descarte para cada uma das duas perdedoras.
