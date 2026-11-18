---
layout: default
---

# Aula 26 (C++): solucoes comentadas

Compare apos tentar os [exercicios](../26.md#21-exercicios-obrigatorios).

1, 2, 3 e 5. PIMPL executavel em dois arquivos:

```cpp
// relogio.h
#pragma once

#include <memory>

class Relogio {
public:
    Relogio();
    ~Relogio(); // definido no .cpp: aqui Impl ainda e incompleto
    Relogio(const Relogio&) = delete;
    Relogio& operator=(const Relogio&) = delete;
    Relogio(Relogio&&) noexcept;
    Relogio& operator=(Relogio&&) noexcept;

    void avanca(long segundos);
    long agora() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
```

```cpp
// relogio.cpp
#include "relogio.h"

#include <vector>

struct Relogio::Impl {
    long segundos = 0;
    std::vector<long> historico; // dependencias pesadas ficam aqui
};

Relogio::Relogio() : impl_(std::make_unique<Impl>()) {}
Relogio::~Relogio() = default;
Relogio::Relogio(Relogio&&) noexcept = default;
Relogio& Relogio::operator=(Relogio&&) noexcept = default;

void Relogio::avanca(long segundos)
{
    impl_->segundos += segundos;
    impl_->historico.push_back(impl_->segundos);
}

long Relogio::agora() const
{
    return impl_->segundos;
}
```

```cpp
// main.cpp
#include <iostream>
#include "relogio.h"

int main()
{
    Relogio r;
    r.avanca(90);
    r.avanca(30);
    std::cout << "agora: " << r.agora() << '\n'; // 120
    Relogio outro = std::move(r);
    std::cout << "movido: " << outro.agora() << '\n';
    return 0;
}
```

2. Quem inclui `relogio.h` depende so da interface; trocar `historico` por outro desenho muda `relogio.cpp` e recompila **um** arquivo, nao a arvore inteira de clientes. Sem PIMPL, qualquer campo novo no header forca recompilar todos os `.cpp` que o incluem — a cascata e esse custo multiplicado.

3. Para ABI: o tamanho e o layout publicos de `Relogio` passam a ser "um ponteiro", estaveis entre versoes da implementacao. O que PIMPL **nao** resolve: funcoes virtuais e suas tabelas, excecoes que atravessam, alocador usado dos dois lados e mudancas na propria interface publica.

4. PIMPL e exagero numa classe interna pequena, sem fronteira de biblioteca, cujos clientes recompilam junto no mesmo projeto: voce paga uma alocacao e uma indirecao por objeto (e perde `constexpr`/inline) para proteger uma fronteira que nao existe.

5. `unique_ptr<Impl>` combina porque Impl e incompleto no header: o `unique_ptr` nao precisa do tamanho para existir, so para ser destruido. Por isso o destrutor e declarado no header e **definido no `.cpp`**, onde Impl esta completo — esquecer isso e o erro de compilacao classico do padrao ("invalid application of sizeof to incomplete type").

**Autoverificacao:** `agora: 120` e `movido: 120`, mudar o `Impl` nao toca o header e o destrutor esta definido onde Impl e completo.
