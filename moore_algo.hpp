#ifndef MOORE_ALGO_HPP
#define MOORE_ALGO_HPP
#pragma once
#include <string>
#include <vector>
#include <set>
#define NO_OF_CHARS 256

std::set<int> mooreSearchDocs(const std::string& txt, const std::string& pat, const std::vector<int>& pos2doc);
void badCharHeuristic(const std::string& str, int size, int badchar[256]);
void mooreSearch(const std::string& txt, const std::string& pat, char separador = '$');

#endif
