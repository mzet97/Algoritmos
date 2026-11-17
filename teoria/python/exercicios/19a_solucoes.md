# Solucoes comentadas da aula 19a

Os dois desenhos (linhas e comprimento) quebram em lugares diferentes: delimitador sofre com payload que contem o delimitador; comprimento sofre com len hostil que promete gigabytes — os dois precisam de teto antes de alocar/esperar. Teto de conexoes e deadline sao a mesma politica em duas dimensoes: recurso por cliente limitado no espaco e no tempo.
