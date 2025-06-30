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

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cstring>

#define NO_OF_CHARS 256

struct Node{
  Node* children[NO_OF_CHARS];
  int start; // Start of substring. Int as index in text, because we save memory by not saving extra strings 
  int* end;  // End of substring. 
  Node* suffixLink;
  int suffixIndex;
};

// Global variables
Node* lastNewNode = nullptr;  // Points to the last internal node created during a extension.
int count = 0;                // Node count.
int leafEnd = -1;             // Shared value like a end pointer for every leaf node.
int* rootEnd = nullptr;       // End of the root edge.
int* splitEnd = nullptr;      // End of a new internal node created by the division of an edge in extension. (Rule 2)
int remainingSuffixCount = 0; // Quantity of remaining suffix to insert in the tree in the current phase, controls the repetitions in extension internal loop.

//   activePoint
Node* activeNode = nullptr; // Tells us where to starts extension.
int activeEdge = -1;        // Tells us which path to take using a char in the string at activeEdge's value
int activeLength = 0;       // Length of the active suffix we have walked down activeEdge.

//   Handle multiple documents
vector<int> pos2doc;

//   Text
string text;
int size;

Node* newNode(int start, int* end){
  count++;
  Node* node = new Node;
  for(int i= 0; i<NO_OF_CHARS; i++){
    node->children[i] = nullptr;
  }
  node->start=start;
  node->end=end;
  node->suffixLink = nullptr;
  node->suffixIndex = -1;
  return node;
}

int edgeLength(Node* n){
  return *(n->end)-n->start+1;
}

int walkDown(Node* current){
  while(activeLength){
    if(activeLength >= edgeLength(current)){
      activeEdge = static_cast<int>(text[activeEdge+edgeLength(current)]) - static_cast<int>(' ');
      activeLength -= edgeLength(current);
      activeNode = current;
      return 1;
    }
    return 0;
  }
}


void extension(Node* root, int pos){
  leafEnd = pos;
  remainingSuffixCount++;
  lastNewNode = nullptr;

  while(remainingSuffixCount > 0){
    if(activeLength == 0){
      activeEdge = static_cast<int>(text[pos]) - static_cast<int>(' ');
    }
    if(activeNode->children[activeEdge] == nullptr){ //if activeNode has no edge starting with activeEdge
      activeNode->children[activeEdge] = newNode(pos, &leafEnd);

      if(lastNewNode != nullptr){
	lastNewNode->suffixLink = activeNode;
	lastNewNode = nullptr;
      }
    } else {
      Node* next = activeNode->children[activeEdge];
      if(walkDown(next)){
	continue;
      }
      if(text[next->start + activeLength] == text[pos]){
	if(lastNewNode != nullptr && activeNode != root){
	  lastNewNode->suffixLink = activeNode;
	  lastNewNode = nullptr;
	}
	activeLength++;
	break;
      }
      splitEnd = new int;
      *splitEnd = next->start+activeLength-1;

      Node* split = newNode(next->start, splitEnd);
      activeNode->children[activeEdge] = split;

      split->children[static_cast<int>(text[pos]) - static_cast<int>(' ')] = newNode(pos,&leafEnd);
      next->start += activeLength;
      split->children[activeEdge] = next;

      if(lastNewNode != nullptr){
	lastNewNode->suffixLink = split;
      }
      lastNewNode = split;
    }
    remainingSuffixCount--;
    if(activeNode == root && activeLength > 0){
      activeLength--;
      activeEdge = static_cast<int>(text[pos-remainingSuffixCount + 1]) - static_cast<int>(' ');
    } else if(activeNode != root){
      activeNode = activeNode->suffixLink;
    }
  }
}

int traverseEdge(char *str, int idx, int start, int end)
{
    int k = 0;
    //Traverse the edge with character by character matching
    for(k=start; k<=end && str[idx] != '\0'; k++, idx++)
    {
        if(text[k] != str[idx])
            return -1;  // mo match
    }
    if(str[idx] == '\0')
        return 1;  // match
    return 0;  // more characters yet to match
}

Node* doTraversal(Node *n, char* str, int idx){
    if(n == nullptr){
        return nullptr; // no match
    }
    int res = -1;
    //If node n is not root node, then traverse edge
    //from node n's parent to node n.
    if(n->start != -1)
    {
        res = traverseEdge(str, idx, n->start, *(n->end));
        if(res == -1)  //no match
            return nullptr;
        if(res == 1) //match
        {
            return n;
        }
    }
    //Get the character index to search
    idx += edgeLength(n);
    //If there is an edge from node n going out
    //with current character str[idx], traverse that edge
    if(n->children[str[idx]] != NULL)
        return doTraversal(n->children[str[idx]], str, idx);
    return nullptr;
}

void collectDocs(Node* n, unordered_set<int>& docs){
  if(n==nullptr){
    return;
  }
  if(n->suffixIndex > -1){
    int pos = n->suffixIndex;
    if(pos2doc[pos] != -1){
      docs.insert(pos2doc[pos]);
    }
  } else {
    for(int i=0; i< NO_OF_CHARS; i++){
      if(n->children[i]!=nullptr){
	collectDocs(n->children[i],docs);
      }
    }
  }
}

void checkForSubStringInDocs(Node* root, const string& pattern){
  Node* match_node = doTraversal(root,pattern.c_string(),0);
  cout << "Patrón " << pattern;
  if(!match_node){
    cout << " no encontrado";
  }
  
  unordered_set<int> docs;
  collectDocs(match_node, docs);

  if(docs.empty()){
    cout << " no encontrado";
  } else {
    cout << " encontrado en documentos: ";
    for(int d : docs){
      cout << d + 1 << " ";
    }
    cout << endl;
  }
}

// Function to free memory in post-order traversal
void freeSuffixTreeByPostOrder(Node* n) {
    if (n == nullptr)
        return;
    for (int i = 0; i < NO_OF_CHARS; i++) {
        if (n->children[i] != nullptr) {
            freeSuffixTreeByPostOrder(n->children[i]);
        }
    }
    if (n->suffixIndex == -1)
        delete n->end;
    delete n;
}

void setSuffixIndexByDFS(Node* n, int labelHeight) {
    if (n == nullptr) return;

    int leaf = 1;
    for (int i = 0; i < NO_OF_CHARS; i++) {
        if (n->children[i] != nullptr) {
            if (leaf == 1 && n->start != -1)
            leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight + edgeLength(n->children[i]));
        }
    }
    if (leaf == 1) {
        n->suffixIndex = size - labelHeight;
    }
}

void mapPos2Doc(char separator){
  pos2doc.resize(size);
  int currDoc = 0;
  for(int i=0; i<size; ++i){
    if(text[i] == separator){
      pos2doc[i] = -1;
      currDoc++;
    } else {
      pos2doc[i] = currDoc;
    }
  }
}


Node* buildTree(string& txt){
  text = txt;
  size = text.size();
  rootEnd = new int(-1);
  *rootEnd = -1;
  Node* root = newNode(-1,rootEnd);
  activeNode = root;
  for(int i=0; i<size; i++){
    extension(root, i);
  }
  int labelHeight = 0;
  setSuffixIndexByDFS(root,labelHeight);
  
  return root;
}
