#include<iostream>
#include <cstdlib>
class Node {
public:
    void* ptr = nullptr;
    size_t key;
    Node *left = nullptr;
    Node *right = nullptr;
    Node *next = nullptr;
    size_t height;
};

// A utility function to get maximum
// of two integers
size_t max(size_t a, size_t b);
// A utility function to get height
// of the tree
size_t height(Node *N);
/* Helper function that allocates a
   new node with the given key and
   NULL left and right pointers. */
Node* newNode(size_t key, void* ptr, Node* node);
// A utility function to right
// rotate subtree rooted with y
// See the diagram given above.
Node *rightRotate(Node *y);
// A utility function to left
// rotate subtree rooted with x
// See the diagram given above.
Node *leftRotate(Node *x);
// Get Balance factor of node N
size_t getBalance(Node *N);
Node* insert(Node* root, size_t key, void* ptr, Node* node);
/* Given a non-empty binary search tree,
return the node with minimum key value
found in that tree. Note that the entire
tree does not need to be searched. */
Node * minValueNode(Node* node);
// Recursive function to delete a node
// with given key from subtree with
// given root. It returns root of the
// modified subtree.
Node* deleteNode(Node* root, size_t key);
// A utility function to print preorder
// traversal of the tree.
// The function also prints height
// of every node
void preOrder(Node *root);
// C function to search a given key in a given BST
Node* search(Node* root, size_t key);
