#ifndef SUFFIX_TREE_HPP
#define SUFFIX_TREE_HPP

#include <string>
#include <vector>
#include <unordered_set>

#define NO_OF_CHARS 256

// Estructura de nodo del árbol
struct Node
{
    Node *children[NO_OF_CHARS];
    int start;
    int *end;
    Node *suffixLink;
    int suffixIndex;
};

// Funciones principales
Node *buildTree(std::string &txt);
void mapPos2Doc(char separator);
void checkForSubStringInDocs(Node *root, const std::string &pattern);
void freeSuffixTreeByPostOrder(Node *n);

#endif // SUFFIX_TREE_HPP
