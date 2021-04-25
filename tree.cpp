#include "tree.h"

Node* newNode(size_t key, void* ptr, Node* node){
    node->ptr = ptr;
    node->key = key;
    node->left = nullptr;
    node->right = nullptr;
    node->next = nullptr;
    node->parent = nullptr;
    return node;
}

void childLink(Node* old_node, Node* new_node){
    if(old_node && new_node){
        if(old_node->left){
            new_node->left = old_node->left;
            new_node->left->parent = new_node;
            old_node->left = nullptr;
        }
        if(old_node->right){
            new_node->right = old_node->right;
            new_node->right->parent = new_node;
            old_node->right = nullptr;
        }
    }
}

Node* insert(Node* root, size_t key, void* ptr, Node* node){
    if(!root){
        return newNode(key, ptr, node);
    }

    if (key < root->key) {
        root->left = insert(root->left, key, ptr, node);
        root->left->parent = root;
    }else if (key > root->key) {
        root->right = insert(root->right, key, ptr, node);
        root->right->parent = root;
    }else {
        Node* n = newNode(key, ptr, node);
        n->next = root;
        n->parent = root->parent;
        root->parent = nullptr;
        childLink(root, n);
        root = n;
    }
    return root;
}

Node * minValueNode(Node* node){
    Node* current = node;

    while (current->left != nullptr) {
        current = current->left;
    }

    return current;
}

Node* deleteNode(Node* root, size_t key, void* ptr){
    if(root == nullptr){
        return nullptr;
    }

    if (key < root->key) {
        root->left = deleteNode(root->left, key, ptr);
    }else if (key > root->key) {
        root->right = deleteNode(root->right, key, ptr);
    }else {
        if(root->next != nullptr){
            if(root->ptr != ptr) {
                Node *n = root;
                while (n->next->ptr != ptr) { n = n->next;
                }
                Node* next = n->next;
                n->next = next->next;
                next->next = nullptr;
                return root;
            }else{
                Node* next = root->next;
                next->parent = root->parent;
                root->parent = nullptr;
                root->next = nullptr;
                childLink(root, next);
                return next;
            }
        }

        if(!root->left || !root->right){
            Node* temp = !root->left ? root->right : root->left;
            if(!temp){
                return nullptr;
            }
            temp->parent = root->parent;
            root->left = root->right = root->parent = nullptr;
            return temp;
        }

        Node* temp = minValueNode(root->right);
        if(root->right == temp){
            if(root->left){
                temp->left = root->left;
                temp->left->parent = temp;
            }
        }else{
            if(temp->right){
                temp->parent->left = temp->right;
                temp->right->parent = temp->parent;
                temp->right = nullptr;
            }else{
                temp->parent->left = nullptr;
            }
            childLink(root, temp);
        }
        temp->parent = root->parent;
        root->left = root->right = root->parent = nullptr;
        return temp;
    }
    return root;
}

Node* search(Node* root, size_t key)
{
    if (root == nullptr) {
        return root;
    }

    Node* buff = nullptr;
    Node* node;

    if (root->key < key) {
        node = search(root->right, key);
    }else if(root->key > key){
        buff = root;
        node = search(root->left, key);
    }else{
        return root;
    }

    return (!node && buff) ? buff : node;
}

void print(Node* root, int space){
    if(root == nullptr){
        return;
    }
    char next_space[space];
    for(int i = 0; i < space-1; i++){
        next_space[i] = ' ';
    }
    next_space[space-1] = '\0';
    print(root->right, space + 6);
    if(space) {
        std::cout << next_space;
    }
    std::cout << root->key << std::endl << std::endl;
    print(root->left, space + 5);



}
