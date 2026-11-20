import { VOLUMES, VOLUME_TITULO, listarAulas } from "@/lib/livros";

export default async function Home() {
  const contagens = await Promise.all(
    VOLUMES.map(async (v) => ({ volume: v, total: (await listarAulas(v)).length }))
  );
  return (
    <div>
      <p className="text-sm tracking-wide text-[var(--color-gold)] uppercase">Masterclass aberta · pt-BR</p>
      <h1 className="display text-4xl mt-2 mb-3">Do primeiro programa aos sistemas que sustentam o mundo.</h1>
      <p className="text-[var(--color-paper-dim)] max-w-2xl mb-10">
        Cinco livros para estudar linguagens, memória, redes, concorrência e desempenho — com teoria,
        diagramas, vídeos e exercícios executados de verdade.
      </p>
      <div className="grid gap-4 md:grid-cols-2">
        {contagens.map(({ volume, total }) => (
          <a
            key={volume}
            href={`/${volume}`}
            className="block rounded-lg border border-[var(--color-ink-line)] bg-[var(--color-ink-soft)] p-6 no-underline hover:border-[var(--color-gold)]"
          >
            <div className="display text-2xl text-[var(--color-paper)]">Livro de {VOLUME_TITULO[volume]}</div>
            <div className="text-sm text-[var(--color-paper-dim)] mt-1">{total} aulas</div>
          </a>
        ))}
      </div>
    </div>
  );
}
