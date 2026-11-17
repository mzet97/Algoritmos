---
layout: default
---

# Aula 10a — soluções comentadas

Compare depois de realizar os [exercícios](../10a_arquitetura_e_memoria_virtual.md#8-exercícios-e-avaliação). Endereços, mapas e contagens do laboratório devem vir **da sua execução**; não existe tabela universal de resultados.

1. Em C, `bloco[0] = 1` exige que `bloco` aponte para armazenamento alocado válido com ao menos um byte e que a alocação não tenha sido liberada. O compilador traduz a operação para instruções da ISA escolhida, mas pode mudar a sequência quando otimiza. O sistema operacional define mapeamentos e permissões; CPU/MMU e tabelas realizam ou auxiliam a tradução. C não garante “heap em `[heap]`”, cache, TLB, páginas nem endereço físico.
2. A resposta precisa incluir duas linhas do seu `/proc/PID/maps`, com intervalo e permissões correspondentes aos endereços impressos, e explicar como comparou valores da **mesma execução**. `%p` mostra um ponteiro no formato da implementação; no laboratório Linux, ele identifica um endereço virtual do processo. O mapa lista faixas virtuais, não quadros físicos. Remova o PID e os endereços completos antes de publicar a captura, se não forem necessários.
3. Uma tabela de três execuções é evidência de valores observados, não de causa. A diferença pode envolver randomização, carregamento de bibliotecas, decisões do alocador e ambiente; igualdade também não demonstraria ASLR desabilitado. Compare sem transformar correlação em diagnóstico.
4. *TLB miss*: é preciso obter a tradução virtual→física por outro caminho, frequentemente consultando estruturas de tradução. *Cache de dados miss*: os dados não estão no nível de cache consultado, e o subsistema de memória deve buscá-los em outro nível. *Falha de página*: o SO precisa resolver um acesso que exige intervenção, podendo estabelecer página sob demanda ou rejeitá-lo. Para uma medição futura, formule hipótese sobre acesso sequencial versus saltos, mantenha o mesmo conjunto de dados e número de leituras, faça aquecimento e repetições, registre hardware e interprete variabilidade. Esse desenho ainda pode misturar efeitos de cache, TLB, prefetch e paginação; não atribua o tempo a uma única causa sem controles adicionais.
5. Evite publicar ponteiros/endereço virtual e PID em resposta ou log acessível externamente quando não há necessidade diagnóstica. Antes de `malloc(n * sizeof *p)`, rejeite `n > SIZE_MAX / sizeof *p` e imponha um teto de recursos compatível com o serviço. Também valide a conversão do texto recebido e o retorno de `malloc`; um limite representável pode continuar impraticável. O serviço não deve confiar que o isolamento de processo substitui checagem de limites do objeto C.

**Critério de autoverificação:** você consegue substituir qualquer frase “foi para a pilha/heap/cache” por uma afirmação precisa sobre duração C ou por uma observação identificada como específica do ambiente.
