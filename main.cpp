#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <windows.h>
#include <psapi.h>

#include "kmp.hpp"
#include "moore_algo.hpp"
#include "rabin_karp.hpp"
#include "suffix_tree.hpp"

using namespace std;

//Medición de la memoria usada
size_t getCurrentRSS() {
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize; // bytes
}

//Función para listar todos los archivos
vector<string> listarArchivosTxt(const string& carpeta) {
    vector<string> archivos;
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile((carpeta + "\\*").c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                archivos.push_back(carpeta + "\\" + fd.cFileName);
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
    sort(archivos.begin(), archivos.end());
    return archivos;
}

//Función para crear la carpeta
void crearCarpeta(const string& nombre) {
    CreateDirectory(nombre.c_str(), NULL);
}

//Para leer las líneas
vector<string> leerLineas(const string& archivo, int max_lineas = -1) {
    ifstream file(archivo);
    vector<string> lineas;
    string linea;
    int count = 0;
    while (getline(file, linea)) {
        if (!linea.empty()) {
            lineas.push_back(linea);
            count++;
            if (max_lineas > 0 && count >= max_lineas) break;
        }
    }
    return lineas;
}

//Función para concatenar los archivos
string concatArchivos(const vector<string>& archivos, char separador) {
    string texto;
    for (const auto& archivo : archivos) {
        ifstream f(archivo, ios::binary);
        if (!f) {
            cerr << "No se pudo abrir: " << archivo << endl;
            continue;
        }
        string contenido((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
        texto += contenido + separador;
    }
    return texto;
}

//Función para correr el experimento
void correr_experimento(
    const string& dataset,
    const vector<string>& archivos,
    const vector<string>& patrones,
    ofstream& csv,
    const string& algoritmo,
    const string& tipo_patron
) {
    int REPETICIONES = 5;
    char separador = '$';
    vector<double> tiempos;
    vector<size_t> memorias;
    double t_construccion = 0.0;

    string texto = concatArchivos(archivos, separador);

    for (int rep = 0; rep < REPETICIONES; ++rep) {
        auto inicio = chrono::high_resolution_clock::now();
        size_t mem_ini = getCurrentRSS();

        if (algoritmo == "SuffixTree") {
            auto inicio_construccion = chrono::high_resolution_clock::now();
            Node* root = buildTree(texto);
            auto fin_construccion = chrono::high_resolution_clock::now();
            t_construccion += chrono::duration<double, milli>(fin_construccion - inicio_construccion).count();

            size_t mem_fin = getCurrentRSS();
            size_t consumo_mem = (mem_fin >= mem_ini) ? (mem_fin - mem_ini) : 0;
            memorias.push_back(consumo_mem);

            mapPos2Doc(separador);
            for (const auto& patron : patrones)
                checkForSubStringInDocs(root, patron);

            freeSuffixTreeByPostOrder(root);
        } else {
            if (algoritmo == "KMP") {
                for (const auto& patron : patrones)
                    buscar_kmp(patron, texto);
            } else if (algoritmo == "BoyerMoore") {
                for (const auto& patron : patrones)
                    mooreSearch(texto, patron, separador);
            } else if (algoritmo == "RabinKarp") {
                for (const auto& patron : patrones)
                    buscar_rabin_karp(patron, texto);
            }
            size_t mem_fin = getCurrentRSS();
            size_t consumo_mem = (mem_fin >= mem_ini) ? (mem_fin - mem_ini) : 0;
            memorias.push_back(consumo_mem);
        }

        auto fin = chrono::high_resolution_clock::now();
        double tiempo_ms = chrono::duration<double, milli>(fin - inicio).count();
        tiempos.push_back(tiempo_ms);
    }

    double tiempo_promedio = accumulate(tiempos.begin(), tiempos.end(), 0.0) / REPETICIONES;
    size_t memoria_promedio = accumulate(memorias.begin(), memorias.end(), (size_t)0) / REPETICIONES;
    double tiempo_construccion_prom = (algoritmo == "SuffixTree") ? t_construccion / REPETICIONES : 0;

    csv << dataset << "," << algoritmo << "," << archivos.size() << "," << patrones.size() << "," << tipo_patron << ","
        << tiempo_promedio << "," << tiempo_construccion_prom << "," << memoria_promedio << endl;

    cout << "Ok " << algoritmo << " | Docs: " << archivos.size()
         << " | Patrones: " << patrones.size() << " | " << tipo_patron
         << " | Tiempo: " << tiempo_promedio << " ms | Memoria: " << memoria_promedio << " bytes" << endl;
}

//El main
int main() {
    vector<string> datasets = {"English"};
    vector<int> cantidades_docs_normales = {2, 5, 10, 20, 40}; // Para KMP, BM, RK
    vector<int> cantidades_docs_suffix = {2, 5, 10}; // Para SuffixTree
    vector<int> cantidades_patrones = {5, 10, 50, 100, 200};
    vector<string> algoritmos = {"BoyerMoore", "RabinKarp"};
    vector<string> tipos_patron = {"existentes", "inexistentes"};

    string carpeta_datasets = "datasets";
    string carpeta_datasets_peq = "datasets_pequenos";
    string carpeta_patrones = "patrones";
    string carpeta_resultados = "resultados";

    crearCarpeta(carpeta_resultados);

    ofstream csv(carpeta_resultados + "/resultados.csv");
    csv << "Dataset,Algoritmo,NDocs,NPatrones,TipoPatron,TiempoBusqueda(ms),TiempoConstruccion(ms),Memoria(bytes)" << endl;

    for (const auto& dataset : datasets) {
        cout << "Dataset: " << dataset << endl;

        for (const auto& algoritmo : algoritmos) {
            // Selecciona el vector de cantidades_docs según el algoritmo
            const vector<int>& cantidades_docs = (algoritmo == "SuffixTree") ? cantidades_docs_suffix : cantidades_docs_normales;
            string carpeta = (algoritmo == "SuffixTree")
                             ? carpeta_datasets_peq + "\\" + dataset
                             : carpeta_datasets + "\\" + dataset;

            vector<string> archivos = listarArchivosTxt(carpeta);
            sort(archivos.begin(), archivos.end());

            for (int n_docs : cantidades_docs) {
                if (n_docs > static_cast<int>(archivos.size())) {
                    cout << "No hay suficientes archivos (" << archivos.size() << ") para " << n_docs << " docs en dataset " << dataset << " (" << algoritmo << ")" << endl;
                    continue;
                }
                vector<string> subset_docs(archivos.begin(), archivos.begin() + n_docs);

                for (int n_patrones : cantidades_patrones) {
                    for (const auto& tipo_patron : tipos_patron) {
                        string archivo_patrones = carpeta_patrones + "\\" + dataset + "_" + tipo_patron + ".txt";
                        auto patrones = leerLineas(archivo_patrones, n_patrones);

                        if (patrones.size() < n_patrones) {
                            cout << "Archivo de patrones " << archivo_patrones << " tiene solo " << patrones.size() << " patrones (se pidieron " << n_patrones << ")" << endl;
                            continue;
                        }

                        cout << "Ejecutando " << algoritmo << " sobre " << n_docs << " docs y " << n_patrones << " patrones." << endl;
                        correr_experimento(dataset, subset_docs, patrones, csv, algoritmo, tipo_patron);
                    }
                }
            }
        }
    }
    csv.close();
    cout << "Experimentos completos'" << endl;
    return 0;
}
