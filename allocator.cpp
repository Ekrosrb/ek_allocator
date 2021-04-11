#include "allocator.h"

//void* default_area = nullptr;
//size_t default_area_size = 0;
Node* root = nullptr;
void mem_show(){

}

void mem_free(void* pVoid){

}

void* mem_alloc(size_t size){
    if(size > ALLOC_MAX_SIZE || size == 0){
        return nullptr;
    }
    size = ALIGN(size+HEADER_SIZE);

    header_t* h = best(size);
    if(!h) return nullptr;

    size_t last = h->size - size;

    if(last > HEADER_SIZE + MIN_ALLOCATED_SIZE){
        header_t* last_h = create_header(h, last, nullptr);
        Node* last_n = insert(root, last_h->size, last_h, &last_h->node);
        root = !root ? last_n : root;
    }

    h->status = 1;
    return user_ptr(h);
}

void* mem_realloc(void* ptr, size_t size){
    return nullptr;
}

header_t* best(size_t size){
    Node* node = nullptr;
    header_t* h;
    if(root) {
        node = search(root, size);
    }

    if(!root || !node || !node->ptr){
        size_t s = new_area_size(size);
        void* mem = core_alloc(s);
        return !mem ? nullptr : create_header(nullptr, s, mem);
    }
    void* ptr = node->ptr;
    root = (root == node) ? nullptr : root;
    deleteNode(node, node->key);
    return (header_t*)ptr;
}
//prev
//size - size of new header
//mem
header_t* create_header(header_t* prev, size_t size, void* mem){
    header_t* h;
    if(prev){

        header_t* next = next_h(prev);
        prev->size -= size;
        h = (header_t*)((u8*)prev + prev->size);
        h->size = size;
        h->prev = prev->size;
        h->is_first = 0;

        if(!prev->is_last){
            next->prev = h->size;
            h->is_last = 0;
        }else{
            prev->is_last = 0;
            h->is_last = 1;
        }
    }else{
        h = (header_t*)mem;
        h->prev = 0;
        h->size = size;
        h->is_last = h->is_first = 1;
    }
    h->status = 0;
    return h;
}

header_t* next_h(header_t* h){
    return (header_t *)((u8 *) h + h->size);
}

header_t* prev_h(header_t* h){
    return (header_t *)((u8 *) h - h->prev);
}

size_t new_area_size(size_t size){
    size_t n_size = MIN_AREA_SIZE;
    if(size > n_size){
        n_size = std::ceil((1.0*size)/PAGE_SIZE)*PAGE_SIZE;
    }
    return n_size;
}

void* user_ptr(header_t* h){
    return (u8*)h+HEADER_SIZE;
}

header_t* merge(header_t* first, header_t* second){
    header_t *next, *prev;
    if(next_h(first) == second){
        prev = first;
        next = second;
    }else{
        prev = second;
        next = first;
    }
    prev->size += next->size;

    if(!next->is_last){
        next_h(next)->prev = prev->size;
    }else{
        prev->is_last = 1;
    }
    return prev;
}