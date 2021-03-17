//
// Created by ekros on 17.03.2021.
//

#include "tree.h"

void * Node::getData() {
    return this->ptr;
}

size_t Node::getKey() const {
    return this->key;
}

void Tree::add_node(void *ptr, size_t key) {
    Node *n = node;
    for (;;){
        if (n->getKey() > key) {
            if(n->left != nullptr){
                n = n->left;
            }else{
                n->left = new Node(n, nullptr, nullptr, ptr, key);
                break;
            }
        }else{
            if(n->right != nullptr) {
                n = n->right;
            }else{
                n->right = new Node(n, nullptr, nullptr, ptr, key);
                break;
            }
        }
    }
}

void Tree::concat_node(Node* n){
    Node * this_n = node;
    for(;;){
        if (this_n->getKey() > n->getKey()) {
            if(this_n->left != nullptr){
                this_n = this_n->left;
            }else{
                this_n->left = n;
                break;
            }
        }else{
            if(this_n->right != nullptr) {
                this_n = this_n->right;
            }else{
                this_n->right = n;
                break;
            }
        }
    }
}

void Tree::delete_node(Node *n) {
    Node * parent = n->parent;
    Node * left = n->left;
    Node * right = n->right;

    if(parent == nullptr){
        if(right != nullptr){
            node = right;
            if(left != nullptr){
                concat_node(left);
            }
        }else if(left != nullptr){
            node = left;
        }
    }else{
        if(parent->right == n){
            if(left != nullptr){
                parent->right = left;
                if(right != nullptr){
                    concat_node(right);
                }
            }else if(right != nullptr){
                parent->right = right;
            }
        }else{
            if(right != nullptr){
                parent->left = right;
                if(left != nullptr){
                    concat_node(left);
                }
            }else if(left != nullptr){
                parent->left = left;
            }
        }
    }

    delete n;

}

void *Tree::find(size_t key) {
    Node* n = node;
    Node* prev = nullptr;
    if(n == nullptr){
        return nullptr;
    }
    for(;;){
        size_t n_key = n->getKey();
        if(n_key < key && n->right != nullptr){
            if(prev != nullptr){
                return prev->getData();
            }
            n = n->right;
        }else if(n_key > key && n->left != nullptr){
            prev = n;
            n = n->left;
        }else if(key == n_key){
            return n->getData();
        }else if(prev != nullptr){
            return prev;
        }else{
            return nullptr;
        }
    }
}

void *Tree::find_and_delete(size_t key) {
    Node* n = node;
    Node* prev = nullptr;
    if(n == nullptr){
        return nullptr;
    }
    for(;;){
        size_t n_key = n->getKey();
        if(n_key < key && n->right != nullptr){
            if(prev != nullptr){
                n = prev;
            }
            n = n->right;
        }else if(n_key > key && n->left != nullptr){
            prev = n;
            n = n->left;
        }else if(key == n_key){
            break;
        }else if(prev != nullptr){
            n = prev;
        }else {
            n = nullptr;
        }

    }
    if(n != nullptr) {
        void *ptr = n->getData();
        delete_node(n);
        return ptr;
    }else{
        return nullptr;
    }
}
