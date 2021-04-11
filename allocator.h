#ifndef OOP1TEST_ALLOCATOR_H
#define OOP1TEST_ALLOCATOR_H

#include <cstdio>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <cmath>
#include "core_alloc.h"
#include "avl_tree.h"
#include "config.h"

void mem_show();
void mem_free(void* pVoid);
void* mem_alloc(size_t size);
void* mem_realloc(void* ptr, size_t size);

typedef struct header
{
    header()= default;
    header(u8 status, size_t size, size_t prev, u8 is_first, u8 is_last){
        this->status = status;
        this->size = size;
        this->prev = prev;
        this->is_first = is_first;
        this->is_last = is_last;
    }
    Node node;
    u8 status;
    u8 is_first;
    u8 is_last;
    size_t size;
    size_t prev;
}header_t;

extern Node* root;

header_t* create_header(header_t* prev, size_t size, void* mem);
header_t* best(size_t size);
void* user_ptr(header_t* h);
header_t* merge(header_t* first, header_t* second);
void split(header_t* h, size_t size);
header_t* next_h(header_t* h);
header_t* prev_h(header_t* h);
header_t* get_h(void* h);
void clear_node(Node* h);
size_t new_area_size(size_t size);
#endif //OOP1TEST_ALLOCATOR_H+-