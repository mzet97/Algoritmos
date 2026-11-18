---
layout: default
---

# Aula 25 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../25.md#exercícios-obrigatórios). Executado nesta maquina, que tem GPU AMD acessivel via Vulkan/RADV (verificado com `vulkaninfo`); em maquinas sem GPU, o wgpu cai para o adaptador de software e o exercicio continua valido com essa declaracao. Projeto: `cargo add wgpu pollster`.

Shader com tres buffers (exercicio 1):

```wgsl
@group(0) @binding(0) var<storage, read> a: array<f32>;
@group(0) @binding(1) var<storage, read> b: array<f32>;
@group(0) @binding(2) var<storage, read_write> c: array<f32>;

@compute @workgroup_size(64)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let i = id.x;
    if (i < arrayLength(&a)) {
        c[i] = a[i] + b[i];
    }
}
```

Programa completo (exercicios 1 e 2, com referencia de CPU e comparacao antes dos tempos):

```rust
use std::time::Instant;
use wgpu::util::DeviceExt;

const SHADER: &str = r#"
@group(0) @binding(0) var<storage, read> a: array<f32>;
@group(0) @binding(1) var<storage, read> b: array<f32>;
@group(0) @binding(2) var<storage, read_write> c: array<f32>;

@compute @workgroup_size(64)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let i = id.x;
    if (i < arrayLength(&a)) {
        c[i] = a[i] + b[i];
    }
}
"#;

async fn executar() {
    let n: usize = 1_000_000;
    let a: Vec<f32> = (0..n).map(|i| i as f32 * 0.5).collect();
    let b: Vec<f32> = (0..n).map(|i| (i % 100) as f32).collect();

    // Referencia de CPU: mesmo calculo, conferida elemento a elemento.
    let inicio_cpu = Instant::now();
    let esperado: Vec<f32> = a.iter().zip(&b).map(|(x, y)| x + y).collect();
    let tempo_cpu = inicio_cpu.elapsed();

    let instancia = wgpu::Instance::default();
    let adapter = instancia
        .request_adapter(&wgpu::RequestAdapterOptions::default())
        .await
        .expect("sem adaptador wgpu");
    println!("adaptador: {:?}", adapter.get_info().name);
    let (device, queue) = adapter
        .request_device(&wgpu::DeviceDescriptor::default(), None)
        .await
        .expect("sem device");

    let inicio_gpu_total = Instant::now();
    let buffer_a = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
        label: Some("a"),
        contents: bytemuck::cast_slice(&a),
        usage: wgpu::BufferUsages::STORAGE,
    });
    let buffer_b = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
        label: Some("b"),
        contents: bytemuck::cast_slice(&b),
        usage: wgpu::BufferUsages::STORAGE,
    });
    let buffer_c = device.create_buffer(&wgpu::BufferDescriptor {
        label: Some("c"),
        size: (n * 4) as u64,
        usage: wgpu::BufferUsages::STORAGE | wgpu::BufferUsages::COPY_SRC,
        mapped_at_creation: false,
    });
    let staging = device.create_buffer(&wgpu::BufferDescriptor {
        label: Some("staging"),
        size: (n * 4) as u64,
        usage: wgpu::BufferUsages::MAP_READ | wgpu::BufferUsages::COPY_DST,
        mapped_at_creation: false,
    });

    let modulo = device.create_shader_module(wgpu::ShaderModuleDescriptor {
        label: Some("soma"),
        source: wgpu::ShaderSource::Wgsl(SHADER.into()),
    });
    let pipeline = device.create_compute_pipeline(&wgpu::ComputePipelineDescriptor {
        label: Some("soma"),
        layout: None,
        module: &modulo,
        entry_point: Some("main"),
        compilation_options: Default::default(),
        cache: None,
    });
    let bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: Some("grupo"),
        layout: &pipeline.get_bind_group_layout(0),
        entries: &[
            wgpu::BindGroupEntry { binding: 0, resource: buffer_a.as_entire_binding() },
            wgpu::BindGroupEntry { binding: 1, resource: buffer_b.as_entire_binding() },
            wgpu::BindGroupEntry { binding: 2, resource: buffer_c.as_entire_binding() },
        ],
    });

    let mut encoder = device.create_command_encoder(&wgpu::CommandEncoderDescriptor::default());
    {
        let mut passe = encoder.begin_compute_pass(&wgpu::ComputePassDescriptor::default());
        passe.set_pipeline(&pipeline);
        passe.set_bind_group(0, &bind_group, &[]);
        passe.dispatch_workgroups((n as u32).div_ceil(64), 1, 1);
    }
    encoder.copy_buffer_to_buffer(&buffer_c, 0, &staging, 0, (n * 4) as u64);
    queue.submit(Some(encoder.finish()));
    device.poll(wgpu::Maintain::Wait);

    let fatia = staging.slice(..);
    let (tx, rx) = std::sync::mpsc::channel();
    fatia.map_async(wgpu::MapMode::Read, move |resultado| {
        tx.send(resultado).unwrap();
    });
    device.poll(wgpu::Maintain::Wait);
    rx.recv().unwrap().expect("map falhou");
    let dados = fatia.get_mapped_range();
    let obtido: &[f32] = bytemuck::cast_slice(&dados);
    assert_eq!(&obtido[..], &esperado[..], "GPU diverge da referencia de CPU");
    let tempo_gpu_total = inicio_gpu_total.elapsed();

    println!("cpu: {:?} (referencia)", tempo_cpu);
    println!("gpu total (upload+kernel+download): {:?}", tempo_gpu_total);
    println!("resultados identicos em {} elementos", n);
}

fn main() {
    pollster::block_on(executar());
}
```

(Adicione `bytemuck` as dependencias.)

2. Sobre o perfil real com 100 milhoes: rode o mesmo programa com `n = 100_000_000` em `--release` e anote os dois tempos **nesta ordem de conferencia** — primeiro `assert` de igualdade, depois o relogio. O tempo da GPU aqui inclui upload, kernel e download (o caminho honesto para um calculo que comeca e termina na CPU); medir so o kernel dentro do device e outro numero, tambem legitimo desde que declarado. Em tamanhos pequenos, a CPU vence pelo custo fixo da viagem; o cruzamento depende da maquina — na aula [29a de Python](../../python/29a_hpc_e_modelo_gpu.md) o mesmo fenomeno foi medido no modelo.

**Autoverificacao:** vetor identico ao da referencia antes de qualquer tempo, os tres bindings na ordem do shader e o adaptador usado impresso (GPU real ou software, declarado).
