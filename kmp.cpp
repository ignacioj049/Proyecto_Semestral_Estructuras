#include "kmp.hpp"

std::vector<int> calcularLPS(const std::string& patron) {
    int m = patron.size();
    std::vector<int> lps(m, 0);
    int len = 0, i = 1;

    while (i < m) {
        if (patron[i] == patron[len]) {
            len++;
            lps[i++] = len;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i++] = 0;
            }
        }
    }
    return lps;
}

std::vector<int> buscar_kmp(const std::string& patron, const std::string& texto) {
    std::vector<int> resultados;
    int m = patron.size(), n = texto.size();
    std::vector<int> lps = calcularLPS(patron);
    int i = 0, j = 0;

    while (i < n) {
        if (patron[j] == texto[i]) {
            i++; j++;
        }
        if (j == m) {
            resultados.push_back(i - j);
            j = lps[j - 1];
        } else if (i < n && patron[j] != texto[i]) {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }

    return resultados;
}
