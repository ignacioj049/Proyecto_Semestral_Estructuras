#ifndef SUFFIX_TREE_HPP
#define SUFFIX_TREE_HPP

#include <string>
#include <vector>
#include <unordered_set>

// Estructura de nodo del árbol
struct Node {
    Node* children[256];
    Node* suffixLink;
    int start;
    int* end;
    int suffixIndex;
    Node() : suffixLink(nullptr), start(-1), end(nullptr), suffixIndex(-1) {
        for (int i = 0; i < 256; ++i) children[i] = nullptr;
    }
};

// Funciones principales
Node* buildTree(std::string& txt);
void freeSuffixTreeByPostOrder(Node* n);
void setSuffixIndexByDFS(Node* n, int labelHeight, int depth=0);
void mapPos2Doc(char separator);
void checkForSubStringInDocs(Node* root, const std::string& pattern);
Node* doTraversal(Node* n, const char* str, int idx);
void collectDocs(Node* n, std::unordered_set<int>& docs);

#endif // SUFFIX_TREE_HPP
