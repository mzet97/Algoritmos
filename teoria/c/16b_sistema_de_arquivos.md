---
layout: default
---

# Aula 16b — Sistema de arquivos: nome, inode e o crash no meio

**Competências SDD:** ARQ, OS e SEC. **Pré-requisitos:** aulas [08](08.md) (I/O e buffers), [16](16.md) (syscalls, `mmap`) e [16a](16a_processos_e_observacao.md) (processos e observação). O kernel [32a](32a_kernel_riscv.md) mostra o outro lado (falta sob demanda, syscall); esta aula mostra o lado que todo programa encontra primeiro: o arquivo que sobrevive — ou não — à queda de energia.

**Objetivo observável:** ao terminar, você deverá publicar um arquivo de forma atômica (tmp + `fsync` + `rename`), detectar uma escrita rasgada por checksum, e explicar o que o diretório garante (nome → inode) e o que ele não garante (conteúdo pela metade).

**Motivação:** quase todo bug de durabilidade tem a mesma forma: o programa escreveu "quase tudo" e caiu. Sem disciplina, o leitor seguinte encontra meio arquivo e confia nele. Esta aula estabelece a disciplina mínima: publique de uma vez, desconfie de tudo que não foi publicado, e prove a integridade antes de confiar.

## 1. Contrato antes do código

- **Diretório é mapa, não cofre:** ele liga um nome a um inode. O `rename` troca a ligação de lugar atomicamente — quem abre o nome vê o velho ou o novo, nunca uma mistura.
- **Publicação atômica:** escreva o conteúdo completo num temporário, `fsync` o temporário (desça do page cache para o disco), e só então `rename` sobre o nome final. A ordem é a garantia.
- **Rasgo detectável:** todo registro lido fora do caminho publicado passa pelo checksum antes de ser confiado. Checksum divergente não é "dado alternativo" — é corrupção, e o programa recusa.
- **Fora do contrato:** journaling do ext4/xfs, `fsync` do diretório pai (necessário em produção após `rename`), e o VFS do kernel 32a. A aula mostra o mecanismo em espaço de usuário; o kernel mostra onde ele mora de verdade.

## 2. Por que tmp + fsync + rename, e não escrever direto?

Escrever direto no nome final expõe cada prefixo do arquivo a qualquer leitor e a qualquer crash: a queda sempre encontra *alguma* versão parcial válida o bastante para enganar. O temporário inverte a exposição: enquanto ele está incompleto, ninguém o lê pelo nome final; quando o `rename` acontece, o conteúdo já está inteiro e sincronizado. O preço é um arquivo a mais e um `fsync` a mais — barato perto de um banco que acorda com meio registro e decide o futuro com ele.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/16b_filesystem.c) contém o programa completo:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/16b_filesystem.c -o /tmp/16b
/tmp/16b
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/16b_filesystem.c -o /tmp/16b_asan
/tmp/16b_asan   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística):

```text
fase1 atomico: 27 bytes publicados de uma vez: versao=2...
fase2 rasgo: 16 de 32 bytes, checksum 64cd001c != b64a96c9 -> detectado
fase3: publicado intacto (27 bytes), tmp descartado
16b ok: rename atomico publica, checksum denuncia rasgo
```

Leitura das três fases:

1. **Publicação:** o temporário recebe os 27 bytes com `fsync` e vira `dados.txt` num `rename`. Leitores concorrentes nunca veem 13 bytes.
2. **Rasgo:** metade de um registro é escrita sem `fsync` e sem `rename` — o "crash". A leitura de volta detecta pelo checksum (metade soma diferente do todo) e recusa.
3. **Sobrevivência:** o publicado continua com os mesmos 27 bytes; o temporário é descartado sem pena. Descarte de temporário é rotina, não emergência.

### O que a memória faz aqui?

`write` com loop até completar (escrita parcial é normal, não erro), `fsync` antes de publicar (page cache é memória, não disco), e leitura com teto de buffer. Nada de `mmap` aqui: o ponto é a ordem das operações, não a velocidade delas.

## 4. Erros comuns e implicações defensivas

1. **Publicar sem `fsync`:** o `rename` troca o nome, mas o conteúdo ainda pode estar só no page cache. Queda real perde os dois.
2. **Confiar em tamanho como integridade:** 16 de 32 bytes também é "um arquivo com tamanho". Só o checksum diz que está incompleto.
3. **Reutilizar o temporário entre escritas:** resto da versão anterior vaza para a próxima. Temporário é descartável por definição.
4. **Esquecer o diretório pai:** em produção, `fsync` no fd do diretório após o `rename` garante a própria entrada do nome. A aula declara a ausência; o checklist cobra.
5. **Chamar isto de banco de dados:** sem journal, sem transação e sem concorrência, isto é o degrau zero. O degrau um é o log com `fsync` da 23a.

## 5. Ponte com as demais trilhas

O mesmo trio aparece em toda a masterclass: Go publica config com tmp+rename na trilha de infraestrutura; Python faz checkpoint com fsync na trilha de ETL; Rust persiste o log antes de responder. Em JavaScript, o análogo é o `localStorage` que só confirma após serializar inteiro — meio JSON gravado é o rasgo desta aula em outra roupa.

## 6. Checklist do que esta aula garante

- [ ] Explico a diferença entre nome, inode e conteúdo — e qual dos três o `rename` move.
- [ ] Reproduzo a publicação atômica e digo onde o `fsync` mora na ordem e por quê.
- [ ] Demonstro um rasgo detectado por checksum e a recusa que se segue.
- [ ] Listo o que falta para produção: `fsync` do diretório, journal e concorrência.
- [ ] Aponto onde este mecanismo reaparece: log da 23a, checkpoint e o VFS do kernel 32a.

**Referências primárias:** `man 2 rename` (atomicidade), `man 2 fsync` (durabilidade) e a documentação do [ext4](https://www.kernel.org/doc/html/latest/filesystems/ext4/index.html) sobre ordem de journal. Nada aqui substitui o capítulo de crash consistency de um livro de sistemas — ele diz onde esta aula termina.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
