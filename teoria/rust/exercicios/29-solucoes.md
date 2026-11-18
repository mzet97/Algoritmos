---
layout: default
---

# Aula 29 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../29.md#exercícios-obrigatórios-teóricos-e-práticos). Exercicios teoricos/de laboratorio de sistema: a toolchain eBPF (aya, `bpf-linker`, permissoes de carga) nao esta instalada nem e carregavel neste ambiente de desenvolvimento, entao nada aqui foi anexado a interface alguma — as respostas abaixo sao o raciocinio que a rubrica cobra, com a verificacao declarada como laboratorio futuro.

1. Mudando o retorno para `XDP_DROP`, cada pacote que entra por `lo` e descartado no ponto mais cedo do caminho de rede — antes de virar `skb` completo e antes da pilha IP processar. O `ping 127.0.0.1` passa a mostrar perda total (as requisicoes ICMP nem chegam ao destino local, e as respostas tambem sao descartadas ao entrar): o sintoma esperado e 100% de perda com a interface "de pe", nao erro de rota. Desanexar o programa restaura o ping imediatamente — esse vai-e-volta e o teste de sanidade do laboratorio.

2 e 3. Os tres pontos de gancho:

| Tipo | Onde mora | Pergunta que responde |
| --- | --- | --- |
| XDP | Driver de rede, na entrada do pacote | Aceito, descarto ou redireciono este pacote **agora**? |
| tc | Fila de entrada/saida da pilha de rede | Classifico, moldo ou espelho este trafego? |
| Kprobe | Entrada/saida de funcoes do kernel | O que o kernel esta fazendo (syscalls, VFS, escalonador)? |

Firewall anti-DDoS ultra-rapido: **XDP**, porque descartar antes de alocar as estruturas caras da pilha e exatamente o que segura um flood. Monitor de arquivos acessados: **Kprobe** (ou tracepoints equivalentes) sobre as funcoes de abertura de arquivos do VFS/syscalls — pacote de rede nenhum passa pelo caminho de um `open()` local.

**Autoverificacao:** a previsao do ping escrita **antes** de qualquer teste (perda total em `lo`, recuperacao ao desanexar) e cada cenario casado com o gancho pela natureza do evento, nao pelo nome.
