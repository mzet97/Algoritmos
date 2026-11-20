import { notFound } from "next/navigation";
import { VOLUMES, VOLUME_TITULO, listarAulas, type Volume } from "@/lib/livros";

export async function generateStaticParams() {
  return VOLUMES.map((volume) => ({ volume }));
}

export default async function VolumePage({ params }: { params: Promise<{ volume: string }> }) {
  const { volume } = await params;
  if (!(VOLUMES as readonly string[]).includes(volume)) notFound();
  const aulas = await listarAulas(volume as Volume);
  const comExercicio = aulas.filter((a) => a.exercicios.length > 0).length;
  return (
    <div>
      <p className="text-sm tracking-wide text-[var(--color-gold)] uppercase">Livro</p>
      <h1 className="display text-3xl mt-1 mb-2">Programação de sistemas em {VOLUME_TITULO[volume as Volume]}</h1>
      <p className="text-[var(--color-paper-dim)] mb-8">
        {aulas.length} aulas · {comExercicio} com exercícios verificados por execução
      </p>
      <ol className="space-y-2">
        {aulas.map((a) => (
          <li key={a.slug}>
            <a
              href={`/${volume}/${a.slug}`}
              className="flex items-baseline justify-between gap-4 rounded-md border border-[var(--color-ink-line)] px-4 py-3 no-underline hover:border-[var(--color-gold)]"
            >
              <span className="text-[var(--color-paper)]">{a.titulo}</span>
              {a.exercicios.length > 0 && (
                <span className="text-xs shrink-0 text-[var(--color-sage)]">
                  {a.exercicios.length} exercícios
                </span>
              )}
            </a>
          </li>
        ))}
      </ol>
    </div>
  );
}
