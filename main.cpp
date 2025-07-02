#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <set>
#include "kmp.hpp"
#include "moore_algo.hpp"
#include "rabin_karp.hpp"
#include "suffix_tree.hpp" 

using namespace std;
namespace fs = filesystem;

int main()
{
    string carpeta = "./documentos";
    string textoTotal;
    vector<string> nombresDocumentos;
    char separador = '$';

    // Leer documentos y concatenarlos
    for (const auto &entry : fs::directory_iterator(carpeta))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
        {
            ifstream archivo(entry.path());
            stringstream buffer;
            buffer << archivo.rdbuf();
            textoTotal += buffer.str() + separador;
            nombresDocumentos.push_back(entry.path().filename().string());
        }
    }

    // Ingresar patrón
    string patron;
    cout << "Ingrese el patrón a buscar: ";
    getline(cin, patron);

    // Mapeo posición → documento
    vector<int> pos2doc = mapPos2Doc(textoTotal, separador);

    cout << "\n--- Búsqueda con KMP ---\n";
    vector<int> res_kmp = buscar_kmp(patron, textoTotal);
    set<int> docs_kmp;
    for (int pos : res_kmp)
        if (pos2doc[pos] != -1)
            docs_kmp.insert(pos2doc[pos]);
    for (int d : docs_kmp)
        cout << "Encontrado en: " << nombresDocumentos[d] << endl;

    cout << "\n--- Búsqueda con Boyer-Moore ---\n";
    mooreSearch(textoTotal, patron, separador); // Esta función ya imprime los documentos

    cout << "\n--- Búsqueda con Rabin-Karp ---\n";
    vector<int> res_rk = buscar_rabin_karp(patron, textoTotal, 101);
    set<int> docs_rk;
    for (int pos : res_rk)
        if (pos2doc[pos] != -1)
            docs_rk.insert(pos2doc[pos]);
    for (int d : docs_rk)
        cout << "Encontrado en: " << nombresDocumentos[d] << endl;

    cout << "\n--- Búsqueda con Árbol de Sufijos ---\n";
    mapPos2Doc(separador); // global para el árbol
    Node *root = buildTree(textoTotal);
    checkForSubStringInDocs(root, patron);
    freeSuffixTreeByPostOrder(root);

    return 0;
}
