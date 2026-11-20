"use client";

import { useState } from "react";

const STARTER: Record<string, string> = {
  c: '#include <stdio.h>\n\nint main(void) {\n    printf("ola\\n");\n    return 0;\n}\n',
  cpp: '#include <iostream>\n\nint main() {\n    std::cout << "ola\\n";\n    return 0;\n}\n',
  rust: 'fn main() {\n    println!("ola");\n}\n',
  go: 'package main\n\nimport "fmt"\n\nfunc main() {\n    fmt.Println("ola")\n}\n',
  python: 'def main() -> None:\n    print("ola")\n\n\nif __name__ == "__main__":\n    main()\n',
};

export default function Exercicio({
  linguagem,
  numero,
  enunciado,
}: {
  linguagem: string;
  numero: number;
  enunciado: string;
}) {
  const [codigo, setCodigo] = useState(STARTER[linguagem] ?? "");
  const [saida, setSaida] = useState<string | null>(null);
  const [ok, setOk] = useState<boolean | null>(null);
  const [rodando, setRodando] = useState(false);

  async function executar() {
    setRodando(true);
    setSaida(null);
    try {
      const r = await fetch("/api/run", {
        method: "POST",
        headers: { "content-type": "application/json" },
        body: JSON.stringify({ linguagem, codigo }),
      });
      const j = await r.json();
      setSaida(j.saida ?? "");
      setOk(j.ok ?? false);
    } catch (e) {
      setSaida(`falha de rede: ${String(e)}`);
      setOk(false);
    } finally {
      setRodando(false);
    }
  }

  return (
    <section className="my-6 rounded-lg border border-[var(--color-ink-line)] bg-[var(--color-ink-soft)] p-5">
      <h3 className="display text-lg text-[var(--color-gold-soft)]">
        Exercício {numero} <span className="text-sm text-[var(--color-paper-dim)]">· {linguagem}</span>
      </h3>
      <p className="my-3 whitespace-pre-wrap text-[var(--color-paper)]">{enunciado}</p>
      <label className="text-sm text-[var(--color-paper-dim)]" htmlFor={`ed-${numero}`}>
        Sua solução:
      </label>
      <textarea
        id={`ed-${numero}`}
        value={codigo}
        onChange={(e) => setCodigo(e.target.value)}
        spellCheck={false}
        rows={12}
        className="mt-1 w-full rounded-md border border-[var(--color-ink-line)] bg-[#0c0b08] p-3 font-[var(--font-code)] text-sm text-[var(--color-paper)]"
      />
      <div className="mt-3 flex items-center gap-3">
        <button
          onClick={executar}
          disabled={rodando}
          className="rounded-md bg-[var(--color-gold)] px-5 py-2 font-semibold text-black disabled:opacity-50"
        >
          {rodando ? "Executando…" : "▶ Executar"}
        </button>
        {ok !== null && (
          <span className={ok ? "text-[var(--color-sage)]" : "text-[var(--color-rust)]"}>
            {ok ? "✓ executou com saída 0" : "✗ falhou — leia a saída"}
          </span>
        )}
      </div>
      {saida !== null && (
        <pre className="mt-3 overflow-x-auto rounded-md border border-[var(--color-ink-line)] bg-[#0c0b08] p-3 text-sm whitespace-pre-wrap">
          {saida || "(sem saída)"}
        </pre>
      )}
    </section>
  );
}
