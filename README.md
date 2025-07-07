# Proyecto semestral: Detección de patrones en múltiples archivos

## Organización del proyecto:
~~~
Proyecto_Semestral_Estructuras/
│
├── datasets/               # Los datasets originales (40 archivos por dataset)
├── datasets_pequenos/      # Los datasets reducidos solo para el SuffixTree (10 archivos por dataset)
├── patrones/               # Los patrones generados (existentes/inexistentes para cada dataset)
│
├── Código para dividir datasets/      # Carpeta con el código usado para crear los datasets pequeños desde los grandes
├── Código para generar patrones/      # Carpeta con el código para generar los patrones de búsqueda
│
├── main.cpp                # Código principal del experimento
├── kmp.cpp / kmp.hpp
├── moore_algo.cpp / moore_algo.hpp
├── rabin_karp.cpp / rabin_karp.hpp
├── suffix_tree.cpp / suffix_tree.hpp
│
├── .gitignore
├── README.md              
~~~
## Requisitos:
-Compilador g++ 
-Windows (usa psapi para medir la memoria RAM)
-Tener todas las carpetas de datasets, datasets_pequenos, y patrones listas

## Compilación:
~~~
g++ -std=c++17 main.cpp moore_algo.cpp kmp.cpp rabin_karp.cpp suffix_tree.cpp -o experimento.exe -lpsapi
~~~

El ejecutable será: experimento.exe

Luego se ejecuta el experimento:
./experimento.exe

Los resultados se guardarán en la carpeta resultados/ como un archivo CSV
## Notas sobre los Datasets y Suffix Tree:

-datasets/: Cada subcarpeta contiene 40 archivos grandes (originales), para KMP, Boyer-Moore, Rabin-Karp

-datasets_pequenos/: Cada subcarpeta contiene 10 archivos pequeños, solo usados para el algoritmo SuffixTree, para evitar uso excesivo de memoria RAM
 
 >**Importante**:
  Los algoritmos tradicionales (KMP, Boyer-Moore, Rabin-Karp) se ejecutan con los archivos de la carpeta datasets/, mientras que el SuffixTree se ejecuta solo con los archivos de la carpeta datasets_pequenos/

Patrones:
En la carpeta patrones/ hay archivos <dataset>_existentes.txt y <dataset>_inexistentes.txt generados automáticamente,donde cada archivo contiene una lista de patrones (uno por línea) de longitud fija, usados como consultas de búsqueda

## Generación de patrones y división de Datasets

-Dividir datasets grandes a pequeños:
El código fuente está en la carpeta Código para dividir datasets/, y se utilizó para crear la estructura datasets_pequenos/ desde los archivos grandes

-Generar patrones automáticamente:
El código fuente está en la carpeta Código para generar patrones/, y se utilizó para generar los archivos de patrones a partir de los datasets existentes
