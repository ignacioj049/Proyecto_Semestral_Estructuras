#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <sys/stat.h>

// Cambia esto para el tamaño deseado (en MB)
#define CHUNK_SIZE_MB 1
#define BUFFER_SIZE (CHUNK_SIZE_MB * 1024 * 1024)

using namespace std;

// Crea una carpeta si no existe
void crearCarpeta(const string& nombre) {
    CreateDirectoryA(nombre.c_str(), NULL);
}

// Corta un archivo en archivos pequeños y los guarda en out_dir con el mismo nombre + _NN
void split_file(const string& input_file, const string& output_dir, const string& base_name) {
    ifstream in(input_file, ios::binary);
    if (!in) {
        cerr << "No se pudo abrir el archivo de entrada: " << input_file << endl;
        return;
    }
    crearCarpeta(output_dir);
    vector<char> buffer(BUFFER_SIZE);
    int idx = 0;
    while (in) {
        in.read(buffer.data(), BUFFER_SIZE);
        streamsize bytes_read = in.gcount();
        if (bytes_read == 0) break;
        string out_name = output_dir + "\\" + base_name + "_" + (idx < 10 ? "0" : "") + to_string(idx);
        ofstream out(out_name, ios::binary);
        if (!out) {
            cerr << "No se pudo crear el archivo de salida: " << out_name << endl;
            break;
        }
        out.write(buffer.data(), bytes_read);
        idx++;
    }
    cout << " - " << base_name << ": " << idx << " archivos pequeños generados en " << output_dir << endl;
}

// Lista archivos en una carpeta
vector<string> listarArchivos(const string& carpeta) {
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
    return archivos;
}

// Lista subcarpetas en una carpeta
vector<string> listarSubcarpetas(const string& carpeta) {
    vector<string> subcarpetas;
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile((carpeta + "\\*").c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                strcmp(fd.cFileName, ".") != 0 &&
                strcmp(fd.cFileName, "..") != 0) {
                subcarpetas.push_back(fd.cFileName);
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
    return subcarpetas;
}

int main() {
    string datasets_dir = "datasets";
    string out_root = "datasets_pequenos";
    crearCarpeta(out_root);

    auto subcarpetas = listarSubcarpetas(datasets_dir);
    if (subcarpetas.empty()) {
        cout << "No se encontraron datasets en " << datasets_dir << endl;
        return 0;
    }
    cout << "Procesando datasets en " << datasets_dir << "...\n";
    for (const auto& ds : subcarpetas) {
        string carpeta_ds = datasets_dir + "\\" + ds;
        string out_dir = out_root + "\\" + ds;
        crearCarpeta(out_dir);
        auto archivos = listarArchivos(carpeta_ds);
        cout << "[ " << ds << " ]\n";
        for (const auto& archivo : archivos) {
            // Extrae el nombre base del archivo (sin ruta)
            size_t last_bar = archivo.find_last_of("\\/");
            string nombre_base = (last_bar != string::npos) ? archivo.substr(last_bar + 1) : archivo;
            // Remueve extensión si quieres (opcional):
            size_t punto = nombre_base.find_last_of(".");
            if (punto != string::npos) nombre_base = nombre_base.substr(0, punto);
            split_file(archivo, out_dir, nombre_base);
        }
    }
    cout << "\n¡Listo! Todos los datasets divididos en archivos pequeños en " << out_root << endl;
    return 0;
}
