// Diagramas piloto injetados pelo site (sem tocar nos .md do livro).
// Chave: "volume/slug". Renderizados via Mermaid na página da aula.
export const DIAGRAMAS: Record<string, { titulo: string; chart: string }> = {
  "c/01": {
    titulo: "Figura 1 — Da fonte ao executável",
    chart: `flowchart LR
  SRC["programa.c<br/>fonte"] --> PRE["programa.i<br/>pré-processador<br/>gcc -E"]
  PRE --> ASM["programa.s<br/>assembly<br/>gcc -S"]
  ASM --> OBJ["programa.o<br/>objeto<br/>gcc -c"]
  OBJ --> EXE["programa<br/>executável<br/>linker"]`,
  },
  "go/23c_quorum_e_reparo": {
    titulo: "Figura 1 — Por que W + R > N impede leitura obsoleta",
    chart: `flowchart TB
  subgraph Escrita["Escrita W=2 (réplicas 0,1)"]
    W0["r0: [2,0,0] 222"]
    W1["r1: [2,0,0] 222"]
  end
  subgraph Leitura["Leitura R=2, p.ex. {1,2}"]
    R1["r1: [2,0,0] 222"]
    R2["r2: [0,0,0] 111 obsoleta"]
  end
  W1 -. "interseção: r1 viu a escrita" .-> R1
  R1 --> V["veredito 222 + reparo de r2"]`,
  },
  "go/20b_tls_e_ssrf": {
    titulo: "Figura 1 — Primeiro o túnel, depois o crachá",
    chart: `sequenceDiagram
  participant Cli as cliente
  participant Srv as servidor 127.0.0.1
  Cli->>Srv: ClientHello (TLS)
  Srv->>Cli: certificado efêmero
  Cli->>Cli: SHA-256 == pin? (TOFU)
  Cli->>Srv: GET /quem + Bearer SEGREDO
  Srv->>Cli: 200 OK / 401 NEGADO (negação única)`,
  },
  "go/28c_multino_simulado": {
    titulo: "Figura 1 — Protocolo mínimo em dois processos",
    chart: `sequenceDiagram
  participant Coord as coordenador
  participant T1 as trabalhador metade 0
  participant T2 as trabalhador metade 1
  Coord->>T1: METADE ini fim (deadline)
  Coord->>T2: METADE ini fim (deadline)
  T1->>Coord: OK checksum dt
  T2->>Coord: OK checksum dt
  Coord->>Coord: combinada == referência?`,
  },
};

export function diagramaDe(volume: string, slug: string) {
  return DIAGRAMAS[`${volume}/${slug}`] ?? null;
}
