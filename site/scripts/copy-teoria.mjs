// Copia apenas os *.md de ../teoria para ./teoria_build antes do build,
// para que o deploy (que empacota só site/) tenha as aulas disponíveis.
// Em desenvolvimento local nada muda: lib/livros.ts prefere ../teoria.
import { promises as fs } from "node:fs";
import path from "node:path";

const RAIZ = path.resolve(import.meta.dirname, "..");
const ORIGEM = path.resolve(RAIZ, "..", "teoria");
const DESTINO = path.join(RAIZ, "teoria_build");
const VOLUMES = ["c", "cpp", "rust", "go", "python"];

async function existe(p) {
  try {
    await fs.access(p);
    return true;
  } catch {
    return false;
  }
}

let copiados = 0;
for (const v of VOLUMES) {
  const de = path.join(ORIGEM, v);
  const para = path.join(DESTINO, v);
  if (!(await existe(de))) continue;
  await fs.mkdir(para, { recursive: true });
  const arquivos = (await fs.readdir(de)).filter((f) => f.endsWith(".md"));
  for (const f of arquivos) {
    await fs.copyFile(path.join(de, f), path.join(para, f));
    copiados += 1;
  }
}
console.log(`teoria_build: ${copiados} arquivos .md copiados`);
if (copiados === 0) throw new Error("nenhum .md encontrado em ../teoria");
