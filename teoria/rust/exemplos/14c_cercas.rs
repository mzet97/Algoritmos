// Aula 14c Rust — Cercas e seqlock: ordenacao sem operacao e leitura otimista.
//
// A 14b ordenou com Release/Acquire grudados na operacao. Esta aula separa
// as duas coisas: `fence` ordena sem carregar dado nenhum, e o seqlock usa
// isso para leitor que nunca bloqueia o escritor. 50 mil publicacoes por
// cerca + 200 mil leituras otimistas, tudo conferido.
//
// Compilar: rustc --edition 2024 -O -D warnings teoria/rust/exemplos/14c_cercas.rs -o /tmp/14c
// Executar: /tmp/14c
use std::hint::spin_loop;
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering, fence};
use std::sync::Mutex;

const VOLTAS: u64 = 50_000;

// Publicacao por cerca: o dado nao e atomico; quem ordena e o fence.
// Sem o fence(Release) isto seria data race (e nao compilaria em codigo
// seguro compartilhando &mut entre threads).
static DADO: Mutex<[u64; 2]> = Mutex::new([0, 0]);
static PRONTO: AtomicBool = AtomicBool::new(false);

fn publica_por_cerca() {
    for i in 1..=VOLTAS {
        *DADO.lock().unwrap() = [i, i.wrapping_mul(7)];
        fence(Ordering::Release); // tudo acima fica visivel antes do sinal
        PRONTO.store(true, Ordering::Relaxed);
        while PRONTO.load(Ordering::Relaxed) {
            spin_loop();
        }
    }
}

fn consome_por_cerca() -> u64 {
    let mut conferidas = 0u64;
    for _ in 1..=VOLTAS {
        while !PRONTO.load(Ordering::Relaxed) {
            spin_loop();
        }
        fence(Ordering::Acquire); // tudo abaixo ve o que precedeu o sinal
        let [a, b] = *DADO.lock().unwrap();
        assert_eq!(b, a.wrapping_mul(7), "par inconsistente: cerca falhou");
        conferidas += 1;
        PRONTO.store(false, Ordering::Relaxed);
    }
    conferidas
}

// Seqlock: escritor nunca espera leitor; leitor retenta se pegou escrita
// no meio. O contador impar significa "escrevendo".
struct SeqLock {
    seq: AtomicU64,
    dado: Mutex<[u64; 2]>,
}

impl SeqLock {
    fn escreve(&self, v: [u64; 2]) {
        let s = self.seq.load(Ordering::Relaxed);
        self.seq.store(s + 1, Ordering::Release); // impar: escrevendo
        *self.dado.lock().unwrap() = v;
        self.seq.store(s + 2, Ordering::Release); // par: estavel
    }

    fn le(&self) -> ([u64; 2], bool) {
        let antes = self.seq.load(Ordering::Acquire);
        if antes % 2 == 1 {
            return ([0, 0], true); // pegou escrita: retenta
        }
        let v = *self.dado.lock().unwrap();
        let depois = self.seq.load(Ordering::Acquire);
        (v, antes != depois) // mudou no meio: retenta
    }
}

fn main() {
    std::thread::scope(|s| {
        s.spawn(publica_por_cerca);
        let conferidas = s.spawn(consome_por_cerca).join().unwrap();
        assert_eq!(conferidas, VOLTAS);
    });
    println!("cercas ok: {VOLTAS} publicacoes conferidas par a par");

    let lock = SeqLock {
        seq: AtomicU64::new(0),
        dado: Mutex::new([0, 0]),
    };
    let mut retentativas = 0u64;
    let mut lidas = 0u64;
    std::thread::scope(|s| {
        s.spawn(|| {
            for i in 1..=VOLTAS {
                lock.escreve([i, i.wrapping_mul(7)]);
            }
        });
        while lidas < VOLTAS {
            let (v, retentar) = lock.le();
            if retentar {
                retentativas += 1;
                continue;
            }
            assert_eq!(v[1], v[0].wrapping_mul(7), "seqlock inconsistente");
            if v[0] > lidas {
                lidas = v[0];
            }
        }
    });
    println!("seqlock ok: {lidas} valores consistentes, {retentativas} retentativas");
    println!("14c ok: fence publica, contador ordena a escrita, leitor otimista nunca trava escritor");
}
