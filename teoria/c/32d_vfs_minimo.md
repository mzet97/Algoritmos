---
layout: default
---

# Aula 32d — VFS mínimo: o resolvedor é o sistema de arquivos (C4)

**Competências SDD:** OS, SEC, ARQ. **Pré-requisitos:** aula [32a](32a_kernel_riscv.md) (boot, PMP, Sv39, traps, syscalls, 2 harts) e aula [36 de C++](../cpp/36.md) (convenção de syscall: número em `a7`). Toolchain registrada: `riscv64-linux-gnu-gcc` 16.2.1 e QEMU 10.2.2 (`virt`, `-bios none`, `-smp 1`).

**Objetivo observável:** ao terminar, você deverá executar `teoria/c/kernel32d/`, abrir `/dados/const.txt` pelo resolvedor de caminho e explicar por que `/dados/../versao.txt` é recusado — e o que o VFS não promete.

**Motivação:** a 32a entrega o kernel com sete marcadores e declara o VFS como lacuna; esta aula a preenche sem fingir um sistema de arquivos de produção. O ponto arquitetural: VFS não é disco — é *resolução de caminho + descritores*. A tabela aqui é estática (três arquivos, um diretório), mas o resolvedor anda componente a componente pelo pai, confere cada nível e nega subida, diretório e ausente. Se um dia existir outro suporte, o resolvedor continua valendo; o que muda é de onde vêm os bytes.

## 1. Contrato antes do código

- `make -C teoria/c/kernel32d && python3 teoria/c/kernel32d/test_kernel.py teoria/c/kernel32d/kernel.elf` imprime `KERNEL32D_OK`.
- Syscalls em modo-S sobre a base da 32a: `a7 == 3` abre (caminho → fd ou -1), `a7 == 4` lê (fd, destino, máximo → tamanho ou -1), `a7 == 5` fecha. Os arquivos `boot.S`, `trap.S` e `link.ld` são os da 32a: o estágio prova que o VFS vive acima da fronteira existente.
- Fora do contrato: escrita, montagem de outro dispositivo, permissões por usuário, links, remoção com lixeira. A árvore é estática e somente-leitura — ROM com resolvedor, e dizê-lo é parte do contrato.

## 2. O resolvedor em três regras

**Regra 1 — anda pelo pai, não pela string.** Cada componente é procurado entre os filhos do diretório atual (`nos[i].pai == atual`), com comparação de comprimento + conteúdo. Comparar o caminho inteiro como string pronta aceitaria prefixo errado (`/dados` casando `/dados2`); andar nível a nível não.

**Regra 2 — `..` não sobe, diretório não abre.** Subida é recusada no parse (antes de qualquer busca) e diretório resolvido com sucesso devolve -1 no `open`. As duas negações são decisões de segurança distintas: `..` é traversal, diretório é tipo errado — e o teste verifica cada uma com a mensagem de defeito própria (`DOTDOT_PASSOU`, `DIR_ABRIU`).

**Regra 3 — o resumo do conteúdo é campo a campo.** (Herança da 23g: o `prefixo_igual` compara bytes do nome, e o tamanho vem contado, não presumido.) Nome com `/` embutido não existe porque o separador é consumido pelo andador, não pelo comparado.

## 3. Exemplo completo e executável

O [núcleo deste estágio](kernel32d/kernel.c) (`boot.S`, `trap.S`, `link.ld`, `Makefile` e `test_kernel.py` no mesmo diretório):

```bash
make -C teoria/c/kernel32d kernel.elf
python3 teoria/c/kernel32d/test_kernel.py teoria/c/kernel32d/kernel.elf
# KERNEL32D_OK marcadores=BOOT_OK,VFS_OK,DONE
```

Transcrição serial esperada (determinística, saída 0):

```text
BOOT_OK
kernel32d v1
PI=314159
VFS_OK
DONE
```

## 4. Erros comuns e implicações defensivas

1. **Comparar o caminho inteiro:** `strcmp("/dados", "/dados2")` difere, mas `strncmp` com tamanho errado casa prefixo. Andar por componente elimina a classe inteira de bugs de prefixo.
2. **Aceitar `..` "porque a árvore é estática":** hoje não há para onde subir; amanhã, com outro suporte montado, o mesmo resolvedor permissivo vira traversal. Negar agora é contrato, não pessimismo.
3. **Abrir diretório como arquivo:** ler um diretório como bytes vaza estrutura interna para o chamador. Tipo errado nega no `open`, não no `read` — depois de aberto, o dano já tem fd.
4. **Chamar de "sistema de arquivos":** sem escrita, sem montagem e sem permissão, é resolvedor + ROM. O nome do marcador é `VFS_OK` porque o que foi testado foi resolução e descritores — o resto está no checklist como falta, não como promessa.

## 5. Ponte com as demais trilhas

C++ faz a ROM didática por id na [38](../cpp/38.md) (sem caminho, sem diretório); aqui o caminho existe e é resolvido de verdade — o passo seguinte que a 38 declarou como futuro. Rust valida ponteiros de usuário no kernel da 28a; aqui a validação equivalente (ponteiro do caminho, `a1` de leitura) segue declarada como trabalho futuro na mesma linha da 38. Em Python/Go, o análogo honesto é `open()` com `pathlib`/`os.path` — com a diferença de que lá o SO resolve por você, e aqui a aula mostra o que "resolver" significa.

## 6. Critério de aprovação e checklist

- [ ] Rodo o estágio e mostro `/dados/const.txt` resolvido em dois níveis.
- [ ] Explico por que `/dados/../versao.txt` é recusado no parse (e não na busca).
- [ ] Digo por que diretório resolvido nega no `open` em vez de ler como bytes.
- [ ] Descrevo o que quebra se a comparação for por string inteira (prefixo).
- [ ] Nomeio o que falta: escrita, montagem, permissão, validação de ponteiro de usuário, links.

**Referências primárias:** o conceito de VFS (camada que separa resolvedor de suporte, presente em todo Unix didático — o resolvedor aqui é próprio) e o contrato `open(2)/read(2)` do POSIX como especificação de comportamento. Nenhum texto foi copiado — o resolvedor foi confirmado executando este estágio no QEMU.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
