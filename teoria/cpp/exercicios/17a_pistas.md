# Pistas da aula 17a

1. Dentro do handler só vale o que é async-signal-safe: escrever `sig_atomic_t` e retornar. Tudo que o laço precisa saber chega pelo flag; o resto acontece fora do handler.
2. Rode mutex e atômico na mesma máquina e carga, depois repita com todos disputando o mesmo contador (sem partição). A inversão do vencedor é o dado: explique-a com linha de cache.
3. Tire o `lock_guard`, recompile com `-fsanitize=thread` e rode. O relatório mostra duas pilhas (escrita x escrita ou leitura x escrita) e os endereços: transcreva-os.
4. `timeout -s TERM 1 ./conc17a` mata durante o `sleep_for`: o flag interrompe o laço restante. Com `SIG_IGN`, o sinal é descartado e o programa completa as 3 iterações — compare as saídas.
