//
// Created by ekros on 17.03.2021.
//

#ifndef OOP1TEST_TREE_H
#define OOP1TEST_TREE_H

#include <cstdint>

class Node {
private:
    void* ptr;
    size_t key;
public:
    void* getData();
    size_t getKey() const;
    Node()= default;
    Node(Node* parent, Node* left, Node* right, void* ptr, size_t key){
        this->parent = parent;
        this->left = left;
        this->right = right;
        this->ptr = ptr;
        this->key = key;
    }

    Node* parent;
    Node* left;
    Node* right;
};


class Tree {
public:
    explicit Tree(void* ptr, size_t key){
        node = new Node(nullptr, nullptr, nullptr, ptr, key);
    };
    Node* node;
    void add_node(void* ptr, size_t key);
    void delete_node(Node* n);
    void* find(size_t key);
    void* find_and_delete(size_t key);
    ~Tree(){
        delete node;
    }
private:
    void concat_node(Node* n);
};


#endif //OOP1TEST_TREE_H
