---
layout: default
---

# Aula 15 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../15.md#15-exercícios-obrigatórios). **Ambiente:** o Miri nao esta instalado nesta maquina (`cargo miri` indisponivel na toolchain estavel), entao os testes abaixo rodaram em execucao normal com asserts; rode `cargo miri test` onde ele existir antes de declarar os exercicios 2 e 4 verificados por Miri. Essa lacuna e declarada, nao contornada.

```rust
use std::alloc::{alloc, dealloc, handle_alloc_error, Layout};
use std::ptr;

fn meu_split_at_mut<T>(slice: &mut [T], mid: usize) -> (&mut [T], &mut [T]) {
    assert!(mid <= slice.len(), "mid fora do slice");
    let len = slice.len();
    let ptr = slice.as_mut_ptr();
    // Safety: mid <= len (assert acima); as duas metades nao se sobrepoem
    // e juntas cobrem exatamente o slice original.
    unsafe {
        (
            std::slice::from_raw_parts_mut(ptr, mid),
            std::slice::from_raw_parts_mut(ptr.add(mid), len - mid),
        )
    }
}

struct MeuVec<T> {
    ptr: *mut T,
    len: usize,
    cap: usize,
}

impl<T> MeuVec<T> {
    fn novo(cap: usize) -> MeuVec<T> {
        if cap == 0 {
            return MeuVec { ptr: std::ptr::NonNull::dangling().as_ptr(), len: 0, cap: 0 };
        }
        let layout = Layout::array::<T>(cap).expect("layout");
        // Safety: layout tem tamanho > 0 quando cap > 0 e T nao e ZST aqui.
        let raw = unsafe { alloc(layout) } as *mut T;
        if raw.is_null() {
            handle_alloc_error(layout);
        }
        MeuVec { ptr: raw, len: 0, cap }
    }

    fn push(&mut self, valor: T) {
        assert!(self.len < self.cap, "MeuVec deste laboratorio nao cresce");
        // Safety: len < cap, entao ptr.add(len) esta dentro da alocacao.
        unsafe { ptr::write(self.ptr.add(self.len), valor) };
        self.len += 1;
    }

    fn pop(&mut self) -> Option<T> {
        if self.len == 0 {
            return None;
        }
        self.len -= 1;
        // Safety: len antigo era > 0; o elemento em len esta inicializado.
        Some(unsafe { ptr::read(self.ptr.add(self.len)) })
    }

    fn get(&self, indice: usize) -> Option<&T> {
        if indice < self.len {
            // Safety: indice < len, elemento inicializado.
            Some(unsafe { &*self.ptr.add(indice) })
        } else {
            None
        }
    }

    fn len(&self) -> usize {
        self.len
    }
}

impl<T> Drop for MeuVec<T> {
    fn drop(&mut self) {
        // Safety: cada elemento em [0, len) foi inicializado por push
        // e ainda nao foi lido por pop.
        unsafe {
            ptr::drop_in_place(ptr::slice_from_raw_parts_mut(self.ptr, self.len));
            if self.cap > 0 {
                let layout = Layout::array::<T>(self.cap).expect("layout");
                dealloc(self.ptr as *mut u8, layout);
            }
        }
    }
}

// O allow abaixo e a licao em si: o compilador moderno detecta este
// transmute como desnecessario e sugere from_ne_bytes/from_le_bytes.
#[allow(unnecessary_transmutes)]
fn bytes_para_u32_transmute(bytes: [u8; 4]) -> u32 {
    // Safety: [u8; 4] e u32 tem o mesmo tamanho; qualquer padrao de bits
    // de u8 e um u32 valido. Ainda assim, prefira from_ne_bytes abaixo.
    unsafe { std::mem::transmute(bytes) }
}

fn bytes_para_u32_safe(bytes: [u8; 4]) -> u32 {
    u32::from_ne_bytes(bytes)
}

struct Node<T> {
    valor: T,
    proximo: *mut Node<T>,
}

struct ListaBruta<T> {
    cabeca: *mut Node<T>,
    len: usize,
}

impl<T> ListaBruta<T> {
    fn nova() -> ListaBruta<T> {
        ListaBruta { cabeca: ptr::null_mut(), len: 0 }
    }
    fn push_front(&mut self, valor: T) {
        let node = Box::into_raw(Box::new(Node { valor, proximo: self.cabeca }));
        self.cabeca = node;
        self.len += 1;
    }
    fn pop_front(&mut self) -> Option<T> {
        if self.cabeca.is_null() {
            return None;
        }
        // Safety: cabeca veio de Box::into_raw e ainda nao foi liberada.
        let caixa = unsafe { Box::from_raw(self.cabeca) };
        self.cabeca = caixa.proximo;
        self.len -= 1;
        Some(caixa.valor)
    }
    fn len(&self) -> usize {
        self.len
    }
}

impl<T> Drop for ListaBruta<T> {
    fn drop(&mut self) {
        while self.pop_front().is_some() {}
    }
}

union FloatBits {
    f: f32,
    u: u32,
}

fn bits_via_union(valor: f32) -> u32 {
    // Safety: le o campo 'u' logo apos escrever 'f'; ambos tem 4 bytes.
    unsafe {
        let fb = FloatBits { f: valor };
        fb.u
    }
}

struct Buffer<T> {
    ptr: *mut T,
    len: usize,
    cap: usize,
}

impl<T> Buffer<T> {
    fn new(cap: usize) -> Buffer<T> {
        assert!(cap > 0);
        let layout = Layout::array::<T>(cap).expect("layout");
        let raw = unsafe { alloc(layout) } as *mut T;
        if raw.is_null() {
            handle_alloc_error(layout);
        }
        Buffer { ptr: raw, len: 0, cap }
    }
    fn write(&mut self, idx: usize, val: T) {
        assert!(idx < self.cap, "write fora da capacidade");
        assert!(idx <= self.len, "write deve ser sequencial ou substituir");
        // Safety: idx < cap; se idx < len, substitui destruindo o antigo.
        unsafe {
            if idx < self.len {
                ptr::drop_in_place(self.ptr.add(idx));
            }
            ptr::write(self.ptr.add(idx), val);
        }
        if idx >= self.len {
            self.len = idx + 1;
        }
    }
    fn read(&self, idx: usize) -> &T {
        assert!(idx < self.len, "read fora do comprimento");
        // Safety: idx < len, inicializado por write.
        unsafe { &*self.ptr.add(idx) }
    }
    fn len(&self) -> usize {
        self.len
    }
    fn capacity(&self) -> usize {
        self.cap
    }
}

impl<T> Drop for Buffer<T> {
    fn drop(&mut self) {
        // Safety: [0, len) inicializados; a alocacao e deste Buffer.
        unsafe {
            ptr::drop_in_place(ptr::slice_from_raw_parts_mut(self.ptr, self.len));
            let layout = Layout::array::<T>(self.cap).expect("layout");
            dealloc(self.ptr as *mut u8, layout);
        }
    }
}

fn main() {
    let mut v = vec![1, 2, 3, 4, 5];
    let (a, b) = meu_split_at_mut(&mut v, 2);
    a[0] = 10;
    b[0] = 30;
    println!("split: {:?} {:?}", a, b);

    let mut meu: MeuVec<String> = MeuVec::novo(4);
    meu.push(String::from("um"));
    meu.push(String::from("dois"));
    let primeiro = meu.get(0).cloned();
    let removido = meu.pop();
    println!("meuvec: len={} get={:?} pop={:?}", meu.len(), primeiro, removido);
    assert_eq!(meu.len(), 1);

    let bytes = [0x04u8, 0x03, 0x02, 0x01];
    println!("u32: transmute={} safe={}", bytes_para_u32_transmute(bytes), bytes_para_u32_safe(bytes));
    assert_eq!(bytes_para_u32_transmute(bytes), bytes_para_u32_safe(bytes));

    let mut lista: ListaBruta<i32> = ListaBruta::nova();
    lista.push_front(1);
    lista.push_front(2);
    println!("lista bruta: len={} pop={:?}", lista.len(), lista.pop_front());
    assert_eq!(lista.len(), 1);

    for valor in [3.14f32, -0.0, f32::INFINITY, f32::NAN] {
        println!("bits de {}: union={:#010x} to_bits={:#010x}", valor, bits_via_union(valor), valor.to_bits());
        assert_eq!(bits_via_union(valor), valor.to_bits());
    }

    let mut buf: Buffer<String> = Buffer::new(4);
    buf.write(0, String::from("alfa"));
    buf.write(1, String::from("beta"));
    println!("buffer: len={} cap={} [0]={}", buf.len(), buf.capacity(), buf.read(0));
    assert_eq!(buf.read(1), "beta");
    println!("testes ok");
}
```

3 e 5 sao o par didatico da aula: `transmute`/union funcionam, mas a std ja oferece a forma segura (`from_ne_bytes`, `to_bits`) que expressa a intencao e dispensa o bloco `unsafe`. Unsafe novo so se justifica onde nao ha forma segura equivalente — e ai com o comentario `# Safety` dizendo qual invariante o chamador garante, como nos blocos acima.

7. Tres exemplos da std para analisar no seu texto: `Vec::push` (invariante: capacidade reservada antes de escrever sem inicializar), `slice::get_unchecked` (quem chama garante o indice; a API segura `get` cobra isso com checagem) e `String::from_raw_parts` (quem chama transfere a posse de um buffer alocado compativel). Em todos, o unsafe e pequeno, cercado por uma API que cobra as pre-condicoes em codigo seguro.

**Autoverificacao:** split mutando as duas metades sem aliasing, `MeuVec<String>` com pop devolvendo o valor (Drop correto aparece ao rodar sob ASan/Miri onde houver) e os bits do union iguais aos de `to_bits()` nos quatro valores.
