//
// Created by ekros on 17.03.2021.
//

#include "allocator.h"


area_t* default_area = nullptr;
//u8* ptr;

void init_area(area_t* parent, size_t size, size_t heap_size){
    LPVOID v = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
    parent->next = new area_t (nullptr, size, v, heap_t(nullptr, heap_size, (u8*)v, (u8*)v));
    parent->next->heap.area = parent->next;
    *((header_t *)parent->next->heap.ptr) = header_t(&parent->next->heap, 0, parent->next->heap.size, 0);
}

void init_area(){
    LPVOID v = VirtualAlloc(nullptr, MIN_AREA_SIZE, MEM_COMMIT, PAGE_READWRITE);
    default_area = new area_t (nullptr, MIN_AREA_SIZE, v, heap_t(nullptr, MIN_HEAP_SIZE, (u8*)v, (u8*)v));
    default_area->heap.area = default_area;
    *((header_t *)default_area->heap.ptr) = header_t(&default_area->heap, 0, default_area->heap.size, 0);
}

//void create(){
//    static heap_t vm;
//    ptr = vm.heap;
//    header_t h;
//    h.status = 0;
//    h.size = HEAP_SIZE;
//    h.prev = 0;
//    *((header_t*)ptr) = h;
//    init = true;
//}

void heap_realloc(heap_t* heap, size_t size){
    if(heap->area->size >= size){
        ((header_t *)heap->ptr)->size += size-heap->size;
        heap->size = size;
    }
}

header_t* best(size_t size){
    if(default_area == nullptr){
        init_area();
    }
    area_t* area = default_area;
    for(;;){
        if(area->size >= size){
            if(area->heap.size < size){
                auto* h = reinterpret_cast<header_t *>(area->heap.ptr);
                size_t n_size = area->heap.size - h->size + size + HEADER;
                if (n_size <= area->size) {
                    heap_realloc(&area->heap, n_size);
                    return h;
                }
            }else{
                auto* best_h = reinterpret_cast<header_t *>(area->heap.ptr);
                if(best_h->prev == 0 && best_h->size >= size + HEADER){
                    return best_h;
                }else {
                    u8 *prev = (u8 *) best_h - best_h->prev;
                    auto *prev_h = reinterpret_cast<header_t *>(prev);
                    for (;;) {
                        if (prev_h->status == 0 && prev_h->size >= size &&
                            prev_h->size < best_h->size) {
                            best_h = prev_h;
                        }
                        if (prev_h->prev == 0) {
                            break;
                        } else {
                            prev = prev - prev_h->prev;
                            prev_h = reinterpret_cast<header_t *>(prev);
                        }
                    }
                    if(best_h->status == 0 && best_h->size >= size){
                        if((u8*)best_h == area->heap.ptr){
                            if(best_h->size >= size + HEADER){
                                return best_h;
                            }
                        }else {
                            return best_h;
                        }
                    }
                }
                auto* h = reinterpret_cast<header_t *>(area->heap.ptr);
                if(h->status != 1) {
                    size_t n_size = area->heap.size - h->size + size + HEADER;
                    if (n_size <= area->size) {
                        heap_realloc(&area->heap, n_size);
                        return h;
                    }
                }
            }
        }
        if(area->next == nullptr){
            break;
        }
        area = area->next;
    }
    size += HEADER;
    if(size < MIN_HEAP_SIZE){
        init_area(area, MIN_HEAP_SIZE, MIN_AREA_SIZE);
    }else if(size > MIN_AREA_SIZE){
        size_t s = std::ceil(1.0*size/MIN_AREA_SIZE)*MIN_AREA_SIZE;
        init_area(area, s, size);
    }else{
        init_area(area, MIN_AREA_SIZE, size);
    }
    return reinterpret_cast<header_t *>(area->next->heap.ptr);
}

header_t * create_header(u8* ptr, heap_t* heap, u8 status, size_t size, size_t prev, const u8* next){
    header_t header(heap, status, size, prev);
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
header_t * create_header_ptr(u8* ptr, heap_t* heap, size_t size, size_t prev){
    header_t header(heap, 0, size, prev);
    *(header_t*)ptr = header;
    heap->ptr = ptr;
    return reinterpret_cast<header_t *>(ptr);
}

void* mem_alloc(size_t size)
{
    size += HEADER;
    header_t* h = best(size);
    void* user_ptr = (u8*)h + HEADER;

    size_t n_size = h->size - size;

    if(n_size >= HEADER) {
        if (h->heap->ptr == (u8 *) h) {
            create_header_ptr((u8 *) h + size, h->heap, n_size, size);
        } else {
            create_header((u8 *) h + size, h->heap, 0, n_size, size, (u8 *) h + h->size);
        }
        h->size = size;
    }
    h->status = 1;
    return user_ptr;

}


void* mem_realloc(void* ptr, size_t size){
    size += HEADER;
    auto* h = (header_t *)((u8 *) ptr - HEADER);
    auto* next = (header_t *)((u8*)h + h->size);
    if(next->status == 0) {
        size_t n_size = h->size + next->size;
        if (n_size >= size) {
            if (n_size - size >= HEADER) {
                if ((u8 *) next == h->heap->ptr) {
                    create_header_ptr((u8 *) h + size, h->heap, n_size - size, size);
                } else {
                    create_header((u8 *) h + size, h->heap, 0, n_size - size, size, (u8 *) next + next->size);
                }
                h->size = size;
                return ptr;
            }
            h->size += next->size;
            return ptr;
        } else if ((u8 *) next == h->heap->ptr) {
            size_t n_heap_size = h->heap->size - next->size + size + HEADER;
            if (n_heap_size <= h->heap->area->size) {
                heap_realloc(h->heap, n_heap_size);
                n_size = h->size + next->size - HEADER;
                create_header_ptr((u8 *) next + next->size - HEADER, h->heap, HEADER, n_size);
                h->size = n_size;
                return ptr;
            }
        }
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
    ((header_t*)t)->status = 0;
    heap_t* heap = ((header_t*)t)->heap;

    header_t* next = nullptr;
    header_t* prev = nullptr;
    if(((header_t*)t)->prev != 0){
        prev = reinterpret_cast<header_t *>((u8 *) t - ((header_t *) t)->prev);
    }
    if(t != heap->ptr){
        next = reinterpret_cast<header_t *>((u8 *) t + ((header_t *) t)->size);
    }

    if(next != nullptr && next->status == 0){
        ((header_t*)t)->size += next->size;
        if((u8*)next != heap->ptr){
            auto* h = reinterpret_cast<header_t *>((u8 *) next + next->size);
            h->prev += ((header_t*)t)->size;
        }else{
            heap->ptr = (u8*)t;
            next = (header_t*)t;
        }

    }

    if(prev != nullptr && prev->status == 0){
        prev->size += ((header_t*)t)->size;
        if(next != nullptr && next != t){
            next->prev = prev->size;
        }else{
            heap->ptr = (u8*)prev;
        }
    }
    if(((header_t*)heap->ptr)->size == heap->size){
        if(heap->area == default_area){
            if(default_area->next == nullptr){
                VirtualFree(heap->area->mem, 0, MEM_RELEASE);
                delete default_area;
                default_area = nullptr;
            }else{
                default_area = default_area->next;
                VirtualFree(heap->area->mem, 0, MEM_RELEASE);
                delete heap->area;
            }
        }else {
            area_t* a = default_area;
            for (;;) {
                if(a->next == nullptr){
                    return;
                }
                if (a->next == heap->area && a->next->next == nullptr){
                    VirtualFree(heap->area->mem, 0, MEM_RELEASE);
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
        auto* h = reinterpret_cast<header_t *>(area->heap.ptr);
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