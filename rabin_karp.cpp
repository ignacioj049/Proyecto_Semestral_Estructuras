#include "rabin_karp.hpp"

std::vector<int> buscar_rabin_karp(const std::string& patron, const std::string& texto, int q) {
    std::vector<int> resultados;
    int d = 256;
    int m = patron.size();
    int n = texto.size();
    int h = 1;
    int p = 0, t = 0;

    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    for (int i = 0; i < m; i++) {
        p = (d * p + patron[i]) % q;
        t = (d * t + texto[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (texto[i + j] != patron[j]) {
                    match = false;
                    break;
                }
            }
            if (match) resultados.push_back(i);
        }

        if (i < n - m) {
            t = (d * (t - texto[i] * h) + texto[i + m]) % q;
            if (t < 0) t += q;
        }
    }

    return resultados;
}
