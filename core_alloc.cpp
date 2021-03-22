//
// Created by ekros on 22.03.2021.
//


#include "core_alloc.h"

LPVOID core_alloc(size_t size){
    return VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
}
void core_free(LPVOID ptr){
    VirtualFree(ptr, 0, MEM_RELEASE);
}