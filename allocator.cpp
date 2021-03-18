//
// Created by ekros on 17.03.2021.
//

#include "allocator.h"


area_t* default_area = nullptr;
//u8* ptr;

void init_area(area_t* parent, size_t size){
    LPVOID v = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
    parent->next = new area_t (nullptr, size, v, v, new Tree(v, size));
    *((header_t *)parent->next->ptr) = header_t(parent->next, 0, parent->next->size, 0);
}

void init_area(){
    LPVOID v = VirtualAlloc(nullptr, MIN_AREA_SIZE, MEM_COMMIT, PAGE_READWRITE);
    default_area = new area_t (nullptr, MIN_AREA_SIZE, v, v, new Tree(v, MIN_AREA_SIZE));
    *((header_t *)default_area->ptr) = header_t(default_area, 0, default_area->size, 0);
}

header_t* best(size_t size){
    if(default_area == nullptr){
        init_area();
    }
    size += HEADER;
    area_t* area = default_area;
    for(;;){
        if(area->size >= size){
            void* ptr = area->tree->find_and_delete(size);
            if(ptr != nullptr){
                return (header_t *)ptr;
            }
//            auto* best = (header_t*)((u8*)area->ptr);
//            if(best->prev == 0){
//                if(best->size >= size) {
//                    return best;
//                }else{
//                    break;
//                }
//            }
//            auto* h = (header_t *)((u8*)best - best->prev);
//            for(;;){
//                if(h->status == 0 && h->size >= size && h->size <= best->size){
//                    best = h;
//                }
//                if(h->prev != 0){
//                    h = (header_t *)((u8*)h - h->prev);
//                }else{
//                    if(best->size >= size){
//                        return best;
//                    }else{
//                        break;
//                    }
//                }
//            }
        }
        if(area->next == nullptr){
            break;
        }
        area = area->next;
    }
    if(size < MIN_AREA_SIZE){
        init_area(area, MIN_AREA_SIZE);
    }else if(size > MIN_AREA_SIZE){
        size_t s = std::ceil(1.0*size/MIN_AREA_SIZE)*MIN_AREA_SIZE;
        init_area(area, s);
    }else{
        init_area(area, MIN_AREA_SIZE);
    }
    return reinterpret_cast<header_t *>(area->next->ptr);
}

header_t * create_header(u8* ptr, area_t* area, u8 status, size_t size, size_t prev, const u8* next){
    header_t header(area, status, size, prev);
    *(header_t*)ptr = header;
    ((header_t *)next)->prev = size;
    return reinterpret_cast<header_t *>(ptr);
}
/*
 * ptr - new header addr
 * heap
 * size - new size of chunk
 * prev - prev chunk size
 * */
header_t * create_header_ptr(u8* ptr, area_t * area, size_t size, size_t prev){
    header_t header(area, 0, size, prev);
    *(header_t*)ptr = header;
    area->ptr = ptr;
    return reinterpret_cast<header_t *>(ptr);
}

void* mem_alloc(size_t size)
{
    size += HEADER;
    header_t* h = best(size);
    void* user_ptr = (u8*)h + HEADER;

    size_t n_size = h->size - size;

    if(n_size >= HEADER) {
        header_t * n_header;
        if (h->area->ptr == (u8 *) h) {
            n_header = create_header_ptr((u8 *) h + size, h->area, n_size, size);
        } else {
            n_header = create_header((u8 *) h + size, h->area, 0, n_size, size, (u8 *) h + h->size);
        }
        h->area->tree->add_node(n_header, n_header->size);
        h->size = size;
    }
    h->status = 1;
    return user_ptr;
}


void* mem_realloc(void* ptr, size_t size){
    size += HEADER;
    auto* h = (header_t *)((u8 *) ptr - HEADER);
    auto* next = (header_t *)((u8*)h + h->size);
    size_t n_size = h->size + next->size;
    if(next->status == 0 && n_size >= size) {
        if(next == next->area->ptr){
            if(n_size - size >= HEADER){
                header_t * n_header = create_header_ptr((u8*)h + size, h->area, n_size - size, size);
                h->area->tree->add_node(n_header, n_header->size);
                h->size = size;
            }
        }else {
            if (n_size - size >= HEADER) {
                header_t * n_header = create_header((u8*)h+size, h->area, 0, n_size - size, size, (u8*)next + next->size);
                h->area->tree->add_node(n_header, n_header->size);
                h->size = size;
            } else {
                h->size += next->size;
                ((header_t*)((u8*)next + next->size))->prev = h->size;
            }
        }
        next->area->tree->delete_by_ptr(next, next->size);
    }
    size -= HEADER;
    void* new_ptr = mem_alloc(size);
    memcpy(ptr, new_ptr, h->size-HEADER);
    mem_free(ptr);
    return new_ptr;
}

void mem_free(void* pVoid)
{
    void* t = (u8*)pVoid - HEADER;
    auto* header = (header_t*)t;
    header->status = 0;
    area* area = header->area;

    header_t* next = nullptr;
    header_t* prev = nullptr;
    if(((header_t*)t)->prev != 0){
        prev = reinterpret_cast<header_t *>((u8 *) t - header->prev);
    }
    if(t != area->ptr){
        next = reinterpret_cast<header_t *>((u8 *) t + header->size);
    }

    if(next != nullptr && next->status == 0){
        ((header_t*)t)->size += next->size;
        area->tree->delete_by_ptr(next, next->size);
        if((u8*)next != area->ptr){
            auto* h = reinterpret_cast<header_t *>((u8 *) next + next->size);
            h->prev += ((header_t*)t)->size;
        }else{
            area->ptr = (u8*)t;
            next = header;
        }

    }

    if(prev != nullptr && prev->status == 0){
        area->tree->delete_by_ptr(prev, prev->size);
        prev->size += ((header_t*)t)->size;
        if(next != nullptr && next != t){
            next->prev = prev->size;
        }else{
            area->ptr = (u8*)prev;
        }
        header = prev;
    }
    area->tree->add_node(header, header->size);

    if(((header_t*)(area->ptr))->size == area->size){
        if(area == default_area){
            if(area->next == nullptr){
                VirtualFree(area->mem, 0, MEM_RELEASE);
                delete area;
                default_area = nullptr;
            }else{
                default_area = default_area->next;
                VirtualFree(area->mem, 0, MEM_RELEASE);
                delete area;
            }
        }else {
            area_t* a = default_area;
            for (;;) {
                if(a->next == nullptr){
                    return;
                }
                if (a->next == area && a->next->next == nullptr){
                    VirtualFree(area->mem, 0, MEM_RELEASE);
                    delete a->next;
                    a->next = nullptr;
                    return;
                }else{
                    a = a->next;
                }
            }
        }
    }
}

void mem_show()
{
    area_t* area = default_area;
    printf("MEMORY LOG");
    for(;area != nullptr;){
        auto* h = reinterpret_cast<header_t *>(area->ptr);
        printf("\n    AREA [%x] CHUNKS\n", area);
        for(;;){
            printf("    Data + HEADER. [%x]. Memory of heap [%u]. Status: [%u]\n", h, h->size, h->status);
            if(h->prev == 0){
                break;
            }
            h = reinterpret_cast<header_t *>((u8 *) h - h->prev);
        }
        area = area->next;
    }
    printf("\n");
}