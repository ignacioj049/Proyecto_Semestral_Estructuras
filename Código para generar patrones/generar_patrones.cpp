#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <random>
#include <algorithm>

namespace fs = std::filesystem;

// Mostrar todos los archivos encontrados en una carpeta
void listar_todos_los_archivos(const std::string& carpeta) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (entry.is_regular_file()) {
            std::cout << "Archivo: " << entry.path().filename().string() << std::endl;
            count++;
        }
    }
    std::cout << "Total archivos encontrados: " << count << std::endl;
}

// Leer y concatenar TODO el contenido de todos los archivos
std::string leer_y_concatenar(const std::string& carpeta) {
    std::string texto_total;
    int archivos_contados = 0;
    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path(), std::ios::binary);
            if (!file) {
                std::cout << "No se pudo abrir: " << entry.path() << std::endl;
                continue;
            }
            std::string contenido((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            texto_total += contenido;
            archivos_contados++;
        }
    }
    std::cout << "Archivos leidos en " << carpeta << ": " << archivos_contados << std::endl;
    return texto_total;
}

// Generar subcadenas únicas de longitud fija desde el texto total
std::unordered_set<std::string> extraer_subcadenas(const std::string& texto, int sub_len) {
    std::unordered_set<std::string> subs;
    if (texto.size() < sub_len) return subs;
    for (size_t i = 0; i + sub_len <= texto.size(); ++i) {
        subs.insert(texto.substr(i, sub_len));
        if (subs.size() >= 50000) break;
    }
    return subs;
}

// Generar palabra aleatoria (inexistente)
std::string palabra_aleatoria(int len) {
    static std::mt19937 rng{std::random_device{}()};
    static const std::string chars = "abcdefghijklmnopqrstuvwxyz";
    std::string res;
    for (int i = 0; i < len; ++i)
        res += chars[rng() % chars.size()];
    return res;
}

int main() {
    std::string datasets_path = "datasets_pequenos";
    std::string patrones_path = "patrones";
    int cantidad_patrones = 500;
    int longitud_patron = 10; // Cambia esto si necesitas otro largo

    fs::create_directories(patrones_path);

    for (const auto& ds_entry : fs::directory_iterator(datasets_path)) {
        if (!ds_entry.is_directory()) continue;
        std::string nombre_dataset = ds_entry.path().filename().string();
        std::string carpeta_dataset = ds_entry.path().string();
        std::cout << "Procesando dataset: " << nombre_dataset << std::endl;

        // Listar todos los archivos
        listar_todos_los_archivos(carpeta_dataset);

        // Leer y concatenar el contenido de los archivos
        std::string texto_total = leer_y_concatenar(carpeta_dataset);
        std::cout << "Tamaño texto concatenado: " << texto_total.size() << std::endl;

        if (texto_total.empty()) {
            std::cout << "ATENCIÓN: ¡No se encontró texto en el dataset " << nombre_dataset << "!\n";
            continue;
        }

        // Mostrar primeros 200 caracteres (diagnóstico)
        std::cout << "Primeros 200 caracteres: ";
        std::cout << texto_total.substr(0, std::min((size_t)200, texto_total.size())) << std::endl;

        // Extraer subcadenas únicas
        auto subs = extraer_subcadenas(texto_total, longitud_patron);
        std::vector<std::string> subs_vec(subs.begin(), subs.end());
        std::cout << "Subcadenas únicas encontradas (" << subs_vec.size() << ") en " << nombre_dataset << std::endl;

        // Guardar patrones existentes
        if (subs_vec.empty()) {
            std::cout << "ATENCIÓN: No se generaron patrones EXISTENTES para " << nombre_dataset << " (quizá archivos demasiado cortos o vacíos)\n";
        } else {
            std::shuffle(subs_vec.begin(), subs_vec.end(), std::mt19937{std::random_device{}()});
            if (subs_vec.size() > cantidad_patrones)
                subs_vec.resize(cantidad_patrones);

            std::ofstream f_exist(patrones_path + "/" + nombre_dataset + "_existentes_peq.txt");
            for (const auto& w : subs_vec)
                f_exist << w << "\n";
            f_exist.close();
            std::cout << "Patrones EXISTENTES de " << nombre_dataset << " generados exitosamente!" << std::endl;
        }

        // Guardar patrones inexistentes
        std::unordered_set<std::string> subs_set(subs_vec.begin(), subs_vec.end());
        std::ofstream f_no(patrones_path + "/" + nombre_dataset + "_inexistentes_peq.txt");
        int generados = 0;
        while (generados < cantidad_patrones) {
            std::string palabra = palabra_aleatoria(longitud_patron);
            if (subs_set.count(palabra) == 0) {
                f_no << palabra << "\n";
                generados++;
            }
        }
        f_no.close();
        std::cout << "Patrones INEXISTENTES de " << nombre_dataset << " generados exitosamente!" << std::endl;
    }

    std::cout << "¡Todos los patrones fueron generados para todos los datasets!" << std::endl;
    return 0;
}
