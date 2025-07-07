// Implementation of Ukkonen's Sufix Tree Construction
// bibliography: https://www.geeksforgeeks.org/dsa/ukkonens-suffix-tree-construction-part-1/
//               read from part 1 to 6
//               https://www.geeksforgeeks.org/dsa/suffix-tree-application-2-searching-all-patterns/
//               for search patters in a suffix tree
// by Chloe

// Tree rules
//  Root can have zero, one or more children
//  Each internal node, other than the root, has at least two children
//  Each edge is labelled with a nonempty substring of S
//  No two edges from the same node start with the same character

// Extension rules
//  Rule 1: If the path from root labelled S[j..i] ends at leaf edge (i.e. S[i] is last character on leaf edge) then character S[i+1] is just added to the end of the label on that leaf edge.
//  Rule 2: If the path from the root labelled S[j..i] ends at non-leaf edge and next character is not S[i+1], create:
//    - A new internal node at that point
//    - A new leaf edge from internal node starting with S[i+1].
//  Rule 3: If the path from the root labelled S[j..i] ends at non-leaf edge and next character is s[i+1] (already in tree), do nothing.

//  Important note, given a node, there will only one edge starting from one and only one edge starting  from one character. There will not be more than one edges going out of any node, starting with same character.
#include "suffix_tree.hpp"
#include <string>
#include <vector>
#include <unordered_set>

#define NO_OF_CHARS 256


// Global variables
std::vector<int> pos2doc;
std::string text;
int size;

Node* lastNewNode = nullptr;  // Points to the last internal node created during a extension.
int leafEnd = -1;             // Shared value like a end pointer for every leaf node.
int* rootEnd = nullptr;       // End of the root edge.
int remainingSuffixCount = 0; // Quantity of remaining suffix to insert in the tree in the current phase, controls the repetitions in extension internal loop.

//   activePoint
Node* activeNode = nullptr; // Tells us where to starts extension.
int activeEdge = -1;        // Tells us which path to take using a char in the string at activeEdge's value
int activeLength = 0;       // Length of the active suffix we have walked down activeEdge.

Node* newNode(int start, int* end){
  Node* node = new Node;
  node->start=start;
  node->end=end;
  node->suffixLink = nullptr;
  node->suffixIndex = -1;
  for(int i= 0; i<NO_OF_CHARS; i++){
    node->children[i] = nullptr;
  }
  return node;
}

int edgeLength(Node* n){
  return *(n->end)-n->start+1;
}

int walkDown(Node* current){
    if(activeLength >= edgeLength(current)){
      activeEdge += edgeLength(current);
      activeLength -= edgeLength(current);
      activeNode = current;
      return 1;
    }
    return 0;
}


void extension(Node* root, int pos){
  leafEnd = pos;
  remainingSuffixCount++;
  lastNewNode = nullptr;

  while (remainingSuffixCount > 0) {
      if (activeLength == 0)
          activeEdge = pos;
      char cur = text[activeEdge];
      if (activeNode->children[(unsigned char)cur] == nullptr) {            
        activeNode->children[(unsigned char)cur] = newNode(pos, &leafEnd);
          if (lastNewNode != nullptr) {
              lastNewNode->suffixLink = activeNode;
              lastNewNode = nullptr;
          }
      } else {
          Node* next = activeNode->children[(unsigned char)cur];            
          if (walkDown(next)) continue;
          if (text[next->start + activeLength] == text[pos]) {
              if (lastNewNode && activeNode != root) lastNewNode->suffixLink = activeNode;
              activeLength++;
              break;
          }
          int* splitEnd = new int(next->start + activeLength - 1);            
          Node* split = newNode(next->start, splitEnd);
          activeNode->children[(unsigned char)cur] = split;
          split->children[(unsigned char)text[pos]] = newNode(pos, &leafEnd);
          next->start += activeLength;
          split->children[(unsigned char)text[next->start]] = next;
          if (lastNewNode) lastNewNode->suffixLink = split;            
          lastNewNode = split;
        }
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        } else if (activeNode != root)
            activeNode = activeNode->suffixLink ? activeNode->suffixLink : root;
    }
}

void mapPos2Doc(char separator) {
    pos2doc.resize(size);
    int currDoc = 0;
    for (int i = 0; i < size; ++i) {
        if (text[i] == separator) {
            pos2doc[i] = -1;
            currDoc++;
        } else {
            pos2doc[i] = currDoc;
        }
    }
}

int traverseEdge(const char* str, int idx, int start, int end) {
    //Traverse the edge with character by character matching
    for(int k=start; k<=end && str[idx] != '\0'; k++, idx++)
    {
        if(text[k] != str[idx])
            return -1;  // mo match
    }
    if(str[idx] == '\0')
        return 1;  // match
    return 0;  // more characters yet to match
}

Node* doTraversal(Node* n, const char* str, int idx){
    if (n == nullptr) return nullptr;
    int res = -1;
    if (n->start != -1) {
        res = traverseEdge(str, idx, n->start, *(n->end));
        if (res == -1) return nullptr;
        if (res == 1) return n;
    }
    idx += edgeLength(n);
    if (str[idx] == '\0') return nullptr;
    if (n->children[(unsigned char)str[idx]])
        return doTraversal(n->children[(unsigned char)str[idx]], str, idx);
    return nullptr;
}


void collectDocs(Node* n, std::unordered_set<int>& docs) {
    if (n == nullptr) return;
    if (n->suffixIndex > -1) {
        int pos = n->suffixIndex;
        if (pos < (int)pos2doc.size() && pos2doc[pos] != -1)
            docs.insert(pos2doc[pos]);
    } else {
        for (int i = 0; i < NO_OF_CHARS; i++) {
            if (n->children[i]) collectDocs(n->children[i], docs);
        }
    }
}

void checkForSubStringInDocs(Node* root, const std::string& pattern) {
    if (pattern.empty()) return;
    Node* match_node = doTraversal(root, pattern.c_str(), 0);
    std::unordered_set<int> docs;
    collectDocs(match_node, docs);
    // No imprime nada, solo busca
}

void setSuffixIndexByDFS(Node* n, int labelHeight, int depth) {
    if (!n) return;
    int leaf = 1;
    for (int i = 0; i < NO_OF_CHARS; i++) {
        if (n->children[i]) {
            if (leaf == 1 && n->start != -1) leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight + edgeLength(n->children[i]), depth + 1);
        }
    }
    if (leaf) n->suffixIndex = size - labelHeight;
}

// Function to free memory in post-order traversal
void freeSuffixTreeByPostOrder(Node* n) {
    if (!n) return;
    for (int i = 0; i < NO_OF_CHARS; ++i)
        if (n->children[i]) freeSuffixTreeByPostOrder(n->children[i]);
    if (n->suffixIndex == -1 && n->end) delete n->end;
    delete n;
}

Node* buildTree(std::string& txt) {
    text = txt;
    size = text.size();
    if (rootEnd) { delete rootEnd; rootEnd = nullptr; }
    lastNewNode = nullptr; leafEnd = -1; remainingSuffixCount = 0;
    activeNode = nullptr; activeEdge = -1; activeLength = 0;
    pos2doc.clear();
    rootEnd = new int(-1);
    *rootEnd = -1;
    Node* root = newNode(-1, rootEnd);
    activeNode = root;
    for (int i = 0; i < size; ++i) extension(root, i);
    setSuffixIndexByDFS(root, 0, 0);
    return root;
}