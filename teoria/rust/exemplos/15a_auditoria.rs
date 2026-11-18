// Aula 15a Rust — auditoria de `unsafe`: cada bloco com invariante escrita.
//
// Uma pilha de capacidade fixa sobre `MaybeUninit`: o `unsafe` existe em
// quatro lugares, e cada um tem pre-condicao, pos-condicao e teste que a
// cobra. Auditoria nao e "revisar com cuidado" — e invariante escrita +
// teste que falha se a invariante quebrar.
//
// Compilar: rustc --edition 2024 -O -D warnings teoria/rust/exemplos/15a_auditoria.rs -o /tmp/15a_rs
// Executar: /tmp/15a_rs
// Clippy: rustc --edition 2024 --crate-type lib teoria/rust/exemplos/15a_auditoria.rs -o /dev/null (sem warnings)
use std::mem::MaybeUninit;

struct PilhaFixa<T, const N: usize> {
    dados: [MaybeUninit<T>; N],
    tamanho: usize,
}

// INVARIANTE (vale em todo ponto observavel):
//   dados[..tamanho] inicializados; dados[tamanho..] nao inicializados.
impl<T, const N: usize> PilhaFixa<T, N> {
    fn nova() -> Self {
        // SAFETY: array de MaybeUninit nao exige inicializacao; tamanho=0
        // torna a invariante vacuamente verdadeira.
        let dados: [MaybeUninit<T>; N] = unsafe { MaybeUninit::uninit().assume_init() };
        PilhaFixa { dados, tamanho: 0 }
    }

    fn cheia(&self) -> bool {
        self.tamanho == N
    }

    fn push(&mut self, valor: T) -> Result<(), T> {
        if self.cheia() {
            return Err(valor);
        }
        // SAFETY: tamanho < N pela checagem; escreve o slot antes de
        // incrementar, entao dados[..tamanho+1] fica inicializado.
        self.dados[self.tamanho].write(valor);
        self.tamanho += 1;
        Ok(())
    }

    fn pop(&mut self) -> Option<T> {
        if self.tamanho == 0 {
            return None;
        }
        // SAFETY: tamanho > 0, logo dados[tamanho-1] esta inicializado pela
        // invariante; decrementa antes de qualquer outro acesso ao slot.
        self.tamanho -= 1;
        let valor = unsafe { self.dados[self.tamanho].assume_init_read() };
        Some(valor)
    }
}

impl<T, const N: usize> Drop for PilhaFixa<T, N> {
    fn drop(&mut self) {
        // SAFETY: descarta exatamente dados[..tamanho], os inicializados;
        // o resto e MaybeUninit e nao precisa de drop.
        for i in 0..self.tamanho {
            unsafe {
                self.dados[i].assume_init_drop();
            }
        }
    }
}

fn main() {
    let mut p: PilhaFixa<String, 2> = PilhaFixa::nova();
    assert!(p.pop().is_none()); // vazia: pop e None, sem UB
    assert!(p.push("a".to_string()).is_ok());
    assert!(p.push("b".to_string()).is_ok());
    assert!(p.push("c".to_string()).is_err()); // cheia: recusa sem perder o valor
    assert_eq!(p.pop().as_deref(), Some("b"));
    assert_eq!(p.pop().as_deref(), Some("a"));
    assert!(p.pop().is_none());
    println!("pilha: vazio, cheio, ordem LIFO e recusa sem perda");

    // Drop com conteudo: se o Drop descartasse errado (duplo drop ou falta),
    // o contador abaixo divergiria — a auditoria cobra o Drop com numero.
    use std::sync::atomic::{AtomicUsize, Ordering};
    static DROPS: AtomicUsize = AtomicUsize::new(0);
    #[derive(Debug)]
    struct Conta;
    impl Drop for Conta {
        fn drop(&mut self) {
            DROPS.fetch_add(1, Ordering::SeqCst);
        }
    }
    {
        let mut q: PilhaFixa<Conta, 4> = PilhaFixa::nova();
        q.push(Conta).unwrap();
        q.push(Conta).unwrap();
        q.pop(); // um sai por pop (drop no fim do teste), um fica para o Drop
    }
    assert_eq!(DROPS.load(Ordering::SeqCst), 2);
    println!("drop: exatamente os 2 valores tiveram Drop (pop + descarte)");
    println!("15a ok: 4 blocos unsafe, 4 invariantes, 4 testes que as cobram");
}
