# Pistas da aula 28b

1. A entrada da IDT tem o offset fatiado em tres campos (0-15, 16-31, 32-63) separados por seletor/IST/tipo. O construtor recebe o endereco inteiro e distribui; o teste afirma cada fatia com um endereco de bits conhecidos (0x1122334455667788).
2. O stub precisa: cabecalho Multiboot, tabelas de paginas, PAE/LME/PG, GDT64, far jump e so entao chamar o kmain Rust de 64 bits. O no_mangle aparece exatamente nessa chamada.
3. Sem IST configurado e em anel 0, a CPU nao troca a pilha na interrupcao: a frase precisa do "neste componente". Com IST, a troca existe e a correspondencia com mscratch fica de pe — com donos diferentes (TSS contra CSR).
4. Aberto para kernel completo: syscall, APIC/timer, SMP, drivers. O mapa aceita o componente porque a etapa comparativa promete travessia + trap + tabela honesta, nao um segundo kernel inteiro.
