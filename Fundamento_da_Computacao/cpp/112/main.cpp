#include "../bib1.hpp"
#include <unordered_map>

int converte(int x, const std::string& u1, const std::string& u2) {
    std::unordered_map<std::string, double> m = {
        {"km",1000},{"hm",100},{"dam",10},{"m",1},{"dm",0.1},{"cm",0.01},{"mm",0.001},
        {"kg",1000},{"hg",100},{"dag",10},{"g",1},{"dg",0.1},{"cg",0.01},{"mg",0.001},
        {"kl",1000},{"hl",100},{"dal",10},{"l",1},{"dl",0.1},{"cl",0.01},{"ml",0.001},
    };
    return (int)(x * m[u1] / m[u2]);
}

int main() {
    while (true) {
        try {
            int x = std::stoi(entrada());
            std::string u1 = entrada(), u2 = entrada();
            std::cout << converte(x, u1, u2) << "\n";
        } catch (...) { break; }
    }
}
