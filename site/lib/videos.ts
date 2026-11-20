// Curadoria piloto: "volume/slug" -> YouTube videoId (verificado via oEmbed).
// Aula sem entrada mostra estado vazio honesto, nunca embed quebrado.
export const VIDEOS: Record<string, { id: string; titulo: string }> = {
  "c/01": { id: "KJgsSFOSQv0", titulo: "C Programming Tutorial for Beginners" },
  "python/00_ponte_javascript": { id: "rfscVS0vtbw", titulo: "Learn Python – Full Course for Beginners" },
  "go/23c_quorum_e_reparo": { id: "AET3z76mNuw", titulo: "Database Replication Explained in 5 Minutes (Master-Slave, Quorum & Leaderless)" },
};

export function videoDe(volume: string, slug: string) {
  return VIDEOS[`${volume}/${slug}`] ?? null;
}
