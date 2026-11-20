"use client";

import { useState } from "react";

export default function Video({ youtubeId, titulo }: { youtubeId: string; titulo: string }) {
  const [aberto, setAberto] = useState(false);
  if (!aberto) {
    return (
      <button
        onClick={() => setAberto(true)}
        className="group relative block w-full max-w-2xl overflow-hidden rounded-lg border border-[var(--color-ink-line)] text-left"
        aria-label={`Assistir: ${titulo}`}
      >
        {/* eslint-disable-next-line @next/next/no-img-element */}
        <img
          src={`https://i.ytimg.com/vi/${youtubeId}/hqdefault.jpg`}
          alt=""
          className="aspect-video w-full object-cover"
          loading="lazy"
        />
        <span className="absolute inset-0 flex items-center justify-center">
          <span className="rounded-full bg-[var(--color-gold)] px-5 py-3 text-lg text-black">▶</span>
        </span>
        <span className="absolute bottom-0 left-0 right-0 bg-black/70 px-4 py-2 text-sm">{titulo}</span>
      </button>
    );
  }
  return (
    <iframe
      className="aspect-video w-full max-w-2xl rounded-lg border border-[var(--color-ink-line)]"
      src={`https://www.youtube.com/embed/${youtubeId}?autoplay=1&rel=0`}
      title={titulo}
      allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
      allowFullScreen
    />
  );
}
