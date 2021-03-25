//
// Created by ekros on 17.03.2021.
//

#ifndef OOP1TEST_ALLOCATOR_H
#define OOP1TEST_ALLOCATOR_H

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include "core_alloc.h"

#include "tree.h"

#define u8 char
#define MIN_AREA_SIZE 204800
#define AREA_STRUCT_SIZE sizeof(area_t)


void mem_show();
void mem_free(void* pVoid);
void* mem_alloc(size_t size);
void* mem_realloc(void* ptr, size_t size);


struct area;

typedef struct header
{
    header()= default;
    header(area* area, u8 status, size_t size, size_t prev){
        this->area = area;
        this->status = status;
        this->size = size;
        this->prev = prev;
    }
    area* area;
    u8 status;
    size_t size;
    size_t prev;
}header_t;

typedef struct area{
    area() {
        next = nullptr;
        prev = nullptr;
        size = -1;
        mem = nullptr;
        ptr = nullptr;
    };
    area(area* next, area* prev, size_t size, LPVOID mem, void* ptr){
        this->next = next;
        this->prev = prev;
        this->size = size;
        this->mem = mem;
        this->ptr = ptr;
    }
    area* next{};
    area* prev{};
    size_t size{};
    LPVOID mem{};
    void * ptr;

    area* create_new_area(size_t area_size);
}area_t;

#define HEADER sizeof(header_t)

extern area_t* default_area;
extern area_t* last_area;
extern Tree tree;

header_t* get_header(void* ptr);
header_t* next_header(header_t* h);
header_t* prev_header(header_t* h);
#endif //OOP1TEST_ALLOCATOR_H
