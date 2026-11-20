import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "Masterclass de Programação de Sistemas",
  description: "Cinco livros abertos: C, C++, Rust, Go e Python — teoria, diagramas, vídeos e exercícios com execução real.",
};

export default function RootLayout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="pt-BR">
      <body>
        <header className="border-b border-[var(--color-ink-line)]">
          <div className="mx-auto max-w-5xl px-6 py-4 flex items-baseline justify-between">
            <a href="/" className="display text-xl text-[var(--color-gold-soft)] no-underline">
              Masterclass de Sistemas
            </a>
            <span className="text-sm text-[var(--color-paper-dim)]">C · C++ · Rust · Go · Python</span>
          </div>
        </header>
        <main className="mx-auto max-w-5xl px-6 py-8">{children}</main>
        <footer className="border-t border-[var(--color-ink-line)] mt-16">
          <div className="mx-auto max-w-5xl px-6 py-6 text-sm text-[var(--color-paper-dim)]">
            Conteúdo das aulas em <code>teoria/</code> · exercícios executados no Judge0 auto-hospedado,
            sem persistência de código.
          </div>
        </footer>
      </body>
    </html>
  );
}
