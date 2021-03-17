//
// Created by ekros on 17.03.2021.
//

#ifndef OOP1TEST_ALLOCATOR_H
#define OOP1TEST_ALLOCATOR_H

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <memoryapi.h>


#define u8 uint8_t
#define MIN_HEAP_SIZE 32768
#define MIN_AREA_SIZE 204800


void mem_show();
void mem_free(void* pVoid);
void* mem_alloc(size_t size);
void* mem_realloc(void* ptr, size_t size);

struct area;

typedef struct virtual_heap
{
    virtual_heap()= default;
    virtual_heap(area* area, size_t size, u8* heap, u8* ptr){
        this->area = area;
        this->size = size;
        this->heap = heap;
        this->ptr = ptr;
    }
    area* area;
    size_t size;
    u8* heap{};
    u8* ptr{};
}heap_t;

typedef struct header
{
    header()= default;
    header(heap_t* heap, u8 status, size_t size, size_t prev){
        this->heap = heap;
        this->status = status;
        this->size = size;
        this->prev = prev;
    }
    heap_t* heap;
    u8 status;
    size_t size;
    size_t prev;
}header_t;

typedef struct area{
    area()= default;
    area(area* next, size_t size, LPVOID mem, heap_t heap){
        this->next = next;
        this->size = size;
        this->mem = mem;
        this->heap = heap;
    }
    area* next{};
    size_t size{};
    LPVOID mem{};
    heap_t heap;
    void* ptr;
}area_t;

#define HEADER sizeof(header_t)

extern area_t* default_area;

#endif //OOP1TEST_ALLOCATOR_H
