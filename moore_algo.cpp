#include "moore_algo.hpp"
#include <iostream>


using namespace std;

void badCharHeuristic(const string& str, int size, int badchar[NO_OF_CHARS]){
  //Inicializar index de carácter para badchar en -1 
  for(int i=0; i<NO_OF_CHARS; i++){
    badchar[i]=-1;
  }
  //Guardar posición de última ocurrencia de carácter
  for(int i=0; i<size; i++){
    badchar[(int)str[i]]=i;
  }
}

void mooreSearch(const string& txt, const string& pat){
  int txt_size = txt.size();
  int pat_size = pat.size();
  int badchar[NO_OF_CHARS];
  //rellenar badchar con preprocessamiento badCharHeuristic
  badCharHeuristic(pat, pat_size, badchar);

  int shift = 0;
  while(s<=txt_size-pat_size){
    int i = pat_size-1;

    while(i>=0 && pat[i] == txt[shift+i]){
      i--;
    }

    if(i<0){
      cout << "Patrón en posición: " << shift << endl;
      //Cambiar posición (shift) del patrón para que alinie el próximo carácter con respecto a la última aparición de éste en el patrón
      shift+= (shift+pat_size<txt_size) ? pat_size - badchar[txt[shift+pat_size]] : 1;
    } else {

      //Cambiar posición del patron para que el "bad character" en el texto se alinie con su última aparición en el patrón.
      shift += max(1, i-badchar[txt[shift+i]]);
  }
}
