// C++ program to delete a node from AVL Tree
#include "avl_tree.h"
using namespace std;

size_t height(Node *N)
{
    if (N == nullptr || !N->ptr)
        return 0;
    return N->height;
}

size_t max(size_t a, size_t b)
{
    return (a > b)? a : b;
}
Node* newNode(size_t key, void* ptr, Node* node)
{
    node->ptr = ptr;
    node->key = key;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1; // new node is initially
    // added at leaf
    return node;
}

Node *rightRotate(Node *y)
{
    Node *x = y->left;
    Node *T2 = x->right;

    // Perform rotation 
    x->right = y;
    y->left = T2;

    // Update heights 
    y->height = max(height(y->left),
                    height(y->right)) + 1;
    x->height = max(height(x->left),
                    height(x->right)) + 1;

    // Return new root 
    return x;
}

Node *leftRotate(Node *x)
{
    Node *y = x->right;
    Node *T2 = y->left;

    // Perform rotation 
    y->left = x;
    x->right = T2;

    // Update heights 
    x->height = max(height(x->left),
                    height(x->right)) + 1;
    y->height = max(height(y->left),
                    height(y->right)) + 1;

    // Return new root 
    return y;
}

size_t getBalance(Node *N)
{
    if (N == nullptr)
        return 0;
    return height(N->left) -
           height(N->right);
}

Node* insert(Node* root, size_t key, void* ptr, Node* node)
{
    /* 1. Perform the normal BST rotation */
    if (root == nullptr)
        return(newNode(key, ptr, node));

    if (key < root->key)
        root->left = insert(root->left, key, ptr, node);
    else if (key > root->key)
        root->right = insert(root->right, key, ptr, node);
    else { // Equal keys not allowed
        Node *n = root;
        while (n->next != nullptr) {
            n = n->next;
        }
        n->next = newNode(key, ptr, node);
        return root;
    }
    /* 2. Update height of this ancestor root */
    root->height = 1 + max(height(root->left),
                           height(root->right));

    /* 3. Get the balance factor of this 
        ancestor root to check whether
        this root became unbalanced */
    int balance = getBalance(root);

    // If this root becomes unbalanced,
    // then there are 4 cases 

    // Left Left Case 
    if (balance > 1 && key < root->left->key)
        return rightRotate(root);

    // Right Right Case 
    if (balance < -1 && key > root->right->key)
        return leftRotate(root);

    // Left Right Case 
    if (balance > 1 && key > root->left->key)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Left Case 
    if (balance < -1 && key < root->right->key)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    /* return the (unchanged) root pointer */
    return root;
}

Node * minValueNode(Node* node)
{
    Node* current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != nullptr)
        current = current->left;

    return current;
}

Node* deleteNode(Node* root, size_t key)
{

    // STEP 1: PERFORM STANDARD BST DELETE 
    if (root == nullptr)
        return root;

    if ( key < root->key )
        root->left = deleteNode(root->left, key);
    else if( key > root->key )
        root->right = deleteNode(root->right, key);
    else {
        if(root->next != nullptr){
            Node* n = root->next;
            root->next = n->next;
            return n;
        }
        // node with only one child or no child 
        if( (root->left == nullptr) || (root->right == nullptr) ){
            Node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {// One child case
                *root = *temp; // Copy the contents of
                // the non-empty child
            }
            temp->ptr = nullptr;
            temp->right = nullptr;
            temp->left = nullptr;
//            delete temp;
        } else {
            // node with two children: Get the inorder 
            // successor (smallest in the right subtree) 
            Node* temp = minValueNode(root->right);

            // Copy the inorder successor's 
            // data to this node 
            root->key = temp->key;
            root->ptr = temp->ptr;

            // Delete the inorder successor 
            root->right = deleteNode(root->right,
                                     temp->key);
        }
    }

    // If the tree had only one node
    // then return 
    if (root == nullptr || root->ptr == nullptr)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE 
    root->height = 1 + max(height(root->left),
                           height(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF 
    // THIS NODE (to check whether this 
    // node became unbalanced) 
    int balance = getBalance(root);

    // If this node becomes unbalanced, 
    // then there are 4 cases 

    // Left Left Case 
    if (balance > 1 &&
        getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case 
    if (balance > 1 &&
        getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case 
    if (balance < -1 &&
        getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case 
    if (balance < -1 &&
        getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// A utility function to print preorder 
// traversal of the tree. 
// The function also prints height 
// of every node 
void preOrder(Node *root)
{
    if(root != nullptr && root->ptr != nullptr)
    {
        std::cout << root->key << " ";
        preOrder(root->left);
        preOrder(root->right);
    }
}

Node* search(Node* root, size_t key)
{
    if (root == nullptr || root->ptr == nullptr) {
        return root;
    }

    Node* buff = nullptr;
    Node* node = nullptr;

    if (root->key < key) {
        node = search(root->right, key);
    }else if(root->key > key){
        buff = root;
        node = search(root->left, key);
    }else{
        if(root->next != nullptr){
            Node* n = root->next;
            root->next = n->next;
            return n;
        }else{
            return root;
        }
    }

    return (!node && buff) ? buff : node;
}