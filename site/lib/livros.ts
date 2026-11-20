import { promises as fs } from "node:fs";
import path from "node:path";

export const VOLUMES = ["c", "cpp", "rust", "go", "python"] as const;
export type Volume = (typeof VOLUMES)[number];

export const VOLUME_TITULO: Record<Volume, string> = {
  c: "C",
  cpp: "C++",
  rust: "Rust",
  go: "Go",
  python: "Python",
};

const TEORIA = path.resolve(process.cwd(), "..", "teoria");

// Arquivos de apoio editorial, não aulas: fora do site.
const EXCLUIDOS = new Set(["prompt.md", "MAPA_SDD.md", "indice.md"]);

export interface Aula {
  volume: Volume;
  slug: string;
  titulo: string;
  markdown: string;
  exercicios: string[];
}

function tituloDe(md: string, slug: string): string {
  const m = md.match(/^#\s+(.+)$/m);
  return m ? m[1].trim() : slug;
}

function exerciciosDe(md: string): string[] {
  const m = md.match(/##\s+(?:\d+\.\s+)?Exerc[ií]cios [Oo]brigat[óo]rios\s*\n([\s\S]*?)(?=\n##\s|\n---\n|\s*$)/);
  if (!m) return [];
  const numerados = m[1].match(/^\d+\.\s+.+$/gm);
  if (numerados && numerados.length > 0) return numerados;
  // Formato alternativo (trilha Rust): subseções "### Exercício N — título".
  const blocos = m[1]
    .split(/(?=^###\s+Exercício\s+\d+\b)/gm)
    .map((s) => s.trim())
    .filter((s) => /^###\s+Exercício\s+\d+\b/.test(s))
    .map((s) =>
      s
        .replace(/^###\s+/, "")
        .replace(/^Exercício\s+\d+\s*[—–-]\s*/, "")
        .slice(0, 2000),
    );
  return blocos;
}

export async function listarAulas(volume: Volume): Promise<Aula[]> {
  "use cache";
  const dir = path.join(TEORIA, volume);
  const arquivos = (await fs.readdir(dir))
    .filter((f) => f.endsWith(".md") && !EXCLUIDOS.has(f))
    .sort();
  const aulas: Aula[] = [];
  for (const arquivo of arquivos) {
    const slug = arquivo.replace(/\.md$/, "");
    const markdown = await fs.readFile(path.join(dir, arquivo), "utf8");
    aulas.push({ volume, slug, titulo: tituloDe(markdown, slug), markdown, exercicios: exerciciosDe(markdown) });
  }
  return aulas;
}

export async function lerAula(volume: Volume, slug: string): Promise<Aula | null> {
  "use cache";
  if (EXCLUIDOS.has(`${slug}.md`)) return null;
  try {
    const markdown = await fs.readFile(path.join(TEORIA, volume, `${slug}.md`), "utf8");
    return { volume, slug, titulo: tituloDe(markdown, slug), markdown, exercicios: exerciciosDe(markdown) };
  } catch {
    return null;
  }
}
