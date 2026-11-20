// Reescreve links relativos dos .md para rotas do site.
// Só toca hrefs que terminam em .md; o resto passa intacto.
export function resolverLink(volume: string, href: string): string {
  if (/^(https?:|mailto:|#)/.test(href)) return href;
  const [caminho, frag] = href.split("#");
  const sufixo = frag ? `#${frag}` : "";
  if (!caminho.endsWith(".md")) return href;
  const base = caminho.replace(/\.md$/, "");
  if (/^exercicios\/.+[-_](pistas|solucoes)$/.test(base)) return `#exercicios${sufixo}`;
  const m = base.match(/^(?:\.\.\/([a-z+]+)\/|\.\/)?([^/]+)$/);
  if (!m) return href;
  const vol = m[1] ?? volume;
  const slug = m[2];
  if (!/^[a-z]+$/.test(vol) || !/^[a-zA-Z0-9_.\-]+$/.test(slug)) return href;
  return `/${vol}/${slug}${sufixo}`;
}
