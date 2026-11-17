#include <cassert>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

struct Cheia {
    char a;
    double b;
    char c;
};

struct Compacta {
    double b;
    char a;
    char c;
};

// Régua de syscall: um milhao de getpid.
static double medir_getpid(int repeticoes) {
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < repeticoes; ++i) {
        volatile pid_t p = getpid();
        (void)p;
    }
    const auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(t1 - t0).count() / repeticoes;
}

// Conta regioes totais e executaveis em /proc/self/maps.
static void resumir_mapas(int &total, int &exec) {
    std::ifstream mapas("/proc/self/maps");
    assert(mapas && "sem /proc/self/maps: rode em Linux/WSL");
    std::string linha;
    total = 0;
    exec = 0;
    while (std::getline(mapas, linha)) {
        ++total;
        if (linha.find("r-xp") != std::string::npos) {
            ++exec;
        }
    }
}

int main() {
    static_assert(sizeof(Cheia) == 24, "layout esperado: 1+7+8+1+7");
    static_assert(sizeof(Compacta) == 16, "layout esperado: 8+1+1+6");
    static_assert(alignof(Cheia) == 8, "alinhada a double");
    std::cout << "sizeof(Cheia)=" << sizeof(Cheia)
              << " sizeof(Compacta)=" << sizeof(Compacta) << "\n";

    // 1. Processo filho com codigo de saida conhecido.
    const pid_t filho = fork();
    assert(filho >= 0);
    if (filho == 0) {
        _exit(7);
    }
    int estado = 0;
    assert(waitpid(filho, &estado, 0) == filho);
    assert(WIFEXITED(estado) && WEXITSTATUS(estado) == 7);
    std::cout << "filho colhido: codigo=" << WEXITSTATUS(estado) << "\n";

    // 2. Régua de syscall.
    const int rep = 1000000;
    std::cout << "getpid: " << medir_getpid(rep) * 1e9 << " ns/chamada ("
              << rep << " repeticoes)\n";

    // 3. Mapa de memoria do proprio processo.
    int total = 0, exec = 0;
    resumir_mapas(total, exec);
    std::cout << "regioes mapeadas: " << total << ", executaveis: " << exec << "\n";
    assert(total > 0 && exec > 0);

    // 4. Faltas de pagina: menores (sem I/O) x maiores (com I/O).
    struct rusage antes{}, depois{};
    assert(getrusage(RUSAGE_SELF, &antes) == 0);
    // new[] nao inicializa: as paginas nascem no toque abaixo, uma falta por pagina.
    // (vector(n, 0) zeraria tudo na construcao e estragaria a medicao.)
    const std::size_t tamanho = 8u * 1024u * 1024u;
    unsigned char *bloco = new unsigned char[tamanho];
    const long pagina = sysconf(_SC_PAGESIZE);
    assert(pagina > 0);
    for (std::size_t i = 0; i < tamanho; i += (std::size_t)pagina) {
        bloco[i] = 1; // uma falta menor por pagina nova
    }
    assert(getrusage(RUSAGE_SELF, &depois) == 0);
    std::cout << "faltas menores: " << antes.ru_minflt << " -> " << depois.ru_minflt << "\n";
    std::cout << "faltas maiores: " << antes.ru_majflt << " -> " << depois.ru_majflt << "\n";
    assert(depois.ru_minflt > antes.ru_minflt);
    delete[] bloco;

    // 5. Entrada nao confiavel contida no filho.
    const pid_t f2 = fork();
    assert(f2 >= 0);
    if (f2 == 0) {
        execl("/nao/existe/de/jeito/nenhum", "x", static_cast<char *>(nullptr));
        _exit(127); // execl so retorna em erro
    }
    int e2 = 0;
    assert(waitpid(f2, &e2, 0) == f2);
    assert(WIFEXITED(e2) && WEXITSTATUS(e2) == 127);
    std::cout << "exec falho contido: codigo=" << WEXITSTATUS(e2) << "\n";
    return 0;
}
