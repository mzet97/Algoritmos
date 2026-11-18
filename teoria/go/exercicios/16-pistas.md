---
layout: default
---

# Aula 16 (Go): pistas graduais

Volte aos [exercicios](../16.md#13-exercicios-obrigatorios) antes de abrir as solucoes.

1. `/ping` responde texto puro `pong` com `w.Write`.
2. JSON de saida pede `Content-Type` e `json.NewEncoder(w).Encode`.
3. Na leitura, decode para uma struct de entrada e valide o campo obrigatorio antes de responder.
4. O cliente com timeout usa `http.Client{Timeout: ...}` — o cliente padrao nao tem prazo nenhum.
5. Bootstrap (criar mux, subir servidor) separado dos handlers deixa os handlers testaveis com `httptest`.
