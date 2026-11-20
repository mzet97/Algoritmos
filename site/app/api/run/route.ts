import { NextResponse } from "next/server";
import { spawnSync } from "node:child_process";
import { promises as fs } from "node:fs";
import os from "node:os";
import path from "node:path";

// Execução de código dos exercícios via Judge0 auto-hospedado.
//
// - Nenhum código de aluno é compilado/executado no servidor Next.js em
//   produção: tudo vai para JUDGE0_URL (instância própria, ex.
//   http://localhost:2358).
// - Efêmero: não persistimos código nem submissões aqui; cada chamada cria
//   uma submissão Judge0 com `wait=true` e devolve só stdout/stderr.
// - Desenvolvimento offline: com ALLOW_LOCAL_EXEC=1 o fallback local abaixo é
//   usado (mesmo comportamento anterior, sem sandbox — nunca em produção).

const LINGUAGENS = ["c", "cpp", "rust", "go", "python"] as const;
type Linguagem = (typeof LINGUAGENS)[number];

// IDs padrão do Judge0 Community Edition.
const JUDGE0_IDS: Record<Linguagem, number> = {
  c: 50, // C (GCC)
  cpp: 54, // C++ (GCC)
  rust: 73, // Rust
  go: 60, // Go
  python: 71, // Python 3
};

// Limite de memória (address space, em KB) por linguagem. O toolchain Go 1.13
// reserva vários GB virtuais ao compilar/rodar; sem isso o `go build` morre
// com pthread_create/EAGAIN dentro do isolate. As demais linguagens passam
// com o padrão do servidor (128 MB).
const JUDGE0_MEMORIA: Partial<Record<Linguagem, number>> = {
  go: 4194304,
};

const MAX_CODIGO = 64 * 1024;
const MAX_SAIDA = 32 * 1024;
const COMPILA_MS = 30_000;
const EXEC_MS = 15_000;
const JUDGE0_MS = 60_000;

function corta(s: string): string {
  if (s.length <= MAX_SAIDA) return s;
  return s.slice(0, MAX_SAIDA) + `\n…(saída cortada em ${MAX_SAIDA} bytes)`;
}

function decodifica(b64: string | null | undefined): string {
  if (!b64) return "";
  try {
    return Buffer.from(b64, "base64").toString("utf8");
  } catch {
    return "";
  }
}

interface Judge0Resposta {
  stdout?: string | null;
  stderr?: string | null;
  compile_output?: string | null;
  message?: string | null;
  status?: { id?: number; description?: string };
  time?: string | null;
}

async function viaJudge0(
  base: string,
  token: string | undefined,
  lang: Linguagem,
  codigo: string,
): Promise<{ ok: boolean; saida: string }> {
  const url = `${base.replace(/\/$/, "")}/submissions?base64_encoded=true&wait=true`;
  const corpo: Record<string, unknown> = {
    language_id: JUDGE0_IDS[lang],
    source_code: Buffer.from(codigo, "utf8").toString("base64"),
    cpu_time_limit: 5,
    wall_time_limit: 10,
    enable_per_process_and_thread_time_limit: true,
    enable_per_process_and_thread_memory_limit: true,
  };
  if (JUDGE0_MEMORIA[lang] !== undefined) {
    corpo.memory_limit = JUDGE0_MEMORIA[lang];
  }
  const ctrl = new AbortController();
  const timer = setTimeout(() => ctrl.abort(), JUDGE0_MS);
  try {
    const r = await fetch(url, {
      method: "POST",
      headers: {
        "content-type": "application/json",
        ...(token ? { authorization: `Bearer ${token}` } : {}),
      },
      body: JSON.stringify(corpo),
      signal: ctrl.signal,
    });
    if (!r.ok) {
      return { ok: false, saida: `executor indisponível (HTTP ${r.status}) — avise o operador` };
    }
    const j = (await r.json()) as Judge0Resposta;
    const compilacao = decodifica(j.compile_output);
    const stdout = decodifica(j.stdout);
    const stderr = decodifica(j.stderr);
    const sistema = j.message ? decodifica(j.message) : "";
    const partes = [compilacao && `compilação:\n${compilacao}`, stdout, stderr, sistema].filter(Boolean);
    const saida = corta(partes.join("\n").trim() || "(sem saída)");
    const aceito = j.status?.id === 3; // 3 = Accepted
    return { ok: aceito, saida };
  } catch (e) {
    return { ok: false, saida: `falha ao falar com o executor: ${String(e)}` };
  } finally {
    clearTimeout(timer);
  }
}

// --- Fallback local (desenvolvimento offline, ALLOW_LOCAL_EXEC=1) ---

async function emTmp(fn: (dir: string) => Promise<{ ok: boolean; saida: string }>): Promise<{ ok: boolean; saida: string }> {
  const dir = await fs.mkdtemp(path.join(os.tmpdir(), "livro-"));
  try {
    return await fn(dir);
  } finally {
    await fs.rm(dir, { recursive: true, force: true });
  }
}

function roda(cmd: string, args: string[], cwd: string, ms: number): { codigo: number | null; saida: string } {
  const r = spawnSync(cmd, args, { cwd, timeout: ms, encoding: "utf8", maxBuffer: MAX_SAIDA * 4 });
  const saida = corta((r.stdout ?? "") + (r.stderr ?? "") + (r.error ? `\n[erro: ${r.error.message}]` : ""));
  return { codigo: r.status, saida };
}

async function viaLocal(lang: Linguagem, codigo: string): Promise<{ ok: boolean; saida: string }> {
  return emTmp(async (dir) => {
    const rodaBin = (bin: string) => roda(bin, [], dir, EXEC_MS);
    switch (lang) {
      case "python": {
        const f = path.join(dir, "prog.py");
        await fs.writeFile(f, codigo);
        const r = roda("python3", [f], dir, EXEC_MS);
        return { ok: r.codigo === 0, saida: r.saida };
      }
      case "go": {
        const f = path.join(dir, "prog.go");
        await fs.writeFile(f, codigo);
        const r = roda("go", ["run", f], dir, EXEC_MS);
        return { ok: r.codigo === 0, saida: r.saida };
      }
      case "c":
      case "cpp":
      case "rust": {
        const ext = lang === "c" ? "c" : lang === "cpp" ? "cpp" : "rs";
        const f = path.join(dir, `prog.${ext}`);
        const bin = path.join(dir, "prog");
        await fs.writeFile(f, codigo);
        const comp =
          lang === "c"
            ? roda("gcc", ["-std=c11", "-Wall", "-Wextra", "-Werror", f, "-o", bin], dir, COMPILA_MS)
            : lang === "cpp"
              ? roda("g++", ["-std=c++20", "-Wall", "-Wextra", "-Werror", f, "-o", bin], dir, COMPILA_MS)
              : roda("rustc", ["--edition", "2024", "-O", f, "-o", bin], dir, COMPILA_MS);
        if (comp.codigo !== 0) return { ok: false, saida: `falha de compilação:\n${comp.saida}` };
        const r = rodaBin(bin);
        return { ok: r.codigo === 0, saida: r.saida };
      }
    }
  });
}

export async function POST(req: Request) {
  let corpo: { linguagem?: string; codigo?: string };
  try {
    corpo = await req.json();
  } catch {
    return NextResponse.json({ ok: false, saida: "corpo JSON inválido" }, { status: 400 });
  }
  const { linguagem, codigo } = corpo;
  if (!linguagem || !(LINGUAGENS as readonly string[]).includes(linguagem)) {
    return NextResponse.json({ ok: false, saida: "linguagem inválida (c, cpp, rust, go, python)" }, { status: 400 });
  }
  if (typeof codigo !== "string" || codigo.length === 0 || codigo.length > MAX_CODIGO) {
    return NextResponse.json({ ok: false, saida: "código vazio ou acima de 64 KiB" }, { status: 400 });
  }
  const lang = linguagem as Linguagem;

  const judge0 = process.env.JUDGE0_URL;
  if (judge0) {
    const resultado = await viaJudge0(judge0, process.env.JUDGE0_TOKEN, lang, codigo);
    return NextResponse.json(resultado);
  }
  if (process.env.ALLOW_LOCAL_EXEC === "1") {
    return NextResponse.json(await viaLocal(lang, codigo));
  }
  return NextResponse.json(
    { ok: false, saida: "executor não configurado — o operador precisa definir JUDGE0_URL" },
    { status: 503 },
  );
}
