import { notFound } from "next/navigation";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import { VOLUMES, lerAula, listarAulas, type Volume } from "@/lib/livros";
import { videoDe } from "@/lib/videos";
import { diagramaDe } from "@/lib/diagramas";
import { resolverLink } from "@/lib/links";
import Mermaid from "../../components/Mermaid";
import Video from "../../components/Video";
import Exercicio from "../../components/Exercicio";

export async function generateStaticParams() {
  const { listarAulas } = await import("@/lib/livros");
  const params: { volume: string; aula: string }[] = [];
  for (const v of VOLUMES) {
    for (const a of await listarAulas(v)) params.push({ volume: v, aula: a.slug });
  }
  return params;
}

function semExercicios(md: string): string {
  return md.replace(/\n##\s+(?:\d+\.\s+)?Exerc[ií]cios [Oo]brigat[óo]rios[\s\S]*?(?=\n##\s+7\.|\n---\n|\s*$)/, "");
}

export default async function AulaPage({ params }: { params: Promise<{ volume: string; aula: string }> }) {
  const { volume, aula } = await params;
  if (!(VOLUMES as readonly string[]).includes(volume)) notFound();
  const a = await lerAula(volume as Volume, aula);
  if (!a) notFound();
  const todas = await listarAulas(volume as Volume);
  const pos = todas.findIndex((t) => t.slug === aula);
  const anterior = pos > 0 ? todas[pos - 1] : null;
  const proxima = pos >= 0 && pos < todas.length - 1 ? todas[pos + 1] : null;
  const video = videoDe(volume, aula);
  const diagrama = diagramaDe(volume, aula);
  const semFront = a.markdown.replace(/^---\n[\s\S]*?\n---\n/, "");

  return (
    <article>
      <p className="text-sm tracking-wide text-[var(--color-gold)] uppercase">
        <a href={`/${volume}`} className="no-underline">← Livro de {volume}</a>
      </p>
      <div className="prose-livro mt-2">
        <ReactMarkdown
          remarkPlugins={[remarkGfm]}
          urlTransform={(url) => resolverLink(volume, url)}
          components={{
            code(props) {
              const { className, children } = props;
              if (className?.includes("language-mermaid")) {
                return <Mermaid chart={String(children)} />;
              }
              return <code className={className}>{children}</code>;
            },
          }}
        >
          {semExercicios(semFront)}
        </ReactMarkdown>
      </div>

      {diagrama && (
        <section className="mt-10">
          <h2 className="display text-2xl text-[var(--color-gold-soft)]">{diagrama.titulo}</h2>
          <Mermaid chart={diagrama.chart} />
        </section>
      )}

      <section className="mt-10">
        <h2 className="display text-2xl text-[var(--color-gold-soft)]">Vídeo relacionado</h2>
        {video ? (
          <div className="mt-4">
            <Video youtubeId={video.id} titulo={video.titulo} />
          </div>
        ) : (
          <p className="mt-2 text-[var(--color-paper-dim)]">
            Nenhum vídeo curado para esta aula ainda — a curadoria segue aberta.
          </p>
        )}
      </section>

      {a.exercicios.length > 0 && (
        <section className="mt-10" id="exercicios">
          <h2 className="display text-2xl text-[var(--color-gold-soft)]">
            Exercícios — execute aqui
          </h2>
          <p className="mt-2 text-[var(--color-paper-dim)]">
            Escreva sua solução e execute no Judge0. Tente antes de abrir as pistas do livro.
          </p>
          {a.exercicios.map((e, i) => (
            <Exercicio key={i} linguagem={volume} numero={i + 1} enunciado={e.replace(/^\d+\.\s*/, "")} />
          ))}
        </section>
      )}

      <nav
        aria-label="Navegação entre aulas"
        className="mt-12 flex items-start justify-between gap-6 border-t border-[var(--color-ink-line)] pt-5"
      >
        <div>
          {anterior && (
            <a href={`/${volume}/${anterior.slug}`} className="no-underline">
              <span className="block text-xs uppercase tracking-wide text-[var(--color-paper-dim)]">
                ← Anterior
              </span>
              <span className="display text-lg text-[var(--color-gold-soft)]">{anterior.titulo}</span>
            </a>
          )}
        </div>
        <div className="text-right">
          {proxima && (
            <a href={`/${volume}/${proxima.slug}`} className="no-underline">
              <span className="block text-xs uppercase tracking-wide text-[var(--color-paper-dim)]">
                Próxima →
              </span>
              <span className="display text-lg text-[var(--color-gold-soft)]">{proxima.titulo}</span>
            </a>
          )}
        </div>
      </nav>
    </article>
  );
}
