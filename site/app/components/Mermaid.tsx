"use client";

import { useEffect, useRef } from "react";
import mermaid from "mermaid";

let configurado = false;

export default function Mermaid({ chart }: { chart: string }) {
  const ref = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!configurado) {
      mermaid.initialize({
        startOnLoad: false,
        theme: "dark",
        themeVariables: {
          primaryColor: "#1d1a13",
          primaryTextColor: "#ece5d3",
          primaryBorderColor: "#c9a227",
          lineColor: "#b9b09a",
          secondaryColor: "#2e2a20",
          tertiaryColor: "#0c0b08",
        },
      });
      configurado = true;
    }
    const el = ref.current;
    if (!el) return;
    let vivo = true;
    mermaid
      .render(`mmd-${Math.random().toString(36).slice(2)}`, chart)
      .then(({ svg }) => {
        if (vivo) el.innerHTML = svg;
      })
      .catch((e: unknown) => {
        if (vivo) el.innerHTML = `<pre>Diagrama inválido: ${String(e).slice(0, 200)}</pre>`;
      });
    return () => {
      vivo = false;
    };
  }, [chart]);

  return <div ref={ref} className="my-4 overflow-x-auto rounded-lg border border-[var(--color-ink-line)] bg-[#0c0b08] p-4" />;
}
