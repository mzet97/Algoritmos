---
layout: default
---

# Solucoes comentadas da aula 14b

Relaxed nos dois lados remove a aresta Release/Acquire: restam duas operacoes atomicas sem ordem entre si, e a leitura de DADO nao tem garantia de enxergar a escrita que precedeu PRONTO. Passar em x86 e a definicao de bug dependente de hardware. No rendezvous, produtor e consumidor avancam em lockstep (vazao menor, latencia minima); teto grande desacopla rajadas ao preco de memoria. recv_timeout e o padrao de cancelamento para worker bloqueado em canal: timeout curto, checa flag, volta a esperar.
