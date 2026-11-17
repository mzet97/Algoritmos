# Pistas da aula 16b

1. Varra n = 64, 128, ..., 4096 e anote a razao: ela nasce quando a matriz passa do nivel de cache em questao. Compare com os tamanhos de L1/L2/LLC de /sys/devices/system/cpu/cpu0/cache ou lscpu.
2. Arquivo real adiciona page cache e possivel disco; pipe adiciona o outro processo e copias. Ordene pelos termos que cada um soma.
3. Buffer maior amortiza o custo fixo da travessia: custo por byte cai com o tamanho do buffer ate a banda mandar. Meça 1, 64 e 4096 no mesmo arquivo.
4. /proc/self/maps: ache o binario (r-xp), o heap ([heap]), a pilha ([stack]) e o [vdso]. Cada linha e uma decisao de mmap que alguem tomou por voce.
