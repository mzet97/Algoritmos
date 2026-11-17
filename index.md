---
layout: default
title: Início
description: Escolha um dos cinco livros gratuitos e acompanhe a revisão editorial da masterclass de programação de sistemas.
---

<section class="hero" aria-labelledby="hero-title">
  <div class="hero-inner">
    <div class="hero-copy">
      <p class="eyebrow">Masterclass aberta · em português brasileiro</p>
      <h1 id="hero-title">Do primeiro programa aos sistemas que sustentam o mundo.</h1>
      <p class="hero-lead">Cinco livros para estudar linguagens, memória, redes, concorrência e computação de alto desempenho com teoria e prática.</p>
      <div class="hero-actions">
        <a class="button button-primary" href="{{ '/teoria/c/indice.html' | relative_url }}">Começar pelo livro de C</a>
        <a class="button button-secondary" href="{{ '/teoria/README.html' | relative_url }}">Entender a proposta</a>
      </div>
    </div>
    <div class="hero-note" aria-label="Estrutura do percurso">
      <span class="note-kicker">Percurso de aprendizagem</span>
      <ol>
        <li><span>01</span> Fundamentos e modelos mentais</li>
        <li><span>02</span> Código, depuração e medição</li>
        <li><span>03</span> Sistemas, redes e concorrência</li>
        <li><span>04</span> Projetos e análise de falhas</li>
      </ol>
      <p>O avanço depende de evidências práticas, não da quantidade de páginas lidas.</p>
    </div>
  </div>
</section>

<section class="books-section page-container" id="livros" aria-labelledby="books-title">
  <div class="section-heading">
    <p class="eyebrow">Biblioteca</p>
    <h2 id="books-title">Escolha uma linguagem. Construa a base completa.</h2>
    <p>Cada livro é independente. O estado editorial indica quais partes já passaram pelo piloto de revisão.</p>
  </div>

  <div class="books-grid">
    <article class="book-card book-card-featured">
      <div class="book-card-top"><span class="book-letter" aria-hidden="true">C</span><span class="status status-pilot">Conteúdo completo · em revisão</span></div>
      <h3>Livro de C</h3>
      <p>47 arquivos: base de memória, sistemas distribuídos, kernel RISC-V executado em QEMU e comparativo x86-64. Gate editorial aberto (revisão das aulas originais).</p>
      <a class="text-link" href="{{ '/teoria/c/indice.html' | relative_url }}">Abrir índice de C <span aria-hidden="true">→</span></a>
    </article>
    <article class="book-card">
      <div class="book-card-top"><span class="book-letter" aria-hidden="true">C++</span><span class="status">Conteúdo completo · em revisão</span></div>
      <h3>Livro de C++</h3>
      <p>44 arquivos: abstrações, recursos, segurança de memória com sanitizers e kernel em C++ freestanding.</p>
      <a class="text-link" href="{{ '/teoria/cpp/indice.html' | relative_url }}">Abrir índice de C++ <span aria-hidden="true">→</span></a>
    </article>
    <article class="book-card">
      <div class="book-card-top"><span class="book-letter" aria-hidden="true">Rs</span><span class="status">Conteúdo completo · em revisão</span></div>
      <h3>Livro de Rust</h3>
      <p>43 arquivos: ownership, distribuídos, vetorização e kernel no_std pelos mesmos testes das trilhas C/C++.</p>
      <a class="text-link" href="{{ '/teoria/rust/indice.html' | relative_url }}">Abrir índice de Rust <span aria-hidden="true">→</span></a>
    </article>
    <article class="book-card">
      <div class="book-card-top"><span class="book-letter" aria-hidden="true">Go</span><span class="status">Conteúdo completo · em revisão</span></div>
      <h3>Livro de Go</h3>
      <p>42 arquivos: serviços, redes, relógios lógicos, ferramenta de observação do SO e projeto integrador.</p>
      <a class="text-link" href="{{ '/teoria/go/indice.html' | relative_url }}">Abrir índice de Go <span aria-hidden="true">→</span></a>
    </article>
    <article class="book-card">
      <div class="book-card-top"><span class="book-letter" aria-hidden="true">Py</span><span class="status">Conteúdo completo · em revisão</span></div>
      <h3>Livro de Python</h3>
      <p>46 arquivos: da linguagem ao protocolo TCP puro, segurança defensiva, processos e plataforma distribuída.</p>
      <a class="text-link" href="{{ '/teoria/python/indice.html' | relative_url }}">Abrir índice de Python <span aria-hidden="true">→</span></a>
    </article>
  </div>
</section>

<section class="method-section" aria-labelledby="method-title">
  <div class="page-container method-inner">
    <div>
      <p class="eyebrow">Método</p>
      <h2 id="method-title">Aprender é demonstrar o que se sabe fazer.</h2>
    </div>
    <div class="method-copy">
      <p>As competências ligam explicação, implementação, depuração e medição. Exercícios e projetos produzem a evidência para avançar.</p>
      <a class="text-link" href="{{ '/teoria/ESPECIFICACAO_SDD.html' | relative_url }}">Ler a especificação SDD <span aria-hidden="true">→</span></a>
      <a class="text-link" href="{{ '/teoria/AUDITORIA.html' | relative_url }}">Ver a auditoria editorial <span aria-hidden="true">→</span></a>
    </div>
  </div>
</section>
