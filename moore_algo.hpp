#ifndef HEADER_MOORE
#define HEADER_MOORE

#include <string>
#define NO_OF_CHARS 256

void badCharHeuristic(string str, int size, int badchar[NO_OF_CHARS]);
void mooreSearch(string txt, string pat);

#endif
