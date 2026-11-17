#ifndef BIB1_HPP
#define BIB1_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <stdexcept>

// Lê uma linha de stdin ignorando linhas vazias.
// Lança std::runtime_error em EOF.
inline std::string entrada() {
    std::string line;
    while (true) {
        if (!std::getline(std::cin, line))
            throw std::runtime_error("EOF");
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            continue;
        auto end = line.find_last_not_of(" \t\r\n");
        return line.substr(start, end - start + 1);
    }
}

// Formata double no estilo do Python (shortest round-trip).
inline std::string python_float(double x) {
    if (std::isinf(x)) return x < 0 ? "-inf" : "inf";
    if (std::isnan(x)) return "nan";
    char buf[64];
    double ax = std::fabs(x);
    if (ax == 0.0 || (ax >= 1e-4 && ax < 1e16)) {
        for (int d = 0; d <= 20; d++) {
            std::snprintf(buf, sizeof(buf), "%.*f", d, x);
            if (std::strtod(buf, nullptr) == x) {
                if (d == 0) return std::string(buf) + ".0";
                std::string s(buf);
                while (s.size() > 1 && s.back() == '0' && s[s.size()-2] != '.')
                    s.pop_back();
                return s;
            }
        }
    }
    for (int prec = 1; prec <= 17; prec++) {
        std::snprintf(buf, sizeof(buf), "%.*g", prec, x);
        if (std::strtod(buf, nullptr) == x) return std::string(buf);
    }
    std::snprintf(buf, sizeof(buf), "%.17g", x);
    return std::string(buf);
}

#endif
