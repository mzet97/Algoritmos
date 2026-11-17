# Soluções comentadas da aula 28a

## 1. Teto de Amdahl contra medido

Com p=0,99: N=2 → 1,98x; N=4 → 3,88x; N=8 → 7,48x. O medido fica abaixo — e a diferença não é "overhead misterioso": é banda de memória (160 MB por passada, lidos por todos os núcleos ao mesmo tempo), criação de goroutines e GC. Some `GOMAXPROCS` à conta: com menos CPUs que workers, o teto cai antes de Amdahl.

## 2. `byte` contra `int64`

Menos bytes por elemento = menos tráfego = teto mais alto... até o trabalho por byte ficar tão pequeno que o overhead do laço e do particionamento domina. Vetorização (SIMD) muda o jogo de novo: 8-32 elementos por instrução. Previsão honesta exige medir nos dois tipos, não extrapolar de um.

## 3. Concatenar contra somar

Concatenar aloca e copia a cada `+=` (crescimento amortizado ajuda, mas não zera); somar acumula em registrador. A razão típica é ordens de magnitude — e o `go vet` não pega, porque está tudo "correto". Benchmark com invariante de conteúdo (compare a string final, não só o tamanho) impede trapaça.

## 4. Conta que vira o veredito

`2 * bytes / banda + t_kernel < t_cpu`. Com 160 MB: a 16 GiB/s, a viagem custa ~19 ms; o kernel precisaria ser quase instantâneo E a CPU lenta para compensar. A 100 GiB/s com kernel de 10 ms, a viagem cai para ~3 ms e o kernel domina — veredito pode virar para cargas maiores. Mesma fórmula, conclusões opostas: a conta decide, não a torcida.
