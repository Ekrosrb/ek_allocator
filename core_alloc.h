//
// Created by ekros on 22.03.2021.
//

#ifndef OOP1TEST_CORE_ALLOC_H
#define OOP1TEST_CORE_ALLOC_H


#include <memoryapi.h>

LPVOID core_alloc(size_t size);
void core_free(LPVOID ptr);


#endif //OOP1TEST_CORE_ALLOC_H
