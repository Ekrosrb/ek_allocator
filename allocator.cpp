//
// Created by ekros on 17.03.2021.
//

#include "allocator.h"


area_t* default_area;
area_t* last_area;

Tree tree = Tree();
//u8* ptr;



area* area::create_new_area(size_t area_size) {
    LPVOID v = core_alloc(area_size);
    this->next = (area_t*)(v);
    *this->next = area_t(nullptr, last_area, area_size-AREA_STRUCT_SIZE, v, (u8*)v+AREA_STRUCT_SIZE);
    *get_header(this->next->ptr) = header_t(this->next, 0, this->next->size, 0);
    return this->next;
}

int area_equals(area_t* a, area_t* b){
    return a->ptr == b->ptr;
}

void init_area(){
    LPVOID v = core_alloc(MIN_AREA_SIZE+AREA_STRUCT_SIZE);
    default_area = (area_t *)(v);
    *default_area = area_t(nullptr, nullptr, MIN_AREA_SIZE-AREA_STRUCT_SIZE, v, (u8*)v+AREA_STRUCT_SIZE);
    last_area = default_area;
    tree.add_node(default_area->ptr, MIN_AREA_SIZE-AREA_STRUCT_SIZE);
    *get_header(default_area->ptr) = header_t(default_area, 0, default_area->size, 0);
}

header_t* get_header(void* ptr){
    return (header_t*)ptr;
}

header_t* next_header(header_t* h){
    return h == h->area->ptr ? nullptr : get_header((u8*)h + h->size);
}

header_t* prev_header(header_t* h){
    return h->prev == 0 ? nullptr : get_header((u8*)h - h->prev);
}

header_t* best(size_t size){
    if(default_area == nullptr){
        init_area();
    }
    size += HEADER;
    void* ptr = tree.find_and_delete(size);
    if(ptr != nullptr){
        return get_header(ptr);
    }
    size_t n_size = MIN_AREA_SIZE+sizeof(area_t);
    if(size > n_size){
        n_size = std::ceil(1.0*size/MIN_AREA_SIZE)*MIN_AREA_SIZE;
    }
    last_area = last_area->create_new_area(n_size);
    return get_header(last_area->ptr);
}

header_t * create_header(u8* ptr, area_t* area, u8 status, size_t size, size_t prev, u8* next){
    header_t header(area, status, size, prev);
    *get_header(ptr) = header;
    get_header(next)->prev = size;
    return (header_t *)(ptr);
}
/*
 * ptr - new header addr
 * heap
 * size - new size of chunk
 * prev - prev chunk size
 * */
header_t * create_header_ptr(u8* ptr, area_t * area, size_t size, size_t prev){
    header_t header(area, 0, size, prev);
    *get_header(ptr) = header;
    area->ptr = ptr;
    return get_header(ptr);
}

void* mem_alloc(size_t size)
{
    size += size%2 + HEADER;
    header_t* h = best(size);
    void* user_ptr = (u8*)h + HEADER;

    size_t n_size = h->size - size;

    if(n_size >= HEADER) {
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
    size += size%2 + HEADER;
    auto* h = get_header((u8 *) ptr - HEADER);

    auto* next = next_header(h);
    size_t n_size = h->size + next->size;
    if(next->status == 0 && n_size >= size) {
        if(next == next->area->ptr){
            if(n_size - size >= HEADER){
                header_t * n_header = create_header_ptr((u8*)h + size, h->area, n_size - size, size);
                tree.add_node(n_header, n_header->size);
                h->size = size;
            }
        }else {
            if (n_size - size >= HEADER) {
                header_t * n_header = create_header((u8*)h+size, h->area, 0, n_size - size, size, (u8*)next_header(next));
                tree.add_node(n_header, n_header->size);
                h->size = size;
            } else {
                h->size += next->size;
                next_header(next)->prev = h->size;
            }
        }
        tree.delete_by_ptr(next, next->size);
    }
    size -= size%2 + HEADER;
    void* new_ptr = mem_alloc(size);
    memcpy(new_ptr, ptr, h->size-HEADER);
    mem_free(ptr);
    return new_ptr;
}

void mem_free(void* pVoid)
{
    void* t = (u8*)pVoid - HEADER;
    auto* header = get_header(t);
    header->status = 0;
    area* area = header->area;

    header_t* next = next_header(header);
    header_t* prev = prev_header(header);

    if(next != nullptr && next->status == 0){
        header->size += next->size;
        tree.delete_by_ptr(next, next->size);
        if((u8*)next != area->ptr){
            auto* h = next_header(next);
            h->prev = header->size;
        }else{
            area->ptr = (u8*)t;
            next = header;
        }

    }

    if(prev != nullptr && prev->status == 0){
        tree.delete_by_ptr(prev, prev->size);
        prev->size += header->size;
        if(next != nullptr && next != t){
            next->prev = prev->size;
        }else{
            area->ptr = (u8*)prev;
        }
        header = prev;
    }
    tree.add_node(header, header->size);

      if(get_header(area->ptr)->size == area->size){
          if(area_equals(area, default_area)) {
              default_area = default_area->next;
          }else{
              if(area == last_area){
                  last_area = default_area;
              }else {
                  area->prev->next = area->next;
              }
          }
          tree.delete_by_ptr(area->ptr, header->size);
          core_free(area->mem);
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
            printf("    Data + HEADER. [%x]. Memory of chunk [%u]. Status: [%u]\n", h, h->size, h->status);
            if(h->prev == 0){
                break;
            }
            h = prev_header(h);
        }
        area = area->next;
    }
    printf("\n");
}


