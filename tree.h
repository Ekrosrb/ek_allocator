#include<iostream>
#include <cstdlib>
class Node {
public:
    void* ptr = nullptr;
    size_t key;
    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;
    Node *next = nullptr;
};

Node* newNode(size_t key, void* ptr, Node* node);

Node* insert(Node* root, size_t key, void* ptr, Node* node);

Node * minValueNode(Node* node);

Node* deleteNode(Node* root, size_t key, void* ptr);
// C function to search a given key in a given BST
Node* search(Node* root, size_t key);

void print(Node* root, int space);
