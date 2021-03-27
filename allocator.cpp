//
// Created by ekros on 17.03.2021.
//

#include "allocator.h"


area_t* default_area;
area_t* last_area;

Tree tree = Tree();

void* mem_alloc(size_t size)
{
    size = alignment(size) + HEADER_SIZE;
    header_t* h = best(size);
    void* user_ptr = (u8*)h + HEADER_SIZE;
    size_t n_size = h->size - size;

    if(n_size >= HEADER_SIZE) {
        header_t * n_header;
        if (h->area->ptr == (u8 *) h) {
            n_header = create_header_ptr((u8 *) h + size, h->area, n_size, size);
        } else {
            n_header = create_header((u8 *) h + size, h->area, 0, n_size, size, (u8 *) next_header(h));
        }
        tree.add_node(n_header, n_header->size);
        h->size = size;
    }
    h->status = 1;
    return user_ptr;
}


void* mem_realloc(void* ptr, size_t size){
    size = alignment(size);
    auto* h = get_header((u8 *) ptr - HEADER_SIZE);

    auto* next = next_header(h);
    size_t n_size = h->size + next->size;

    if(next->status == 0 && n_size >= size) {
        size += HEADER_SIZE;
        size_t last_size = n_size - size;
        if(next == next->area->ptr && last_size >= HEADER_SIZE){
                header_t * n_header = create_header_ptr((u8*)h + size, h->area, last_size, size);
                tree.add_node(n_header, n_header->size);
                h->size = size;
        }else {
            if (last_size >= HEADER_SIZE) {
                header_t * n_header = create_header((u8*)h+size, h->area, 0, last_size, size, (u8*)next_header(next));
                tree.add_node(n_header, n_header->size);
                h->size = size;
            } else {
                h->size += next->size;
                next_header(next)->prev = h->size;
            }
        }
        tree.delete_by_ptr(next, next->size);
        return (u8*)h+HEADER_SIZE;
    }
    void* new_ptr = mem_alloc(size);
    memmove(new_ptr, ptr, h->size - HEADER_SIZE);
    mem_free(ptr);
    return new_ptr;
}

void mem_free(void* pVoid)
{
    void* t = (u8*)pVoid - HEADER_SIZE;
    auto* header = get_header(t);
    header->status = 0;
    area* area = header->area;

    header = merge_prev(merge_next(header, next_header(header)), prev_header(header));

    if(get_header(area->ptr)->size == area->size){
        free_area(area);
    }else{
        tree.add_node(header, header->size);
    }
}

void mem_show()
{
    area_t* area = default_area;
    printf("MEMORY LOG");
    for(;area != nullptr;){
        auto* h = (header_t *)(area->ptr);
        printf("\n    AREA [%x] CHUNKS\n", area);
        for(;;){
            printf("    Data + HEADER_SIZE. [%x]. Memory of chunk [%u]. Status: [%u]\n", h, h->size, h->status);
            if(h->prev == 0){
                break;
            }
            h = prev_header(h);
        }
        area = area->next;
    }
    printf("\n");
}

header_t* best(size_t size){
    if(default_area == nullptr){
        init_area(new_area_size(size));
    }
    void* ptr = tree.find_and_delete(size);
    if(ptr != nullptr){
        return get_header(ptr);
    }
    last_area = last_area->create_new_area(new_area_size(size));
    return get_header(last_area->ptr);
}

header_t* merge_next(header_t* current, header_t* next){
    if(next != nullptr && next->status == 0){
        current->size += next->size;
        tree.delete_by_ptr(next, next->size);
        if((u8*)next != next->area->ptr){
            auto* h = next_header(next);
            h->prev = current->size;
        }else{
            next->area->ptr = (u8*)current;
        }
    }
    return current;
}
header_t* merge_prev(header_t* current, header_t* prev){
    if(prev != nullptr && prev->status == 0){
        tree.delete_by_ptr(prev, prev->size);
        prev->size += current->size;
        header_t * next = next_header(current);
        if(next != nullptr){
            next->prev = prev->size;
        }else{
            prev->area->ptr = (u8*)prev;
        }
        current = prev;
    }
    return current;
}

void free_area(area_t* area){
    if(area == default_area) {
        if(default_area->next != nullptr){
            default_area = default_area->next;
            default_area->prev = nullptr;
        }else {
            default_area = nullptr;
        }
    }else if(area == last_area) {
        if(last_area == default_area){
            default_area = last_area = nullptr;
        }else{
            last_area = last_area->prev;
            last_area->next = nullptr;
        }
    }else{
        area->prev->next = area->next;
        area->next->prev = area->prev;
    }
    core_free(area->mem);
}

area* area::create_new_area(size_t area_size) {
    LPVOID v = core_alloc(area_size);
    this->next = (area_t*)(v);
    *this->next = area_t(nullptr, last_area, area_size-AREA_STRUCT_SIZE, v, (u8*)v+AREA_STRUCT_SIZE);
    *get_header(this->next->ptr) = header_t(this->next, 0, this->next->size, 0);
    return this->next;
}

void init_area(size_t area_size){
    LPVOID v = core_alloc(area_size);
    default_area = (area_t *)(v);
    *default_area = area_t(nullptr, nullptr, area_size-AREA_STRUCT_SIZE, v, (u8*)v+AREA_STRUCT_SIZE);
    last_area = default_area;
    tree.add_node(default_area->ptr, default_area->size);
    *get_header(default_area->ptr) = header_t(default_area, 0, default_area->size, 0);
}

size_t alignment(size_t size){
    return size + (ALIGN - size % ALIGN);
}

size_t new_area_size(size_t size){
    size_t n_size = MIN_AREA_SIZE+AREA_STRUCT_SIZE+HEADER_SIZE;
    if(size > n_size){
        n_size = std::ceil(1.0*size/MIN_AREA_SIZE)*MIN_AREA_SIZE;
    }
    return n_size;
};

header_t* get_header(void* ptr){
    return (header_t*)ptr;
}

header_t* next_header(header_t* h){
    return h == h->area->ptr ? nullptr : get_header((u8*)h + h->size);
}

header_t* prev_header(header_t* h){
    return h->prev == 0 ? nullptr : get_header((u8*)h - h->prev);
}

header_t * create_header(u8* ptr, area_t* area, u8 status, size_t size, size_t prev, u8* next){
    header_t header(area, status, size, prev);
    *get_header(ptr) = header;
    get_header(next)->prev = size;
    return (header_t *)(ptr);
}

header_t * create_header_ptr(u8* ptr, area_t * area, size_t size, size_t prev){
    header_t header(area, 0, size, prev);
    *get_header(ptr) = header;
    area->ptr = ptr;
    return get_header(ptr);
}