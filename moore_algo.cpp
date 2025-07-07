#include "moore_algo.hpp"
#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace std;

vector<int> mapPos2Doc(const string& txt, char separator){
  vector<int> pos2doc(txt.size());
  int currDoc = 0;
  for(int i=0; i<txt.size(); ++i){
    if(txt[i] == separator){
      pos2doc[i] = -1;
      currDoc++;
    } else {
      pos2doc[i] = currDoc;
    }
  }
  return pos2doc;
}

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

std::set<int> mooreSearchDocs(const std::string& txt, const std::string& pat, const std::vector<int>& pos2doc) {
  set<int> docs;
  int txt_size = txt.size();
  int pat_size = pat.size();
  int badchar[NO_OF_CHARS];
  //rellenar badchar con preprocessamiento badCharHeuristic
  badCharHeuristic(pat, pat_size, badchar);

  int shift = 0;
  while(shift<=txt_size-pat_size){
    int i = pat_size-1;

    while(i>=0 && pat[i] == txt[shift+i]){
      i--;
    }

    if(i<0){
      int doc = pos2doc[shift];
      if(doc > -1){
	docs.insert(doc);
      }
      //Cambiar posición (shift) del patrón para que alinie el próximo carácter con respecto a la última aparición de éste en el patrón
      shift+= (shift+pat_size<txt_size) ? pat_size - badchar[txt[shift+pat_size]] : 1;
    } else {

      //Cambiar posición del patron para que el "bad character" en el texto se alinie con su última aparición en el patrón.
      shift += max(1, i-badchar[txt[shift+i]]);
    }
  }
    return docs;
}

void mooreSearch(const string& txt, const string& pat, char separator){
  vector<int> pos2doc = mapPos2Doc(txt, separator);
  set<int> docs = mooreSearchDocs(txt, pat, pos2doc);
  /*cout << "Patrón " << pat;
  if(docs.empty()){
    cout << " no encontrado" << endl;
  } else {
    cout << " encontrado en documentos: ";
    for(int d : docs){
      cout << d+1 << " ";
    }
    cout << endl;
  }*/
}
